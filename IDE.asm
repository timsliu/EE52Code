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
;    SetupDMA          - sets up the DMA control registers
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
;    5/17/16   Tim Liu    Rewrote Get_blocks to use a loop
;    5/17/16   Tim Liu    Wrote SetupDMA function
;    5/17/16   Tim Liu    Updated comments
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
;Registers Used:     flags register
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
;Registers Used:     None
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
    AND  BL, DH                         ;bit mask passed in DH
    CMP  BL, DL                         ;check if the register is ready
    JE   CheckIDEBusyDone               ;IDE ready - done
    JMP  CheckIDEBusyLoop               ;otherwise keep looping until ready

CheckIDEBusyDone:
    POP   BX
    POP   SI
    POP   ES
    RET


CheckIDEBusy    ENDP

;Name:               SetupDMA
;
;Description:        This writes to 5 DMA control registers to set
;                    up a DMA transfer.
; 
;Operation:          The function first calculates the physical
;                    address of the destination pointer by calling
;                    the function CalculatePhysical. The function
;                    then writes to D0STL, D0SRCH, D0SRCL, and D0TC.
;                    The function restores all registers and returns
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
;Registers Used:     None
;
;Known Bugs:         None
;
;Limitations:        None
;
;Author:             Timothy Liu
;
;Last Modified:      5/17/16

SetupDMA        PROC    NEAR

SetupDMAStart:                                ;save registers
    PUSH    AX
    PUSH    BX
    PUSH    CX
    PUSH    DX
    PUSH    SI
    PUSH    ES

SetupDMAWrite:                                ;write to DMA control registers
    MOV   AX, SS                              ;copy stack segment to ES
    MOV   ES, AX
    MOV   SI, BP                              ;copy base pointer
    ADD   SI, DestPointer                     ;calculate address of destination ptr
    CALL  CalculatePhysical                   ;physical address returned in CX, BX

    MOV   DX, D0DSTH                          ;address of high destination pointer
    MOV   AX, CX                              ;copy high 4 bits of physical address
    OUT   DX, AX                              ;write to peripheral control block

    MOV   DX, D0DSTL                          ;address of low destination pointer
    MOV   AX, BX                              ;copy low 16 bits of physical address
    OUT   DX, AX                              ;write to peripheral control block

    MOV   DX, D0SRCH                          ;address of high source pointer
    MOV   AX, D0SRCHVal                       ;high 16 bits of source phy address
    OUT   DX, AX                              ;write the high source pointer
    
    MOV   DX, D0SRCL                          ;address of low source pointer
    MOV   AX, D0SRCLVal                       ;low 16 bits of source phy address
    OUT   DX, AX                              ;write the low source pointer

    MOV   DX, D0TC                            ;address of DMA transfer count
    MOV   AX, NumTransfers                    ;value to write to transfer count
    OUT   DX, AX                              ;write to transfer count register

SetupDMADone:                                 ;restore registers and return
    POP    ES
    POP    SI
    POP    DX
    POP    CX
    POP    BX
    POP    AX
    RET

SetupDMA        ENDP


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
;Operation:          The function first saves the registers to the stack.
;                    The function then uses BP to index into the stack and
;                    copy the number of sectors to read to SectorsRemaining
;                    and sets SectorsRead to 0. The function then loops through
;                    IDERegTable and writes to the IDE registers. The function
;                    calls CheckIDEBusy to check that the appropriate 
;                    status flags are set before writing to the IDE register.
;                    The function indexes into the stack and copies the 
;                    IDE register values to write into the stack or it
;                    writes a constant value to the IDE register, depending on
;                    register. After writing to the IDE registers, the function
;                    calls SetupDMA to set up the DMA control registers, except
;                    for D0Con. The function checks that the IDE is ready to
;                    transfer data and then writes to D0Con to initiate the
;                    DMA transfer. After the transfer is complete, the function 
;                    increments SectorsRead and recalculates the DMA
;                    destination pointer and the LBA. The function loops
;                    repeatedly until all sectors have been read. The function
;                    returns with the number of sectors read in AX.                 
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
;Shared Variables:   SectorsRemaining (R/W) - number of sectors left to read
;                    SectorsRead(R/W) - sectors the function has read
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
;Last Modified       5/17/16   

Get_Blocks        PROC    NEAR
                  PUBLIC  Get_Blocks

GetBlocksStart:                               ;starting label
    PUSH    BP                                ;save base pointer
    MOV     BP, SP                            ;use BP to index into stack
    PUSH    BX                                ;save registers
    PUSH    CX
    PUSH    DX
    PUSH    SI

GetBlocksLoadRemaining:                       ;load number of sectors remaining
    MOV    CX, SS:[BP+8]                      ;total sectors to read
    MOV    SectorsRemaining, CX               ;shared variable number of sectors
    MOV    SectorsRead, 0                     ;no sectors have been read

GetBlocksCheckLeft:
    CMP    SectorsRemaining, 0                ;check if no sectors left
    JE     GetBlocksDone                      ;finished - go to end

GetBlocksWriteSegment:                        ;load IDE segment into ES
    MOV    AX, IDESegment
    MOV    ES, AX                             ;segment of IDE register
    MOV    AX, 0                              ;number of registers written to

