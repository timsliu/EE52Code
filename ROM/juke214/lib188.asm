        NAME  LIB188

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;                                                                            ;
;                                    LIB188                                  ;
;                          Library Routines for 80188                        ;
;                              MP3 Jukebox Project                           ;
;                                   EE/CS 52                                 ;
;                                                                            ;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

; This file contains a number of functions needed in the 80188 MP3 Jukebox
; project.  The public functions included are:
;    abs_    - find the absolute value of the passed integer
;    strcat_ - concatenate second passed string to first passed string
;    strcpy_ - copy second passed string to first passed string
;    strlen_ - return the length of the passed string
;
; The local functions included are:
;    none
;
; Revision History:
;     6/16/05  Glen George              initial revision
;     6/4/06   Glen George              fixed bug in strlen_, it wasn't
;                                          updating the string position




; local include files
;    none




CODE    SEGMENT  PUBLIC  'CODE'
CGROUP  GROUP  CODE

        ASSUME  CS:CGROUP




; abs_
;
; Description:       This function computes the absolute value of the passed
;                    integer.
;
; Operation:         If the passed integer is negative, it is negated, if it
;                    is positive it is left unchanged.
;
; Arguments:         [SP + 2] (int) - value to find the absolute value of.
; Return Value:      AX - absolute value of the passed integer.
;
; Local Variables:   BP - frame pointer.
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
; Registers Changed: flags.
; Stack Depth:       1 word
;
; Author:            Glen George
; Last Modified:     June 16, 2005

abs_    PROC    NEAR
        PUBLIC  abs_


arg     EQU     WORD PTR [BP + 4]       ;argument for the function


absStart:
        PUSH    BP                      ;setup the stack frame pointer
        MOV     BP, SP
        ;JMP    absCompute              ;now compute the absolute value


absCompute:
        MOV     AX, arg                 ;get the passed argument
        OR      AX, AX                  ;set flags
        JNS     absEnd                  ;if positive - have abs - done
        NEG     AX                      ;otherwise negate to get abs
        ;JMP    absEnd                  ;and done now


absEnd:                                 ;done computing absolute value
        POP     BP                      ;restore BP and return
        RET


abs_    ENDP




; strcat_
;
; Description:       This function concatenates the second passed string to
;                    the first passed string.  The strings are passed as far
;                    pointers (segment and offset).
;
; Operation:         The end of the first string is found.  Then the second
;                    string is copied character by character to the end of the
;                    first string.
;
; Arguments:         [SP + 2] (char far *) - pointer to destination string
;                                            (string to concatenate to).
;                    [SP + 6] (char far *) - pointer to source string (string
;                                            to concatenate).
; Return Value:      DX | AX - pointer to the passed destination string.
;
; Local Variables:   BP - frame pointer
;                    ES - source and destination segment.
;                    BX - destination offset.
;                    SI - source offset.
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
; Registers Changed: flags, ES, BX.
; Stack Depth:       2 words
;
; Author:            Glen George
; Last Modified:     June 16, 2005

strcat_ PROC    NEAR
        PUBLIC  strcat_


dest    EQU     DWORD PTR [BP + 4]      ;destination
destOff EQU     WORD PTR [BP + 4]       ;destination offset
destSeg EQU     WORD PTR [BP + 6]       ;destination segment
src     EQU     DWORD PTR [BP + 8]      ;source
srcOff  EQU     WORD PTR [BP + 8]       ;source offset
srcSeg  EQU     WORD PTR [BP + 10]      ;source segment
NULL    EQU     0                       ;NULL character (end of string)


strcatStart:
        PUSH    BP                      ;setup the stack frame pointer
        MOV     BP, SP
        PUSH    SI                      ;can't trash SI
        ;JMP    findEnd                 ;find end of destination


findEnd:                                ;find end of destination string
        LES     BX, dest                ;get destination pointer (ES:BX)
findEndLoop:                            ;loop finding end of string
        CMP     BYTE PTR ES:[BX], NULL  ;end of string?
        JE      DoCat                   ;end of destination - do concat
        INC     BX                      ;otherwise try next character
        JMP     findEndLoop

DoCat:                                  ;do the concatenation
        MOV     SI, srcOff              ;get source offset
