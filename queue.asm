        NAME    QUEUE

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;                                                                            ;
;                                   QUEUE                                    ;
;                             QUEUE Functions                                ;
;                                   EE/CS 51                                 ;
;                                                                            ;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


; Functions for intializing a queue, adding elements to a queue, removing
; elements, and checking if the queue is full or empty
;
; Table of Contents:
;     QueueInit    Line 88
;     QueueEmpty   Line 151
;     QueueFull    Line 200
;     DeQueue      Line 278
;     EnQueue      Line 388
;
; Revision History:
;     10/20/15     Tim Liu     started writing functions
;     10/21/15     Tim Liu     fixed syntax errors
;     10/21/15     Tim Liu     replaced DX with BX for accessing contents
;     10/22/15     Tim Liu     updated comments
;
;local include files
$INCLUDE (queue.inc)

CGROUP  GROUP   CODE


CODE    SEGMENT PUBLIC 'CODE'


        ASSUME  CS:CGROUP


;Name:              QueueInit(address, length, size)
;
;Description:       This function initializes a queue and prepares the
;                   structure for use. The function takes three arguments
;                   - an address where the queue begins (address), the
;                   length of the queue (length), and the size of each
;                   element in the queue (size). This function sets the
;                   value of the head and tail index to zero. The queue
;                   is always a set length of 1024 bytes. One of
;                   the attributes of the queue is a variable that counts
;                   the number of elements that have been filled. This
;                   prevents elements from being stored to the queue
;                   if the queue is already full. The queue is stored
;                   starting at the address specified in SI. The argument
;                   length is effectively ignored.
;               
;
;Operation:         The function fills in the attributes of the queue located
;                   at the specified address. The head and tail indices are
;                   both set to zero, and the number of filled elements is
;                   set to zero. If the size of each element is a
;                   byte, the integer 1 is stored at the attribute
;                   Queue.size. Otherwise, the integer 2 is stored at
;                   Queue.size.
;
;Arguments:         address (DS:SI) - starting location for the queue
;                   length (AX) - maximum number of elements that can be 
;                                 stored in the queue
;                   size(BL) - whether each element is a byte or a word;
;                              non-zero value indicates words and a zero
;                              indicates bytes
;
;Return Values:     None
;
;Local Variables:   None
;
;Output:            None
;
;Error Handling:    None
;
;Stack Depth:       0 words
;
;Algorithms:        None
;
;Known Bugs:        None
;
;Limitations:       The queue can only take a limited number of elements,
;                   specified by LENGTH (1024 bytes).
;
;Author:            Timothy Liu
;
;Last modified:     October 21, 2015

QueueInit     PROC    NEAR
              PUBLIC  QueueInit

QueueInitStart:
        CMP     BL, 0                   ;check element size - word or byte
        JE      SizeByte                ;if 0, size is a byte
        JMP     SizeWord                ;otherwise it's a word
        
SizeByte:                               ;store that size is a byte
        MOV     [SI].word_byte, 1       ;each element is 1 byte
        JMP     QueueInit2              ;finish the function
        
SizeWord:                               ;store that size is a word
        MOV     [SI].word_byte, 2       ;each element is 2 bytes (1 word)
        ;JMP    QueueInit2              ;finish the function
        
QueueInit2:                             ;finish initializing
        MOV     [SI].head, 0            ;set head index to 0
        MOV     [SI].tail, 0            ;set tail index to 0
        MOV     [SI].filled, 0          ;no elements filled
        ;JMP    EndQueueInit
        
EndQueueInit:
        RET

QueueInit     EndP

;Name: QueueEmpty
;
;Description:       This function determines whether the queue at an address 
;                   specified by DS:SI is empty. If the queue is empty, the
;                   function sets the zero flag. Otherwise, the function
;                   resets the zero flag.
;
;Operation:         The function checks if the struct attribute “filled” is
;                   equal to zero. If the filled attribute is zero, the
;                   queue is empty. Otherwise, the queue is not empty.
;                
;Arguments:         address (DS:SI) - starting location for the queue
;                 
;Return Values:     zero flag - set if queue is empty; otherwise reset
;                 
;Local Variables:   None
;
;Output:            None
;
;Error Handling:    None
;
;Stack Depth:       0 words
;
;Algorithms:        None
;
;Known Bugs:        None
;
;Limitations:       None
;
;Author:            Timothy Liu
;
;Last modified:     October 21, 2015


QueueEmpty    PROC    NEAR
              PUBLIC  QueueEmpty
              
QueueEmptyStart:
        CMP     [SI].filled, 0  ;check if 0 elements filled
        RET                     ;
        
QueueEmpty    EndP

