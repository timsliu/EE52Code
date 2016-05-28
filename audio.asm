    NAME    AUDIO
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;                                                                            ;
;                                  AUDIO Code                                ;
;                           Audio Related Functions                          ;
;                                   EE/CS 52                                 ;
;                                                                            ;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

; Description: This file contains the functions relating to the audio
;              output.

; Table of Contents
;
;    AudioIRQOn   -turns on INT0 audio data request interrupts
;    AudioEH      -event handler for audio data request interrupts
;    AudioOutput  -outputs audio data to the MP3 decoder
;    Audio_Play   -sets up shared variables for outputting audio
;    Audio_Halt   -stops audio play by turning off ICON0 interrupts
;    Update       -returns if NextBuffer is empty


; Revision History:
;
;    5/18/16    Tim Liu    created file
;    5/20/16    Tim Liu    wrote outlines for all functions
;    5/20/16    Tim Liu    wrote Audio_Halt and AudioEH
;    5/21/16    Tim Liu    wrote AudioOutput
;    5/21/16    Tim Liu    wrote Audio_Play
;    5/21/16    Tim Liu    wrote AudioIRQOn
;    5/21/16    Tim Liu    wrote Update
;
; local include files
$INCLUDE(AUDIO.INC)
$INCLUDE(MIRQ.INC)
$INCLUDE(GENERAL.INC)

CGROUP    GROUP    CODE
DGROUP    GROUP    DATA



CODE SEGMENT PUBLIC 'CODE'

        ASSUME  CS:CGROUP, DS:DGROUP 

;external function declarations

;Name:               AudioIRQOn
;
;Description:        This function enables data request interrupts from the
;                    MP3 decoder. The function writes ICON0ON to ICON0Address.
;                    The function also sends an EOI to clear out the interrupt
;                    handler.
; 
;Operation:          The function copies ICON0ON to AX and copies ICON0Address
;                    to DX. The function then outputs the address to the
;                    peripheral control block. The function then outputs
;                    INT0EOI to INTCtrlrEOI to clear the interrupt controller.
;
;Arguments:          None
;
;Return Values:      None
;
;Local Variables:    None
;
;Shared Variables:   None
;
;Output:             None
;
;Error Handling:     None
;
;Algorithms:         None
;
;Registers Used:     None
;
;Known Bugs:         None
;
;Limitations:        None
;
;Author:             Timothy Liu
;
;Last Modified       5/21/16

AudioIRQOn            PROC    NEAR
                      PUBLIC  AudioIRQOn

AudioIRQOnStart:                          ;save registers
    PUSH    AX
    PUSH    DX

AudioIRQOnOutput:                         ;turn on INT0 data request interrupts
                                          ;and send an EOI
    MOV     DX, ICON0Address              ;address of INT0 interrupt controller
    MOV     AX, ICON0On                   ;value to start int 0 interrupts
    OUT     DX, AX

    MOV     DX, INTCtrlrEOI               ;address of interrupt EOI register
    MOV     AX, INT0EOI                   ;INT0 end of interrupt
    OUT     DX, AX                        ;output to peripheral control block

AudioIRQOnDone:                           ;restore registers and return
    POP     DX
    POP     AX
    RET



AudioIRQOn        ENDP



;Name:               AudioEH
;
;Description:        This function handles audio data request interrupts.
;                    The function is called whenever the VS1011 MP3
;                    decoder needs more data.
; 
;Operation:          The function first saves the registers that will be
;                    modified by AudioOutput. The function calls AudioOutput,
;                    which checks if the data buffers have data and serially
;                    outputs data to the MP3 decoder. The function then restores
;                    the registers and IRET.
;
;Arguments:          None
;
;Return Values:      None
;
;Local Variables:    None
;
;Shared Variables:   None
;
;Output:             None
;
;Error Handling:     None
;
;Algorithms:         None
;
;Registers Used:     None
;
;Known Bugs:         None
;
;Limitations:        None
;
;Author:             Timothy Liu
;
;Last Modified       5/19/16

AudioEH        PROC    NEAR
               PUBLIC  AudioEH

AudioEHStart:                            ;save the registers
    PUSH    AX
    PUSH    CX
    CALL    AudioOutput                  ;call function to output audio data

