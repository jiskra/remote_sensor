#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdarg.h>
#include <stddef.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <termios.h>
#include <time.h>
#include <stdio.h>
#include <getopt.h>
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include "read_comment.h"
#include "output.h"

#define I2C_DEV "/dev/i2c-0"
#define OLED_ADDR 0x3C

#define SERVER_PORT 8000
#define DIST_ADDR "192.168.0.5"
#define SRC_ADDR "192.168.0.5"
#define PHONE_NUMBER "15996315105"

unsigned char battery[6];
unsigned char alarm_dis[6];
unsigned char check_counter=0;
int last[6]={-1,-1,-1,-1,-1,-1};
int console_last=-1;
int verbose=0; //verbose mode
int file_counter=0;

int fd_i2c_;


pthread_mutex_t uart_lock=PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t socket_lock=PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t check_lock=PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t file_lock=PTHREAD_MUTEX_INITIALIZER;
struct sockaddr_in dist_addr,source_addr;
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

    printf("open uart:%s, baud:%d\n", dev, baud);
    return fd;
}
void usage(){
	printf("USE the -R -L -N -C -P for remoter ip addr and for local addr and phone number.\n");
	printf("-R  <address> Server IP address.\n");
	printf("-L  <address> Local IP address.\n");
	printf("-p  <port>    address port");
	printf("-N  <telephone number> Default Phone Number.\n");
	printf("-C  <ID> Set the Channel id.\n");
	printf("-P  <PANID> Set the PANID.\n");
	printf("-v  <level> verbose mode.\n");
	printf("       1   first level.\n");
	printf("       2   second level.\n");
}
int main(int argc, char **argv){
    int fd;
    char *serv_addr=NULL;
    char *local_addr=NULL;
    char *phone_number=NULL;
    char *hub_id=NULL;
    int port=0;
    unsigned char channel_id;
    unsigned short PANID;
    const char const_serv_addr[20]=DIST_ADDR;
    const char const_local_addr[20]=SRC_ADDR;
    const char const_phone_number[20]=PHONE_NUMBER;
    const char const_hub_id[20]="Test Sensor Hub";
    const char const_file_path[50]="~/";

    struct com_socket_fd com_socket_fd_inst;
    pthread_t read_com_t,output_t,read_socket_t,period_read_t,heart_t,period_send_message_t;
#ifdef OLED
    pthread_t oled_display_t;
    int fd_i2c;
    int fd_sim900a;
#endif
    int c;
    while ((c = getopt(argc, argv, "hv:R:L:N:H:C:P:p:")) != EOF) {

    		switch (c) {
    		case 'R':
    			serv_addr = optarg;
    			break;
    		case 'L':
    			local_addr= optarg;
    			break;
    		case 'N':
    			phone_number = optarg;
    			break;
    		case 'H':
    		    hub_id = optarg;
    		    break;
    		case 'C':
    		    channel_id = atoi(optarg);
    		    break;
    		case 'p':
    		    port = atoi(optarg);
    		    break;
    		case 'P':
    			PANID=atoi(optarg);
    			break;
    		case 'v':
    		    verbose = atoi(optarg);
    		    break;
    		case 'h':
    			usage();
    		    break;
    		default:
    			usage();
    			return -1;
    		}
    	}

    if (serv_addr==NULL)   //if the parameter is not set,use the default
    	serv_addr=const_serv_addr;
    if (local_addr==NULL)
    	local_addr=const_local_addr;
    if (phone_number==NULL)
    	phone_number=const_phone_number;
    if (hub_id==NULL)
    	hub_id=const_hub_id;
    if (port==0)
    	port=SERVER_PORT;
    if (verbose){
    	printf("SERVER IP ADDRESS is %s\n",serv_addr);
    	printf("LOCAL IP ADDRESS is %s\n",local_addr);
    	printf("SERVER PORT is %d\n",port);
    	printf("PHONE_NUMBER is %s\n",phone_number);
    	printf("HUB_ID is %s\n",hub_id);
    	printf("PAIN_ID is %x\n",PANID);
    }

    /* 创建UDP套接口 */
    struct sockaddr_in server_addr;
     bzero(&server_addr, sizeof(server_addr));
     bzero(&dist_addr,sizeof(dist_addr));
     bzero(&source_addr,sizeof(source_addr));

     server_addr.sin_family = AF_INET;
     server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
     server_addr.sin_port = htons(port);

     dist_addr.sin_addr.s_addr = inet_addr((const char*)serv_addr);
     dist_addr.sin_port = htons(port);
     dist_addr.sin_family=AF_INET;

     source_addr.sin_addr.s_addr = inet_addr((const char*)local_addr);
     source_addr.sin_port = htons(port);
     source_addr.sin_family=AF_INET;

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
#ifdef OLED
    int fd_sd;
    time_t timer;//time_t就是long int 类型
    struct tm *tblock;
    timer = time(NULL);
    tblock = localtime(&timer);
    char filename_buff[100];

    sprintf(filename_buff,"\\tmp\\mounts\\SD-P1\\%s.dat", asctime(tblock));
    fd_sd=open(filename_buff,O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
    if (verbose)
    	perror("Creat the datalog file.\n");

#endif
     //open uart fd
#ifdef OLED
    fd = uart_open("/dev/ttyS1",115200);
    printf("Target is ARM Open Serial /dev/ttyS1.\n");

    fd_sim900a = uart_open("/dev/ttyS2",115200);
    printf("Open uart2 for SIM900A to send message.\n");
    if (fd_sim900a==-1)
    	printf("Open Uart2 error,the Message is unreachable.\n");
    com_socket_fd_inst.fd_sim900a=fd_sim900a;
#else
    fd = uart_open("/dev/ttyUSB0",115200);
    printf("Target is x86 Open Serial /dev/ttyUSB0.\n");
#endif
    if(fd == -1){
        printf("open the com and set it.it's failed!\n");
        return -1;
    }
    //open i2c fd
#ifdef OLED
    printf("hello, open i2c for display\n");
       fd_i2c = open(I2C_DEV, O_RDWR);
       if (fd_i2c < 0)
       {
          printf("open "I2C_DEV"failed\n");
          return -1;
       }
       //bind the device of the fd;
       if (ioctl(fd_i2c, I2C_SLAVE, OLED_ADDR) < 0)
       {         /* 设置芯片地址 */
          printf("oictl:set slave address failed\n");
          return -1;
       }
       fd_i2c_=fd_i2c;
#endif


    com_socket_fd_inst.fd_com=fd;
    com_socket_fd_inst.fd_socket=server_socket_fd;
#ifdef OLED
    com_socket_fd_inst.fd_sim900a=fd_sim900a;
#endif
    com_socket_fd_inst.hub_id=hub_id;
    com_socket_fd_inst.phone_number[0]=phone_number;
    com_socket_fd_inst.num_of_phone_number=1;
    com_socket_fd_inst.channel_id=channel_id;
    com_socket_fd_inst.PANID=PANID;
    com_socket_fd_inst.serv_addr=serv_addr;
    com_socket_fd_inst.local_addr=local_addr;




#ifdef OLED
    com_socket_fd_inst.fd_i2c=fd_i2c;
    com_socket_fd_inst.fd_sd=fd_sd;
#endif
    if(pthread_create(&read_com_t,NULL,(void *)thread_read_com,(void *)&com_socket_fd_inst) == -1){
        printf("Create the thread of read com error!\n");
        return -1;
    }
    perror("Create the thread of read com");
    if (pthread_create(&read_socket_t,NULL,(void *)thread_read_socket,(void *)&com_socket_fd_inst) ==-1){
    	printf("Create the thread of read socket error!\n");
    	return -1;
    }
    perror("Create the thread of read socket");

    if (pthread_create(&output_t,NULL,(void *)thread_printmonitor,(void *)0) ==-1){
    	printf("Create the thread of output error!\n");
    	return -1;
    }
    perror("Create the thread of output");
    if (pthread_create(&period_read_t,NULL,(void *)thread_period_read_power,(void *)&com_socket_fd_inst) ==-1){
        	printf("Create the thread of period_read error!\n");
        	return -1;
        }
    perror("Create the thread of period_read");



#ifdef OLED
    if (pthread_create(&period_send_message_t,NULL,(void *)thread_message_alarm,(void *)&com_socket_fd_inst) ==-1){
            printf("Create the thread of send_message error!\n");
            return -1;
            }
    perror("Create the thread of send_message ");
    if (pthread_create(&oled_display_t,NULL,(void *)thread_oled_display,(void *)&com_socket_fd_inst) ==-1){
            	printf("Create the thread of period_read error!\n");
            	return -1;
            }
    perror("Create the thread of period_read");

    printf("OLED Display over.\n");
#endif

    if (pthread_create(&heart_t,NULL,(void *)*thread_period_heartbeat,(void *)&com_socket_fd_inst) ==-1){
        	printf("Create the thread of heart_beat error!\n");
        	return -1;
        }

    //等待 线程结束
    pthread_join(read_com_t, NULL);
    pthread_join(read_socket_t,NULL);
    pthread_join(output_t,NULL);
    pthread_join(heart_t,NULL);
    pthread_join(heart_t,NULL);
#ifdef OLED
    pthread_join(oled_display_t, NULL);
#endif
    return 0;

}
