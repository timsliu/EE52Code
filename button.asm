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
;        4/21/16   Tim Liu    wrote stub function for ButtonDebounce
;        4/21/16   Tim Liu    wrote InitButtons
;        4/21/16   Tim Liu    wrote ButtonDebounce
;        4/24/16   Tim Liu    added enqueue call to ButtonDebounce
;        4/24/16   Tim Liu    wrote Key_available
;        4/24/16   Tim Liu    wrote GetKey
;
;
; Table of Contents
;
;        InitButtons - initializes the functions needed for buttons
;        ButtonDebounce - scans a the key address and debounces
;        key_available - returns TRUE if there is a valid key
;        getkey - returns the key code for the debounced key

; local include files

$INCLUDE(BUTTON.INC)
$INCLUDE(QUEUE.INC)
$INCLUDE(GENERAL.INC)


CGROUP    GROUP    CODE
DGROUP    GROUP    DATA

CODE SEGMENT PUBLIC 'CODE'

        ASSUME  CS:CGROUP, DS:DGROUP

;external function declarations

    EXTRN    QueueInit:NEAR                 ;initializes a queue
    EXTRN    QueueFull:NEAR                 ;check if queue is full
    EXTRN    Enqueue:NEAR                   ;add event to queue
    EXTRN    QueueEmpty:NEAR                ;check if the queue is empty
    EXTRN    Dequeue:NEAR                   ;remove element from queue

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



InitButtons    PROC    NEAR
               PUBLIC  InitButtons

InitButtonsStart:
    MOV    DebounceCnt, DebounceTime     ;load the debounce counter
    MOV    LastRead, NoButtonPressed     ;nothing pressed yet

InitButtonsQueue:                        ;label for initializing queue
    PUSH   SI                            ;save registers
    PUSH   BX

InitButtonsQueueArgs:                    ;set up arguments for QueueInit
    LEA    SI, ButtonQueue               ;load address of the queue
    MOV    BL, ByteQueueType             ;make ButtonQueue a byte queue
    CALL   QueueInit                     ;initialize queue

InitButtonsEnd:                          ;finished - restore registers
    POP    BX
    POP    SI

    RET

InitButtons ENDP



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
;Registers Changed:  Flag registers
;
;Known Bugs:         None
;
;Limitations:        None
;
;Last Modified:      4/24/16
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


ButtonDebounce        PROC    NEAR
                      PUBLIC  ButtonDebounce


ButtonDebounceStart:
    PUSH  AX                               ;save registers
    PUSH  SI

ButtonDebounceRead:
    IN     AL, ButtonAddress               ;read the button byte

CheckButtonPressed:
    CMP    AL, NoButtonPressed             ;check if no button is pressed
    JE     ResetPress                      ;if no key pressed, go to label

    CMP    AL, LastRead                    ;check if button is same as last
    JNE    ResetPress                      ;reset if different button
    JMP    HaveButton                      ;otherwise take care of button

ResetPress:
    MOV    DebounceCnt, DebounceTime       ;reset the debounce counter
    CMP    AL, NoButtonPressed             ;if a different key is pressed
    JNE    UpdateLastPressed               ;then update the last pressed
    JMP    ButtonDebounceEnd               ;finish the function

UpdateLastpressed:
    MOV    LastRead, AL                     ;update last read key
    JMP    ButtonDebounceEnd                ;end

HaveButton:
    DEC    DebounceCnt                      ;one fewer cycle to wait
    CMP    DebounceCnt, 0                   ;check if debounce is over
    JE     SendButtonPress                  ;if debounced go to label
    JMP    ButtonDebounceEnd                ;otherwise end the function

SendButtonPress:
    
    LEA    SI, ButtonQueue                  ;load address - arg for queue funds
    CALL   QueueFull                        ;Check if the queue is full
    JZ     ButtonDebounceQFull              ;full - jump to emergency label

    CALL   EnQueue                          ;if not full, enqueue key pattern
    MOV    DebounceCnt, RepeatRate          ;set up auto repeat
    JMP    ButtonDebounceEnd                ;go to function end

ButtonDebounceQFull:
    JMP   ButtonDebounceEnd                 ;nothing for now - later set
                                            ;an abort flag

ButtonDebounceEnd:
    POP    SI                               ;restore registers
    POP    AX
    RET                                     ;end of function - return

ButtonDebounce ENDP




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
;Registers Used:     AX
;
;Known Bugs:         None
;
;Limitations:        None
;
;Last Modified:      4/24/16

;Outline
;Key_Available():
;    QueueAddress = Address(buttonQueue)        ;load the argument
;    IF QueueEmpty(QueueAddress) == TRUE:       ;call function to check if
;                                               ;button queue is empty
;        HaveButton == FALSE                    ;no button available
;    ELSE:                                      ;otherwise, there’s a button
;        HaveButton == TRUE                     ;indicate a button is ready
;    RETURN

Key_Available    PROC    NEAR
                 PUBLIC  Key_Available

Key_AvailableStart:
    PUSH    SI                                  ;save register

Key_AvailableCheck:                             ;check if queue is empty
    LEA     SI, ButtonQueue                     ;load QueueEmpty argument
    CALL    QueueEmpty                          ;check if queue is empty
    JZ      Key_AvailableNo                     ;Queue empty - no key
    JMP     Key_AvailableYes                    ;otherwise there is key
    

Key_AvailableYes:                               ;label if key is available
    MOV    AX, TRUE                             ;load return value
    JMP    Key_AvailableDone                    ;finish function

Key_AvailableNo:                                ;label if no key available
    MOV    AX, FALSE                            ;load return value
    JMP    Key_AvailableDone                    ;finish function

Key_AvailableDone:                              ;end of function
    POP     SI                                  ;restore register
    RET

Key_Available    ENDP


;Name:               getkey
;
;Description:        This function returns the key code for a debounced
;                    key. The function does not return until it has
;                    a valid key.                    
;
;Operation:          Load the starting address of the button queue to
;                    register SI. Clear out AX. Then, call Dequeue
;                    to remove a button event from the button queue.
;                    Return with the button key code.
;
;Arguments:          None
;
;Return Values:      KeyCode (AX) - key code corresponding to one of the
;                    key presses.
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
;Registers Used:     AX
;
;Known Bugs:         None
;
;Limitations:        None
;
;Last Modified:      4/24/16

;Outline

GetKey        PROC    NEAR
              PUBLIC  GetKey

GetKeyStart:                               ;starting label
    PUSH    SI                             ;save the registers
    XOR     AX, AX                         ;clear out return register

GetKeyDequeue:
    LEA    SI, ButtonQueue                 ;argument for Dequeue
    CALL   DeQueue                         ;remove button press from queue

GetKeyDone:                                ;end of function
    POP    SI                              ;restore register
    RET

GetKey    ENDP



CODE ENDS

;start data segment

DATA    SEGMENT PUBLIC  'DATA'

DebounceCnt        DW    ?     ;how many more irq before calling Enqueue
LastRead           DB    ?     ;value of last key read after masking
ButtonQueue    QueueStruct<>   ;allocate the button queue


DATA ENDS


END