AudioEHSendEOI:
    MOV     DX, INTCtrlrEOI               ;address of interrupt EOI register
    MOV     AX, INT0EOI                   ;INT0 end of interrupt
    OUT     DX, AX                        ;output to peripheral control block

AudioEHDone:                             ;restore registers and return
    POP     CX
    POP     AX
    
    IRET                                 ;IRET from interrupt handlers

AudioEH        ENDP


;Name:               AudioOutput
;
;Description:        This function sends data serially to the MP3 decoder.
;                    The function copies bytes from CurrentBuffer and performs
;                    bit banging to output the bytes. The function transfer
;                    Bytes_Per_Transfer each time the function is called. If
;                    CurBuffLeft is equal to zero, then the function swaps
;                    the NextBuffer into CurrentBuffer and continues playing
;                    from CurrentBuffer. The function also sets the NeedData
;                    flag to indicate that more data is need so that
;                    NextBuffer is filled. The function is called whenever
;                    the MP3 decoder sends a data request interrupt.
;                    If both the current buffer and next buffer are empty, the
;                    function calls Audio_Halt to shut off data request interrupts.
;                    Interrupts are not restored until more data is provided.
; 
;Operation:          The function first checks if CurBuffLeft is equal to
;                    to zero, indicating the current buffer is empty.
;                    If the current data buffer is empty, the function
;                    makes the next buffer the current buffer and sets
;                    NeedData to indicate that a new buffer is needed. If
;                    the next buffer is also empty, then the function 
;                    calls Audio_Halt to turns off ICON0 interrupts and returns.
;                    If there is
;                    data in the current buffer, then the function outputs
;                    BytesPerTransfer bytes starting at CurrentBuffer.
;                    The address pointed to by CurrentBuffer is copied to ES:SI.
;                    AudioOutput copies the byte ES:SI points to
;                    and outputs the bits serially. The first bit (MSB) 
;                    is output to PCS3. After the first bit is output, the
;                    other bits are shifted to DB0 and output to PCS2
;                    until the byte is fully output. The function increments
;                    SI after each byte transfer and outputs BytesPerTransfer
;                    bytes. After the bytes are output, the function
;                    decrements CurBuffLeft by BytesPerTransfer. The function
;                    copies SI to CurrentBuffer[0] to update the offset of
;                    the buffer. The function copies ES to CurrentBuffer[1] to
;                    update the segment. CurrentBuffer points to the next byte
;                    to output The size of the passed buffers MUST be
;                    a multiple of BytesPerTransfer. 
;                    
;
;Arguments:          None
;
;Return Values:      None
;
;Local Variables:    CX - Bytes left to transfer
;                    SI - offset of current buffer pointer
;                    ES - segment of current buffer pointer
;
;Shared Variables:   CurrentBuffer(R/W) - 32 bit address of current data buffer
;                                         being played from
;                    CurBuffLeft(R/W)   - bytes left in the data buffer
;                    NextBuffer(R)      - 32 bit address of next data buffer
;                    NextBuffLeft(R)    - bytes left in next data buffer
;                    NeedData(R/W)      - indicates more data is needed 
;
;Output:             MP3 audio output data output to MP3 decoder through
;                    DB0
;
;Error Handling:     None
;
;Algorithms:         None
;
;Registers Used:     AX, CX - these registers are preserved by event handler
;                    Flag register
;
;Known Bugs:         None
;
;Limitations:        Size of audio data buffers is assumed to be a multiple
;                    of BYTES_PER_TRANSFER
;                    Data buffers are assumed to be entirely in a single segment
;
;Author:             Timothy Liu
;
;Last Modified       5/21/16


;Outline
;AudioOutput()
;    IF    CurBuffLeft = 0:          ;Current buffer going to run out
;        IF NeedData == True:        ;both buffers are empty - panic!
;            ICON0 = ICON0Off        ;shut off the interrupt handler
;            CALL AudioHalt          ;these two are the same things
;        CurrentBuffer = NextBuffer  ;make the next buffer the current buffer
;        CurBufferLeft = NextBuffLeft
;        NeedData = TRUE             ;indicate more data is needed
;    ELSE:                           ;there is enough data
;        For i in BytesPerTransfer   ;loop outputting 32 bytes
;            AL = [CurrentBuffer]    ;load byte to output
;            SHL                         ;put most significant byte in DB0
;            OUT AL, PCS3                ;first bit goes to PCS3
;            For j in LowBits            ;loop outputting other 7 bits
;                                        ;loop will be unrolled for speed
;                SHL                     ;shift to next bit
;                OUT AL, PCS2            ;output the next bit
;            [CurrentBuffer] += 1           ;increment to next byte
;        CurBufferLeft -= BytesPerTransfer ;32 fewer bytes in buffer
        