GetBlocksIDELoop:                             ;loop writing instructions to IDE
    CMP    AX, NumIDERegisters                ;number of IDE registers written to
    JE     GetBlocksPrepareDMA                ;done writing - send read sector com.
    IMUL   BX, AX, SIZE IDERegEntry           ;calculate table offset

GetBlocksPrepReg:                             ;prepare to a register
    MOV    DH, CS:IDERegTable[BX].FlagMask    ;look up bit mask
    MOV    DL, CS:IDERegTable[BX].IDEReady    ;value indicating IDE is ready
    CALL   CheckIDEBusy                       ;return when IDE is not busy
    MOV    SI, CS:IDERegTable[BX].RegOffset   ;offset of IDE register
    CMP    CS:IDERegTable[BX].BPIndex, NoStackArg    ;check if reg value is stack arg
    JE     GetBlocksConstant                  ;go to label to prepare constant command
    ;JMP   GetBlocksStackArg                  ;otherwise it’s a stack argument

GetBlocksStackArg:                            ;argument is on the stack
    PUSH   BP                                 ;save base pointer
    ADD    BP, CS:IDERegTable[BX].BPIndex     ;change base pointer to point to address
    MOV    DL, SS:[BP]                        ;load the argument
    OR     DL, CS:IDERegTable[BX].ArgMask     ;apply mask
    POP    BP                                 ;restore the base pointer
    JMP    GetBlocksOutput                    ;go write to the register

GetBlocksConstant:
    MOV    DL, CS:IDERegTable[BX].ConstComm   ;write the constant command

GetBlocksOutput:
    MOV    ES:[SI], DL                        ;output to the IDE register - value in DL
    INC    AX                                 ;one more command written
    JMP    GetBlocksIDELoop                   ;back to top of loop for writing to regs

GetBlocksPrepareDMA:                          ;set up DMA control registers
    CALL   SetupDMA                           ;call function to set up DMA registers

GetBlocksCheckTransfer:                       ;check if IDE is ready to transfer data
    MOV   DH, IDETransferMask                 ;mask out unimportant status bits
    MOV   DL, IDETransfer                     ;value to compare to
    CALL  CheckIDEBusy                        ;return when IDE is ready

GetBlocksDMA:                                 ;write to DxCON and perform DMA
    MOV   DX, D0Con                           ;address of DxCon register
    MOV   AX, D0ConVal                        ;value to write to DxCon
    OUT   DX, AX                              ;write to DMA to initiate transfer
    INC   SectorsRead                         ;one more sector has been read 

GetBlocksRecalculate:                         ;recalculate LBA and destination pointer
    MOV   AX, SS                              ;copy stack segment to extra segment
    MOV   ES, AX
    MOV   SI, BP                              ;pointer to LBA start block
    ADD   SI, LBA07                           ;calculate address of LBA0:7 register
    MOV   AX, SecPerTran                      ;number of blocks read
    CALL  Add32Bit                            ;recalculate the LBA start block
    
    MOV   SI, BP                              ;pointer to destination pointer
    ADD   SI, DestPointer                     ;calculate address of dest. pointer
    MOV   AX, NumTransfers                    ;amount to increment destination pointer
    CALL  Add32Bit                            ;recalculate destination pointer
    DEC   SectorsRemaining                    ;one fewer sector to read
    JMP   GetBlocksCheckLeft                  ;jump to top of loop
  
GetBlocksDone:
    MOV    AX, SectorsRead                    ;return number of sectors read
    POP    SI
    POP    DX                                 ;restore registers
    POP    CX
    POP    BX
    POP    BP
    RET

Get_Blocks      ENDP

; IDERegTable
; Description:   This table contains IDERegEntry structs describing what
;                values to output to the IDE registers. Each table entry
;                corresponds to a different IDE register being written to.
;                The function get_blocks indexes into the table and looks
;                up the values to be written, where to write them to, and 
;                other information.
;
; Last Modified: 5/17/16
;                
; Author:        Timothy Liu
;  
              
IDERegTable        LABEL    IDERegEntry

;   IDERegEntry<FlagMask    , IDEREady, RegOffset    , BPIndex   , ConstComm    , ArgMask  > ;IDERegEntry Struc

    IDERegEntry<SCRdyMask   , SCRdy   , SCOffset     , NoStackArg, SecPerTran   , BlankMask> ;sector count register
    IDERegEntry<LBARdyMask  , LBARdy  , LBA70Offset  , LBA07     , NoConstant   , BlankMask> ;LBA (0:7) register
    IDERegEntry<LBARdyMask  , LBARdy  , LBA158Offset , LBA815    , NoConstant   , BlankMask> ;LBA (8:15) register
    IDERegEntry<LBARdyMask  , LBARdy  , LBA2316Offset, LBA2316   , NoConstant   , BlankMask> ;LBA (16:23) register
    IDERegEntry<DeLBARdyMask, DeLBARdy, DeLBAOffset  , DeLBA     , NoConstant   , DeLBAMask> ;Device LBA register
    IDERegEntry<ComRdyMask  , ComRdy  , ComOffset    , NoStackArg, ReadSector   , BlankMask> ;IDE Command register


CODE ENDS

DATA    SEGMENT PUBLIC  'DATA'

SectorsRemaining    DW    ?      ;sectors left to read
SectorsRead         DW    ?      ;sectors that have been read
DATA    ENDS


        END