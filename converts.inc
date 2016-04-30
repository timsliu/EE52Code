;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;                                                                            ;
;                                 CONVERTS.INC                               ;
;                             Conversion Functions                           ;
;                                   EE/CS 51  				       ;
;				    Include File                              ;
;                                                                            ;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


; This file contains the definitions for the conversion file.
;
; Revision History:
;	10/13/15	Timothy Liu	Wrote .inc file	


; ASCII definitions

NULLchar	EQU	0	;an ASCII <NULL> - the end of string character
NEGchar		EQU	45	;an ASCII <negative> symbol
DIGIToffset	EQU	48	;ASCII representation = 48 + digit(0-9)
AFoffset	EQU	55	;ASCII representation = 55 + digit(A-F)