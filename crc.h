#include "types.h"

/* Calcola il CRC in un file a partire da ini_ofs per un numero di bytes
   dato da len. Se len e` zero prosegue fino alla fine del file, il file
   deve essere aperto in modo da permettere la lettura (r, r+, w+) */
UWORD fcalc_crc(FILE *fh, long ini_ofs, ULONG len);

/* Calcola il CRC in una zona di memoria a partire da ini_addr per un numero
   di bytes dato da len */
UWORD mcalc_crc(void *ini_addr, ULONG len);

/* update crc reverse */
UWORD updcrcr(UWORD crc, UWORD code);
