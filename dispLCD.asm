    NAME    DISPLCD

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;                                                                            ;
;                                    DISPLCD                                 ;
;                            LCD Display Functions                           ;
;                                   EE/CS 51                                 ;
;                                                                            ;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

; Description:    Functions for scanning the keys.
;
; Revision History:
;        2/4/16    Tim Liu    created file
;        4/27/16   Tim Liu    wrote InitDisplay and added data/code segments
;        4/28/16   Tim Liu    Added busy flag read and looping to InitDisplay
;        4/29/16   Tim Liu    wrote SecToTime
;        4/29/16   Tim Liu    wrote DisplayLCD
;        5/4/16    Tim Liu    wrote DisplayTime
;        5/4/16    Tim Liu    wrote DisplayArtist
;        5/4/16    Tim Liu    wrote DisplayStringCopy helper function
;
;
; Table of Contents
;
;    InitDisplay - initializes shared variables for display
;    DisplayLCD - writes characters to the LCD
;    SecToTime - converts time elapsed to mm:ss ASCII format
;    Display_Time - displays the passed time to the LCD
;    Display_Status - displays the passed status to the LCD
;    Display_Title - displays track title on the LCD
;    Display_Artist - displays track artist on the LCD
;    DisplayStringCopy - helper function that copies a string to buffer

; local include files
$INCLUDE(GENERAL.INC)
$INCLUDE(DISPLCD.INC)

CGROUP    GROUP    CODE
DGROUP    GROUP    DATA

CODE SEGMENT PUBLIC 'CODE'

        ASSUME  CS:CGROUP, DS:DGROUP

;external function declarations

        EXTRN    Dec2String:NEAR            ;convert decimals to strings

;Name:               InitDisplayLCD
;
;Description:        This function initializes the shared variables for
;                    the display functions. The function also writes 
;                    InitLCDVal to LCDInsReg to turn on the display
;                    and turn on the cursor.
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
;Input:              None
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
;Last Modified:      4/28/16

;Outline


InitDisplayLCD        PROC    NEAR
                      PUBLIC  InitDisplayLCD
InitDisplayStart:              ;starting label
    PUSH   AX                  ;save register

InitDisplayOut:                ;output setup command to LCD
    MOV    AL, LCDInitVal      ;load LCD initialization command
    OUT    LCDInsReg, AL       ;write display control command

InitDisplayCheckBusy:
    IN     AL, LCDInsReg       ;read the status register
    AND    AL, BusyFlagMask    ;mask out lower 7 bits
    CMP    AL, BusyReady       ;check if busy flag is set
    JE     InitDisplayFunSet   ;not busy - output function set
    JMP    InitDisplayCheckBusy;not ready - keep looping

InitDisplayFunSet:             ;output function set command to LCD
    MOV    AL, LCDFunSetVal    ;load function set command
    OUT    LCDInsReg, AL       ;write function set command


InitDisplayLCDDone:            ;done with function
    POP   AX                   ;restore register

    RET                        

InitDisplayLCD    ENDP



;Name:               DisplayLCD
;
;Description:        This function takes two arguments. The first argument is
;                    the address of a string for it to display. The second
;                    argument is an integer describing the type of
;                    information to be displayed. The second argument is used
;                    as an index into a byte table that stores the starting
;                    address of each type of data. The function then writes
;                    ASCII characters to the LCD one character at a time.
;                    The function stops writing when it reaches a null
;                    character in the string passed to it. Once the function
;                    is done writing, the function will return.
;
;Operation:          The string to write is passed to the function through
;                    ES:SI. The type of information is passed through BX
;                    as an integer. The integer is used to index into
;                    DisplayInfoTable to find the starting cursor position
;                    for each type of information. The function
;                    loops through the string that was passed and writes
;                    to the LCD. After each character is written, the 
;                    function increments the cursor to the next character.
;                    The function loops checking the busy flag after each
;                    write to the LCD. Only once the LCD busy flag is clear
;                    will the function write the next character.
;                    Once the ASCII null character is reached the function
;                    returns.
;
;Arguments:          String(ES:SI) - pointer to string to display
;                    Type (BX) - integer indicating type of info to display
;
;Return Values:      None
;
;Local Variables:    Cursor - position of the cursor
;
;
;Shared Variables:   None
;
;Input:              None
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
;Last Modified:      4/28/16