;Name:               QueueFull
;
;Description:        This function determines whether the queue at the 
;                    address DS:SI is full. If the queue is full, the 
;                    zero flag is set. Otherwise, the zero flag is reset.
;
;Operation:          This function first calculates the maximum number of 
;                    elements that can be stored in the queue by dividing 
;                    the length of the queue by the size of each 
;                    element. The function then compares the number of 
;                    filled elements with the maximum number of possible
;                    elements. If the number of elements filled is equal
;                    to the maximum number of elements that can be stored, 
;                    the zero flag is set. Otherwise, the zero flag is reset.
;
;Arguments:          address (DS:SI) - starting location for the queue
;
;Return Values:      zero flag - set if full; otherwise reset
;               
;Local Variables:    Maximum elements (AX) - the maximum number of elements that
;                    can be stored in the queue. Found by dividing
;                    the length of the queue by the attribute “size”.
;    
;Output:             None
;
;Error Handling:     None
;
;Stack Depth:        1 word
;
;Algorithms:         None
;
;Known Bugs:         None
;
;Limitations:        None
;
;Author:             Timothy Liu
;
;Last modified:      October 21, 2015

QueueFull       PROC    NEAR
                PUBLIC  QueueFull
                
QueueFullStart:
        PUSH    AX                  ;don't trash AX
        MOV     AX, array_size      ;AX represents maximum elements in queue
        CMP     [SI].word_byte, 2   ;check if elements are bytes or words
        JE      QueueFullDivide     ;divide by two if elements are words
        JMP     QueueFullCheck      ;otherwise max_elements = array_size
        
QueueFullDivide:                    ;if elements are words, divide by two
        SHR     AX, 1               ;divide length by two to get max elements
        ;JMP    QueueFullCheck
        
QueueFullCheck:
        CMP     [SI].filled, AX     ;compare filled elements with max_elements
        POP     AX                  ;restore AX
        RET

QueueFull       ENDP

;Name:              DeQueue

;Description:       This function removes an element from the head of the queue.
;                   If each element is a word, then the element is moved to AX.
;                   If the element is a byte, the element is loaded to AL. If 
;                   the queue is empty, the function waits until something is 
;                   placed in the queue before removing it and storing it in AX. 
;                   The function does not return until there is an element  
;                   in the queue to remove. The function finally increments 
;                   the head pointer and decrements the number of 
;                   filled elements.
;
;Operation:         The function calls the function QueueEmpty to check 
;                   if there are any elements in the queue. If the queue is 
;                   empty, the  function loops infinitely until an element 
;                   is placed in the queue. The function then multiplies the 
;                   value of the head index by the size of each element to 
;                   find how far from the beginning of the queue array the 
;                   head pointer is. The function then loads an element 
;                   into either AX or AL, corresponding to loading a word or 
;                   a byte. The function increments the head pointer and  
;                   decrements the attribute “filled.” The function calculates 
;                   the maximum number of elements that can be stored in the 
;                   queue by dividing length by the attribute size. To 
;                   handle wraparound, mod (max_elements) is taken of the 
;                   head index.
;                   
;
;Arguments:         address (DS:SI) - starting location for the queue 
;                   
;Return Values:     AL/AX - value at the head of the queue. Will return a byte 
;                   to AL if the size of each element is a byte, and will
;                   return the word to AX if the size of each element is
;                   a word.
;Local Variables:   head_pointer (BX) - holds address of the head
;                   offset (AX) - the product of the size of each queue element
;                            (in bytes) and the value of the head index. Used to
;                            calculate the location of the head pointer
;                
;Output:            None

;Error Handling:    None
;
;Stack Depth:       1 word
;
;Algorithms:        None
;
;Known Bugs:        None
;
;Limitations:       None
;
;Author:            Timothy Liu
;
;Last modified:     October 21, 2015


DeQueue         PROC    NEAR
                PUBLIC  DeQueue
DeQueueWait:
        CALL    QueueEmpty              ;check if the queue is empty
        JZ      DeQueueWait             ;wait and loop if queue is empty
        ;JMP     DeQueueOffset          ;If not empty, go to next label
        
DeQueueOffset:
        PUSH    BX                      ;don't trash the BX register
        MOV     AX, [SI].head           ;move head index to AX
        CMP     [SI].word_byte, 2       ;check the size of the element
        JE      DeQueueDoubleOffset     ;double offset size if element is word
        JMP     DeQueueFindHead         ;otherwise go find the head
        
DeQueueDoubleOffset:
        SHL     AX, 1                   ;double offset if the elements are words
        ;JMP    DeQueueFindHead         ;
        
DeQueueFindHead:
        LEA     BX, [SI].content        ;store location of start of array in BX
        ADD     BX, AX                  ;head pointer is at start + offset
        ;JMP    DeQueueCheckSize
        
DeQueueCheckSize:
        CMP     [SI].word_byte, 1       ;check size of element
        JE      DeQueueByte             ;if byte, go to DeQueueByte
        JMP     DeQueueWord             ;otherwise, go to DeQueueWord
        
DeQueueByte:                            ;if element is a byte
        MOV     AL, [BX]                ;put value at head in AL
        JMP    DeQueueUpdateHead
     
