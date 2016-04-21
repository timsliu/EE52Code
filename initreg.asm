    NAME  INITREG
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;                                                                            ;
;                             INIT Registers MP3                             ;
;                         Initialize Register Functions                      ;
;                                   EE/CS 52                                 ;
;                                                                            ;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


; Description:  This file contains the functions for initializing the chip 
;               selects and control registers.

; Table of Contents
;
;   InitCS          -Initialize chip select
;   InitCon         -Initialize the control registers


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
;       4/19/16     Timothy Liu     Commented out InitCon - may delete later
;       4/20/16     Timothy Liu     Moved write to LMCS to startup.asm
; local include files

$INCLUDE(INITREG.INC)


CGROUP    GROUP    CODE

CODE SEGMENT PUBLIC 'CODE'

        ASSUME  CS:CGROUP

; external function declarations


;
; InitCS
;
; Description:       Initialize the Peripheral Chip Selects on the 80188.
;
; Operation:         Write the initial values to the PACS and MPCS, MMCS,
;                    LMCS, and UMCS values.
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
; Last Modified:     4/5/16

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


        MOV     DX, UMCSreg     ;setup to write to MPCS register
        MOV     AX, UMCSval
        OUT     DX, AL          ;write MPCSval to MPCS



        RET                     ;done so return


InitCS  ENDP


;
; InitCon
;
; Description:       Initialize the control registers on the 80188.
;
; Operation:         Write the initial values to RELREG (PCB relocation),
;                    RFBASE (refresh base address), RFTIME (refresh clock),
;                    RFCON (Refresh Control), DxCON (DMAControl).
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
; Last Modified:     04/5/16

;InitCS  PROC    NEAR
;        PUBLIC  InitCS
;
;
;        MOV     DX, RELREGreg     ;setup to write to RELREG register
;        MOV     AX, RELREGval
;        OUT     DX, AL            ;write RELREGval to RELREG
;
;        MOV     DX, RFBASEreg     ;setup to write to RFBASE register
;        MOV     AX, RFBASEval
;        OUT     DX, AL            ;write RFBASEval to RFBASE
;
;        MOV     DX, RFTIMEreg     ;setup to write to RFTIME register
;        MOV     AX, RFTIMEval
;        OUT     DX, AL            ;write RFTIMEval to RFTIME
;
;        MOV     DX, RFCONreg      ;setup to write to RFCON register
;        MOV     AX, RFCONval
;        OUT     DX, AL            ;write RFCONval to RFCON
;
;        MOV     DX, DxCONreg      ;setup to write to DxCON register
;        MOV     AX, DxCONval
;        OUT     DX, AL            ;write DxCONval to DxCON
;
;
;
;        RET                     ;done so return
;
;
;InitCS  ENDP


CODE ENDS


        END
