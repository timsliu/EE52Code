;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;                                                                            ;
;                                   Timer1.inc                               ;
;                              Tiimer1 Include File                          ;
;                                   EE/CS 51                                 ;
;                                                                            ;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

; This file contains the definitions for timer1.asm.
;
; Revision History:
;    11/8/15    Timothy Liu    initial revision



;Addresses
Timer1Ctrl        EQU    0FF5EH    ;address of Timer 1 Control Register
Timer1MaxCntA     EQU    0FF5AH    ;address of Timer 1 Max Count A Register
Timer1Count       EQU    0FF58H    ;address of Timer 1 count register

;Control Register Values

Timer1CtrlVal     EQU    1110000000000001B  ;Timer 1 Control Register value
                        ;1---------------  enable timer
                        ;-1--------------  write to control
                        ;--1-------------  enable interrupts
                        ;---0------------  use Maxcount Compare A
                        ;----000000––————  reserved
                        ;----------0-----  read only
                        ;-----------0----  disable retriggering
                        ;------------0---  prescaler not used
                        ;-------------0--  use internal clock
                        ;--------------0-  single max count mode
                        ;---------------1  continuous mode

;Timing Definitions

COUNTS_PER_FMS    EQU   1000   ;counts for every 0.4 milliseconds (2.5khz)
                               ;RefreshRows(DRAM.INC) are refreshed each
                               ;timer1 interrupt. 256 rows must be refreshed
                               ;every 4 milliseconds for the MH256 DRAM