AudioOutput        PROC    NEAR
                   PUBLIC  AudioOutput

AudioOutputStart:                            ;starting label - save registers
    PUSH    SI
    PUSH    ES

AudioOutputCheckCur:                         ;check if current buffer is empty
    CMP    CurBuffLeft, 0                    ;no bytes left in buffer
    JE     AudioOutputCheckNext              ;check if next buffer is empty
    JMP    AudioOutputByteLoopPrep           ;Current buffer not empty - 
                                             ;output data

AudioOutputCheckNext:
    CMP    NeedData, TRUE                    ;see if next buffer is empty
    JE     AudioOutputEmpty                  ;both buffers are empty
    ;JMP    AudioOutputSwap                  ;make NextBuffer -> CurrentBuffer

AudioOutputSwap:                             ;read from NextBuffer
   MOV    AX, NextBuffer[0]                  ;copy segment of NextBuffer
   MOV    CurrentBuffer[0], AX               ;make NextBuffer CurrentBuffer

   MOV    AX, NextBuffer[1]                  ;copy offset of NextBuffer
   MOV    CurrentBuffer[1], AX

   MOV    AX, NextBuffLeft                   ;copy bytes left of NextBuffer
   MOV    CurBuffLeft, AX                    ;to CurBuffLeft

   MOV    NeedData, TRUE                     ;indicate more data is needed
   JMP    AudioOutputByteLoopPrep            ;prepare to output data

AudioOutputEmpty:                            ;both audio buffers are empty
   CALL   Audio_Halt                         ;switch off audio interrupts
   JMP    AudioOutputDone                    ;can’t output any data

AudioOutputByteLoopPrep:                     ;prepare to output buffer data
    MOV   CX, Bytes_Per_Transfer             ;number bytes left to transfer
                                             ;for this interrupt
    MOV   AX, CurrentBuffer[1]               ;copy buffer segment to ES
    MOV   ES, AX

    MOV   SI, CurrentBuffer[0]               ;copy buffer offset to SI
    ;JMP  AudioOutputLoop                    ;go to loop

AudioOutputLoop:
    CMP   CX, 0                              ;check if no bytes left
    JE    AudioOutputDone                    ;no bytes left - function done
    MOV   AL, ES:[SI]                        ;copy byte to be transferred

AudioOutputSerial:                           ;serially send data to MP3 - MSB
                                             ;first
    XOR   AH, AH                             ;only low byte has valid data
    MOV   DX, PCS3Address                    ;address to output DB7 to
    ROL   AL, 1                              ;output MSB on DB0
    OUT   DX, AX                             ;first bit goes to PCS3 to trigger
                                             ;BSYNC

    MOV   DX, PCS2Address                    ;address to output bits 0-6
    ROL   AL, 1                              ;shift so DB6 is LSB
    OUT   DX, AX                             ;output other bits to PCS2
    
    ROL   AL, 1                              ;shift so DB5 is LSB
    OUT   DX, AX                             ;output other bits to PCS2
    
    ROL   AL, 1                              ;shift so DB4 is LSB
    OUT   DX, AX                             ;output other bits to PCS2
    
    ROL   AL, 1                              ;shift so DB3 is LSB
    OUT   DX, AX                             ;output other bits to PCS2
    
    ROL   AL, 1                              ;shift so DB2 is LSB
    OUT   DX, AX                             ;output other bits to PCS2
    
    ROL   AL, 1                              ;shift so DB1 is LSB
    OUT   DX, AX                             ;output other bits to PCS2
    
    ROL   AL, 1                              ;shift so DB0 is LSB
    OUT   DX, AX                             ;output other bits to PCS2

AudioOutputUpdateByte:
    DEC   CX                                 ;one fewer byte left to transfer
    INC   SI                                 ;update pointer to next byte
    JNC   AudioOutputLoop                    ;SI didn’t overflow - same segment
                                             ;go back to loop
    ;JMP  AudioOutputUpdateSegment           ;SI overflowed - update the segment

