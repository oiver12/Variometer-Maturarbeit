#ifndef TYPES_H
#define TYPES_H

//--Unsigned 
typedef unsigned char 	BYTE;	// 8 Bit
typedef unsigned short	WORD;	//16 Bit
typedef unsigned long	QUAD;	//32 Bit

typedef unsigned char	UINT8;	// 8 Bit
typedef unsigned short	UINT16;	//16 Bit
typedef unsigned long	UINT32;	//32 Bit	//Mc20011008 removed (Conflict redefinition MS setsd.h)

typedef unsigned char	UI8;	// 8 Bit
typedef unsigned short	UI16;	//16 Bit
typedef unsigned long	UI32;	//32 Bit

//--Signed 
typedef char			CHAR;	// 8 Bit
typedef short			SHORT;	//16 Bit
typedef long			LONG;	//32 Bit

typedef char			INT8;	// 8 Bit
typedef short			INT16;	//16 Bit
typedef int				INT32;	//32 Bit

typedef char			SI8;	// 8 Bit
typedef short			SI16;	//16 Bit
typedef int				SI32;	//32 Bit

//--Floating Point 
typedef float			FLOAT32;//32 Bit
typedef double			FLOAT64;//64 Bit


//--Boolean 
typedef int				BOOL;	//32 Bit	
#define false			0
#define true			1
#define FALSE			0
#define TRUE			1


//--Handles
#if !defined NULL
#define NULL			0
#endif
typedef void* 			HANDLE;	//32 Bit
typedef void (*FUNC)(void);


//--Bit Manipulation:
#define getBit(reg,bit)		(reg &(1<<bit))
#define setBit(reg,bit)		{reg|=(1<<bit);}
#define clrBit(reg,bit)		{reg&=~(1<<bit);}
#define changeBit(reg,bit)	{reg=(reg&(1<<bit)?reg&~(1<<bit):reg|(1<<bit));}

//---Byte Manipulation
#define hiByte(word)		(word>>8)
#define loByte(word)		(word&0xff)


///////////////////////////////////////////////////////////////////////////////
#endif //TYPES_H

