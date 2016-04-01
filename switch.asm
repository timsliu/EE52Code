    NAME    SWITCH

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;                                                                            ;
;                                    Switch                                  ;
;                               Switch Functions                             ;
;                                   EE/CS 51                                 ;
;                                                                            ;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

; Description:    Functions for scanning the keys.
;
; Revision History:
;        2/3/16    Tim Liu    created file
;        2/4/16    Tim Liu    finished writing outline
;
;
; Table of Contents
;
;        InitSwitches - initializes the functions needed for switches
;        SwitchDebounce - scans a the key address and debounces
;        key_available - returns TRUE if there is a valid key
;        getkey - returns the key code for the debounced key

;Name:               InitSwitches
;
;Description:        This function initializes the shared variables for the
;                    switch functions. The function sets the value of
;                    DebounceCnt to zero and sets LastRead to NoKeyPressed.
;
;Operation:          The function sets each of the three shared variables
;                    to the listed values. DebounceCnt is set to zero and
;                    LastRead is set to NoSwitchPressed.
;
;Arguments:          None
;
;Return Values:      None
;
;Local Variables:    None
;
;Shared Variables:   KeyCode (W) - code for the key being pressed
;                    LastRead
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
;InitSwitches()
;    DebounceCnt = 0                     ;clear the debounce timer
;    LastRead = NoSwitchPressed          ;indicate no key is pressed
;    RETURN



; ###### FUNCTION CODE  ######




;Name:               SwitchDebounce()
;
;Description:        This function reads the address of the switches to
;                    see if a switch is pressed. If a switch is pressed,
;                    the function debounces the press and enqueues the switch
;                    code to the SwitchQueue using the function EnqueueEvent.
;                    This function also implements auto repeat. If the 
;                    same switch is held down, then the switch press will
;                    be recorded every RepeatRate milliseconds. Only one
;                    button can be pushed at a time. If multiple buttons
;                    are pressed, the function will act like no
;                    buttons are being pressed. This function is called by
;                    the interrupt handler SwitchHandler every millisecond.
;
;Operation:          When called, the function reads in from the address
;                    SwitchAddress. If the value of the input is equal to
;                    NoSwitchPressed or if the value of the input is not
;                    equal to LastRead, then the function resets the debounce
;                    counter. If the value of the input is different from
;                    the last input, then LastRead is updated. If the input
;                    is the same as LastRead, then the function decrements
;                    DebounceCnt. Once DebounceCnt reaches zero, the function
;                    calls EnqueueEvent and enqueues the key press to the
;                    SwitchQueue.
;
;Arguments:          None
;
;Return Values:      None
;
;Local Variables:    KeyInput (AL) - input from the key being pressed.
;
;Shared Variables:   DebounceCnt (R/W) - how many additional interrupts
;                    must occur before the button press is debounced
;                    LastRead (R/W) - the value of the last key that was
;                    pressed.
;
;Input:              8 possible switch presses from 8 different switches
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
;
;Outline
;
;SwitchDebounce()

;Input(ReadAddress, SwitchInput)          ;read a word from the port
;
;IF (SwitchInput == NoKeyPressed) OR      ;check if switch was pressed
;    (SwitchInput != LastRead):           ;check if switch is same
;    DebounceCnt = DebounceTime           ;reset the debounce counter
;    LastRead = KeyInput                  ;update which switch was pressed
;
;ELSE:                                    ;if a switch was pressed
;    DebounceCnt --                       ;debounce the switch
;    IF DebounceCnt == 0:                 ;switch is debounced
;        CALL EnQueue                     ;enqueue the switch press event
;        DebounceCnt = RepeatRate         ;implement auto-repeat
;
;RETURN


; ###### SwitchDebounce CODE  ######





;Name:               Key_Available
;
;Description:        This function checks if there is a switch press ready
;                    for processing. The function calls QueueEmpty to check
;                    if the SwitchQueue is empty. If SwitchQueue is empty,
;                    then the function returns TRUE and if there is no
;                    switch press ready the function returns FALSE.
;
;Operation:          Call QueueEmpty to check if the SwitchQueue has any
;                    elements in it. If QueueEmpty returns with the 
;                    SwitchQueue empty, the function returns with FALSE in
;                    AX. Otherwise, the function returns TRUE in AX.
;
;Arguments:          None
;
;Return Values:      HaveSwitch (AX) - TRUE if a switch is ready to be
;                    processed; FALSE if no switch press
;
;Local Variables:    QueueAddress (SI) - address of queue. Argument to
;                    QueueEmpty
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
;Last Modified:      2/4/16

;Outline
;Key_Available():
;    QueueAddress = Address(SwitchQueue)        ;load the argument
;    IF QueueEmpty(QueueAddress) == TRUE:       ;call function to check if
;                                               ;switch queue is empty
;        HaveSwitch == FALSE                    ;no switch available
;    ELSE:                                      ;otherwise, there’s a switch
;        HaveSwitch == TRUE                     ;indicate a switch is ready
;    RETURN



END














