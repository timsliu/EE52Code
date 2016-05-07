    NAME  TIMER0M
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;                                                                            ;
;                                    TIMER0M                                 ;
;                              Timer0 - MP3 Functions                        ;
;                                   EE/CS 51                                 ;
;                                                                            ;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


; Description:  This file contains the functions for initializing the chip 
;               select, clearing the interrupt vector table, installing the
;               event handler, and initializing the timer.

; Table of Contents
;
;   
;   InitTimer0              -start the timer
;   ButtonEH                -event handler for buttons

; Revision History::
;       10/27/15    Tim Liu     initial revision
;       10/28/15    Tim Liu     initdisplay initializes DS
;       10/29/15    Tim Liu     added timer event handler
;       11/3/15     Tim Liu     TimerEventHandler also handles key presses
;       11/5/15     Tim Liu     Changed name of TimerEventHandler to
;                               MuxKeypadEventHandler
;       12/1/15     Tim Liu     Changed all Timer to Timer0
;       12/1/15     Tim Liu     Added IRQ.INC file
;       4/5/16      Tim Liu     Changed name to Timer0M for MP3 player
;       4/21/16     Tim Liu     Changed MuxKeypandEventHandler to ButtonEH
;       5/5/16      Tim Liu     Added call to UpdateClock to ButtonEH


; local include files

$INCLUDE(TIMER0M.INC)
$INCLUDE(MIRQ.INC)
$INCLUDE(GENERAL.INC)


CGROUP    GROUP    CODE



CODE SEGMENT PUBLIC 'CODE'

        ASSUME  CS:CGROUP

; external function declarations

        EXTRN       ButtonDebounce:NEAR      ;scan and check keypad
        EXTRN       UpdateClock:NEAR         ;update clock tracking milliseconds


; InitTimer0
;
; Description:       Initialize the 80188 timer0.  The timer is initialized
;                    to generate interrupts every COUNTS_PER_MS clock cycles.
;                    The interrupt controller is also initialized to allow the
;                    timer interrupts. Timer #0 counts COUNTS_PER_MS.
;
; Operation:         Timer0 is first reset. The appropriate values are written
;                    to the timer control registers in the PCB. Finally, the
;                    interrupt controller is setup to accept timer interrupts
;                    and any pending interrupts are cleared by sending 
;                    a TimerEOI to the interrupt controller.
;                    
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
; Registers Changed: AX, DX
; Stack Depth:       0 words
;
; Author:            Timothy Liu
; Last Modified:     10/27/15

InitTimer0      PROC    NEAR
                PUBLIC  InitTimer0


        MOV     DX, Tmr0Count   ;initialize the count register to 0
        XOR     AX, AX
        OUT     DX, AL

        MOV     DX, Tmr0MaxCntA  ;setup max count for 1ms counts
        MOV     AX, COUNTS_PER_MS
        OUT     DX, AL

        MOV     DX, Tmr0Ctrl    ;setup the control register
        MOV     AX, Tmr0CtrlVal
        OUT     DX, AL


                                ;initialize interrupt controller for timers
        MOV     DX, INTCtrlrCtrl;setup the interrupt control register
        MOV     AX, INTCtrlrCVal
        OUT     DX, AL

        MOV     DX, INTCtrlrEOI ;send a timer EOI (to clear out int. controller)
        MOV     AX, TimerEOI
        OUT     DX, AL


        RET                     ;done so return


InitTimer0       ENDP



; ButtonEH
;
; Description:       This procedure is the event handler for the timer0
;                    interrupt. This function saves the registers and
;                    calls ButtonDebounce. Every call to ButtonDebounce
;                    scans the buttons and checks for a button press.
;                    The procedure also calls UpdateClock, which updates
;                    the number of milliseconds that have elapsed.
;                    The function then pops the stack
;                    and sends an EOI.
;
; Operation:         Save all the registers and call ButtonDebounce to scan 
;                    the 8 UI buttons for key presses. Call UpdateClock
;                    to increment the MP3 timer Send an EOI at the end.
;                    
; Arguments:         None.
; Return Value:      None.
;
; Local Variables:   None
; Shared Variables:  None.
;
; Input:             None.
; Output:            None
;
; Error Handling:    None.
;
; Algorithms:        None.
; Data Structures:   None.
;
; Registers Changed: None
;
; Author:            Timothy Liu
; Last Modified:     5/5/16

ButtonEH                    PROC    NEAR
                            PUBLIC  ButtonEH

        PUSH    AX                      ;save the registers
        PUSH    BX                      ;
        PUSH    DX                      ;
        Call    ButtonDebounce          ;check the keypad
        CALL    UpdateClock             ;increment milliseconds elapsed


EndButtonEH:                            ;done taking care of the timer

        MOV     DX, INTCtrlrEOI         ;send Timer EOI to the INT controller
        MOV     AX, TimerEOI
        OUT     DX, AL

        POP     DX                      ;restore the registers
        POP     BX
        POP     AX


        IRET                            ;and return




ButtonEH       ENDP


CODE ENDS


        END