AudioOutputUpdateSegment:
    MOV   AX, ES                             ;use accumulator to perform addition
    ADD   AX, Segment_Overlap                ;change segment so ES:SI points to
                                             ;next physical address
    MOV   ES, AX                             ;write new segment back to ES
    JMP   AudioOutputLoop                    ;go back to loop

AudioOutputDone:                             ;stub function for now 
    MOV    CurrentBuffer[0], SI              ;store the buffer location to 
                                             ;start reading from
    MOV    AX, ES                            ;store the updated buffer segment
    MOV    CurrentBuffer[1], AX
    SUB    CurBuffLeft, Bytes_Per_Transfer   ;update number of bytes left in
                                             ;the buffer
    POP    ES
    POP    SI
    RET

AudioOutput    ENDP


;Name:               Audio_Play(unsigned short int far *, int)
;
;Description:        This function is called when the audio output is 
;                    started. This function is passed the address of the
;                    data buffer. The address is stored in CurrentBuffer.
;                    The function multiplies the second argument, the length
;                    of the buffer in words, by WORD_SIZE and moves the 
;                    product to the shared variable
;                    CurBuffLeft. The function then calls AudioIRQON enable
;                    data request interrupts. Finally, the function indicates
;                    that the next buffer is empty and more data is needed.
; 
;Operation:          The function first copies the stack pointer to BP and 
;                    indexes into the stack. The function copies the 32 bit 
;                    address passed as the first argument to CurrentBuffer.
;                    The function indexes into the stack to copy the second
;                    argument into CurBuffLeft, which is the number of words
;                    left in the buffer to play. The function then calls
;                    AudioIRQON to enable data request
;                    interrupts. The function writes TRUE to NeedData to indicate
;                    that the next buffer is empty.
;
;Arguments:          unsigned short int far * - address of data buffer
;                    int - length of buffer in words
;
;Return Values:      None
;
;Local Variables:    None
;
;Shared Variables:   CurrentBuffer(W) - 16 bit address of current data buffer
;                                       being played from
;                    CurBuffLeft(W) -   number of words left in the data buffer
;
;Output:             None
;
;Error Handling:     None
;
;Algorithms:         None
;
;Registers Used:     None
;
;Known Bugs:         None
;
;Limitations:        None
;
;Author:             Timothy Liu
;
;Last Modified       5/21/16

Audio_Play        PROC    NEAR
                  PUBLIC  Audio_Play

AudioPlayStart:                          ;set up BP to index into stack
    PUSH    BP                           ;save base pointer
    MOV     BP, SP                       ;base pointer used to index into stack
    PUSH    AX                           ;save register

AudioPlayArgs:                           ;pull the arguments from the stack
    MOV     AX, SS:[BP+4]                ;buffer offset
    MOV     CurrentBuffer[0], AX         ;write offset to CurrentBuffer

    MOV     AX, SS:[BP+6]                ;buffer segment
    MOV     CurrentBuffer[1], AX         ;write buffer segment to CurrentBuffer

    MOV     AX, SS:[BP+8]                ;length of the buffer in words
    SHL     AX, 1                        ;double to convert to number of bytes
    MOV     CurBuffLeft, AX              ;load number of bytes left

AudioPlayNeedData:                       ;indicate that the next buffer is empty
    MOV     NeedData, TRUE               ;next buffer is empty

AudioPlayIRQON:
    CALL    AudioIRQOn                   ;turn audio data request interrupts on

AudioPlayDone:                           ;restores registers
    POP     AX
    POP     BP
    RET
    

Audio_Play    ENDP


;Name:               Audio_Halt
;
;Description:        This function terminates the output of audio data. The 
;                    function does not return any value.
; 
;Operation:          The function writes the value ICON0OFF to ICON0ADDRESS.
;                    This disables interrupts from INT0 and disables MP3
;                    audio data request interrupts. The function then returns.
;
;Arguments:          None
;
;Return Values:      None
;
;Local Variables:    None
;
;Shared Variables:   None
;
;Output:             None
;
;Error Handling:     None
;
;Algorithms:         None
;
;Registers Used:     None
;
;Known Bugs:         None
;
;Limitations:        None
;
;Author:             Timothy Liu
;
;Last Modified       5/21/16

Audio_Halt        PROC    NEAR
                  PUBLIC  Audio_Halt

AudioHaltStart:                        ;starting label - save registers
    PUSH    AX
    PUSH    DX

