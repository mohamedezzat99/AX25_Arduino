#ifndef AX25_CRC_H
#define AX25_CRC_H
#include "std_types.h"

uint16 computeCRC(uint8 *, uint16 *);
void AX25_putCRC(uint8 *, uint16 *);

#endif /* AX25_CRC_H */
