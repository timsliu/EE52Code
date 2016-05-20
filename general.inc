;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;                                                                            ;
;                                  General.INC                               ;
;                               General include file                         ;
;                                   EE/CS 51                                 ;
;                                                                            ;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

; This file contains general definitions and constants.
;
; Revision History:
;    11/3/15     Timothy Liu     initial revision
;    11/5/15     Timothy Liu     fixed formatting
;    11/5/15     Timothy Liu     update for HW6 - added timer1vec
;    11/17/15    Timothy Liu     update for HW7 - added Serial_Vector and INT2EOI
;    11/19/15    Timothy Liu     removed interrupt related definitions
;    12/5/15     Timothy Liu     added ASCII definitions



BitsPerNibble        EQU        4         ;number of bits in a nibble

OffSize              EQU        2         ;offset size in bytes

;Ascii definitions
ASCII_CR             EQU       13         ;ASCII carriage return
ASCII_D              EQU       68         ;ASCII code for D
ASCII_E              EQU       69         ;ASCII E
ASCII_F              EQU       70         ;F character for fire (laser on)
ASCII_I              EQU       73         ;I character
ASCII_L              EQU       76         ;L character
ASCII_O              EQU       79         ;O character for off (laser off)
ASCII_P              EQU       80         ;P character
ASCII_R              EQU       82         ;R character
ASCII_S              EQU       83         ;S character
ASCII_T              EQU       84         ;T character
ASCII_NULL           EQU        0         ;ASCII null character
ASCII_SPACE          EQU       32         ;ASCII space
ASCII_COLON          EQU       58         ;ASCII colon
ASCII_RArrow         EQU       62         ;ASCII > symbol

TRUE                 EQU        1         ;true
FALSE                EQU        0         ;false

WORD_SIZE            EQU        2         ;2 bytes per word
FAR_SIZE             EQU        2         ;2 words per far address