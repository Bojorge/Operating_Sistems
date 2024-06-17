#include "driver_lib.h"
#include <stdio.h>
#include <unistd.h>


int main() {
    int fd = open_serial_port("/dev/ttyACM0");

    //RELEASE_PEN(fd);
    //MOVE_FORWARDS(fd);

    int bytes_written = send_word(fd, "QUE\n");

    //MOVE_BACK(fd);
    //MOVE_FORWARDS(fd);
    //RELEASE_PEN(fd);
    //RETRACT_PEN(fd);
    //DOT_BUZZER(fd); 
    //DASH_BUZZER(fd);

    close_serial_port(fd);

    return 0;
}