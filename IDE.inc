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

IDEStatusSeg    EQU     8000h       ;segment of the IDE status register
IDEStatusOffset EQU     0E00h       ;offset of the IDE status register
IDEStatusMask   EQU     11001000b   ;mask everything but the busy, device
                                    ;ready, and data request flags
IDEReady        EQU     01001000b   ;01001000b
                                    ;0-------   not busy
                                    ;-1------   device ready
                                    ;----1---   data request ready
; DMA control register addresses
D0SRCL          EQU     0FFC0H      ;address of source address pointer low
D0SRCH          EQU     0FFC2H      ;address of source address pointer high
D0DSTL          EQU     0FFC4H      ;address of dest. address pointer low
D0DSTH          EQU     0FFC6H      ;address of dest. address pointer high
D0TC            EQU     0FFC8H      ;address of DMA transfer count register
D0Con           EQU     0FFCAH      ;address of DMA control register

; DMA values
DxConVal1       EQU     00004H      ; value to allow changing start bit
                                    ;0000000000000-00b unimportant
                                    ;0000000000000100b set change start bit
DxConVal2       EQU     0B426H      ; value to write to DxCON to initiate DMA
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
D0TCval         EQU     512         ;number of transfers performed by DMA

