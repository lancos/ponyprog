#include "types.h"

/* Calcola il CRC in un file a partire da ini_ofs per un numero di bytes
   dato da len. Se len e` zero prosegue fino alla fine del file, il file
   deve essere aperto in modo da permettere la lettura (r, r+, w+) */
uint16_t fcalc_crc(FILE *fh, long ini_ofs, long len);

/* Calcola il CRC in una zona di memoria a partire da ini_addr per un numero
   di bytes dato da len */
uint16_t mcalc_crc(void *ini_addr, long len);

/* update crc reverse */
uint16_t updcrcr(uint16_t crc, uint16_t code);
