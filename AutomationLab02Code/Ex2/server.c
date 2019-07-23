// Server for the automation lab exercise
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "crc.h"

#define ACTION_NULL         0
#define ACTION_SET_DATA     1
#define ACTION_READ_DATA    2
#define ACTION_KEEPALIVE    3
#define ACTION_ACK          4

#define MY_ID               1

int main(int argc, char *argv[])
{
    //
    // Networking setup
    //
    int listenfd = 0, connfd = 0;
    struct sockaddr_in serv_addr; 
    int bytes_received = 0;
    int retry_counter = 0;
    
    unsigned char sendBuff[5];
    unsigned char tmpBuff[3];
    unsigned char recvBuff[1025];

    listenfd = socket(AF_INET, SOCK_STREAM, 0);

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(5000); 

    bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)); 

    listen(listenfd, 10); 
    //
    // End of networking setup
    //

    // Simulated sensor data variable
    unsigned int my_data = 91;
    
    while(1)
    {
        printf("Waiting for connection from master.\n");
        connfd = accept(listenfd, (struct sockaddr*)NULL, NULL); 
        printf("Incoming connection accepted.\n");

        retry_counter = 0;

        // Peek into the waiting data buffer
        bytes_received = recv(connfd , recvBuff , 1025 , MSG_DONTWAIT & MSG_PEEK);
        
        while( retry_counter < 10 ) {
            
            if ( bytes_received > 4 ) {
                printf("Got a packet\n");
                // Now we can really read the data and remove them from the waiting buffer
                recv(connfd , recvBuff , 5 , MSG_DONTWAIT);
    
                switch (recvBuff[4]) {
                    
                    case ACTION_KEEPALIVE:
                        printf("Got a KEEPALIVE packet, sending an ACK back\n");
                        tmpBuff[2] = ACTION_ACK;
                        tmpBuff[1] = MY_ID;
                        tmpBuff[0] = 0;
                        break;

                    case ACTION_SET_DATA:
                        printf("Got a SET DATA packet, sending an ACK back\n");
                        tmpBuff[2] = ACTION_ACK;
                        tmpBuff[1] = MY_ID;
                        tmpBuff[0] = 0;
                        break;

                    case ACTION_READ_DATA:
                        printf("Got a READ DATA packet, sending the data back\n");
                        tmpBuff[2] = ACTION_READ_DATA;
                        tmpBuff[1] = MY_ID;
                        tmpBuff[0] = my_data;
                        break;
                    
                }
                //printf(recvBuff);

                unsigned int crc = ModRTU_CRC(tmpBuff, 3);
                
                sendBuff[4] = tmpBuff[2];
                sendBuff[3] = tmpBuff[1];
                sendBuff[2] = tmpBuff[0];
                sendBuff[1] = (crc >> 8) & 0xff;
                sendBuff[0] = crc & 0xff;
                
                write(connfd, sendBuff, sizeof(sendBuff));
                
                if ( bytes_received > 5 )
                    bytes_received -= 5;
                else
                    bytes_received = 0;
                
                retry_counter = 0;
            }
            else {
                bytes_received = recv(connfd , recvBuff , 1025 , MSG_DONTWAIT & MSG_PEEK);
                retry_counter += 1;
            }
        }

        printf("No package received, closing connection.\n");
        close(connfd);

     }
}
