;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;                                                                            ;
;                                  Timer0M.INC                               ;
;                           Timer0 - MP3 includefile                         ;
;                                   EE/CS 52                                 ;
;                                                                            ;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

; This file contains the definitions for timer.asm.
;
; Revision History:
;    11/4/15     Timothy Liu     initial revision
;    04/4/16     Timothy Liu     changed name to Timer0M.INC for MP3 player
;    04/4/16     Timothy Liu     changed COUNTS_PER_MS to 20 mHZ clock value
;    05/6/16     Timothy Liu     corrected COUNTS_PER_MS value for 24 mHz



; Addresses
Tmr0Ctrl        EQU     0FF56H          ;address of Timer 0 Control Register
Tmr0MaxCntA     EQU     0FF52H          ;address of Timer 0 Max Count A Register
Tmr0Count       EQU     0FF50H          ;address of Timer 0 Count Register

; Control Register Values
Tmr0CtrlVal     EQU     1110000000000001B ;Timer 0 Control Register value
                       ;1---------------  enable timer
                       ;-1--------------  write to control
                       ;--1-------------  enable interrupts
                       ;---0000000-0000-  reserved
                       ;----------0-----  read only
                       ;---------------1  continuous mode
; Timing Definitions

COUNTS_PER_MS   EQU     3000            ;number of timer counts per 1 ms
                                        ;(assumes 24 MHz external clock)