DoCatLoop:                              ;loop copying characters
        MOV     ES, srcSeg              ;need source segment
        MOV     AL, ES:[SI]             ;get source character
        MOV     ES, destSeg             ;get destination segment
        MOV     ES:[BX], AL             ;and copy the character
        CMP     AL, NULL                ;did we just copy the NULL character?
        JE      DoneCat                 ;if so, done with concatenation
        INC     BX                      ;otherwise move to next characters
        INC     SI
        JMP     DoCatLoop               ;and loop


DoneCat:                                ;done with concatenation
        MOV     AX, destOff             ;setup return value
        MOV     DX, destSeg
        ;JMP    strcatEnd               ;and done now


strcatEnd:                              ;done concatenating strings
        POP     SI                      ;restore registers and return
        POP     BP
        RET


strcat_ ENDP




; strcpy_
;
; Description:       This function copies the second passed string to the
;                    first passed string.  The strings are passed as far
;                    pointers (segment and offset).
;
; Operation:         The second string is copied character by character to the
;                    first string.
;
; Arguments:         [SP + 2] (char far *) - pointer to destination string
;                                            (string to copy to).
;                    [SP + 6] (char far *) - pointer to source string (string
;                                            to copy from).
; Return Value:      DX | AX - pointer to the passed destination string.
;
; Local Variables:   BP - frame pointer
;                    ES - source and destination segment.
;                    BX - destination offset.
;                    SI - source offset.
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
; Registers Changed: flags, ES, BX.
; Stack Depth:       2 words
;
; Author:            Glen George
; Last Modified:     June 16, 2005

strcpy_ PROC    NEAR
        PUBLIC  strcpy_


strcpyStart:
        PUSH    BP                      ;setup the stack frame pointer
        MOV     BP, SP
        PUSH    SI                      ;can't trash SI

        MOV     BX, destOff             ;setup destination offset
        MOV     SI, srcOff              ;setup source offset
        ;JMP    CopyLoop                ;now copy the strings


CopyLoop:                               ;loop copying characters
        MOV     ES, srcSeg              ;need source segment
        MOV     AL, ES:[SI]             ;get source character
        MOV     ES, destSeg             ;get destination segment
        MOV     ES:[BX], AL             ;and copy the character
        CMP     AL, NULL                ;did we just copy the NULL character?
        JE      DoneCopy                ;if so, done with copying
        INC     BX                      ;otherwise move to next characters
        INC     SI
        JMP     CopyLoop                ;and loop


DoneCopy:                               ;done with copying
        MOV     AX, destOff             ;setup return value
        MOV     DX, destSeg
        ;JMP    strcpyEnd               ;and done now


strcpyEnd:                              ;done copying strings
        POP     SI                      ;restore registers and return
        POP     BP
        RET


strcpy_ ENDP




; strlen_
;
; Description:       This function computes the length of the passed string.
;                    The length is the number of characters, not including the
;                    terminating <null> character.  The string is passed as a
;                    far pointer (segment and offset).
;
; Operation:         The end of the string is found while counting characters.
;
; Arguments:         [SP + 2] (char far *) - pointer to string for which to
;                                            find the length.
; Return Value:      AX - length of the passed string.
;
; Local Variables:   BP - frame pointer
;                    ES - string segment.
;                    BX - string offset.
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
; Registers Changed: flags, ES, BX.
; Stack Depth:       1 word
;
; Author:            Glen George
; Last Modified:     June 4, 2006

strlen_ PROC    NEAR
        PUBLIC  strlen_


strlenStart:
        PUSH    BP                      ;setup the stack frame pointer
        MOV     BP, SP

        LES     BX, dest                ;get the string pointer (ES:BX)
        MOV     AX, 0                   ;length is 0 so far

strlenLoop:                             ;loop until find end of the string
        CMP     BYTE PTR ES:[BX], NULL  ;end of string?
        JE      strlenEnd               ;end of string - all done
        INC     AX                      ;otherwise increment the length
        INC     BX                      ;and go to next character
        JMP     strlenLoop


strlenEnd:                              ;done computing length
        POP     BP                      ;restore BP and return
        RET


strlen_ ENDP



CODE    ENDS



        END