;Outline
;DisplayLCD(String, Type)
;    Type *= SizeOf(DataInfoStruct)        ;multiply by size of table entry    
;    Type += Offset(DataInfoTable)         ;add to table start location
;    Cursor = DataInfoTable[Type].Start    ;set cursor to start position
;    CharLeft = DataInfoTable[Type].MaxSize;set max chars allowed for type
;    WHILE (CharLeft != 0 AND              ;check haven’t written too far
;          ES:[SI] != ASCII_NULL)          ;check for end of string
;        OUT(ES:[SI], LCDDataAddress)      ;output to the display
;        Cursor ++                         ;write at the next position
;        CharLeft —-                       ;one fewer space left to write


DisplayLCD        PROC    NEAR
                  PUBLIC  DisplayLCD

DisplayLCDStart:                           ;save registers
    PUSH    SI
    PUSH    AX

DisplayLCDLookUp:                          ;lookup start address of info type
    MOV    AL, CS:DisplayInfoTable[BX]     ;AL stores LCD DDRAM location

DisplayLCDSetStart:                        ;set cursor to start position
    OUT   LCDInsReg, AL                    ;write cursor pos to ins reg

DisplayLCDCheckEnd:                        ;check if end of buffer reached
    CMP   BYTE PTR ES:[SI], ASCII_NULL     ;buffers are null terminated
    JE    DisplayLCDEnd                    ;reach end of buffer

DisplayLCDBusy:                            ;check if busy flag is set
    IN     AL, LCDInsReg                   ;read the status register
    AND    AL, BusyFlagMask                ;mask out lower 7 bits
    CMP    AL, BusyReady                   ;check if busy flag is set
    JE     DisplayLCDWrite                 ;ready - go write to display
    JMP    DisplayLCDBusy                  ;not ready - keep looping

DisplayLCDWrite:
    MOV    AL, ES:[SI]                     ;copy character to output register
    OUT    LCDDatReg, AL                   ;output to display
    INC    SI                              ;next element of buffer
    JMP    DisplayLCDCheckEnd              ;go check for null char
    
DisplayLCDEnd:                              ;end - restore registers
    POP    AX
    POP    SI
    RET


DisplayLCD        ENDP

;Name:               SecToTime
;
;Description:        The function is passed an unsigned integer as an
;                    and argument. The argument represents the amount of
;                    time remaining in the track in tenths of a second.
;                    This function converts the time remaining in tenths
;                    of a second to minute:second (mm:ss) format. The
;                    function truncates the number of seconds. If the 
;                    amount of time remaining exceeds MAX_TIME, or if
;                    the time to be displayed is TIME_NONE, then the function
;                    displays blank segment patterns where the time should
;                    be displayed.
;                    
;
;Operation:          The function first divides the amount of time remaining
;                    by ten to get the number of seconds. The function then
;                    divides the number of seconds by 60 seconds in a minute
;                    to get the number of minutes remaining. The function
;                    calls Dec2String to convert the number of minutes
;                    to an ASCII string. The ASCII string is written to the
;                    first two characters of TimeBuffer.The function takes
;                    the remainder of the division and calls Dec2String to
;                    convert the number of seconds to a string. The function
;                    writes to location SecondStart of TimeBuffer. The
;                    function then writes ASCII_COLON to location TimeColon
;                    of TimeBuffer. If Time_remaining exceeds MAX_TIME, the
;                    function writes a blank character patterns to the
;                    TimeBuffer.
;
;Arguments:          Time_remaining (AX) - number of tenths of seconds
;                    remaining in the track.
;
;Return Values:      None
;
;Local Variables:    None
;
;Shared Variables:   TimeBuffer (R/W) - buffer for holding time in mm:ss
;                    format
;
;Input:              None
;
;Output:             None
;
;Error Handling:     Checks that the passed time does not exceed MAX_TIME
;
;Algorithms:         None
;
;Registers Used:     AX
;
;Known Bugs:         None
;
;Limitations:        None
;
;Last Modified:      4/28/16

