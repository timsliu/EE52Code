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
;    InstallTimer1Handler   -installs the timer1 handler


;Revision History:
;    4/4/16     Tim Liu    initial revision
;    4/20/16    Tim Liu    uncommented InstallTimer0Handler
;    5/7/16     Tim Liu    wrote InstallTimer1Handler

$INCLUDE(MIRQ.INC)

CGROUP    GROUP    CODE

CODE SEGMENT PUBLIC 'CODE'

        ASSUME    CS:CGROUP

; external function declarations

    ;EXTRN    DreqEH             ;VS1011 data request IRQ handler
    ;EXTRN    DemandEH           ;CON_MP3 data demand handler
    EXTRN    ButtonEH:NEAR       ;checks if a button is pressed
    ;EXTRN    RefreshDRAM:NEAR    ;access PCS4 to refresh DRAM

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
;                    function also writes to the ICON0 register to turn
;                    on INT0 interrupts.
;
; Operation:         Writes the address of the data request event handler
;                    to the address of the INT0 interrupt vector. Write
;                    ICON0Value to ICON0Address to turn on INT0 interrupts.
;                    The function then sends an INT0EOI to clear out the 
;                    interrupt controller.
;
; Arguments:         None.
;
; Return Value:      None.
;
; Local Variables:   None.
;
; Shared Variables:  None.
;
; Input:             None.
;
; Output:            None.
;
; Error Handling:    None.
;
; Algorithms:        None.
;
;
; Author:            Timothy Liu
; Last Modified:     4/4/16

;Outline_Dreq()
;    Clear ES
;    Write to interrupt vector table
;    Write to ICON0 register
;    Send EOI. 

;InstallDreqHandler    PROC    NEAR
;                      PUBLIC  InstallDreqHandler

;##### InstallDemandHandler CODE #######


;InstallDemdandHandler    ENDP

; InstallDemandHandler
;
; Description:       This function installs the event handler for the data
;                    demand interrupt from the CON_MP3 decoder. The
;                    function also writes to the ICON1 register to turn
;                    on INT1 interrupts.
;
; Operation:         Writes the address of the data request event handler
;                    to the address of the INT1 interrupt vector. Write
;                    ICON1Value to ICON1Address to turn on INT1 interrupts.
;                    The function then sends an INT1 EOI to clear out the 
;                    interrupt controller.
;
; Arguments:         None.
;
; Return Value:      None.
;
; Local Variables:   None.
;
; Shared Variables:  None.
;
; Input:             None.
;
; Output:            None.
;
; Error Handling:    None.
;
; Algorithms:        None.
;
;
; Author:            Timothy Liu
; Last Modified:     4/4/16

;Outline_Dreq()
;    Clear ES
;    Write to interrupt vector table
;    Write to ICON1 register
;    Send EOI. 

;InstallDemandHandler    PROC    NEAR
;                        PUBLIC  InstallDemandHandler

;##### InstallDemandHandler CODE #######


;InstallDemandHandler    ENDP

; InstallTimer0Handler
;
; Description:       Install the event handler for the timer0 interrupt.
;
; Operation:         Writes the address of the timer event handler to the
;                    appropriate interrupt vector.
;
; Arguments:         None.
; Return Value:      None.
;
; Local Variables:   None.
; Shared Variables:  None.
;
; Input:             None.
; Output:            None.
;
; Error Handling:    None.
;
; Algorithms:        None.
; Data Structures:   None.
;
;
; Author:            Timothy Liu
; Last Modified:     4/4/16

InstallTimer0Handler  PROC    NEAR
                      PUBLIC  InstallTimer0Handler


        XOR     AX, AX          ;clear ES (interrupt vectors are in segment 0)
        MOV     ES, AX
                                ;store the vector - put location of timer event
								;handler into ES
        MOV     ES: WORD PTR (INTERRUPT_SIZE * Tmr0Vec), OFFSET(ButtonEH)
        MOV     ES: WORD PTR (INTERRUPT_SIZE * Tmr0Vec + 2), SEG(ButtonEH)


        RET                     ;all done, return


InstallTimer0Handler  ENDP


; InstallTimer1Handler
;
; Description:       Install the event handler for the timer1 interrupt.
;
; Operation:         Writes the address of the timer event handler to the
;                    appropriate interrupt vector.
;
; Arguments:         None.
; Return Value:      None.
;
; Local Variables:   None.
; Shared Variables:  None.
;
; Input:             None.
; Output:            None.
;
; Error Handling:    None.
;
; Algorithms:        None.
; Data Structures:   None.
;
;
; Author:            Timothy Liu
; Last Modified:     5/7/16

;InstallTimer1Handler  PROC    NEAR
;                      PUBLIC  InstallTimer1Handler


;        XOR     AX, AX          ;clear ES (interrupt vectors are in segment 0)
;        MOV     ES, AX
                                ;store the vector - put location of timer event
								;handler into ES
;        MOV     ES: WORD PTR (INTERRUPT_SIZE * Tmr1Vec), OFFSET(RefreshDRAM)
;        MOV     ES: WORD PTR (INTERRUPT_SIZE * Tmr1Vec + 2), SEG(RefreshDRAM)


;        RET                     ;all done, return


;InstallTimer1Handler  ENDP
    


CODE ENDS

END