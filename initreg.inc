;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;                                                                            ;
;                                 initreg.INC                                ;
;                       Initialize Registers MP3 Include File                ;
;                                   EE/CS 52                                 ;
;                                                                            ;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

; This file contains the definitions for initcs.asm.
;
; Revision History:
;    11/4/15     Timothy Liu     initial revision
;    11/5/15     Timothy Liu     fixed formatting
;    11/17/15    Timothy Liu     changed to only have values for initcs.asm
;    4/4/16      Timothy Liu     changed name to InitCSM to signify file
;                                is for 80188 MP3 player
;    4/4/16      Timothy Liu     added MMCS, LMCs UMCS reg and val definitions
;                                but no values


; Chip Select Unit Definitions

; Addresses
PACSreg         EQU     0FFA4H          ;address of PACS register
MPCSreg         EQU     0FFA8H          ;address of MPCS register
MMCSreg         EQU
LMCSreg         EQU
UMCSreg         EQU

; Control Register Values
PACSval         EQU     00003H          ;PCS base at 0, 3 wait states
                                        ;0000000000------  starts at address 0
                                        ;----------000---  reserved
                                        ;-------------0--  wait for RDY inputs
                                        ;--------------11  3 wait states
MPCSval         EQU     00183H          ;PCS in I/O space, use PCS5/6, 3 wait states
                                        ;0---------000---  reserved
                                        ;-0000001--------  MCS is 8KB
                                        ;--------1-------  output PCS5/PCS6
                                        ;---------0------  PCS in I/O space
                                        ;-------------0--  wait for RDY inputs
                                        ;--------------11  3 wait states

MMCSval

LMCSval

UMCSval
