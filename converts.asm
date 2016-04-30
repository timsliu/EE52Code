        NAME    CONVERTS

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;                                                                            ;
;                                   CONVERTS                                 ;
;                             Conversion Functions                           ;
;                                   EE/CS 51                                 ;
;                                                                            ;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


; file description including table of contents
;
; Revision History:
;   10/12/15    Tim Liu Wrote functional spec for dec2string
;   10/13/15    Tim Liu Finished writing dec2string
;   10/13/15    Tim Liu Pseudo-code dec2sting & hex2string
;   10/13/15    Tim Liu Wrote functional spec for hex2string
;   10/13/15    Tim Liu Wrote hex2string
;   10/13/15    Tim Liu Changed instructions for push/pop registers
;   10/14/15    Tim Liu Corrected syntax errors, removed error handling
;   10/14/15    Tim Liu Changed jump instructions
;   10/15/15    Tim Liu Removed bug that checked if hex negative

; local include files
$INCLUDE (converts.inc)

CGROUP  GROUP   CODE


CODE    SEGMENT PUBLIC 'CODE'


        ASSUME  CS:CGROUP



; Dec2String
;
; Description:      This function takes a 16 bit signed binary value
;                   as an argument and converts it to decimal. It then
;                   stores a null terminated decimal ASCII representation
;                   as a string starting at a specified address.
;
;
; Operation:        The function begins with the largest power of 10
;                   possible (10000) and divides the argument. The
;                   quotient is a digit, and the remainder is then
;                   divided by the next largest power of ten.
;                   This process is repeated
;                   until the the power of 10 is zero. Each loop iteration
;                   yields the next digit. The digit is converted to ASCII
;                  and stored starting at the specified address.
;
;
; Arguments:        AX - signed 16 bit value to covert to ASCII string
;                   SI - address to start writing ASCII string
;
; Return Value:     SI - string written starting at address DS:SI
;
; Local Variables:  arg(AX) - copy of passed binary value to convert
;                   digit(AX) - computed digit
;                   pwr10(CX) - current power of 10 being computed
;                   address(SI) - address to store string
;           
; Shared Variables: None
; Global Variables: None
;
; Input:        None
; Output:       None
;
; Error Handling:   None
;
; Algorithms:       Repeatedly divide by powers of 10 and get the remainders
;                   Convert to ASCII characters and store
;
; Data Structures:  None
;
; Registers Changed:    flags, AX, BX, CX, DX, SI
; Stack Depth:          16 bytes
;
; Author:       Timothy Liu
; Last Modified:    10/15/15
;
; Pseudo Code
;
;dec_2_string(arg, address)
;   pwr10 = 10000                 # largest possible power of ten
;
;   IF arg < 0:                   # Check if argument is negative
;       *address = '-'            # write a negative sign
;       address ++                # increment address pointer
;       arg *= -1                 # negate the argument
;
;   WHILE (pwr10 > 0):          # loop to get every digit
;       digit = arg/pwr10       # divide to find digit
;       asc = 48 + digit        # ASCII representation
;       *address = asc          # store asc at the address
;       address ++              # increment address pointer
;       arg = arg MOD pwr10     # set next digit
;       pwr10 = pwr10/10        # decrement power of ten
;
;   *address = NULL             # write null character
;   RETURN error, address
    

Dec2String      PROC        NEAR
                PUBLIC      Dec2String

