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
;    5/16/16   Tim Liu    Wrote Get_blocks without error checking
;    5/17/16   Tim Liu    CheckIDEPhysical uses DH DL instead of AH/AL
;    


; local include files
$INCLUDE(IDE.INC)
$INCLUDE(GENERAL.INC)

CGROUP    GROUP    CODE
DGROUP    GROUP    DATA


CODE SEGMENT PUBLIC 'CODE'

        ASSUME  CS:CGROUP, DS:DGROUP 

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
;                    reads the IDE status register and masks the bits with
;                    IDEBitMask which is passed through DH. The function
;                    then compares the result to ReadyMask passed in DL.
;                    If the value is the same, then the
;                    function returns and restores the registers. If the
;                    IDE status register is not ready, then the function
;                    loops repeatedly until the IDE is ready.
;
;Arguments:          ReadyMask (DL)  - bit pattern indicating ready
;                    IDEBitMask (DH) - bit mask ANDed with the status register
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
;Last Modified       5/17/16

CheckIDEBusy    PROC    NEAR

CheckIDEBusyStart:                      ;starting label
    PUSH ES
    PUSH SI                             ;save registers
    PUSH BX

CheckIDEBusyAddress:                    ;set up address of status register
    MOV  BX, IDESegment
    MOV  ES, BX                         ;segment of the IDE Status register
    MOV  SI, IDEStatusOffset            ;offset of the IDE status register

CheckIDEBusyLoop:                       ;loop reading the status register
    MOV  BL, ES:[SI]                    ;read the status register
    AND  BL, DH                         ;bit mask passed in AH
    CMP  BL, DL                         ;check if the register is ready
    JE   CheckIDEBusyDone               ;IDE ready - done
    JMP  CheckIDEBusyLoop               ;otherwise keep looping until ready

CheckIDEBusyDone:
    POP   BX
    POP   SI
    POP   ES
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
;Local Variables:    CX - number of sectors left to read
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
;       CheckBusyFlag(LBA)                 ;write to LBA addresses
;       LBA7:0 = BP + 4
;       CheckBusyFlag(LBA)
;       LBA15:8 = BP + 5
;       CheckBusyFlag(LBA)
;       LBA23:16 = BP + 6
;
;       AL = BP + 7                           ;access LBA 24:31
;       AL = BitMask(AL)                      ;apply bit mask
;       CheckBusyFlag(DeviceLBA)
;       DeviceLBA = AL                        ;write to DeviceLBA register
;
;       CheckBusyFlag(Command)                ;
;       Write READ SECTOR Command             ;execute DMA
;
;       CalculatePhysical()                   ;calculate the physical address
;                                             ;from the segment and offset
;       DxDSTH = CP1                          ;write the destination addresses
;       DxDSTL = CP2
;       DxSRCH = DxSRCHVal                    ;always the same value
;       DxSRCL = DxSRCLVal                    ;start address of MCS2
;       CheckBusyFlag(Ready to Transfer)      ;check ready for data transfer
;       DxCON = DxCONVal                      ;write to DxCON and start DMA
           
    
    

Get_Blocks        PROC    NEAR
                  PUBLIC  Get_Blocks

GetBlocksStart:                               ;starting label
    PUSH    BP                                ;save base pointer
    MOV     SP, BP                            ;use BP to index into stack
    PUSH    AX                                ;save registers
    PUSH    BX
    PUSH    CX
    PUSH    DX

GetBlocksLoadRemaining:                       ;load number of sectors remaining
    MOV    CX, SS:[BP+8]                      ;total sectors to read
    MOV    SectorsLeft, CX                    ;write sectors to read in data seg

GetBlocksCheckLeft:
    CMP    SectorsLeft, 0                     ;check if no sectors left
    JE     GetBlocksWriteRegs                 ;finished - go to end

GetBlocksWriteRegs:                           ;set up LBA address
    MOV    AX, IDESegment
    MOV    ES, AX                             ;segment of IDE register
    MOV    AX, 0                              ;number of registers written to

