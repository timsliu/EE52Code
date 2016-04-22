;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;                                                                            ;
;                                   Button.inc                               ;
;                              Button Include File                           ;
;                                   EE/CS 52                                 ;
;                                                                            ;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

; This file contains the definitions for button.asm
;
; Revision History:
;    4/21/16     Timothy Liu     created file - initial revision


DebounceTime         EQU        50       ;miliseconds to debounce the keypad
RepeatRate           EQU        300      ;miliseconds between auto-repeat

NoButtonPressed      EQU   0FFh         ;indicates no button pressed

ButtonAddress        EQU   0            ;starting address of buttons