Dec2StringInit:             ;initialization
    PUSHA                   ;push registers to stack
    MOV CX, 10000           ;pwr10 starts at 10^4 (10000's digit)
    CMP AX, 0               ;check if argument is negative
    JL  DecIfNegative       ;write negative sign if negative
    JMP DecEndIfNegative    ;otherwise skip if body

DecIfNegative:              ;write a negative sign
    MOV BX, NEGchar         ;prepare to store negative sign
    MOV [SI], BX            ;store a negative sign at address
    INC SI                  ;increment SI
    NEG AX                  ;negate the argument
    ;JMP    DecEndIfNegative    ;

DecEndIfNegative:
    ;JMP    Dec2StringLoop

Dec2StringLoop:             ;loop getting the digits
    CMP CX, 0               ;check if pwr10 > 0
    JBE EndDec2StringLoop   ;exit if pwr10 is not > 0
    ;JMP    Dec2StringLoopBody  ;otherwise get the next digit

Dec2StringLoopBody:         ;get a digit
    XOR DX, DX              ;DX is high order dividend - set to 0
    DIV CX                  ;divide by pwr10; result stored in AX
    ADD AX, DIGITOffset     ;convert digit in AX to ASCII
    MOV [SI], AL            ;store ASCII rep of digit at address
    INC SI                  ;increment address pointer
    ;JMP DecPrepareNext

DecPrepareNext:             ;update pwr10 and prepare next iteration
    MOV BX, DX              ;save mod16 to BX
    MOV AX, CX              ;load current pwr10 to AX
    MOV CX, 10              ;load divisor into CX
    XOR DX, DX              ;clear DX register
    DIV CX                  ;divide by 10 - value stored in AX
    MOV CX, AX              ;load new power of 10 back to CX
    MOV AX, BX              ;move arg = arg MOD pwr10 into AX
    JMP EndDec2StringLoopBody

EndDec2StringLoopBody:
    JMP Dec2StringLoop      ;keep looping (end check is at the top)


EndDec2StringLoop:
    MOV BX, NULLchar        ;prepare to write null character
    MOV [SI], BL            ;write a null terminated character
    POPA                    ;restore registers
    RET

Dec2String  ENDP




; Hex2String
;
; Description:      This function takes a 16 bit unsigned binary value as an
;                   argument and converts it to hexadecimal. It then stores
;                   a null terminated decimal ASCII representation as a
;                   string starting at a specified address.
;
;
; Operation:        The function begins with the largest power of 16
;                   possible (4096) and divides the argument. The quotient
;                   is a digit, and the remainder is then divided by the
;                   next largest power of ten. This process is repeated
;                   until the the power of 16 is zero. Each loop iteration
;                   yields the next digit. The digit is converted to ASCII
;                   and stored starting at the specified address.
;
;
; Arguments:        AX - signed 16 bit value to covert to ASCII string
;                   SI - address to start writing ASCII string

; Return Value:     SI - string written starting at address specified
;
;
; Local Variables:  arg(AX) - copy of passed binary value to convert
;                   dig(AX) - single digit to be converted
;                   asc(BX) - ASCII character
;                   address(SI) - address to store string
;                   pwr16(CX) - current power of 16 being computed
     
; Shared Variables: None
; Global Variables: None
;
; Input:        None
; Output:       None
;
; Error Handling:   None
;
; Algorithms:       Repeatedly divide by powers of sixteen and convert
;                   to ASCII
; Data Structures:  none
;
; Registers Changed:    flags, AX, BX, CX, DX, SI
; Stack Depth:          16 bytes
;
; Author:       Timothy Liu
; Last Modified:    10/15/15
;
;def hex_2_string(arg, address):
;   pwr16 = 4096                    # largest power of 16
;
;
;   WHILE (pwr16 > 0):              # loop to get each digit
;       digit = arg/pwr16           # divide to find digit
;       IF digit < 16:
;           IF digit < 10:
;               asc = 48 + digit    # ASCII 0-9
;           ELSE:
;               asc = 55 + digit    # ASCII A-F
;       *address = asc              # store asc at address
;       address ++                  # increment  pointer
;       arg = arg MOD pwr16         # remainder becomes arg
;       pwr16 = pwr16/16            # decrement power of 16
;   *address = 'NULL'               # write null character
;   RETURN address

Hex2String      PROC        NEAR
                PUBLIC      Hex2String

Hex2StringInit:             ;initialization
    PUSHA                   ;push all the registers to stack
    MOV CX, 4096            ;start with 16^3 (4096)
    ;JMP Hex2StringLoop     ;

Hex2StringLoop:             ;loop getting the digits
    CMP CX, 0               ;check if pwr16 > 0
    JBE EndHex2StringLoop   ;exit if pwr16 is not > 0
    ;JMP    Hex2StringLoopBody  ;otherwise get the next digit

Hex2StringLoopBody:         ;get a digit
    XOR DX, DX              ;DX is high order dividend - set to 0
    DIV CX                  ;divide by power of 16; result in AX
    CMP AX, 9               ;check if digit is <= 9
    JBE Digit09             ;go to 0-9 if digit <=9
    JMP DigitAF             ;otherwise go to DigitAF

Digit09:                    ;convert digit to ASCII 0-9
    ADD AX, DIGITOffset     ;convert digit at AX to ASCII
    JMP StoreAndPrepare

DigitAF:                    ;convert digit to ASCII A-F
    ADD AX, AFoffset        ;convert character at AX to ASCII
    ;JMP    StoreAndPrepare     ;

StoreAndPrepare:            ;store ASCII char
    MOV [SI], AL            ;store character in AL at address
    INC SI                  ;increment address pointer
    MOV BX, DX              ;save mod 16 to BX
    MOV AX, CX              ;load current pwr16 to AX
    MOV CX, 16              ;load divisor into CX
    XOR DX, DX              ;clear DX
    DIV CX                  ;divide by 16 - value stored in AX
    MOV CX, AX              ;load new power of 16 back to CX
    MOV AX, BX              ;move arg = arg MOD pwr16 into AX
    JMP EndHex2StringLoopBody

EndHex2StringLoopBody:
    JMP Hex2StringLoop      ;keep looping (end check is at the top)


EndHex2StringLoop:
    MOV BX, NULLchar        ;prepare to write null character
    MOV [SI], BL            ;write a null terminated character,
    POPA                    ;restore all registers
    RET


Hex2String  ENDP



CODE    ENDS



        END
