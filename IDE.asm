    NAME    IDE
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;                                                                            ;
;                                   IDE Code                                 ;
;                             IDE Related Functions                          ;
;                                   EE/CS 52                                 ;
;                                                                            ;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

; Description: This file contains the functions relating to the IDE software.


; Table of Contents:
;
;    Add32Bit          - adds value to 32 bit value
;    CalculatePhysical - calculates physical address from segment/offset
;    CheckIDEBusy      - checks if the IDE is busy
;    Get_Blocks        - retrieves number of blocks from IDE

; Revision History:
;    5/8/16    Tim Liu    Created file
;    5/9/16    Tim Liu    Created skeleton of Get_blocks
;    5/12/16   Tim Liu    Outlined Get_blocks
;    5/13/16   Tim Liu    Wrote Add32Bit
;    5/13/16   Tim Liu    Wrote outline for CalculatePhysical
;    5/13/16   Tim Liu    Wrote CalculatePhysical
;    5/14/16   Tim Liu    Fixed bugs in Add32Bit and Calculate Physical


; local include files
$INCLUDE(IDE.INC)
$INCLUDE(GENERAL.INC)

CGROUP    GROUP    CODE


CODE SEGMENT PUBLIC 'CODE'

        ASSUME  CS:CGROUP 

;external function declarations

;Name:               Add32Bit
;
;Description:        This function adds a value to a 32 bit unsigned value in
;                    memory. The function is passed two arguments - the 
;                    value to add in AX and the address of the 32 bit value
;                    in ES:SI. 
; 
;Operation:          The function adds AX to the low word pointed to by
;                    ES:SI. The function then adds with carry 0 to the
;                    high word pointed to by ES:SI+1 to add the carry
;                    flag.
;
;Arguments:          AX - value to add
;                    ES:SI - address of 32 bit value
;
;Return Values:      None
;
;Local Variables:    None
;
;Shared Variables:   None
;
;Output:             None
;
;Error Handling:     None
;
;Algorithms:         None
;
;Registers Used:     SI, flags register
;
;Known Bugs:         None
;
;Limitations:        None
;
;Author:             Timothy Liu
;
;Last Modified       5/13/16

Add32Bit        PROC    NEAR
                PUBLIC  Add32Bit

Add32BitStart:                           ;starting label
    ADD    ES:[SI], AX                   ;add value to low word
    ADC    WORD PTR ES:[SI+2], 0         ;add the carry flag

Add32BitEnd:
    RET                                  ;function done


ADD32Bit    ENDP

;Name:               CalculatePhysical
;
;Description:        This function calculates the physical address from
;                    the segment and the offset. The segment and offset
;                    are passed to the function through ES:SI. The
;                    function writes the 20 bit physical address to
;                    BX and CX with the low 16 bits in BX and the high
;                    nibble in CX.
; 
;Operation:          The function copies the segment in ES:[SI+1] to CX.
;                    The function then shifts CX so that the high order
;                    nibble is in the lowest nibble and the three highest
;                    nibbles are clear. The function then shifts the
;                    high order word in ES:[SI] to the left to multiply
;                    it by 16. The function places the lower order word
;                    ES:[SI] in BX and adds the high order word ES:[SI+1]
;                    to the low order word in BX. Finally, the function
;                    adds with carry 0 to CX to carry the highest order
;                    nibble. The function then returns with the low nibble
;                    in BX and the high nibble in CX.
;
;Arguments:          ES:SI - 32 bit segment and offset
;
;Return Values:      BX - low 16 bits of physical address
;                    CX - high 4 bits of physical address
;
;Local Variables:    None
;
;Shared Variables:   None
;
;Output:             None
;
;Error Handling:     None
;
;Algorithms:         None
;
;Registers Used:     BX, CX
;
;Known Bugs:         None
;
;Limitations:        None
;
;Author:             Timothy Liu
;
;Last Modified       5/13/16

CalculatePhysical        PROC    NEAR

CalculatePhysicalStart:                  ;starting label
    PUSH    AX                           ;register
    PUSH    DX                           ;save register

CalculatePhysicalCopy:                   ;copy seg/offset to register
    MOV     BX, ES:[SI]                  ;copy offset to register
    MOV     CX, ES:[SI+2]                ;copy segment to register
    MOV     DX, ES:[SI+2]                ;second copy of segment

CalculatePhysicalShift:                  ;shift registers to prepare for add
    SHR     CX, 3*BitsPerNibble          ;high order of seg in lowest nibble
    SHL     DX, BitsPerNibble            ;shift copy of segment by one
                                         ;nibble to prepare for add

CalculatePhysicalAdd:                    ;calculate the 20 bit address
    ADD    BX, DX                        ;calculate low 16 bits of address
    ADC    CX, 0                         ;add carry bit to highest nibble

CalculatePhysicalDone:                   ;end of function
   POP     DX                            ;restore registers
   POP     AX
   RET

CalculatePhysical    ENDP




