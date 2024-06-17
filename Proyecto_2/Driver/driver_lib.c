#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <termios.h>
#include "driver_lib.h"

/*
* @description: Opens a serial port and returns the file descriptor
* @param: device_file - the file path of the serial port (e.g. /dev/ttyACM0)
* @return: file descriptor of the serial port
*/
int open_serial_port(const char* device_file) {

    //open file
    int fd = open(device_file, O_RDWR | O_NOCTTY | O_NDELAY);
    if (fd == -1) {
        perror("Failed to open serial port");
        return -1;
    }

    //OPTION SETUP:
    //So that the serial port works properly (raw binary communication with no special processing behind it)
    struct termios options;

    //Get the current options for the port
    memset(&options, 0, sizeof options);
    if (tcgetattr(fd, &options) != 0) {
        perror("tcgetattr");
        return -1;
    }

    //Set the baud rate
    cfsetispeed(&options, B9600);
    cfsetospeed(&options, B9600);


    options.c_cflag = (options.c_cflag & ~CSIZE) | CS8; // 8-bit chars
    options.c_iflag &= ~IGNBRK;                         // disable break processing
    options.c_lflag = 0;                                // no signaling chars, no echo,
                                                        // no canonical processing
    options.c_oflag = 0;                                // no remapping, no delays
    options.c_cc[VMIN]  = 0;                            // read doesn't block
    options.c_cc[VTIME] = 5;                            // 0.5 seconds read timeout

    options.c_iflag &= ~(IXON | IXOFF | IXANY);         // shut off xon/xoff ctrl

    options.c_cflag |= (CLOCAL | CREAD);                // ignore modem controls,
                                                        // enable reading
    options.c_cflag &= ~(PARENB | PARODD);              // shut off parity
    options.c_cflag |= 0;
    options.c_cflag &= ~CSTOPB;
    options.c_cflag &= ~CRTSCTS;

    options.c_cc[VMIN]  = 0;            // read doesn't block (VERY IMPORTANT)
    options.c_cc[VTIME] = 5;            // 0.5 seconds read timeout

    if (tcsetattr(fd, TCSANOW, &options) != 0) {
        perror("tcsetattr");
        return -1;
    }

    //wait for the arduino to reset an initialize the serial port (WITHOUT THIS THE ARDUINO WILL NOT RECEIVE DATA AND NOTHING WORKS)
    sleep(2);

    return fd;
}

/*
* @description: Sends a word to the serial port
* @param: fd - file descriptor of the serial port (should be opened with open_serial_port)
*/
int send_word(int fd, const char* word) {
    int n_written = write(fd, word, strlen(word));
    if (n_written == -1) {
        perror("Failed to write to serial port");
        return -1;
    }

    return n_written;
}


void MOVE_FORWARDS(int fd){
    send_word(fd, "F\n");   
}

void MOVE_BACK(int fd){
    send_word(fd, "B\n");   
}

void RELEASE_PEN(int fd){
    send_word(fd, "1\n");   
}

void RETRACT_PEN(int fd){
    send_word(fd, "2\n");   
}

void DOT_BUZZER(int fd){
    send_word(fd, ".\n");   
}

void DASH_BUZZER(int fd){
    send_word(fd, "-\n");   
}

/*
* @description: Closes the serial port opened with open_serial_port
* @param: fd - file descriptor of the serial port
*/
void close_serial_port(int fd) {
    close(fd);
}