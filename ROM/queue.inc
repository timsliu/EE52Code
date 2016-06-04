;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;                                                                            ;
;                                   QUEUE                                    ;
;                             Conversion Functions                           ;
;                                   EE/CS 51                                 ;
;                                                                            ;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


; This file contains the definitions for the queue structure and several
; constants.
;
; Revision History:
;   10/20/15    Tim Liu   Wrote struct definition
;   10/21/15    Tim Liu   Changed names to avoid protected names
;   10/22/15    Tim Liu   Updated comments
;   4/21/16     Tim Liu   Changed array_size to 256 bytes

;Queue definitions

array_size         EQU     256    ;number of bytes in a queue
ModByteMask        EQU     255    ;number to AND with to get mod 1024
ModWordMask        EQU     127     ;number to AND with to get mod 512

WordQueueType      EQU        1    ;make a word queue
ByteQueueType      EQU        0    ;make a byte queue

; Structure for queue

QueueStruct    STRUC
    word_byte  DB                 ?     ;size of each element
    filled     DW                 ?     ;number of elements filled
    head       DW                 ?     ;value of head index
    tail       DW                 ?     ;value of tail index
    content    DB array_size DUP (?)    ;array for storing contents
QueueStruct    ENDS