AudioHaltWrite:                        ;turn off data request interrupts
    MOV    DX, ICON0Address            ;address of INT0 control register
    MOV    AX, ICON0OFF                ;value to turn off data request IRQ
    OUT    DX, AX                      ;shut off interrupts

AudioHaltDone:                         ;done - restore labels and return
    POP    DX
    POP    AX
    RET


Audio_Halt    ENDP

;Name:               Update
;
;Description:        This function stores the address of a fresh audio buffer
;                    if the secondary audio buffer is empty. The function
;                    returns TRUE if the passed buffer was stored and a new
;                    buffer with more audio data is needed. The function
;                    returns FALSE if more audio data is not needed. The
;                    function is passed the address of the new buffer, and the
;                    length of the new buffer. If the new audio buffer is
;                    stored, then the length of the new audio buffer is 
;                    stored in NextBufferLeft.
; 
;Operation:          The function copies SP to BP and uses the base pointer
;                    to index into the stack. The checks the flag NeedData
;                    to see if more data is needed. If more data is needed,
;                    then the function copies the first argument - the address
;                    of the data buffer - into NextBuffer. Next, 
;                    the function multiplies the second argument (the 
;                    address of the new buffer) by WORD_SIZE and moves the
;                    product into NextBufferLeft, which is the number of 
;                    bytes remaining in NextBuffer. The function then resets
;                    the NeedData flag to FALSE, indicating that
;                    there is data in NextBuffer. If the passed pointer is
;                    used, then the function returns TRUE. If more data is
;                    not needed (NeedData was False) , then the function                  
;                    does nothing but return FALSE. The function calls
;                    AudioIRQOn to turn on INT0 data request interrupts if the
;                    new buffer was used.
;
;Arguments:          unsigned short int far* - address of new audio buffer
;                    int - length of the new buffer in words
;
;Return Values:      TRUE if more data was needed; FALSE otherwise
;
;Local Variables:    None
;
;Shared Variables:   NextBuffer(W) - pointer to second data buffer
;                    NextBufferLen(W) - length of the passed data buffer
;                    NeedData(R/W) - indicates if more data is needed
;
;Output:             None
;
;Error Handling:     None
;
;Algorithms:         None
;
;Registers Used:     AX
;
;Known Bugs:         None
;
;Limitations:        None
;
;Author:             Timothy Liu
;
;Last Modified       5/21/16

Update            PROC    NEAR
                  PUBLIC  Update

UpdateStart:                            ;prepare BP to index into stack
    PUSH    BP                          ;preserve BP
    MOV     BP, SP                      ;use BP as stack index

UpdateCheckNeed:                        ;see if more data is needed
    CMP    NeedData, FALSE              ;
    JE     UpdateNoNeed                 ;next buffer filled - no data needed
    ;JMP   UpdateNextEmpty              ;more data is needed

UpdateNextEmpty:                        ;next buffer is empty
    MOV    AX, SS:[BP+4]                ;offset of the new buffer
    MOV    NextBuffer[0], AX            ;load offset of the new buffer

    MOV    AX, SS:[BP+6]                ;segment of the new buffer
    MOV    NextBuffer[1], AX            ;load the offset of the new buffer

    MOV    AX, SS:[BP+8]                ;length of the new buffer in words
    SHL    AX, 1                        ;double to get length of buffer in bytes
    MOV    NextBuffLeft, AX             ;store the length in bytes

    CALL   AudioIRQOn                   ;turn on data request interrupts
    MOV    AX, TRUE                     ;passed buffer was used
    MOV    NeedData, False              ;NextBuffer is filled - no need for data
    JMP    UpdateDone

UpdateNoNeed:
    MOV    AX, FALSE                    ;not ready for more data

UpdateDone:
    POP    BP
    RET


Update        ENDP

CODE ENDS

;start data segment


DATA    SEGMENT    PUBLIC  'DATA'

CurrentBuffer    DW FAR_SIZE DUP (?)     ;32 bit address of current audio buffer
NextBuffer       DW FAR_SIZE DUP (?)     ;32 bit address of next audio buffer
CurBuffLeft      DW               ?      ;bytes left in current buffer
NextBuffLeft     DW               ?      ;bytes left in next buffer

NeedData         DB               ?      ;flag set when NextBuffer is empty
                                         ;and more data is needed

DATA ENDS

        END