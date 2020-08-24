#include <QFile>

#include "types.h"

/* Calcola il CRC in un file a partire da ini_ofs per un numero di bytes
   dato da len. Se len e` zero prosegue fino alla fine del file, il file
   deve essere aperto in modo da permettere la lettura (r, r+, w+) */
quint16 fcalc_crc(QFile &fh, long ini_ofs, long len);

/* Calcola il CRC in una zona di memoria a partire da ini_addr per un numero
   di bytes dato da len */
quint16 mcalc_crc(void *ini_addr, long len);

/* update crc reverse */
quint16 updcrcr(quint16 crc, quint16 code);
