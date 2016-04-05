    NAME    BUTTON

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;                                                                            ;
;                                    Button                                  ;
;                               Button Functions                             ;
;                                   EE/CS 51                                 ;
;                                                                            ;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

; Description:    Functions for scanning the keys.
;
; Revision History:
;        2/3/16    Tim Liu    created file
;        2/4/16    Tim Liu    finished writing outline
;        4/4/16    Tim Liu    changed button to buttons
;
;
; Table of Contents
;
;        InitButtons - initializes the functions needed for buttons
;        ButtonDebounce - scans a the key address and debounces
;        key_available - returns TRUE if there is a valid key
;        getkey - returns the key code for the debounced key

;Name:               InitButtons
;
;Description:        This function initializes the shared variables for the
;                    button functions. The function sets the value of
;                    DebounceCnt to zero and sets LastRead to NoKeyPressed.
;
;Operation:          The function sets each of the three shared variables
;                    to the listed values. DebounceCnt is set to zero and
;                    LastRead is set to NobuttonPressed.
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
;InitButtons()
;    DebounceCnt = 0                     ;clear the debounce timer
;    LastRead = NoButtonPressed          ;indicate no key is pressed
;    RETURN



; ###### FUNCTION CODE  ######




;Name:               ButtonDebounce()
;
;Description:        This function reads the address of the buttons to
;                    see if a button is pressed. If a button is pressed,
;                    the function debounces the press and enqueues the button
;                    code to the buttonQueue using the function EnqueueEvent.
;                    This function also implements auto repeat. If the 
;                    same button is held down, then the button press will
;                    be recorded every RepeatRate milliseconds. Only one
;                    button can be pushed at a time. If multiple buttons
;                    are pressed, the function will act like no
;                    buttons are being pressed. This function is called by
;                    the interrupt handler buttonHandler every millisecond.
;
;Operation:          When called, the function reads in from the address
;                    buttonAddress. If the value of the input is equal to
;                    NoButtonPressed or if the value of the input is not
;                    equal to LastRead, then the function resets the debounce
;                    counter. If the value of the input is different from
;                    the last input, then LastRead is updated. If the input
;                    is the same as LastRead, then the function decrements
;                    DebounceCnt. Once DebounceCnt reaches zero, the function
;                    calls EnqueueEvent and enqueues the key press to the
;                    buttonQueue.
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
;Input:              8 possible button presses from 8 different buttons
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
;buttonDebounce()

;Input(ReadAddress, buttonInput)          ;read a word from the port
;
;IF (ButtonInput == NoKeyPressed) OR      ;check if button was pressed
;    (ButtonInput != LastRead):           ;check if button is same
;    DebounceCnt = DebounceTime           ;reset the debounce counter
;    LastRead = KeyInput                  ;update which button was pressed
;
;ELSE:                                    ;if a button was pressed
;    DebounceCnt --                       ;debounce the button
;    IF DebounceCnt == 0:                 ;button is debounced
;        CALL EnQueue                     ;enqueue the button press event
;        DebounceCnt = RepeatRate         ;implement auto-repeat
;
;RETURN


; ###### ButtonDebounce CODE  ######





;Name:               Key_Available
;
;Description:        This function checks if there is a button press ready
;                    for processing. The function calls QueueEmpty to check
;                    if the buttonQueue is empty. If buttonQueue is empty,
;                    then the function returns TRUE and if there is no
;                    button press ready the function returns FALSE.
;
;Operation:          Call QueueEmpty to check if the buttonQueue has any
;                    elements in it. If QueueEmpty returns with the 
;                    buttonQueue empty, the function returns with FALSE in
;                    AX. Otherwise, the function returns TRUE in AX.
;
;Arguments:          None
;
;Return Values:      Havebutton (AX) - TRUE if a button is ready to be
;                    processed; FALSE if no button press
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
;    QueueAddress = Address(buttonQueue)        ;load the argument
;    IF QueueEmpty(QueueAddress) == TRUE:       ;call function to check if
;                                               ;button queue is empty
;        HaveButton == FALSE                    ;no button available
;    ELSE:                                      ;otherwise, there’s a button
;        HaveButton == TRUE                     ;indicate a button is ready
;    RETURN



END





