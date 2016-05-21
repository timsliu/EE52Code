;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;                                                                            ;
;                                  AUDIO.INC                                 ;
;                             Audio Include File                             ;
;                                   EE/CS 52                                 ;
;                                                                            ;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; Description: This file contains the definitions for audio.asm

Bytes_Per_Transfer        EQU    32      ;can send 32 bytes each DREQ interrupt
                                         ;the number of bytes per data buffer
                                         ;MUST be a multiple of this constant

PCS2Address               EQU    100H    ;address to output DB0-6 of MP3 data
PCS3Address               EQU    180H    ;address to output DB7 of MP3 data
                                         ;DB7 is output first, and PCS3
                                         ;triggers the BSYNC signal