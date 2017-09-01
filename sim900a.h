#ifndef _SIM900A_H_
#define _SIM900A_H_
#include<stdio.h>
#include<string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
//#include "read_comment.h"

extern int verbose;
int send_message (int fd,char *send_number,char *message);
int init_sim900a(int fd);


#endif