DeQueueWord:                            ;if element is a word
        MOV     AX, [BX]                ;put value at head in AX
        ;JMP    DeQueueUpdateHead
        
DeQueueUpdateHead:
        INC     [SI].head               ;increment head index
        CMP     [SI].word_byte, 1       ;check if size is a byte
        JE      DeQueueModByte          ;mod with 1024 if byte
        JMP     DeQueueModWord          ;mod with 512 if word
        
DeQueueModByte:
        AND     [SI].head, ModByteMask  ;take mod 1024
        JMP     DeQueueEnd              ;

DeQueueModWord:
        AND     [SI].head, ModWordMask  ;take mod 512
        ;JMP    DeQueueEnd
        
DeQueueEnd:
        DEC     [SI].filled             ;one fewer element filled
        POP     BX                      ;restore DX
        RET
                        
DeQueue          EndP

;Name:                EnQueue
;
;Description:         This function adds an element at the tail of the queue.
;                     If the element is a byte, the element is passed in through
;                     AL. If the element is a word, it is passed in through AX.
;                     The function does not pass in the value if the queue is
;                     full. If the queue is full, the function waits until the
;                     queue is not full to add the element at the tail. The tail
;                     index is then incremented and the number of filled 
;                     elements is incremented. 

;Operation:           The function calls the function QueueFull to check if the
;                     queue is full. If the queue is full, the function loops
;                     infinitely until a space opens. The function then 
;                     multiplies the value of the tail index by the size of 
;                     each element to find how far from the beginning of the 
;                     queue array the tail pointer is. The function then loads 
;                     the value from AL or AX into the queue and increments 
;                     the tail pointer. The function also increments the 
;                     attribute “filled.” To handle wraparound,  the maximum 
;                     number of elements that can be stored in the queue
;                     is calculated by dividing the length by the attribute size.
;                     The mod (max_elements) of the tail index is taken to handle
;                     wraparound. 
;                     
;Arguments:           address (DS:SI) - starting location for the queue
;                     value (AL/AX) - value to be added to the queue
;               
;Return Values:       None
;
;Local Variables:     max_elements - the maximum number of elements that can be
;                                    in the queue
;                     offset - the product of the size of each queue element
;                              (in bytes) and the value of the head index. Used
;                              to calculate the location of the tail pointer
;                
;Output:              None
;
;Error Handling:      None
;
;Stack Depth:         0 words
;
;Algorithms:          None
;
;Known Bugs:          None
;
;Limitations:         None
;
;Author:            Timothy Liu
;
;Last modified:     October 21, 2015

EnQueue         PROC    NEAR
                PUBLIC  EnQueue

EnQueueLoop:
        CALL    QueueFull               ;check if queue is full
        JZ      EnQueueLoop             ;if so, wait and loop
        ;JMP     EnQueueOffset           ;otherwise, go calculate the offset

EnQueueOffset:
        PUSH    DX                      ;save DX
        PUSH    BX                      ;save BX
        MOV     DX, [SI].tail           ;put tail index in DX
        CMP     [SI].word_byte, 2       ;check if size is a word
        JE      EnQueueDoubleOffset     ;if so, go double offset size
        JMP     EnQueueFindTail         ;otherwise, go find the tail address
        
EnQueueDoubleOffset:
        SHL     DX, 1                   ;double offset size
        ;JMP    EnQueueFindTail

EnQueueFindTail:
        LEA     BX, [SI].content        ;don't change value of [SI].content
        ADD     BX, DX                  ;tail pointer is start + offset
        ;JMP    EnQueueCheckSize
        
EnQueueCheckSize:
        CMP     [SI].word_byte, 1       ;check size of element
        JE      EnQueueByte             ;if byte, go to EnQueueByte
        JMP     EnQueueWord             ;otherwise, go to EnQueueWord
        
EnQueueByte:
        MOV     [BX], AL                ;move AL to address at DX
        JMP     EnQueueUpdateTail       ;go update the tail
        
EnQueueWord:
        MOV     [BX], AX                ;move AX to address at DX
        ;JMP    EnQueueUpdateTail       ;go update the tail
       
EnQueueUpdateTail:
        INC     [SI].tail               ;increment the tail index
        CMP     [SI].word_byte, 1       ;check if size is a byte
        JE      EnQueueModByte          ;mod with 1024 if byte
        JMP     EnQueueModWord          ;mod with 512 if word

EnQueueModByte:
        AND     [SI].tail, ModByteMask  ;take mod 1024
        JMP     EnQueueEnd              ;go to end
        
EnQueueModWord:
        AND     [SI].tail, ModWordMask  ;take mod 512
        ;JMP    EnQueueEnd              ;go to end

EnQueueEnd:
        INC     [SI].filled             ;increment elements filled
        POP     BX                      ;restore BX
        POP     DX                      ;restore DX
        RET
                
EnQueue         ENDP

CODE    ENDS


        END
