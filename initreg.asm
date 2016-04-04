    NAME  INITREG
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;                                                                            ;
;                             INIT Registers MP3                             ;
;                             Initial Functions                              ;
;                                   EE/CS 52                                 ;
;                                                                            ;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


; Description:  This file contains the functions for initializing the chip 
;               select.

; Table of Contents
;
;   InitCS          -Initialize chip select


; Revision History::
;       10/27/15    Timothy Liu     initial revision
;       10/28/15    Timothy Liu     initdisplay initializes DS
;       10/29/15    Timothy Liu     added timer event handler
;       11/3/15     Timothy Liu     TimerEventHandler also handles key presses
;       11/4/15     Timothy Liu     Removed functions related to timers
;       11/11/15    Timothy Liu     Removed function not related to chip select
;       4/4/16      Timothy Liu     Changed name to InitCSM to distinguish from
;                                   InitCS for EE51
;       4/4/16      Timothy Liu     Added writing to UMCS, LMCS, and MMCS
;       4/4/16      Timothy Liu     Removed GENERAL.INC and changed INITCS.INC
;                                   to INITCSM.INC
;       4/4/16      Timothy Liu     Changed name to InitReg (init registers)
; local include files

$INCLUDE(INITREG.INC)


CGROUP    GROUP    CODE

CODE SEGMENT PUBLIC 'CODE'

        ASSUME  CS:CGROUP

; external function declarations
;
;
;
; InitCS
;
; Description:       Initialize the Peripheral Chip Selects on the 80188.
;
; Operation:         Write the initial values to the PACS and MPCS registers.
;
; Arguments:         None.
; Return Value:      None.
;
; Local Variables:   None.
; Shared Variables:  None.
; Global Variables:  None.
;
; Input:             None.
; Output:            None.
;
; Error Handling:    None.
;
; Algorithms:        None.
; Data Structures:   None.
;
; Registers Changed: AX, DX
;
; Author:            Timothy Liu
; Last Modified:     10/27/15

InitCS  PROC    NEAR
        PUBLIC  InitCS


        MOV     DX, PACSreg     ;setup to write to PACS register
        MOV     AX, PACSval
        OUT     DX, AL          ;write PACSval to PACS

        MOV     DX, MPCSreg     ;setup to write to MPCS register
        MOV     AX, MPCSval
        OUT     DX, AL          ;write MPCSval to MPCS

        MOV     DX, MMCSreg     ;setup to write to MPCS register
        MOV     AX, MMCSval
        OUT     DX, AL          ;write MPCSval to MPCS

        MOV     DX, LMCSreg     ;setup to write to MPCS register
        MOV     AX, LMCSval
        OUT     DX, AL          ;write MPCSval to MPCS

        MOV     DX, UMCSreg     ;setup to write to MPCS register
        MOV     AX, UMCSval
        OUT     DX, AL          ;write MPCSval to MPCS



        RET                     ;done so return


InitCS  ENDP


CODE ENDS


        END
