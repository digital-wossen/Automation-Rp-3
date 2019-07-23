// Client for the automation lab exercise
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "crc.h"

#define STATE_READY         1
#define STATE_AWAITING_DATA 2
#define STATE_AWAITING_ACK  3
#define STATE_ERROR         4
#define STATE_STOP          5

#define ACTION_NULL         0
#define ACTION_SET_DATA     1
#define ACTION_READ_DATA    2
#define ACTION_KEEPALIVE    3
#define ACTION_ACK          4

#define SLAVE_1_ID          1

//
// Initial state
//
int global_state;

//
// Scheduled tasks array
//
unsigned char scheduled_tasks[100][3];
int task_counter = -1;

// Other global variables
int sock;
struct sockaddr_in server;

// The knowledge of the slave devices
int remote_sensor_data = 0;
int remote_sensor_data_initiated = 0;

// Open connection function
int open_conn() {
    //Connect to a remote server
    if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0)
    {
        //printf("Connection failed.\n");
        return 0;
    }
     
    //printf("Connected to the slave\n");
    return 1;
}
    
    
// Close connection function
int close_conn() {
    close(sock);
}

// Send packet function
int send_msg(unsigned char action, unsigned char slave_nb, unsigned char data) {
        //Send some data
    
        unsigned char message[5];
        unsigned char tmp_msg[3];
        
        tmp_msg[2] = action;
        tmp_msg[1] = slave_nb;
        tmp_msg[0] = data;
        
        unsigned int crc = ModRTU_CRC(tmp_msg, 3);
            
        message[4] = tmp_msg[2];
        message[3] = tmp_msg[1];
        message[2] = tmp_msg[0];
        message[1] = (crc >> 8) & 0xff;
        message[0] = crc & 0xff;

        if ( send(sock , message , sizeof(message) , 0) < 0 ) {
            printf("Send failed\n");
            return 0;
        }
        
        printf("Sent message to slave\n");
        return 1;
            
}

// Send packet function
int receive_msg() {
        //Receive a reply from the server
        unsigned char receive_buffer[10] = { 0 };
    
        if ( recv(sock , receive_buffer , sizeof(receive_buffer) , MSG_DONTWAIT) < 0 ) {
            printf("Receive failed\n");
            return 0;
        }
        
        switch (receive_buffer[4]) {
            case ACTION_READ_DATA: 
                if ( receive_buffer[3] == SLAVE_1_ID ) {
                    printf("Received DATA message from slave, updating internal values\n");
                    remote_sensor_data = receive_buffer[2];
                    remote_sensor_data_initiated = 1;
                }
                else {
                    printf("Received message is different than expected\n");
                    return 0;
                }
                break;
            case ACTION_ACK: 
                printf("Received ACK from slave\n");
                if ( receive_buffer[3] != SLAVE_1_ID ) {
                    printf("Received message is different than expected\n");
                    return 0;
                }
                break;
            default:
                printf("Unrecognised message type\n");
                return 0;
        }
        
        //printf("Receive message from slave\n");
        return 1;
}

//
// State transition function
//
void update_state() {
    
    switch (global_state) {

        case STATE_READY:
            task_counter += 1;
            switch ( scheduled_tasks[task_counter][0] ) {

                case ACTION_NULL:
                    global_state = STATE_STOP;
                    printf("Reached the end of scheduled actions\n");
                    break;

                case ACTION_KEEPALIVE:
                    if ( send_msg(ACTION_KEEPALIVE, scheduled_tasks[task_counter][1], scheduled_tasks[task_counter][2]) == 1 )
                        global_state = STATE_AWAITING_ACK;
                    break;

                case ACTION_SET_DATA:
                    if ( send_msg(ACTION_SET_DATA, scheduled_tasks[task_counter][1], scheduled_tasks[task_counter][2]) == 1 )
                        global_state = STATE_AWAITING_ACK;
                    break;

                case ACTION_READ_DATA:
                    if ( send_msg(ACTION_READ_DATA, scheduled_tasks[task_counter][1], scheduled_tasks[task_counter][2]) == 1 )
                        global_state = STATE_AWAITING_DATA;
                    break;
            }
            break;
            
        case STATE_AWAITING_ACK:
            if ( receive_msg() == 1 )
                global_state = STATE_READY;
            break;

        case STATE_AWAITING_DATA:
            if ( receive_msg() == 1 )
                global_state = STATE_READY;
            break;
            
        case STATE_ERROR:
            global_state = STATE_STOP;
            printf("Irrecoverable error, will stop now.\n");
            break;

    }
}

//
// The main loop
//
void main() {
    
    //
    // Networking setup
    //
     
    //Create socket
    sock = socket(AF_INET , SOCK_STREAM , 0);
    if (sock == -1)
    {
        printf("Could not create socket\n");
    }
     
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_family = AF_INET;
    server.sin_port = htons( 5000 );
    //
    // End of networking setup
    //

    //
    // Fill the scheduled tasks queue
    //
    scheduled_tasks[0][0] = ACTION_KEEPALIVE;   scheduled_tasks[0][1] = SLAVE_1_ID;  scheduled_tasks[0][2] = 0;
    scheduled_tasks[1][0] = ACTION_SET_DATA;    scheduled_tasks[1][1] = SLAVE_1_ID;  scheduled_tasks[1][2] = 120;
    scheduled_tasks[2][0] = ACTION_READ_DATA;   scheduled_tasks[2][1] = SLAVE_1_ID;  scheduled_tasks[2][2] = 10;
    scheduled_tasks[3][0] = ACTION_NULL;        scheduled_tasks[3][1] = 0;           scheduled_tasks[3][2] = 0;
    
    
    //
    // Starting the machine
    //
    global_state = STATE_READY;
    
    if ( open_conn() != 1 ) {
        global_state = STATE_ERROR;
    }
    
    while ( (global_state != STATE_STOP) && (task_counter < 99) )
    {
        update_state();
        printf("Current state: %d\n", global_state);
        
        sleep(1);
         
    }

    close_conn();
}
