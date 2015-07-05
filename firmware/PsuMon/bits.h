
typedef struct { 
    unsigned int bit0:1; 
    unsigned int bit1:1; 
    unsigned int bit2:1; 
    unsigned int bit3:1; 
    unsigned int bit4:1; 
    unsigned int bit5:1; 
    unsigned int bit6:1; 
    unsigned int bit7:1; 
} _bits;

typedef struct { 
    unsigned int lo:4; 
    unsigned int hi:4; 
} _nibbles;

#define byte uint8_t
#define word uint16_t

#define _BIT(rg,bt) ((volatile _bits*)&rg)->bit##bt
#define BIT(rg,bt)	(_BIT(rg,bt))

#define BYTE(x,y)	(((char*)&x)[y])

#define LO(x) 		(((volatile _nibbles*)&x)->lo)
#define HI(x) 		(((volatile _nibbles*)&x)->hi)
