#ifndef REG_DEFINES_H
#define REG_DEFINES_H

//defines of lpc2121

// Set byte (8 bits = 0xFF)
#define SETBYTE(WORD, STARTPOS) \
    (WORD |= (0xFFu << STARTPOS))

// Write new byte value
#define WRITEBYTE(WORD, STARTPOS, VAL) \
    (WORD = ((WORD) & ~(0xFFu << STARTPOS)) | ((VAL & 0xFFu) << STARTPOS))

// Read byte
#define READBYTE(WORD, STARTPOS) \
    (((WORD >> STARTPOS) & 0xFFu))


//WRITEBIT IODIR0
#define SETBIT(WORD, BITPOS) \
    WORD |= 1 << BITPOS

#define WRITEBIT(WORD, BITPOS, BITLEVEL) \
    (WORD = ((WORD) & ~(1u << BITPOS)) | ((BITLEVEL & 1u) << BITPOS))

#define READBIT(WORD, BITPOS) \
		(((WORD >> BITPOS)) & 1u)
		
// Set nibble (4 bits = 0xF)
#define SETNIBBLE(WORD, STARTPOS) \
    (WORD |= (0xFu << STARTPOS))

// Write nibble with new value
#define WRITENIBBLE(WORD, STARTPOS, VALUE) \
    (WORD = ((WORD) & ~(0xFu << STARTPOS)) | ((VALUE & 0xFu) << STARTPOS))

// Read nibble
#define READNIBBLE(WORD, STARTPOS) \
    (((WORD >> STARTPOS) & 0xFu))
		
#endif

