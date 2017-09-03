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
#include <stddef.h>
#include <time.h>

#define MAX_NODE_NUM 6
#define COMM_IN_LENGTH 73
#define FRAME_LENGTH 75

#define READ_DATA_UPDATA 0x00
#define READ_LATEST_UPDATA 0x01
#define READ_POWER 0x22
#define READ_UID 0x04
#define READ_AD 0x21

#define READ_THRESHOLD 0x15
#define SET_ZIGBEE 0x50
#define SET_SAMPLE_RATE 0x14

#define READ_NODE_DATA 0x02
#define AD_REPORT 0x21
#define POWER_REPORT 0x22
#define UID_REPORT 0x23
#define EXCEPTION_REPORT 0x11


#define LOST_STATUS 0x02


void *thread_read_com(void *data);
void *thread_read_socket(void *data);
void *thread_period_read_power(void *data);
void *thread_period_heartbeat(void* data);

extern unsigned char battery[];
extern unsigned char alarm_dis[];
extern int last[];
extern unsigned char  sample_interval[];
extern unsigned char send_short_message;
extern int console_last;
extern unsigned char check_counter;
extern struct sockaddr_in dist_addr;
extern struct sockaddr_in source_addr;
extern int verbose;
extern int file_counter;
extern pthread_mutex_t uart_lock;
extern pthread_mutex_t socket_lock;
extern pthread_mutex_t check_lock;
extern pthread_mutex_t file_lock;
extern pthread_mutex_t message_lock;

struct com_socket_fd{
	int fd_com;
	int fd_sim900a;
	int fd_socket;
	int fd_i2c;
	int fd_sd;
	int fd_log;
	char *phone_number[5];
	int num_of_phone_number;
	char *short_message;
	char *hub_id;
	unsigned char channel_id;
	unsigned short PANID;
	char *serv_addr;
	char *local_addr;
};
struct manage_packet{
	unsigned short head;
	char phone_number[5][12];
	short  num_of_phone_number;
	char message[100];
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