;Name:               CheckIDEBusy
;
;Description:        This function checks the IDE to see if it is busy.
;                    The function loops repeatedly checking the IDE until
;                    it is no longer busy. The function does not return
;                    until the IDE is ready. 
; 
;Operation:          The function loads the segment of the IDE status register
;                    into ES and the offset into SI. The function then
;                    reads the IDE status register and compares the value
;                    to IDEReady. If the value is the same, then the
;                    function returns and restores the registers. If the
;                    IDE status register is not ready, then the function
;                    loops repeatedly until the IDE is ready.
;
;Arguments:          None
;
;Return Values:      None
;
;Local Variables:    None
;
;Shared Variables:   None
;
;Output:             None
;
;Error Handling:     None
;
;Algorithms:         None
;
;Registers Used:     ES
;
;Known Bugs:         None
;
;Limitations:        None
;
;Author:             Timothy Liu
;
;Last Modified       5/13/16

CheckIDEBusy    PROC    NEAR

CheckIDEBusyStart:                      ;starting label
    PUSH SI                             ;save registers
    PUSH AX

CheckIDEBusyAddress:                    ;set up address of status register
    MOV  AX, IDEStatusSeg
    MOV  ES, AX                         ;segment of the IDE Status register
    MOV  SI, IDEStatusOffset            ;offset of the IDE status register

CheckIDEBusyLoop:                       ;loop reading the status register
    MOV  AL, ES:[SI]                    ;read the status register
    AND  AL, IDEStatusMask              ;only interested in some bits
    CMP  AL, IDEReady                   ;check if the register is ready
    JE   CheckIDEBusyDone               ;IDE ready - done
    JMP  CheckIDEBusyLoop               ;otherwise keep looping until ready

CheckIDEBusyDone:
    POP   AX
    POP   SI
    RET


CheckIDEBusy    ENDP


;Name:       Get_Blocks(unsigned long int, int, unsigned short int far *)

;
;Description:        This function retrieves a number of blocks from the
;                    IDE and transfers it to a specified address. The
;                    function is passed three arguments - the address
;                    of the blocks, the number of blocks, and the 
;                    address to write to. The function reads from the 
;                    IDE and performs a DMA transfer to the specified
;                    location. The function returns the number of blocks
;                    actually read.
; 
;Operation:          The function first uses BP to index into the stacks
;                    and retrieve the arguments. The function loops through
;                    the top of the stack and copies the arguments to the
;                    array GetBlockArgs. The function then writes to the 
;                    command block registers. The function writes the 
;                    number of sectors to transfer, the LBA address, and 
;                    specifies to use LBA addressing. The function looks
;                    up the addresses to write the commands to
;                    in IDEAddressTable. The function then loops checking 
;                    the ready to transfer data flag of IDEStatusReg.
;                    Once the flag is clear, the function writes to the
;                    command register IDEDMA to initiate DMA. The
;                    function writes the destination pointer address passed
;                    as the third argument to DxDSTH and DxDSTL. The function
;                    writes IDEStartAddress to DxSRCL. To initiate the
;                    DMA, the procedure writes DxConVal to DxCon. The
;                    function (???) somehow returns the number of blocks 
;                    read in AX and restores the registers
;                    
;
;Arguments:          StartBlock(unsigned long int) - starting logical block
;                    to read from
;
;                    NumBlocks(int) - number of blocks to retrieve
;
;                    DestinationPointer(unsigned short in far *) -
;                    address of destination
;                      
;
;Return Values:      AX - number of blocks actually read
;
;Local Variables:    None
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
;Author:             Timothy Liu
;
;Last Modified       5/12/16
;
;Outline
;Get_Blocks(StartBlock, NumBlocks, Destination)
;    Save BP                           ;set up indexing into stack to pull arg
;    BP = SP
;    Save other registers
;    While NumBlocks > 0                   ;loop writing each block
;       Add32Bit(BP+4, BP+6)               ;function to recalculate the LBA
;                                          ;after incrementing the sector
;                                          ;add 256 to low and add carry bit
;       Add32Bit(BP+10, BP+12)             ;recalculate destination pointer
;
;       CheckBusyFlag()                      ;write to LBA addresses
;       LBA7:0 = BP + 4
;       CheckBusyFlag()
;       LBA15:8 = BP + 5
;       CheckBusyFlag()
;       LBA23:16 = BP + 6
;
;       AL = BP + 7                           ;access LBA 24:31
;       AL = BitMask(AL)                      ;apply bit mask
;       CheckBusyFlag()
;       DeviceLBA = AL                        ;write to DeviceLBA register
;
;       CheckBusyFlag()                       ;
;       Write READ SECTOR Command             ;execute DMA
;
;       CalculatePhysical()                   ;calculate the physical address
;                                             ;from the segment and offset
;       DxDSTH = CP1                          ;write the destination addresses
;       DxDSTL = CP2
;       DxSRCH = DxSRCHVal                    ;always the same value
;       DxSRCL = DxSRCLVal                    ;start address of MCS2
;       DxCON = DxCONVal                      ;write to DxCON and start DMA
           
    
    

Get_Blocks        PROC    NEAR
                  PUBLIC  Get_Blocks

Get_BlocksStart:                               ;starting label
    RET


Get_Blocks      ENDP


CODE ENDS

        END