GetBlocksIDELoop:                             ;loop writing instructions to IDE
    CMP    AX, NumIDERegisters                ;number of IDE registers to write to
    JE     GetBlocksReadSector                ;done writing - send read sector com.
    IMUL   BX, AX, SIZE IDERegEntry           ;calculate table offset

GetBlocksPrepReg:                             ;prepare to a register
    MOV    DH, CS:IDERegTable[BX].FlagMask    ;look up bit mask
    MOV    DL, CS:IDERegTable[BX].IDEReady    ;value indicating IDE is ready
    CALL   CheckIDEBusy
    MOV    SI, CS:IDERegTable[BX].RegOffset   ;offset of IDE register
    CMP    CS:IDERegTable[BX].BPIndex, NoStackArg    ;check if reg value is stack arg
    JE     GetBlocksConstant                  ;go to label to prepare constant command
    ;JMP    GetBlocksStackArg                  ;otherwise it’s a stack argument

GetBlocksStackArg:                            ;argument is on the stack
    MOV    AX, BP                             ;save base pointer
    ADD    BP, CS:IDERegTable[BX].BPIndex     ;change base pointer to point to address
    MOV    DL, SS:[BP]                        ;load the argument
    OR     DL, CS:IDERegTable[BX].ArgMask     ;apply mask
    MOV    BP, AX                             ;restore base pointer
    JMP    GetBlocksOutput                    ;go write to the register

GetBlocksConstant
    MOV    DL, CS:IDERegTable[BX].ConstComm   ;write the constant command

GetBlocksOutput:
    MOV    ES:[SI], DL                        ;output to the IDE register - value in DL
    INC    AX                                 ;one more command written
    JMP    GetBlocksIDELoop                   ;back to top of loop for writing to regs

    

    MOV    AH, IDESCRdyMask                   ;load arguments to check sector
    MOV    AL, IDESCRdy                       ;count ready
    CALL   CheckIDEBusy                       ;proc returns when IDE is ready
    MOV    SI, IDESCOffset                    ;sector count register offset
    MOV    ES:[SI], SecPerTran                ;sectors to write per transfer

    MOV    AH, IDELBARdyMask                  ;load arguments to check if IDE
    MOV    AL, IDELBARdy                      ;is ready to be read
    CALL   CheckIDEBusy                       ;don’t return until IDE is ready
    MOV    SI, IDELBA70Offset                 ;LBA(0:7) register offset
    MOV    DL, SS:[BP+4]                      ;copy LBA(0:7) value to register
    MOV    ES:[SI], DL                        ;write LBA(0:7) to IDE register

    MOV    AH, IDELBARdyMask                  ;load arguments to check if IDE
    MOV    AL, IDELBARdy                      ;is ready to be read
    CALL   CheckIDEBusy                       ;don’t return until IDE is ready
    MOV    SI, IDELBA158Offset                ;LBA(8:15) register offset
    MOV    DL, SS:[BP+5]                      ;copy LBA(8:15) value to register
    MOV    ES:[SI], DL                        ;write LBA(8:15) to IDE register

    MOV    AH, IDELBARdyMask                  ;load arguments to check if IDE
    MOV    AL, IDELBARdy                      ;is ready to be read
    CALL   CheckIDEBusy                       ;don’t return until IDE is ready
    MOV    SI, IDELBA2316Offset               ;LBA(16:23) register offset
    MOV    DL, SS:[BP+6]                      ;copy LBA(16:23) value to register
    MOV    ES:[SI], DL                        ;write LBA(16:23) to IDE register

    MOV    AH, IDEDeviceLBARdyMask            ;load arguments to check if IDE
    MOV    AL, IDEDeviceLBARdy                ;is ready to be read
    CALL   CheckIDEBusy                       ;don’t return until IDE is ready
    MOV    SI, IDELBADOffset                  ;Device/LBA register offset
    MOV    DL, SS:[BP+7]                      ;copy LBA(24:31) value to register
    OR     DL, IDEDLBAMask                    ;apply mask to indicate LBA addressing
    MOV    ES:[SI], DL                        ;write LBA(16:23) to IDE register

