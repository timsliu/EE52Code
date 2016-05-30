    NAME    AUDIOS
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;                                                                            ;
;                               AUDIO Stub Code                              ;
;                              Audio Test Functions                          ;
;                                   EE/CS 52                                 ;
;                                                                            ;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

; Description: This files contains a reduced version of AudioEH that plays
;              from a fixed buffer.


; Table of Contents
;
;    AudioEHM     - event handler for audio data request interrupts
;    AudioInit    - initializes the audio buffer
;    AudioOutputM - repeatedly plays a fixed buffer


; Revision History:

;   5/28/16    Tim Liu    created file
;   5/29/16    Tim Liu    fixed some bugs in AudioOutput
;   5/29/16    Tim Liu    changed outputting words to output bytes
;   5/29/16    Tim Liu    changed incrementing SI to adding 1 to set carry
;   5/29/16    Tim Liu    fixed AudioOutput buffer indexes
;
;
; local include files
;
$INCLUDE(AUDIOS.INC)
$INCLUDE(MIRQ.INC)
$INCLUDE(GENERAL.INC)



CGROUP    GROUP    CODE
DGROUP    GROUP    DATA


CODE SEGMENT PUBLIC 'CODE'

        ASSUME  CS:CGROUP, DS:DGROUP 

;Name:               AudioInit
;
;Description:        This function writes the starting value of the fixed
;                    audio buffer to CurrentBuffer.
; 
;Operation:          The function looks up the values of the segment and
;                    offset of the fixed buffer.
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

AudioInit        PROC    NEAR
                 PUBLIC  AudioInit
AudioInitStart:
    PUSH    AX

AudioInitWrite:
    MOV    AX, AudioBufferOffset
    MOV    CurrentBuffer[0], AX

    MOV    AX, AudioBufferSegment
    MOV    CurrentBuffer[2], AX

    MOV    CurBuffLeft, AudioBufferLength

AudioInitDone:
    POP    AX
    RET



AudioInit    ENDP


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
    PUSH    DX
    CALL    AudioOutput                  ;call function to output audio data

AudioEHSendEOI:
    MOV     DX, INTCtrlrEOI               ;address of interrupt EOI register
    MOV     AX, INT0EOI                   ;INT0 end of interrupt
    OUT     DX, AX                        ;output to peripheral control block

AudioEHDone:                             ;restore registers and return
    POP     DX
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



        


AudioOutput        PROC    NEAR
                   PUBLIC  AudioOutput

AudioOutputStart:                            ;starting label - save registers
    PUSH    SI
    PUSH    ES

AudioOutputCheckCur:                         ;check if current buffer is empty
    CMP    CurBuffLeft, 0                    ;no bytes left in buffer
    JE     AudioOutputResetLoop              ;check if next buffer is empty
    JMP    AudioOutputByteLoopPrep           ;Current buffer not empty - 
                                             ;output data

AudioOutputResetLoop:
    MOV    AX, AudioBufferOffset
    MOV    CurrentBuffer[0], AX

    MOV    AX, AudioBufferSegment
    MOV    CurrentBuffer[2], AX

    MOV    CurBuffLeft, AudioBufferLength    


AudioOutputByteLoopPrep:                     ;prepare to output buffer data
    MOV   CX, Bytes_Per_Transfer             ;number bytes left to transfer
                                             ;for this interrupt
    MOV   AX, CurrentBuffer[2]               ;copy buffer segment to ES
    MOV   ES, AX

    MOV   SI, CurrentBuffer[0]               ;copy buffer offset to SI
    ;JMP  AudioOutputLoop                    ;go to loop

AudioOutputLoop:
    CMP   CX, 0                              ;check if no bytes left
    JE    AudioOutputDone                    ;no bytes left - function done
    MOV   AL, ES:[SI]                        ;copy byte to be transferred

AudioOutputSerial:                           ;serially send data to MP3 - MSB
                                             ;first
    MOV   DX, PCS3Address                    ;address to output DB7 to
    ROL   AL, 1                              ;output MSB on DB0
    OUT   DX, AL                             ;first bit goes to PCS3 to trigger
                                             ;BSYNC

    MOV   DX, PCS2Address                    ;address to output bits 0-6
    ROL   AL, 1                              ;shift so DB6 is LSB
    OUT   DX, AL                             ;output other bits to PCS2
    
    ROL   AL, 1                              ;shift so DB5 is LSB
    OUT   DX, AL                             ;output other bits to PCS2
    
    ROL   AL, 1                              ;shift so DB4 is LSB
    OUT   DX, AL                             ;output other bits to PCS2
    
    ROL   AL, 1                              ;shift so DB3 is LSB
    OUT   DX, AL                             ;output other bits to PCS2
    
    ROL   AL, 1                              ;shift so DB2 is LSB
    OUT   DX, AL                             ;output other bits to PCS2
    
    ROL   AL, 1                              ;shift so DB1 is LSB
    OUT   DX, AL                             ;output other bits to PCS2
    
    ROL   AL, 1                              ;shift so DB0 is LSB
    OUT   DX, AL                             ;output other bits to PCS2

AudioOutputUpdateByte:
    DEC   CX                                 ;one fewer byte left to transfer
    ADD   SI, 1                              ;update pointer to next byte
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
    MOV    CurrentBuffer[2], AX
    SUB    CurBuffLeft, Bytes_Per_Transfer   ;update number of bytes left in
                                             ;the buffer
    POP    ES
    POP    SI
    RET

AudioOutput    ENDP


CODE ENDS

;start data segment


DATA    SEGMENT    PUBLIC  'DATA'

CurrentBuffer    DW FAR_SIZE DUP (?)     ;32 bit address of current audio buffer
CurBuffLeft      DW               ?      ;bytes left in current buffer


DATA ENDS

        END