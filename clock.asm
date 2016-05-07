    NAME  CLOCK
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;                                                                            ;
;                                     DRAM                                   ;
;                                 DRAM Functions                             ;
;                                   EE/CS 52                                 ;
;                                                                            ;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; 
; Description:   This file contains the functions relating to the MP3 clock.
;

; Table of Contents:
;
;        InitClock             -initialize shared clock variables
;        UpdateClock           -increments milliseconds elapsed
;        Elapsed_Time          -returns milliseconds since last call

; Revision History:
;
;    5/6/16    Tim Liu    initial revision
;
;

; local include files
$INCLUDE(CLOCK.INC)

CGROUP    GROUP    CODE


CODE SEGMENT PUBLIC 'CODE'

        ASSUME  CS:CGROUP 

;external function declarations


; Name:              UpdateClock
;
;
;Description:        This function updates the shared variable NumMS which
;                    tracks the number of milliseconds that have elapsed.
; 
;Operation:          The function increments the value of the shared
;                    variable NumMs.
;
;Arguments:          None
;
;Return Values:      None
;
;Local Variables:    None
;
;Shared Variables:   NumMs (R/W) - number of milliseconds that have elapsed
;
;Output:             None
;
;Error Handling:     None
;
;Algorithms:         None
;
;Registers Used:     None
;
;Known Bugs:         Does not handle NumMs wrapping at maximum value
;
;Limitations:        None
;
;Author:             Timothy Liu
;
;Last Modified       5/6/6

UpdateClock        PROC    NEAR
                   PUBLIC  UpdateClock

UpdateClockInc:                        ;increment NumMs
    INC    NumMs                       ;one more millisecond elapsed

UpdateClockDone:                       ;done - return function
    RET


UpdateClock    ENDP




CODE ENDS

        END