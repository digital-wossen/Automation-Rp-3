//WOSSEN HAILEMARIAM
//30001173

#include <stdio.h>

#include "crc.h"

int main() {

    unsigned char messagebytes[4][4]; // Storage space for the bytes of four test packets

    //THERE ARE ACTUALLY MANY SOLUTIONS, I DID THREE JUST CLARIFY



            //SOLUTION ONE

    // Test packet 0
    //messagebytes[0][0] = 0x01; messagebytes[0][1] = 0x01; messagebytes[0][2] = 0xC1; messagebytes[0][3] = 0xE0;
    // Test packet 1
    //messagebytes[1][0] = 0x3A; messagebytes[1][1] = 0xE3; messagebytes[1][2] = 0x16; messagebytes[1][3] = 0xE1;
    // Test packet 2
    //messagebytes[2][0] = 0x7C; messagebytes[2][1] = 0xFE; messagebytes[2][2] = 0xA0; messagebytes[2][3] = 0xF0;
    // Test packet 3
    //messagebytes[3][0] = 0xE0; messagebytes[3][1] = 0x01; messagebytes[3][2] = 0xAB; messagebytes[3][3] = 0x37;


            //SOLUTION TWO
    // Test packet 0
    //messagebytes[0][0] = 0x21; messagebytes[0][1] = 0x11; messagebytes[0][2] = 0xD9; messagebytes[0][3] = 0xEC;
    // Test packet 1
    //messagebytes[1][0] = 0xA0; messagebytes[1][1] = 0x39; messagebytes[1][2] = 0x16; messagebytes[1][3] = 0xE1;
    // Test packet 2
    //messagebytes[2][0] = 0x2C; messagebytes[2][1] = 0xB9; messagebytes[2][2] = 0xDC; messagebytes[2][3] = 0xC2;
    // Test packet 3
    //messagebytes[3][0] = 0xEA; messagebytes[3][1] = 0x24; messagebytes[3][2] = 0x00; messagebytes[3][3] = 0x37;

          //SOLUTION TWO
 // Test packet 0
    messagebytes[0][0] = 0x20; messagebytes[0][1] = 0x19; messagebytes[0][2] = 0xD9; messagebytes[0][3] = 0xBA;
    // Test packet 1
    messagebytes[1][0] = 0x12; messagebytes[1][1] = 0x39; messagebytes[1][2] = 0x16; messagebytes[1][3] = 0xD1;
    // Test packet 2
    messagebytes[2][0] = 0x99; messagebytes[2][1] = 0x88; messagebytes[2][2] = 0x6B; messagebytes[2][3] = 0x86;
    // Test packet 3
    messagebytes[3][0] = 0xE0; messagebytes[3][1] = 0x59; messagebytes[3][2] = 0x22; messagebytes[3][3] = 0x33;


    unsigned char msg_body[2];
    unsigned int crc_out;
    unsigned char crc_lsb, crc_msb;
    
    for (int msg_nb = 0; msg_nb < 4; msg_nb++) {
        msg_body[0] = messagebytes[msg_nb][0];
        msg_body[1] = messagebytes[msg_nb][1];
        
        crc_out = ModRTU_CRC(msg_body, 2);
        
        crc_lsb = (crc_out >> 8) & 0xff;
        crc_msb = crc_out & 0xff;   

        if ( (crc_lsb == messagebytes[msg_nb][3]) && (crc_msb == messagebytes[msg_nb][2]) ) {
            printf("Correct CRC value for message number %d\n", msg_nb);
        }
        else {
            printf("Wrong222 CRC value for message number %d\n", msg_nb);                        
        }
    }

}
