#ifndef MYLIB_H
#define MYLIB_H

int open_serial_port(const char* device_file);
int send_word(int fd, const char* word);
void close_serial_port(int fd);

void MOVE_FORWARDS(int fd);

void MOVE_BACK(int fd);

void RELEASE_PEN(int fd);

void RETRACT_PEN(int fd);

void DOT_BUZZER(int fd);

void DASH_BUZZER(int fd);

#endif