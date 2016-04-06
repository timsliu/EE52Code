;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;                                                                            ;
;                                   MIRQ.INC                                 ;
;                          MP3 Interrupt Include File                        ;
;                                   EE/CS 52                                 ;
;                                                                            ;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

; This file contains the definitions for initcs.asm.
;
; Revision History:
;    4/4/16     Timothy Liu     created file and wrote definitions w/o values


;Interrupt Vector Table
FIRST_RESERVED_VEC           EQU        1	;reserve vectors 1-3
LAST_RESERVED_VEC            EQU	  3
NUM_IRQ_VECTORS              EQU      256  ;number of interrupt vectors
INTERRUPT_SIZE               EQU        4  ;each vector is 4 addresses large

;Addresses
INTCtrlrEOI     EQU     0FF22H          ;address of interrupt controller EOI register
ICON0Address                            ;address of ICON0 register
ICON1Address                            ;address of ICON1 register


; End of Interrupt values
NonSpecEOI      EQU     08000H          ;Non-specific EOI command
TimerEOI        EQU     00008H          ;Timer EOI command (same for all timers)
INT0EOI
INT1EOI

; Interrupt Vector
Tmr0Vec         EQU     8               ;interrupt vector for Timer 0
INT0Vec
INT1VEc