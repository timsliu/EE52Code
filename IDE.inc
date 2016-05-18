;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;                                                                            ;
;                                    IDE.inc                                 ;
;                                IDE Include File                            ;
;                                   EE/CS 52                                 ;
;                                                                            ;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

; This file contains the definitions for the IDE.
;
; Revision History:
;    5/9/16    Tim Liu    created file

IDESegment      EQU     8000h       ;segment of the IDE
IDEStatusOffset  EQU     0E00h       ;AB9:11 = 7
SCOffset      EQU     0400h       ;AB9:11 = 2 sector count offset
LBA70Offset   EQU     0600h       ;AB9:11 = 3 LBA(7:0) offset
LBA158Offset  EQU     0800h       ;AB9:11 = 4 LBA(8:15) offset
LBA2316Offset EQU     0A00h       ;AB9:11 = 5 LBA(16:23) offset
DeLABOffset    EQU     0C00h       ;AB9:11 = 6 device/LBA reg offset
ComOffset EQU     0E00h       ;AB9:11 = 7 command register

SCRdyMask    EQU     10001000b   ;care about BSY and DRQ
SCRdy        EQU     00000000b   ;BSY and DRQ both zero

ComRdyMask   EQU     01000000b   ;care about DRDY flag
ComRdy   EQU     01000000b   ;DRDY must be one to write command

LBARdyMask   EQU     10001000b   ;care about BSY and DRQ
LBARdy       EQU     00000000b   ;BSY and DRQ both zero

DeLBARdyMask   EQU    10001000b   ;care about BSY and DRQ
DeLBARdy       EQU    00000000b   ;BSY and DRQ both zero

IDETransferMask  EQU    00001000b   ;care about DRQ
IDETransfer      EQU    00001000b   ;DRQ must be 1 to transfer data 

DeLBAMask    EQU      11100000b   ;value ORd with get_blocks argument
                                    ;and written to LBA device register
                                    ;11100000b
                                    ;111-----   specify LBA addressing
                                    ;---0----   device select 0
                                    ;----XXXX   LBA (27:31)

 

ReadSector    EQU    20h         ;IDE Read Sector command 



; DMA control register addresses
D0SRCL          EQU     0FFC0H      ;address of source address pointer low
D0SRCH          EQU     0FFC2H      ;address of source address pointer high
D0DSTL          EQU     0FFC4H      ;address of dest. address pointer low
D0DSTH          EQU     0FFC6H      ;address of dest. address pointer high
D0TC            EQU     0FFC8H      ;address of DMA transfer count register
D0Con           EQU     0FFCAH      ;address of DMA control register

; DMA values

DxConVal        EQU     0B426H      ; value to write to DxCON to initiate DMA
                                    ;1011010000100110b
                                    ;1---------------  destination in memory
                                    ;-0--------------  don’t decrement dest.
                                    ;--1-------------  increment dest. pointer
                                    ;---1------------  source in memory space
                                    ;----0-----------  don’t decrement source
                                    ;-----1----------  increment source ptr.
                                    ;------0---------  terminal count - ignored
                                    ;-------0--------  no interrupt request
                                    ;--------00------  unsynchronized transfer
                                    ;----------1-----  high priority
                                    ;-----------0----  external DMA
                                    ;------------0---  reserved
                                    ;-------------1--  enable changing start bit
                                    ;--------------1-  arm DMA channel
                                    ;---------------0  perform byte transfers
DxSRCHVal       EQU     0CH         ;bits 16:19 of DMA source
DxSRCLVal       EQU     0H          ;bits 0:15 DMA source
                                    ;AB9-11 must be zero for data register
NumTransfer     EQU     512         ;number of transfers performed by DMA

SecPerTran      EQU     1           ;write 1 sector per IDE transfer

NumIDERegisters EQU     6           ;6 IDE registers to write to 
BlankMask       EQU     0           ;value OR’d with to not change bits
NoConstant      EQU     0           ;no constant value to output

;base pointer offsets
LBA07          EQU     4           ;base pointer offset for LBA0:7 register
LBA815         EQU     5           ;base pointer offset for LBA8:15 register
LBA2316        EQU     6           ;base pointer offset for LBA16:23 register
DeLBA          EQU     7           ;base pointer offset for Device LBA register
NoStackArg      EQU     0           ;constant indicating reg value is not
                                    ;a stack argument


IDERegEntry    STRUC
    FlagMask    DB        ?         ;mask applied to status register
    IDEReady    DB        ?         ;bit pattern indicating IDE ready
    RegOffset   DW        ?         ;offset of IDE register
    BPIndex     DW        ?         ;value to add to base pointer for stack
                                    ;address
    ConstComm   DB        ?         ;constant command to be written
    ArgMask     DB        ?         ;mask to apply to argument pulled from stack
IDERegEntry    ENDS