GetBlocksReadSector:                          ;write IDE “read sector” command
    MOV   AH, IDEComRdyMask                   ;load arguments to check that
    MOV   AL, IDECommandRdy                   ;IDE can accept commands
    CALL  CheckIDEBusy                        ;check if IDE is busy
    MOV   ES, IDESegment                      ;load segment of IDE Command
    MOV   SI, IDECommandOffset                ;IDE Command register offset
    MOV   ES:[SI], IDEReadSector              ;send “read sector” command

GetBlocksPrepareDMA:                          ;set up DMA control registers
    MOV   AX, SS                              ;copy stack segment to ES
    MOV   ES, AX
    MOV   SI, BP+10                           ;pointer to destination address
    CALL  CalculatePhysical                   ;physical address returned in CX, BX

    MOV   DX, D0DSTH                          ;address of high destination pointer
    MOV   AX, CX                              ;copy high 4 bits of physical address
    OUT   AX, DX                              ;write to peripheral control block

    MOV   DX, D0DSTL                          ;address of low destination pointer
    MOV   AX, BX                              ;copy low 16 bits of physical address
    OUT   AX, DX                              ;write to peripheral control block

    MOV   DX, D0SRCH                          ;address of high source pointer
    MOV   AX, DxSRCHVal                       ;high 16 bits of source phy address
    OUT   AX, DX                              ;write the high source pointer
    
    MOV   DX, D0SRCL                          ;address of low source pointer
    MOV   AX, DxSRCLVal                       ;low 16 bits of source phy address
    OUT   AX, DX                              ;write the low source pointer

    MOV   DX, D0TC                            ;address of DMA transfer count
    MOV   AX, NumTransfers                    ;value to write to transfer count
    OUT   AX, DX                              ;write to transfer count register

GetBlocksCheckTransfer:                       ;check if IDE is ready to transfer data
    MOV   AH, IDETransferMask                 ;mask out unimportant status bits
    MOV   AL, IDETransfer                     ;value to compare to
    CALL  CheckIDEBusy                        ;check if IDE ready

GetBlocksDMA:
    MOV   DX, DxCon                           ;address of DxCon register
    MOV   AX, DxConVal                        ;value to write to DxCon
    OUT   AX, DX                              ;write to DMA to initiate transfer

GetBlocksRecalculate:
    MOV   AX, SS                              ;copy stack segment to extra segment
    MOV   ES, AX
    MOV   SI, BP+4                            ;pointer to LBA start block
    MOV   AX, NumTransfers                    ;amount to increment address by
    CALL  Add32Bit                            ;recalculate the LBA start block
    
    MOV   SI, BP+10                           ;pointer to destination pointer
    CALL  Add32Bit                            ;recalculate destination pointer
    DEC   SectorsLeft                         ;one fewer sector to read
    JMP   GetBlocksCheckLeft                  ;jump to top of loop
  
GetBlocksDone:
    POP    DX
    POP    CX
    POP    BX
    POP    AX
    POP    BP
    RET

Get_Blocks      ENDP

IDERegTable        LABEL    IDERegEntry

    IDERegEntry<SCRdyMask   , SCRdy   , SCOffset     , NoStackArg, SecPerTran   , BlankMask> ;sector count register
    IDERegEntry<LBARdyMask  , LBARdy  , LBA70Offset  , LBA07     , NoConstant   , BlankMask> ;LBA (0:7) register
    IDERegEntry<LBARdyMask  , LBARdy  , LBA1580Offset, LBA815    , NoConstant   , BlankMask> ;LBA (8:15) register
    IDERegEntry<LBARdyMask  , LBARdy  , LBA2316Offset, LBA2316   , NoConstant   , BlankMask> ;LBA (16:23) register
    IDERegEntry<DeLBARdyMask, DeLBARdy, DeLBAOffset  , DeLBA     , NoConstant   , DeLBAMask> ;Device LBA register
    IDERegEntry<ComRdyMask  , ComRdy  , ComOffset    , NoStackArg, ReadSector   , BlankMask> ;IDE Command register


CODE ENDS

DATA    SEGMENT PUBLIC  'DATA'

SectorsLeft        DW    ?     ;how many more sectors left to read


DATA ENDS

        END