;Outline
;SecToTime()
;    IF Time_Remaining <= MAX_TIME:      ;check time doesn’t exceed limit    
;        Time_remaining /= 10            ;convert to seconds
;        Seconds = Time_remaining mod 60   ;mod 60 to get seconds
;        Minutes = Time_remaining / 60   ;divide by 60 to get minutes
;        Dec2String(TimeBuffer, Minutes) ;convert minutes to ASCII string
;                                    ;and write to time buffer
;        Dec2String(TimeBuffer + SecondStart, Seconds)
;                                    ;write seconds to time buffer
;        TimeBuffer[TimeColon] = ASCII_Colon  ;write colon between mm:ss
;        Carry Flag = 0                  ;clear carry flag for no error
;    ELSE:
;        Carry Flag = 1                  ;MAX_TIME exceeded
;    RETURN

SecToTime        PROC    NEAR
                 PUBLIC  SecToTime

SecToTimeStart:                          ;starting label - save registers
    PUSH   BX                            ;save registers
    PUSH   DX                            
    PUSH   SI

SecToTimeCheck:                          ;check time doesn’t exceed MAX_TIME
    CMP    AX, MAXTIME                   ;
    JA     SecToTimeBlankLoad            ;time too high to display
    ;JMP   SecToTimeDivide               ;time under limit-start calculating

SecToTimeDivide:
    MOV    BX, 10                        ;tenths of a second in a second
    XOR    DX, DX                        ;clear out the high order byte
    DIV    BX                            ;divide time to get seconds left

    MOV    BX, 60                        ;divide by seconds in a minute
    XOR    DX, DX                        ;clear out high order byte
    DIV    BX                            ;minutes in AX seconds in DX


SecToTimeWriteTime:                      ;write time to TimeBuffer
    XCHG   AX, DX                        ;swap minutes and seconds
                                         ;so that sec in AX and min in DX
    LEA    SI, TimeBuffer                ;load argument for Dec2String
    ADD    SI, SecPos                    ;compute location for writing sec
    CALL   Dec2String                    ;write seconds to TimeBuf
    MOV    AX, DX                        ;copy minutes to Dec2String arg
    LEA    SI, TimeBuffer                ;address to write minutes to
    CALL   Dec2String                    ;write seconds to TimeBuffer

SectoTimeWriteColon:                     ;write colon between min and sec
    MOV    BX, ColonPos                  ;load index of colon
    MOV    TimeBuffer[BX], ASCII_COLON   ;write colon
    JMP    SecToTimeDone                 ;done with function
    
SecToTimeBlankLoad:                      ;write blank segment patterns
    MOV    BX, 0                         ;array index

SecToTimeBlankLoop:
    CMP    BX, TimeBufSize               ;check if array has been filled
    JE     SecToTimeBlankEnd             ;done writing 5 blanks

SecToTimeWriteBlank:                     ;write blanks to the TimeBuffer
    MOV    TimeBuffer[BX], ASCII_SPACE   ;
    INC    BX                            ;move index to next element
    JMP    SecToTimeBlankLoop            ;go back to loop

SecToTimeBlankEnd:
    MOV    BX, TimeBufSize - 1           ;index of last element of buffer
    MOV    TimeBuffer[BX], ASCII_NULL    ;time buffer is null terminated

SecToTimeDone:
    POP    SI                            ;restore registers
    POP    DX
    POP    BX
    RET


SecToTime    ENDP

