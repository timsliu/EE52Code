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

IDEStatusReg    EQU              ;address of the IDE status register
IDEDMA      EQU              ;IDE command to initiate DRAM

; DMA control register addresses
DxDSTH          EQU              ;address of destination address pointer high
DxDSTL          EQU              ;address of destination address pointer low
DxSRCL          EQU              ;address of source address pointer low
DxCon           EQU              ;address of DMA control register

; DMA values
DxConVal        EQU              ;value to write to DxCON to initiate DMA
DxSRCHVal       EQU              ;high byte of DMA source
DxSRCLVal       EQU              ;low byte of DMA source

