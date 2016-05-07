/****************************************************/
/*													*/
/*	Intel Corporation Proprietary Information.		*/
/*	This listing is supplied under the terms of a	*/
/*	license agreement with Intel Corporation and	*/
/*	may not be copied nor disclosed except in		*/
/*	accordance with the terms of that agreement.	*/
/*													*/
/****************************************************/

/****************************************************/
/* i86.h                                            */
/* To be used with iC-86 V4.0 Compiler              */
/****************************************************/

#pragma _builtin_(lockset=56)
unsigned char	lockset (	unsigned char	*lockptr, 
							unsigned char	newvalue);

#pragma _builtin_(enable=49)
void	enable (void);

#pragma _builtin_(disable=50)
void	disable (void);

#pragma _builtin_(causeinterrupt=54)
void	causeinterrupt (unsigned char	number);

#pragma _builtin_(interruptptr=52)
void	(*interruptptr(void(*handler)(void)))(void);

#pragma _builtin_(halt=51)
void	halt (void);

#pragma _builtin_(getflags=114)
unsigned short	getflags (void);

#pragma _builtin_(setflags=114)
void	setflags (unsigned short	value);

#define	FLAG_CARRY      0x0001
#define	FLAG_PARITY     0x0004
#define	FLAG_AUXCARRY	0x0010
#define	FLAG_ZERO       0x0040
#define	FLAG_SIGN       0x0080
#define	FLAG_TRAP       0x0100
#define	FLAG_INTERRUPT	0x0200
#define	FLAG_DIRECTION	0x0400
#define	FLAG_OVERFLOW	0x0800

#pragma _builtin_ (inbyte=104)
unsigned char	inbyte (unsigned short	port);

#pragma _builtin_(inword=105)
unsigned int    inword (unsigned short	port);

#pragma _builtin_(outbyte=102)
void	outbyte (unsigned short port, unsigned char value);

#pragma _builtin_(outword=103)
void    outword (unsigned short port, unsigned int value);

#pragma _builtin_(blockinbyte=70)
void    blockinbyte (	unsigned short	port, 
						unsigned char	*destination,
						unsigned 		count);

#pragma _builtin_(blockinword=71)
void    blockinword (	unsigned short	port,
						unsigned int	*destination,
						unsigned		count);

#pragma _builtin_(blockoutbyte=68)
void    blockoutbyte (	unsigned short	port,
						unsigned char	*source,
						unsigned		count);

#pragma _builtin_(blockoutword=69)
void    blockoutword (	unsigned short	port,
						unsigned int	*source,
						unsigned		count);

#pragma _builtin_(initrealmathunit=106)
void    initrealmathunit (void);

#pragma _builtin_(setrealmode=108)
void    setrealmode (unsigned short mode);

#pragma _builtin_(getrealstatus=107)
unsigned short	getrealstatus (void);
#define     I87_INVALID_OPERATION		0x0001
#define     I87_DENORMALIZED_OPERAND	0x0002
#define     I87_ZERO_DIVIDE				0x0004
#define     I87_OVERFLOW				0x0008
#define     I87_UNDERFLOW				0x0010
#define     I87_PRECISION				0x0020

#define     I87_CONTROL_PRECISION		0x0300
#define     I87_PRECISION_24_BIT		0x0000
#define     I87_PRECISION_53_BIT		0x0200
#define     I87_PRECISION_64_BIT		0x0300

#define     I87_CONTROL_ROUNDING		0x0C00
#define     I87_ROUND_NEAREST			0x0000
#define     I87_ROUND_DOWN				0x0400
#define     I87_ROUND_UP				0x0800
#define     I87_ROUND_CHOP				0x0C00

#define     I87_CONTROL_INFINITY		0x1000
#define     I87_INFINITY_PROJECTIVE		0x0000
#define     I87_INFINITY_AFFINE			0x1000

#define     I87_STATUS_ERROR			0x0080
#define     I87_STATUS_STACKTOP_MASK	0x3800
#define     I87_STATUS_STACKTOP_SHIFT	11
#define     I87_STACK_TOP(env)			(((env).status & I87_STATUS_STACKTOP_MASK) \
										>> I87_STATUS_STACKTOP_SHIFT)
#define     I87_STATUS_BUSY				0x8000

#define     I87_STATUS_CONDITION_CODE	0x4700
#define     I87_CONDITION_C0			0x0100
#define     I87_CONDITION_C1			0x0200
#define     I87_CONDITION_C2			0x0400
#define     I87_CONDITION_C3			0x4000

#pragma _builtin_(saverealstatus=109)
void    saverealstatus (struct i87_state *savearea);

#pragma     ALIGN(i87_real_address)
struct      i87_real_address
{
    unsigned    offset: 16, : 0;
    unsigned    opcode: 11, : 1;
    unsigned    selector: 4, : 0;
};

#pragma     ALIGN(i87_protected_address)
struct      i87_protected_address
{
    unsigned    offset;
    unsigned    selector: 16, : 0;
};

union       i87_address
{
    struct i87_real_address			real;
    struct i87_protected_address	protected;
};

#define     I87_ADDRESS(addr)	(((unsigned long)(addr).selector \
								<< (8 * sizeof(int))) \
								+ (unsigned)(addr).offset)

#pragma     ALIGN(i87_environ)
struct      i87_environment
{
    unsigned			control: 16, : 0;
    unsigned			status: 16, : 0;
    unsigned			tag: 16, : 0;
    union i87_address	instruction;
    union i87_address	operand;
};

#define     I87_TAG_MASK	0x0003
#define     I87_TAG_SHIFT	2
#define     I87_TAG(x, y)	(((x).tag >> (I87_TAG_SHIFT * (y))) & I87_TAG_MASK)

#pragma     NOALIGN(i87_tempreal)
struct      i87_tempreal
{
    char		significand[8];
    unsigned    exponent: 15;
#if defined(SBITFIELD)
    signed      sign: 1;
#else
    unsigned	sign: 1;
#endif
};

typedef     struct i87_tempreal     tempreal_t;
struct      i87_state
{
    struct i87_environment	environment;
    tempreal_t				stack[8];
};

#pragma _builtin_(restorerealstatus=110)
void    restorerealstatus (struct i87_state *savearea);
