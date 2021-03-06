#ifndef _OUTPUT_H_
#define _OUTPUT_H_
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdarg.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>

#include "read_comment.h"
#include "sim900a.h"
extern unsigned char battery[];
extern unsigned char alarm_dis[];
extern int last[];
extern int console_last;
extern unsigned char check_counter;
extern int verbose; //Debug message display
extern char revision_char[110];

void *thread_printmonitor(void *data);
void *thread_oled_display(void *arg);
void *thread_message_alarm(void *data);
#endif
