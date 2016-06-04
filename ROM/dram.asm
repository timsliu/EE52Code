    NAME  DRAM
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;                                                                            ;
;                                     DRAM                                   ;
;                                 DRAM Functions                             ;
;                                   EE/CS 52                                 ;
;                                                                            ;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

; Description:   This file contains the functions relating to the DRAM.
;

; Table of Contents:
;
;        RefreshDRAM           -accesses PCS4 to trigger DRAM refresh

; Revision History:
;
;    5/6/16    Tim Liu    initial revision
;
;
; local include files
$INCLUDE(DRAM.INC)

CGROUP    GROUP    CODE


CODE SEGMENT PUBLIC 'CODE'

        ASSUME  CS:CGROUP 

;external function declarations

;Name:               RefreshDRAM
;
;Description:        This function triggers a CAS before RAS refresh. The
;                    function loops and reads PCS4Address RefreshRows
;                    number of times. The function is called by
;                    DRAMRefreshEH at every timer1 interrupt.
; 
;Operation:          The function first saves the registers. It then loads
;                    the constant RefreshRows into BX. RefreshRows is the
;                    number of rows that will be refreshed with each
;                    call to RefreshDRAM. The function then loops
;                    and reads from PCS4Address RefreshRows times. The
;                    function then restores the registers and returns.
;
;Arguments:          None
;
;Return Values:      None
;
;Local Variables:    BX - number of rows left to refresh
;
;Shared Variables:   None
;
;Output:             None
;
;Error Handling:     None
;
;Algorithms:         None
;
;Registers Used:     AX, BX, DX
;
;Known Bugs:         None
;
;Limitations:        None
;
;Author:             Timothy Liu
;
;Last Modified       5/6/16

RefreshDRAM        PROC    NEAR
                   PUBLIC  RefreshDRAM

RefreshDRAMStart:                    ;starting label
                                     ;registers saved by event handler
    MOV     BX, RefreshRows          ;load number of rows left to refresh
    MOV     DX, PCS4Address          ;load address to read from

RefreshDRAMLoop:                     ;loop reading PCS4Address
    CMP    BX, 0                     ;check if no rows left to refresh
    JE     RefreshDRAMDone           ;no rows left - done with function
    IN     AX, DX                    ;read PCS4 to trigger refresh
    DEC    BX                        ;one fewer road left to refresh
    JMP    RefreshDRAMLoop           ;keep looping

RefreshDRAMDone:                     ;done refreshing - registers saved by EH
    RET


RefreshDRAM        ENDP



CODE ENDS

        END