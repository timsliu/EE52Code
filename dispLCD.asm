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

;Name:               InitDisplayLCD
;
;Description:        This function initializes the shared variables for
;                    the display functions.
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
;Last Modified:      2/4/16

;Outline


; ###### FUNCTION CODE  ######




;Name:               DisplayLCD
;
;Description:        This function takes two arguments. The first argument is
;                    the address of a string for it to display. The second
;                    argument is an integer describing the type of
;                    information to be displayed. The second argument is used
;                    as an index into a table of structs. A table lookup
;                    is used to find the starting location on the LCD
;                    to write to for the given information type and the
;                    maximum allowed length. The function then writes
;                    ASCII characters to the LCD one character at a time.
;                    The function checks that the information is not
;                    written beyond its allowed space. Once the function
;                    is done writing, the function will return.
;
;Operation:          The string to write is passed to the function through
;                    ES:SI. The type of information is passed through BX
;                    as an integer. The integer is multiplied by the 
;                    size of each struct and added to the starting location
;                    of DisplayInfoTable, which is a table of structs.
;                    A table lookup into DisplayInfoTable is done to
;                    find the starting cursor position and the maximum
;                    space allotted to the information. The maximum
;                    number of characters is stored in CX. The function
;                    loops through the string that was passed and writes
;                    to the LCD. After each character is written, the 
;                    function increments the cursor to the next character
;                    and decrements the number of spaces allowed. Once
;                    the ASCII null character is reached or if the function
;                    reaches the end of the maximum allowed spaces for the 
;                    information, the function returns.
;
;Arguments:          String(ES:SI) - pointer to string to display
;                    Type (BX) - integer indicating type of info to display
;
;Return Values:      None
;
;Local Variables:    Cursor - position of the cursor
;                    CharLeft - counter for how many more characters are
;                    allowed for the information type
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




; ###### DisplayLCD CODE  ######


;Name:               SecToTime
;
;Description:        The function is passed an unsigned integer as an
;                    and argument. The argument represents the amount of
;                    time remaining in the track in tenths of a second.
;                    This function converts the time remaining in tenths
;                    of a second to minute:second (mm:ss) format. The
;                    function truncates the number of seconds. If the 
;                    amount of time remaining exceeds MAX_TIME, the function
;                    returns with the carry flag set to indicate an error.
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
;                    function sets the carry flag. Otherwise, the function
;                    returns with the carry flag cleared.
;
;Arguments:          Time_remaining (AX) - number of tenths of seconds
;                    remaining in the track.
;
;Return Values:      Carry_Flag - set if the passed time exceeds MAX_TIME
;                    otherwise cleared
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
;Registers Used:     None
;
;Known Bugs:         None
;
;Limitations:        None
;
;Last Modified:      2/4/16

;Outline
;SecToTime()
;    IF Time_Remaining <= MAX_TIME:      ;check time doesn’t exceed limit    
;        Time_remaining /= 10            ;convert to seconds
;        Seconds = Time_remaining % 60   ;mod 60 to get seconds
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


; ###### SecToTime CODE #####


;Name:               DisplayTime(Deci_Left)
;
;Description:        This function calls the DisplayLCD function to
;                    display the time remaining in the track. The 
;                    function first calls the SecToTime function to
;                    convert the number of seconds the ASCII mm:ss format.
;                    The function then calls DisplayLCD with the starting
;                    address of TimeBuffer to be displayed. The function
;                    also passes Type_Time to Display LCD to indicate
;                    that the time remaining is being displayed. If SecToTime
;                    returns with the carry flag set indicating an error,
;                    the function does not display a time.
;
;Operation:          The function passes the argument of DisplayTime to 
;                    SecToTime to convert the time to an ASCII
;                    string in mm:ss format. If SecToTime returns with
;                    the carry flag set indicating an error, the function
;                    does not display a time. Otherwise, the function
;                    then passes the starting address of TimeBuffer and
;                    DisplayTime_Type to DisplayLCD.
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
;Last Modified:      2/4/16

;Outline
;DisplayTime(Deci_Left)
;    Carry = SecToTime(Deci_Left)           ;convert time to mm:ss
;    IF Carry = 0:                          ;indicates no MAX_TIME error
;        DisplayLCD(TimeBuffer, Type_Time)  ;display the time
;    ELSE:                                  ;otherwise don’t do anything
;        PASS
;    RETURN


; ###### Display_Time CODE #######

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
;                    DisplayLCD with the string and TypeTitle to indicate
;                    to Display LCD to display the title.
;
;Operation:          The function first copies DS to ES.
;                    The function then calls DisplayLCD with the address of 
;                    the string to display. The function also passes
;                    TypeTitle to indicate that it is a title being
;                    displayed.
;
;Arguments:          Title_String - address of string to display
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
;Last Modified:      2/4/16

;Outline
;Display_Title(Title_String)
;    ES = DS                             ;DisplayLCD takes ES as segment
;    DisplayLCD(Title_String, TypeTitle) ;display title and indicate to
;                                        ;function that it’s a title
;    RETURN



; ###### FUNCTION CODE  ######



;Name:               Display_Artist(char far * Artist_String)
;
;Description:        This function is passed the address of the string
;                    to be displayed. The function calls the function
;                    DisplayLCD with the string and TypeArtist to indicate
;                    to Display LCD to display the artist.
;
;Operation:          The function first copies DS to ES.
;                    The function then calls DisplayLCD with the address of 
;                    the string to display. The function also passes
;                    TypeArtist to indicate that it is a title being
;                    displayed.
;
;Arguments:          Artist_String - address of string to display
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
;Last Modified:      2/4/16

;Outline
;Display_Artist(Artist_String)
;    ES = DS                               ;DisplayLCD takes ES as segment
;    DisplayLCD(Artist_String, TypeArtist) ;display artist and indicate to
;                                          ;function that it’s a artist
;    RETURN



; ###### FUNCTION CODE  ######




END















