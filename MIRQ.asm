    NAME    MIRQ
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;                                                                            ;
;                                MP3 Interrupts                              ;
;                           MP3 Interrupt Functions                          ;
;                                   EE/CS 51                                 ;
;                                                                            ;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

; Description:  This files contains the functions relating to interrupts
;               for the MP3 player. The functions clear the interrupt
;               vector table, installs the hardware and timer interrupts,
;               and installs a handler for illegal interrupts.
;

; Table of Contents
;
;    ClrIRQVectors          -clear the interrupt vector table
;    IllegalEventHandler    -takes care of illegal events
;    InstallDreqHandler     -installs VS1011 data request IRQ handler
;    InstallDemandHandler   -installs CON_MP3 data demand IRQ handler
;    InstallTimer0Handler   -installs the timer0 handler


Revisioin History:
;    4/4/16

$INCLUDE(MIRQ.INC)

CGROUP    GROUP    CODE

CODE SEGMENT  PUBLIC ‘CODE’

        ASSUME    CS:GROUP

; external function declarations

    ;EXTRN    DreqEH             ;VS1011 data request IRQ handler
    ;EXTRN    DemandEH           ;CON_MP3 data demand handler
    ;EXTRN    ButtonEH           ;checks if a button is pressed

; ClrIRQVectors
;
; Description:      This functions installs the IllegalEventHandler for all
;                   interrupt vectors in the interrupt vector table.  Note
;                   that all 256 vectors are initialized so the code must be
;                   located above 400H.  The initialization skips  (does not
;                   initialize vectors) from vectors FIRST_RESERVED_VEC to
;                   LAST_RESERVED_VEC.
;
; Arguments:        None.
; Return Value:     None.
;
; Local Variables:  CX    - vector counter.
;                   ES:SI - pointer to vector table.
; Shared Variables: None.
; Global Variables: None.
;
; Input:            None.
; Output:           None.
;
; Error Handling:   None.
;
; Algorithms:       None.
; Data Structures:  None.
;
; Registers Used:   flags, AX, CX, SI, ES
; Stack Depth:      0 word
;
; Author:           Timothy Liu
; Last Modified:    10/27/15

ClrIRQVectors   PROC    NEAR
                PUBLIC  ClrIRQVectors


InitClrVectorLoop:              ;setup to store the same handler 256 times

        XOR     AX, AX          ;clear ES (interrupt vectors are in segment 0)
        MOV     ES, AX
        MOV     SI, 0           ;initialize SI to the first vector

        MOV     CX, NUM_IRQ_VECTORS      ;maximum number to initialize


ClrVectorLoop:                  ;loop clearing each vector
                                ;check if should store the vector
        CMP     SI, INTERRUPT_SIZE * FIRST_RESERVED_VEC
        JB      DoStore         ;if before start of reserved field - store it
        CMP     SI, INTERRUPT_SIZE * LAST_RESERVED_VEC
        JBE     DoneStore       ;if in the reserved vectors - don't store it
        ;JA     DoStore         ;otherwise past them - so do the store

DoStore:                        ;store the vector
        MOV     ES: WORD PTR [SI], OFFSET(IllegalEventHandler)
        MOV     ES: WORD PTR [SI + OffSize], SEG(IllegalEventHandler)

DoneStore:                      ;done storing the vector
        ADD     SI, INTERRUPT_SIZE           ;update pointer to next vector

        LOOP    ClrVectorLoop   ;loop until have cleared all vectors
        ;JMP    EndClrIRQVectors;and all done


EndClrIRQVectors:               ;all done, return
        RET


ClrIRQVectors   ENDP



; IllegalEventHandler
;
; Description:       This procedure is the event handler for illegal
;                    (uninitialized) interrupts.  It does nothing - it just
;                    returns after sending a non-specific EOI.
;
; Operation:         Send a non-specific EOI and return.
;
; Arguments:         None.
; Return Value:      None.
;
; Local Variables:   None.
; Shared Variables:  None.
; Global Variables:  None.
;
; Input:             None.
; Output:            None.
;
; Error Handling:    None.
;
; Algorithms:        None.
; Data Structures:   None.
;
; Registers Changed: None
; Stack Depth:       2 words
;
; Author:            Timothy Liu
; Last Modified:     10/27/15

IllegalEventHandler     PROC    NEAR
                        PUBLIC  IllegalEventHandler

        NOP                             ;do nothing (can set breakpoint here)

        PUSH    AX                      ;save the registers
        PUSH    DX

        MOV     DX, INTCtrlrEOI         ;send a non-sepecific EOI to the
        MOV     AX, NonSpecEOI          ;   interrupt controller to clear out
        OUT     DX, AL                  ;   the interrupt that got us here

        POP     DX                      ;restore the registers
        POP     AX

        IRET                            ;and return


IllegalEventHandler     ENDP

; InstallDreqHandler
;
; Description:       This function installs the event handler for the data
;                    request interrupt from the VS1011 MP3 decoder. The
;                    function
;
; Operation:         None
;
; Arguments:         None.
; Return Value:      None.
;
; Local Variables:   None.
; Shared Variables:  None.
; Global Variables:  None.
;
; Input:             None.
; Output:            None.
;
; Error Handling:    None.
;
; Algorithms:        None.
; Data Structures:   None.
;
; Registers Changed: None
;
; Author:            Timothy Liu
; Last Modified:     4/4/16

InstallDreqHandler    PROC    NEAR
                      PUBLIC  InstallDreqHandler

##### InstallDreqHandler CODE #######


InstallDreqHandler    ENDP
    


CODE ENDS

END