;Name:               DisplayTime(Deci_Left)
;
;Description:        This function calls the DisplayLCD function to
;                    display the time remaining in the track. The 
;                    function first calls the SecToTime function to
;                    convert the number of seconds the ASCII mm:ss format.
;                    The function then calls DisplayLCD with the starting
;                    address of TimeBuffer to be displayed. The function
;                    also passes TypeTime to Display LCD to indicate
;                    that the time remaining is being displayed. 
;
;Operation:          The function passes the argument of DisplayTime to 
;                    SecToTime to convert the time to an ASCII
;                    string in mm:ss format. The argument is passed through
;                    AX. SecToTime writes the time to TimeBuffer. Display
;                    Time then calls the function DisplayLCD to display
;                    the time. The address of the time buffer is loaded
;                    into SI and incremented by TimeBufStartInd since the 
;                    first several elements of TimeBuffer are blanks. DS is copied
;                    to ES and ES:SI is passed to DisplayLCD. The constant
;                    TypeTime is copied to AX and passed to DisplayLCD
;                    to indicate that the time should be displayed
;
;Arguments:          Deci_Left - tenths of seconds left in track
;
;Return Values:      None
;
;Local Variables:    None
;
;Shared Variables:   None
;
;Input:              None
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
;Last Modified:      5/4/16



Display_Time        PROC    NEAR
                    PUBLIC  Display_Time

DisplayTimeStart:                           ;starting label
    PUSH    SI                              ;save register
    PUSH    BX

DisplayTimeWrite:                           ;call function to write time
    CALL    SecToTime                       ;AX has time - write to TimeBuffer

DisplayTimeLoadArg:                         ;load arguments
    LEA    SI, TimeBuffer                   ;start address of TimeBuffer
    ADD    SI, TimeBufStart                 ;increment to where time starts
    MOV    BX, DS                           ;copy DS to ES
    MOV    ES, BX                           ;
    MOV    AX, TypeTime                     ;arg indicating display the time

DisplayTimeDisplay:                         ;call DisplayLCD to display
    CALL   DisplayLCD                       ;display the time

DisplayTimeDone:                            ;finished - restore registers
    POP    BX
    POP    SI
    RET


Display_Time    ENDP


;Name:               Display_Status(Status)
;
;Description:        This function takes an integer that maps to a status
;                    as its argument. The function looks up the string
;                    associated with the integer in StatusTable, which 
;                    is a table of fixed length strings. The function
;                    calls the function DisplayLCD with the address
;                    of the corresponding string and Type_Status to
;                    indicate that a status string is being displayed.
;
;Operation:          The function multiplies the integer status by the
;                    size of each table entry of StatusTable to find the
;                    offset of the corresponding status string. The
;                    function copies CS to ES and passes the address of the
;                    status string to DisplayLCD. The function also
;                    passes Type_Status to indicate to DisplayLCD that the
;                    status is being displayed.
;
;Arguments:          Status - integer representing status
;
;Return Values:      None
;
;Local Variables:    String (SI) - address of string to write
;
;Shared Variables:   None
;
;Input:              None
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
;Last Modified:      2/4/16

;Outline
;Display_Status(Status)
;    Status *= SIZEOF(StatusTableEntry)      ;multiply by table entry
;    Status += OFFSET(StatusTable)           ;calculate string address
;    ES = CS                                 ;set segment
;    String = StatusTable[Status]            ;string to display
;    DisplayLCD(String, TypeStatus)          ;call Display LCD to show status
;    RETURN



; ###### FUNCTION CODE  ######


;Name:               Display_Title(char far * Title_String)
;
;Description:        This function is passed the address of the string
;                    to be displayed. The function calls the function
;                    DisplayStringCopy to copy the string to the
;                    TitleBuffer. The function then calls DisplayLCD
;                    to display the track name.
;
;Operation:          The function first reads from the stack and copies
;                    the segment  of the string to ES and the offset to SI.
;                    The function then stores the starting address of 
;                    TrackBuffer in BX and TrackBufSize in CX. The 
;                    function calls DisplayStringCopy which writes the 
;                    string to be displayed to TrackBuffer. Display_Title
;                    then copies DS to ES and loads the address of
;                    TrackBuffer to SI. The constant TypeTrack is placed
;                    in AX and the DisplayLCD is called. The function
;                    then restores the saved registers and returns.
;
;Arguments:          Title_String - address of string to display
;                                   passed through stack
;
;Return Values:      None
;
;Local Variables:    None
;
;Shared Variables:   None
;
;Input:              None
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
;Last Modified:      5/4/16




