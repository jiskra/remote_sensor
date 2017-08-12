#ifndef _READ_COMMENT_
#define _READ_COMMENT_
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
#define COMM_IN_LENGTH 73
#define FRAME_LENGTH 75

#define READ_DATA_UPDATA 0x00
#define READ_LATEST_UPDATA 0x01
#define READ_POWER 0x02
#define READ_UID 0x04

void *thread_read_com(void *data);
void *thread_read_socket(void *data);
void *thread_period_read_power(void *data);

extern unsigned char battery[];
extern unsigned char alarm_dis[];
extern unsigned int last[];
extern unsigned int console_last;
extern struct sockaddr_in dist_addr;

extern pthread_mutex_t uart_lock;
extern pthread_mutex_t socket_lock;

struct com_socket_fd{
	int fd_com;
	int fd_socket;
};

/*struct instru_packet{
unsigned char header1;
unsigned char header2;
unsigned char length;
unsigned char instru;
unsigned char node_addr;
unsigned char commend1;
unsigned char commend2;
unsigned char commend3;
unsigned char check_sum;
unsigned char tail1;
unsigned char tail2;
};*/
struct read_instru_packet{
unsigned char header1;
unsigned char header2;
unsigned char length;
unsigned char instru;
unsigned char node_addr;
unsigned char commend1;
unsigned char commend2;
unsigned char commend3;
unsigned char UID[8];
unsigned char check_sum;
unsigned char tail1;
unsigned char tail2;
};
struct upload_data{
unsigned char header1;
unsigned char header2;
unsigned char length;
unsigned char commend_type;
unsigned char node_addr;
unsigned char commend1;
unsigned char commend2;
unsigned char commend3;
unsigned char addata[64];
unsigned char check_sum;
unsigned char tail1;
unsigned char tail2;
};


#undef NULL
#if defined(__cplusplus)
#define NULL 0
#else
#define NULL ((void *)0)
#endif



#endif
