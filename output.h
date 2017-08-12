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

extern unsigned char battery[];
extern unsigned char alarm_dis[];
extern unsigned int last[];
extern unsigned int console_last;

void *thread_printmonitor(void *data);
#endif
