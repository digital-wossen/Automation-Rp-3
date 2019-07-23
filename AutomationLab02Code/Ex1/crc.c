// Compute the MODBUS RTU CRC in C
#include "crc.h"
// unsigned int has 2 bytes in most of C implementations
// unsigned char is the 1 byte data type often used for representing raw bytes

unsigned int ModRTU_CRC(unsigned char buf[], int len) {
  unsigned int crc = 0xFFFF;
  
  for (int pos = 0; pos < len; pos++) {
    crc ^= (unsigned int)buf[pos];          // XOR byte into least
                                      // significant byte of crc
    for (int i = 8; i != 0; i--) {    // Loop over each bit
      if ((crc & 0x0001) != 0) {      // If the LSB is set
        crc >>= 1;                    //  Shift right
        crc ^= 0xA001;                //  and XOR with 0xA001
      }                               //  = 0b1010000000000001
      else                            // Else (LSB is not set)
        crc >>= 1;                    //  Just shift right
    }
  }
  // Watch out for the order of the low and the high byte
  return crc;  
} 
