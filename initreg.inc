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
;    4/5/16      Timothy Liu     NOTE: control reg vals/address not added yet
;    4/19/16     Timothy Liu     wrote register addresses and control reg vals
;    5/9/16      Timothy Liu     changed MMCSval start address


; Chip Select Unit Definitions

; Addresses
PACSreg         EQU     0FFA4H          ;address of PACS register
MPCSreg         EQU     0FFA8H          ;address of MPCS register
MMCSreg         EQU     0FFA6H          ;address of MMCS register
LMCSreg         EQU     0FFA2H          ;address of LMCS register
UMCSreg         EQU     0FFA0H          ;address of UMCS register

; Control Register Values
PACSval         EQU     00003H          ;PCS base at 0, 3 wait states
                                        ;0000000000------  starts at address 0
                                        ;----------000---  reserved
                                        ;-------------0--  wait for RDY inputs
                                        ;--------------11  3 wait states
MPCSval         EQU     04083H          ;PCS in I/O space, use PCS5/6, 3 wait states
                                        ;0---------000---  reserved
                                        ;-1000000--------  MCS is 512KB
                                        ;--------1-------  output PCS5/PCS6
                                        ;---------0------  PCS in I/O space
                                        ;-------------0--  wait for RDY inputs
                                        ;--------------11  3 wait states

MMCSval        EQU     08003H           ;MMCS base 80000H, 3 wait states
                                        ;1000000---------  start address
                                        ;-------000000---  reserved
                                        ;-------------0--  enable bus ready
                                        ;--------------11  3 wait states

LMCSval        EQU     007C3H           ;LCS end at 07FFFFH, 3 wait states
                                        ;00--------------  reserved
                                        ;--00011111------  end address
                                        ;----------000---  reserved
                                        ;-------------0--  enable bus ready
                                        ;--------------11  3 wait states

UMCSval        EQU     03003H           ;UCS base at F0000H, 3 wait states
                                        ;00--------------  reserved
                                        ;--11000000------  start address F0000
                                        ;----------000---  reserved
                                        ;-------------0--  enable bus ready
                                        ;--------------11  3 wait states