Display_Title         PROC    NEAR
                      PUBLIC  Display_Title

DisplayTitleStart:                         ;starting label
    PUSH    BP                             ;save register
    MOV     BP, SP                         ;use BP to index into the stack
    PUSH    SI                             ;save registers
    PUSH    AX
    PUSH    BX
    PUSH    CX

DisplayTitleArgs:                          ;load args for DisplayStringCopy
    MOV     ES, SS:[BP+6]                  ;string segment
    MOV     SI, SS:[BP+4]                  ;string offset
    LEA     BX, TrackBuffer                ;target buffer
    MOV     CX, TrackBufSize               ;size of TrackBuffer
    CALL    DisplayStringCopy              ;copy string to TrackBuffer

DisplayTitleDisplay:                       ;call DisplayLCD
    MOV    AX, DS                          ;copy DS to ES
    MOV    ES, AX
    MOV    AX, TypeTrack                   ;tells DisplayLCD data type
    LEA    SI, TrackBuffer                 ;address of buffer to display
    CALL   DisplayLCD                      ;display the string

DisplayTitleDone:                          ;finished - restore registers
    POP    CX
    POP    BX
    POP    AX
    POP    SI
    POP    BP
    RET

Display_Title    ENDP



;Name:               Display_Artist(char far * Artist_String)
;
;Description:        This function is passed the address of the string
;                    to be displayed. The function calls the function
;                    DisplayStringCopy to copy the string to the
;                    ArtistBuffer. The function then calls DisplayLCD
;                    to display the artist.
;
;Operation:          The function first reads from the stack and copies
;                    the segment  of the string to ES and the offset to SI.
;                    The function then stores the starting address of 
;                    ArtistBuffer in BX and ArtistBufSize in CX. The 
;                    function calls DisplayStringCopy which writes the 
;                    string to be displayed to ArtistBuffer. Display_Artist
;                    then copies DS to ES and loads the address of
;                    ArtistBuffer to SI. The constant TypeArtist is placed
;                    in AX and the DisplayLCD is called. The function
;                    then restores the saved registers and returns.
;
;Arguments:          Artist_String - address of string to display
;                                    segment and offset passed through stack
;
;Return Values:      None
;
;Local Variables:    None
;
;Shared Variables:   None
;
;Input:              None
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
;Last Modified:      5/4/16




Display_Artist        PROC    NEAR
                      PUBLIC  Display_Artist

DisplayArtistStart:                        ;starting label
    PUSH    BP                             ;save register
    MOV     BP, SP                         ;use BP to index into the stack
    PUSH    SI                             ;save registers
    PUSH    AX
    PUSH    BX
    PUSH    CX

DisplayArtistArgs:                         ;load args for DisplayStringCopy
    MOV     ES, SS:[BP+6]                  ;string segment
    MOV     SI, SS:[BP+4]                  ;string offset
    LEA     BX, ArtistBuffer               ;target buffer
    MOV     CX, ArtistBufSize              ;size of ArtistBuffer
    CALL    DisplayStringCopy              ;copy string to ArtistBuffer

DisplayArtistDisplay:                      ;call DisplayLCD
    MOV    AX, DS                          ;copy DS to ES
    MOV    ES, AX
    MOV    AX, TypeArtist                  ;tells DisplayLCD data type
    LEA    SI, ArtistBuffer                ;address of buffer to display
    CALL   DisplayLCD                      ;display the string

DisplayArtistDone:                         ;finished - restore registers
    POP    CX
    POP    BX
    POP    AX
    POP    SI
    POP    BP
    RET

