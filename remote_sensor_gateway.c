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
#include <termios.h>
#include <stdio.h>
#include <fcntl.h>
#include "read_comment.h"
#include "output.h"

#define SERVER_PORT 8000
#define DIST_ADDR "192.168.0.112"

unsigned char battery[6];
unsigned char alarm_dis[6];
unsigned int last[6];
unsigned int console_last;


pthread_mutex_t uart_lock=PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t socket_lock=PTHREAD_MUTEX_INITIALIZER;

struct sockaddr_in dist_addr;
//串口打开的初始化
//参数：串口设备的路径名 dev，设置整型的波特率（注意格式匹配）
//设备路径名出错会报错，波特率输出默认为：9600
//其他参数设默认值：数据位：8位，校验位：无，停止位：1位
int  uart_open (char *dev, int baud)
{
    struct termios tio;
    int fd;
    int baud_flags;

    fd = open(dev, O_RDWR);
    if(fd == -1) {
    //  fprintf(stdout, "open %s error!\n", dev);
        return -1;
    }

    if(tcgetattr(fd, &tio) < 0) {
    //  fprintf(stdout, "tcgetattr error!\n");
        close(fd);
        return -1;
    }

    switch (baud) {
    case 115200:
        baud_flags = B115200;
        break;
    case 57600:
        baud_flags = B57600;
        break;
    case 38400:
        baud_flags = B38400;
        break;
    case 19200:
        baud_flags = B19200;
        break;
    case 9600:
        baud_flags = B9600;
        break;
    case 2400:
        baud_flags = B2400;
        break;
    case 4800:
        baud_flags = B4800;
        break;
    default:
        baud_flags = B9600;
        break;
    }

    fcntl(fd, F_SETFL,O_NONBLOCK);
    tio.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG | ECHOE);
    tio.c_iflag &= ~(ICRNL | INPCK | ISTRIP | IXON | BRKINT);
    tio.c_iflag &= ~(CSIZE | PARENB | CSTOPB);
    tio.c_oflag &= ~(OPOST);
    tio.c_cflag = CS8 | baud_flags | CLOCAL | CREAD;
    tio.c_cc[VMIN] = 0;
    tio.c_cc[VTIME] = 0;

    tcflush(fd, TCIFLUSH);
    if(tcsetattr(fd, TCSAFLUSH, &tio) < 0) {
    //  fprintf(stdout,"tcsetattr error!\n");
        close(fd);
        return -1;
    }

    printf("open uart:%s, baud:%d", dev, baud);
    return fd;
}

int main(){
    int fd;
    struct com_socket_fd com_socket_fd_inst;
    pthread_t read_com_t,output_t,read_socket_t,period_read_t;
    //初始化串口
    /* 创建UDP套接口 */
    struct sockaddr_in server_addr;
     bzero(&server_addr, sizeof(server_addr));
     bzero(&dist_addr,sizeof(dist_addr));
     server_addr.sin_family = AF_INET;
     server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
     server_addr.sin_port = htons(SERVER_PORT);

     dist_addr.sin_addr.s_addr = inet_addr(DIST_ADDR);
     dist_addr.sin_port = htons(SERVER_PORT);
     dist_addr.sin_family=AF_INET;

     /* 创建socket */
     int server_socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
     if(server_socket_fd == -1)
     {
      perror("Create Socket Failed:");
      exit(1);
     }

     /* 绑定套接口 */
     if(-1 == (bind(server_socket_fd,(struct sockaddr*)&server_addr,sizeof(server_addr))))
     {
      perror("Server Bind Failed:");
      exit(1);
     }

    fd = uart_open("/dev/ttyUSB0",115200);
    if(fd == -1){
        printf("open the com and set it.it's failed!");
        return -1;
    }
    com_socket_fd_inst.fd_com=fd;
    com_socket_fd_inst.fd_socket=server_socket_fd;
    if(pthread_create(&read_com_t,NULL,(void *)thread_read_com,(void *)&com_socket_fd_inst) == -1){
        printf("Create the thread of read com error!\n");
        return -1;
    }
    if (pthread_create(&read_socket_t,NULL,(void *)thread_read_socket,(void *)&com_socket_fd_inst) ==-1){
    	printf("Create the thread of read socket error!\n");
    	return -1;
    }
    if (pthread_create(&output_t,NULL,(void *)thread_printmonitor,(void *)0) ==-1){
    	printf("Create the thread of output error!\n");
    	return -1;
    }
    if (pthread_create(&period_read_t,NULL,(void *)thread_period_read_power,(void *)&com_socket_fd_inst) ==-1){
        	printf("Create the thread of period_read error!\n");
        	return -1;
        }

    //等待 线程结束
    pthread_join(read_com_t, NULL);
    pthread_join(read_socket_t,NULL);
    pthread_join(output_t,NULL);
    return 0;

}
