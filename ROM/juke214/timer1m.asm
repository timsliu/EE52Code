    NAME  TIMER1M
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;                                                                            ;
;                                     TIMER1                                 ;
;                               Timer1 Functions                             ;
;                                   EE/CS 52                                 ;
;                                                                            ;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

; Description:    This file contains the functions for initializing timer1
;                 and for handling timer1 interrupts.

;Table Contents
;
;    InitTimer1              -start timer1
;    DRAMRefreshEH           -calls function to refresh DRAM

; Revision History:
;    11/8/15    Timothy Liu    wrote description and table of contents
;    11/9/15    Timothy Liu    wrote timer1 init and timer1 event handler
;    11/11/15   Timothy Liu    changed ASSUME CS:CODE to ASSUME CS:CGROUP
;    5/6/16     Tim Liu        changed name to timer1m (mp3)
;
;
;
; local include files
$INCLUDE(TIMER1M.INC)
$INCLUDE(GENERAL.INC)
$INCLUDE(MIRQ.INC)

CGROUP    GROUP    CODE


CODE SEGMENT PUBLIC 'CODE'

        ASSUME  CS:CGROUP 

;external function declarations

        EXTRN    ControlDCMotors:NEAR         ;procedure to run motors


;Name:               InitTimer1
;
;Description:        Initialize the 80188 timer1.  The timer is initialized
;                    to generate interrupts every COUNTS_PER_QMS clock cycles.
;                    The interrupt controller is also initialized to allow the
;                    timer interrupts. This clock is used by the DC motors for
;                    pulsed width modulation.
; 
;Operation:          Timer1 is first reset. The appropriate values are written
;                    to the timer control registers in the PCB. Finally, the
;                    interrupt controller is setup to accept timer interrupts
;                    and any pending interrupts are cleared by sending 
;                    a TimerEOI to the interrupt controller.
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
;Registers Used:     AX, DX
;
;Known Bugs:         None
;
;Limitations:        None
;
;Author:             Timothy Liu
;Last Modified       11/11/15

InitTimer1       PROC    NEAR
                 PUBLIC  InitTimer1


        MOV     DX, Timer1Count      ;initialize the count register to 0
        XOR     AX, AX
        OUT     DX, AL

        MOV     DX, Timer1MaxCntA    ;setup max count for 1ms counts
        MOV     AX, COUNTS_PER_QMS
        OUT     DX, AL

        MOV     DX, Timer1Ctrl       ;setup the control register
        MOV     AX, Timer1CtrlVal
        OUT     DX, AL


                                ;initialize interrupt controller for timers
        MOV     DX, INTCtrlrCtrl;setup the interrupt control register
        MOV     AX, INTCtrlrCVal
        OUT     DX, AL

        MOV     DX, INTCtrlrEOI ;send a timer EOI (to clear out int. controller)
        MOV     AX, TimerEOI
        OUT     DX, AL


        RET                     ;done so return


InitTimer1       ENDP



;Name:	             DRAMRefreshEH
;		
;Description:       This procedure handles interrupt events from timer1.
;                   The function saves  the registers and calls
;                   RefreshDRAM. Each time RefreshDRAM is
;                   called, an address in PCS4 is read, triggering a
;                   refresh. RefreshDRAM refreshes 8 rows each time it is
;                   called.
;
;Operation:         Save all the registers and call RefreshDRAM. Send an
;                   EOI and restore registers.
;
;Arguments:         None
;
;Return Values:     None
;
;Local Variables:   None
;
;Shared Variables:  None
;
;Output:            None
;
;Error Handling:    None
;
;Algorithms:        None
;
;Registers Used:    None
;
;Known Bugs:        None
;
;Limitations:       None
;Author:            Timothy Liu
;Last Modified      5/6/16

DRAMRefreshEH    PROC    NEAR
                 PUBLIC  DRAMRefreshEH

DRAMRefreshEHStart:
						   
        PUSHA                           ;save the registers
        CALL    RefreshDRAM             ;function to refresh DRAM


DramRefreshEHDone:                      ;done taking care of the timer

        MOV     DX, INTCtrlrEOI         ;send Timer EOI to the INT controller
        MOV     AX, TimerEOI
        OUT     DX, AL

        POPA                            ;restore the registers


        IRET                            ;and return - IRET from event handler
		
DRAMRefreshEH    ENDP

CODE ENDS

       END
