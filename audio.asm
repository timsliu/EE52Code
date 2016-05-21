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
;    
;
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



;Name:               AudioEH
;
;Description:        This function handles audio data request interrupts.
;                    The function is called whenever the VS1011 MP3
;                    decoder needs more data.
; 
;Operation:          The function first saves the registers. It then loads
;                    the constant RefreshRows into BX. RefreshRows is the
;                    number of rows that will be refreshed with each
;                    call to RefreshDRAM. The function then loops
;                    and reads from PCS4Address RefreshRows times. The
;                    function then restores the registers and returns.
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

AudioEHDone:                             ;restore registers and return
    POP     CX
    POP     AX
    
    IRET                                 ;IRET from interrupt handlers

AudioEH        ENDP


;Name:               AudioOutput
;
;Description:        This function sends data serially to the MP3 decoder.
;                    The function copies bytes from CurrentBuffer and performs
;                    bit banging to output the bytes. After each byte is
;                    transferred, the function decrements CurBuffLeft. If
;                    CurBuffLeft is equal to zero, then the function swaps
;                    the NextBuffer into CurrentBuffer and continues playing
;                    from CurrentBuffer. The function also sets the NeedData
;                    flag to indicate that more data is need so that
;                    NextBuffer is filled. The function is called whenever
;                    the MP3 decoder sends a data request interrupt.
; 
;Operation:          The function first checks if CurBuffLeft is equal to
;                    to zero, indicating the current buffer is empty.
;                    If the current data buffer is empty, the function
;                    makes the next buffer the current buffer and sets
;                    NeedData to indicate that a new buffer is needed. If
;                    the next buffer is also empty, then the function 
;                    turns off ICON0 interrupts and returns. If there is
;                    data in the current buffer, then the function outputs
;                    BytesPerTransfer bytes starting at CurrentBuffer.
;                    AudioOutput copies the byte CurrentBuffer points to
;                    and outputs the bits serially. The first bit (MSB) 
;                    is output to PCS3. After the first bit is output, the
;                    other bits are shifted to DB0 and output to PCS2
;                    until the byte is fully output. The function increments
;                    the pointer CurrentBuffer and outputs BytesPerTransfer
;                    bytes. After the bytes are output, the function
;                    decrements CurBuffLeft by BytesPerTransfer. The
;                    size of the passed buffers MUST be a multiple of
;                    BytesPerTransfer.
;                    
;                    
;
;Arguments:          None
;
;Return Values:      None
;
;Local Variables:    CX - Bytes left to transfer
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
;Registers Used:     AX, CX - these registers preserved by event handler
;
;Known Bugs:         None
;
;Limitations:        Size of audio data buffers is assumed to be a multiple
;                    of BYTES_PER_TRANSFER
;                    Data buffers are assumed to be entirely in a single segment
;
;Author:             Timothy Liu

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
    JMP    AudioOutputByteLoopPrep           ;buffers not empty - output data

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
    INC   SI                                 ;update pointer to next byte
    JMP   AudioOutputLoop                    ;prepare to output next byte


AudioOutputDone:                             ;stub function for now 
    MOV    CurrentBuffer[0], SI              ;store the buffer location to 
                                             ;start reading from
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
;                    CurBuffLeft. The function then activates ICON0 to enable
;                    data request interrupts. 
; 
;Operation:          The function first copies the stack pointer to BP and 
;                    indexes into the stack. The function copies the 32 bit 
;                    address passed as the first argument to CurrentBuffer.
;                    The function indexes into the stack to copy the second
;                    argument into CurBuffLeft, which is the number of words
;                    left in the buffer to play. The function then outputs
;                    ICON0ON to ICON0Address to enable data request
;                    interrupts. The function also sends an INT0EOI to the
;                    EOI register and returns.
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


;  #######Audio_Play CODE ########


;Name:               Audio_Halt
;
;Description:        This function terminates the output of audio data. The 
;                    function does not return any value.
; 
;Operation:          The function writes the value ICON0OFF to ICON0ADDRESS.
;                    This disables interrupts from INT0 and disables MP3
;                    audio data request interrupts. The function the returns.
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
;                    then the function multiplies the first argument (the 
;                    address of the new buffer) by WORD_SIZE and moves the
;                    product into NextBufferLeft, which is the number of 
;                    bytes remaining in NextBuffer.
;                    The function resets the NeedData flag
;                    to FALSE, indicating that there is data in both buffers.
;                    If the passed pointer is used, then the function returns
;                    FALSE. If more data is not needed, then the function
;                    does nothing but return FALSE.
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
;Registers Used:     None
;
;Known Bugs:         None
;
;Limitations:        None
;
;Author:             Timothy Liu


;####### UPDATE CODE ###########

CODE ENDS

;start data segment


DATA    SEGMENT    PUBLIC  'DATA'

CurrentBuffer    DW FAR_SIZE DUP (?)     ;32 bit address of current audio buffer
NextBuffer       DW FAR_SIZE DUP (?)     ;32 bit address of next audio buffer
CurBuffLeft      DW               ?      ;bytes left in current buffer
NextBuffLeft     DW               ?      ;bytes left in next buffer

NeedData         DB               ?      ;flag set when CurrentBuffer is empty
                                         ;and more data is needed

DATA ENDS

        END