Display_Artist    ENDP

;Name:          DisplayStringCopy
;
;Description:   This function copies a string into a buffer and writes
;               spaces to the end of the buffer. The function will
;               only write to the end of the buffer and ends all strings
;               with the null character. The function overwrites the
;               entire buffer each time it is called.
;
;Operation:     This function takes three arguments. The address of the
;               string to be copied is passed through ES:SI and the
;               offset of the target buffer is passed through BX. The            
;               length of the target buffer is passed through CX. The function
;               loops through and copies elements from ES:SI to DS:BX.
;               If the passed string is shorter than the buffer, then
;               the function pads the rest of the buffer with ASCII_SPACE.
;               If the passed string is longer than the buffer, then the
;               function will stop copying when there is one element left
;               and write ASCII_NULL to the end. The register DL is used
;               as an intermediary to transfer data from memory to memory.
;
;Arguments:          ES:SI - address of string to copy
;                    BX - address of buffer to copy
;                    CX - number of elements in string buffer
;
;Return Values:      None
;
;Local Variables:    CX - elements left in the string
;                    BX - target buffer location being written to
;                    SI - source string location begin read from
;
;Shared Variables:   None
;
;Input:              None
;
;Output:             None
;
;Error Handling:     None
;
;Algorithms:         None
;
;Registers Used:     CX, BX, SI
;
;Known Bugs:         None
;
;Limitations:        None
;
;Last Modified:      5/4/16

DisplayStringCopy        PROC    NEAR


DisplayStringStart:                    ;save register
    PUSH  DX

DisplayStringLoop:
    CMP    CX, 1                       ;check if one element left
    JE     DisplayStringNull           ;write a null termination char
    CMP    BYTE PTR ES:[SI], ASCII_NULL;check if null char reached in source
    JE     DisplayStringPad            ;if so, write padding to the end
    JMP    DisplayStringWrite          ;otherwise copy to target buffer

DisplayStringWrite:                    ;copy element of string to buffer
    MOV    DL, ES:[SI]                 ;copy contents to intermediary
    MOV    DS:[BX], DL                 ;contents to target buffer
    INC    BX                          ;increment target buffer
    INC    SI                          ;increment source buffer
    DEC    CX                          ;one less element of target to fill
    JMP    DisplayStringLoop           ;back to top of loop

DisplayStringPad:                      ;pad buffer to end of string
    CMP    CX, 1                       ;check if one element left
    JE     DisplayStringNull           ;if so, write null character
    MOV    BYTE PTR DS:[BX], ASCII_SPACE        ;write a space
    INC    BX                          ;increment target buffer
    DEC    CX                          ;one less element less
    JMP    DisplayStringPad            ;pad next element

DisplayStringNull:                     ;write null termination character
    MOV    BYTE PTR DS:[BX], ASCII_NULL         ;write character

DisplayStringEnd:                      ;function over - return
    POP    DX
    RET

DisplayStringCopy        ENDP



;Name:          DisplayInfoTable
;
;Description:   The byte table stores the starting address for each type of
;               information to be displayed. The function DisplayLCD
;               looks up the start position for each information type
;               from this table.
;
;Author:        Timothy Liu
;
;Last Modified  4/29/16

DisplayInfoTable        LABEL    BYTE

;        DB        StartAddress
         DB        080h        ;track name
         DB        08Eh        ;action address
         DB        0C0h        ;artist name
         DB        0CBh        ;time


CODE ENDS

;start data segment

DATA    SEGMENT PUBLIC  'DATA'

TimeBuffer    DB TimeBufSize   DUP (?)        ;allocate buffer for the time
TrackBuffer   DB TrackBufSize  DUP (?)        ;allocate buffer for track name
StatusBuffer  DB StatusBufSize DUP (?)        ;allocate buffer for status
ArtistBuffer  DB ArtistBufSize DUP (?)        ;allocate buffer for artist



DATA ENDS


END