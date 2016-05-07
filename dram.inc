;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;                                                                            ;
;                                   DRAM.INC                                 ;
;                             DRAM - MP3 includefile                         ;
;                                   EE/CS 52                                 ;
;                                                                            ;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

; Description:   This function contains the definitions related to DRAM
                 refresh.


PCS4Address        EQU        240H     ;address in PCS4 to trigger
                                       ;DRAM Refresh

RefreshRows        EQU         16      ;number of rows to refresh for 
                                       ;call to RefreshDRAM

