    NAME  CLOCK
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;                                                                            ;
;                                     Clock                                  ;
;                                Clock Functions                             ;
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
;    5/7/16    Tim Liu    wrote InitClock and Elapsed_Time
;
;

; local include files

CGROUP    GROUP    CODE
DGROUP    GROUP    DATA



CODE SEGMENT PUBLIC 'CODE'

        ASSUME  CS:CGROUP
        ASSUME  DS:DGROUP

;external function declarations

; Name:              InitClock
;
;
;Description:        This function initializes the shared variable
;                    NumMs which tracks how many milliseconds have elapsed.
;                    
; 
;Operation:          Reset NumMs to 0 milliseconds elapsed.
;
;Arguments:          None
;
;Return Values:      None
;
;Local Variables:    None
;
;Shared Variables:   NumMs (W) - number of milliseconds that have elapsed
;
;Output:             None
;
;Error Handling:     None
;
;Algorithms:         None
;
;Registers Used:     None
;
;Known Bugs:         None
;
;Limitations:        None
;
;Author:             Timothy Liu
;
;Last Modified       5/7/16

InitClock        PROC    NEAR
                 PUBLIC  InitClock

InitClockStart:                 ;write value to NumMs
    MOV    NumMs, 0

InitClockDone:                  ;end of function
    RET

InitClock    ENDP


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
;Shared Variables:   NumMs (W) - number of milliseconds that have elapsed
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

; Name:              Elapsed_Time
;
;
;Description:        This function returns how many milliseconds have elapsed
;                    since the function was last called. The value maybe 
;                    zero if the function was recently called.
;                    
; 
;Operation:          The function reads the value of NumMs and copies it
;                    to AX. The function then resets NumMs to zero and
;                    returns.
;
;Arguments:          None
;
;Return Values:      AX - milliseconds elapsed since last call
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
;Registers Used:     AX
;
;Known Bugs:         None
;
;Limitations:        None
;
;Author:             Timothy Liu
;
;Last Modified       5/7/16

Elapsed_Time        PROC    NEAR
                    PUBLIC  Elapsed_Time

Elapsed_TimeRead:                       ;copy the value of NumMs
    MOV    AX, NumMs                    ;place NumMs in return register
    MOV    NumMs, 0                     ;reset NumMs to zero

Elapsed_TimeDone:                       ;function done - return
    RET



Elapsed_Time    ENDP


CODE ENDS

DATA    SEGMENT PUBLIC  'DATA'

NumMs          DW    ?     ;number of milliseconds that have elapsed

DATA    ENDS

        END