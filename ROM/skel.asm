        NAME    SKEL

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;                                                                            ;
;                                   C0SMROM                                  ;
;                           Skeleton Startup Template                        ;
;                    Intel C Small Memory Model, ROM Option                  ;
;                                                                            ;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

; This file contains the skeleton startup code for the MP3 player. The code
; Sets up the chip selects and then enters into an infinite loop. The purpose
; of this file is to run the initialization code and then allow functions
; and other code to be tested on its own.
;
;
; Revision History:
;  5/14/16    Tim Liu    initial revision
;    
; local include files

$INCLUDE(INITREG.INC)


; setup code and data groups
CGROUP  GROUP   CODE
DGROUP  GROUP   DATA, STACK



; the actual startup code - should be executed (jumped to) after reset

CODE    SEGMENT  WORD  PUBLIC  'CODE'

; segment register assumptions

        ASSUME  CS:CGROUP, DS:DGROUP, ES:NOTHING, SS:DGROUP


        EXTRN    InitCS:NEAR            ;initialize chip selects


START:


BEGIN:                                  ;start the program
        CLI                             ;disable interrupts
        MOV     AX, DGROUP              ;initialize the stack pointer
        MOV     SS, AX
        MOV     SP, OFFSET(DGROUP:TopOfStack)

        MOV     AX, DGROUP              ;initialize the data segment
        MOV     DS, AX

        ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        ; user initialization code goes here ;
        ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

        MOV     DX, LMCSreg             ;setup to write to MPCS register
        MOV     AX, LMCSval
        OUT     DX, AL                  ;write MPCSval to MPCS

        CALL    InitCS                  ;initialize chip selects


        STI                             ;enable interrupts

Infinite:
        JMP    Infinite

        JMP     Start                   ;if return - reinitialize and try again


CODE    ENDS

; the stack segment - used for subroutine linkage, argument passing, and
; local variables

STACK   SEGMENT  WORD  STACK  'STACK'


        DB      80 DUP ('Stack   ')             ;320 words

TopOfStack      LABEL   WORD


STACK   ENDS

; the data segment - used for static and global variables

DATA    SEGMENT  WORD  PUBLIC  'DATA'


DATA    ENDS




        END START
