;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;                                                                            ;
;                                   dispLCD.inc                              ;
;                            LCD Display Include File                        ;
;                                   EE/CS 52                                 ;
;                                                                            ;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

; This files contains the definitions for dispLCD
;
; Revision History:
;    4/26/16   Timothy Liu    created file - initial revision
;    4/27/16   Tim Liu   Added buffer sizes and LCD reg addresses and values

LCDInsReg       EQU    80h         ;address of LCD instruction register
LCDDatReg       EQU    81h         ;address of LCD data register
LCDInitVal      EQU    0fh         ;00001--- display on/off control
                                   ;-----1-- display on
                                   ;------1- cursor on
                                   ;-------1 blinking on

LCDFunSetVal    EQU    38h         ;00111000
                                   ;--1----- function set control
                                   ;---1---- 8 bit data
                                   ;----1--- 2 lines
                                   ;-----000 don’t care

BusyFlagMask    EQU    10000000b   ;mask out low 7 bits to get busy flag
BusyReady       EQU    0           ;busy flag is now ready

MaxTime         EQU    35990       ;max time that can be displayed (tenths
                                   ;of second)
TIME_NONE       EQU    65535       ;display no time

;buffer indexes
SecPos            EQU    3         ;index of where to start writing sec
ColonPos          EQU    5         ;index of where to write colon
TimeBufStartInd   EQU    3         ;index of where the time starts - 
                                   ;extra spaces in the buffer are padding 0

;Buffer sizes
TimeBufSize       EQU    9         ;size of time buffer in bytes
                                   ;buffer is extra large b/c Dec2String
                                   ;writes padding zeroes
TrackBufSize      EQU    14        ;size of track buffer in bytes
StatusBufSize     EQU    3         ;size of status buffer in bytes
ArtistBufSize     EQU    11        ;size of artist buffer in bytes