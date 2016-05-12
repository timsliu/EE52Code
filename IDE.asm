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
;    Get_Blocks - retrieves number of blocks from IDE

; Revision History:
;    5/8/16    Tim Liu    Created file
;    5/9/16    Tim Liu    Created skeleton of Get_blocks


; local include files
$(IDE.INC)

CGROUP    GROUP    CODE


CODE SEGMENT PUBLIC 'CODE'

        ASSUME  CS:CGROUP 

;external function declarations

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
;Last Modified       5/9/16

Get_Blocks        PROC    NEAR
                  PUBLIC  Get_Blocks

Get_Blocks


Get_Blocks      ENDP


CODE ENDS

        END