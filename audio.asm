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
;    
;
;
; local include files
$INCLUDE(AUDIO.INC)

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
    PUSH    BX
    PUSH    DX
    CALL    AudioOutput                  ;call function to output audio data

AudioEHDone:                             ;restore registers and return
    POP     DX
    POP     BX
    POP     AX
    
    IRET                                 ;IRET from interrupt handlers

AudioEH        ENDP


;Name:               AudioOutput
;
;Description:        None
; 
;Operation:          None
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



;####### AudioOutput CODE ###########



;Name:               Audio_Play(unsigned short int far *, int)
;
;Description:        This function is called when the audio output is 
;                    started. This function is passed the address of the
;                    data buffer. The address is stored in CurrentBuffer.
;                    The function copies the second argument, the length
;                    of the buffer in words, to the shared variable
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
;Description:        None
; 
;Operation:          None
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



;####### AUDIO_HALT ##########


;Name:               Update
;
;Description:        None
; 
;Operation:          None
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


;####### UPDATE CODE ###########

CODE ENDS

;start data segment


DATA    SEGMENT    PUBLIC  'DATA'

CurrentBuffer    DW FAR_SIZE DUP (?)     ;buffer holds current buffer address
CurBuffLeft      DW      ?               ;words left in current buffer

DATA ENDS

        END