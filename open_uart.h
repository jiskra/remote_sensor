#ifndef _OPEN_UART_H_
#define _OPEN_UART_H_
#include <termios.h>
#include <stdio.h>
#include <fcntl.h>
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
#endif
