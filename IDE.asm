    NAME    IDE
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;                                                                            ;
;                                   IDE Code                                 ;
;                             IDE Related Functions                          ;
;                                   EE/CS 52                                 ;
;                                                                            ;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

; Description: This file contains the functions relating to the IDE software.


; Table of Contents:
;
;    Get_Blocks - retrieves number of blocks from IDE

; Revision History:
;    5/8/16    Tim Liu    Created file
;    5/9/16    Tim Liu    Created skeleton of Get_blocks


; local include files
$(IDE.INC)

CGROUP    GROUP    CODE


CODE SEGMENT PUBLIC 'CODE'

        ASSUME  CS:CGROUP 

;external function declarations

;Name:               Get_Blocks
;
;Description:        None
; 
;Operation:          None
;
;Arguments:          None
;
;Return Values:      None
;
;Local Variables:    None
;
;Shared Variables:   None
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
;Last Modified       5/9/16


CODE ENDS

        END