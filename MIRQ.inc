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
;    5/19/16    Timothy Liu     added INT0 interrupt definitions


;Interrupt Vector Table
FIRST_RESERVED_VEC           EQU        1	;reserve vectors 1-3
LAST_RESERVED_VEC            EQU	  3
NUM_IRQ_VECTORS              EQU      256  ;number of interrupt vectors
INTERRUPT_SIZE               EQU        4  ;each vector is 4 addresses large

;Addresses
INTCtrlrEOI     EQU     0FF22H          ;address of interrupt controller EOI register
INTCtrlrCtrl    EQU     0FF32H          ;address of interrupt controller for timer

ICON0Address    EQU     0FF38H           ;address of ICON0 register
;ICON1Address                            ;address of ICON1 register

; Register Values
INTCtrlrCVal    EQU     00001H          ;set priority for timers to 1 and enable
                                        ;000000000000----  reserved
                                        ;------------0---  enable timer interrupt
                                        ;-------------001  timer priority


; End of Interrupt values
NonSpecEOI      EQU     08000H          ;Non-specific EOI command
TimerEOI        EQU     00008H          ;Timer EOI command (same for all timers)
INT0EOI         EQU     0000CH          ;INT0 EOI
;INT1EOI

; Interrupt Vector
Tmr0Vec         EQU     8               ;interrupt vector for Timer 0
Tmr1Vec         EQU     18              ;interrupt vector for Timer 1
INT0Vec         EQU     12              ;interrupt vector for INT0
;INT1VEc