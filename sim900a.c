#include "sim900a.h"
int send_message (int fd,char *send_number,char *message)
{
    int count=0;
    char  cmgf[]="AT+CMGF=1\r";  //设置短信发送模式为text （0-PDU;1-text）
    char  cmgs[128]="AT+CMGS=\"";
    //char  send_number[16];
    //char  message[128];
    char  reply[128];
    char *message_buff;
    char *send_number_buff;
    message_buff=malloc(256*sizeof(char));
    if (verbose)
    	perror("Malloc buff for Message Sending");
    //send_number[strlen(send_number)-1]='\0';   //去除字符串末端读入的换行符\n;
    strcat(cmgs,send_number);
    strcat(cmgs,"\"\r");

    //message[strlen(message)-1]='\0';
    strcpy(message_buff,message);
    //strcat(message_buff,"\x1a");
    //message_buff[strlen(message_buff)+1]=0x1a;
    /* write cmgf */

    write(fd,cmgf,strlen(cmgf));
    printf("write %s\n",cmgf);
    sleep(2);
    memset(reply,0,sizeof(reply));
    read(fd,reply,sizeof(reply));
    printf("%s\n",reply);

    /* write cmgs */

    write(fd,cmgs,strlen(cmgs));
    printf("writr %s\n",cmgs);
    sleep(5);
    memset(reply,0,sizeof(reply));
    read(fd,reply,sizeof(reply));
    printf("%s\n",reply);


    /*write  message*/

    write(fd,message_buff,strlen(message_buff));
    printf("writr %s\n",message);
    sleep(1);
    write(fd,"\x1a",1);
    printf("write the end char.\n");
    sleep(5);
    //printf("%d\n",0x1a);
    memset(reply,0,sizeof(reply));
    read(fd,reply,sizeof(reply));
    printf("%s\n",reply);

    return 0;
} /* ----- End of send_message()  ----- */
int init_sim900a(int fd){
	char ate0[]="ATE0\r";
	char at[]="AT\r";
	char cops[]="AT+COPS?\r";
	char cpin[]="AT+CPIN?\r";
	char reply[128];

	system("~/sim900a_init.sh");
	sleep(30);
	//write the AT twice for Baund Rate Train

	write(fd,at,strlen(at));
	if (verbose)
		perror("write the First AT.\n");

	write(fd,at,strlen(at));
	if (verbose)
		perror("write the Second AT.\n");
	write(fd,"\x1a",1);
	sleep(5);
	//write the ATE0 to Turn off the echo display
	write(fd,ate0,strlen(ate0));
	if (verbose)
	    perror("write the ATE0 to turn off the Echo Display.\n");
	write(fd,cpin,strlen(cpin));
	if (verbose)
		perror("Check the SIM status.\n");
	sleep(1);
	memset(reply,0,sizeof(reply));
	read(fd,reply,sizeof(reply));
	if (verbose)
		printf("%s",reply);

	if (strstr(reply,"ERROR")!=NULL){
		printf("No sim card error.\n");
		return -1;
	}

	write(fd,cops,strlen(cpin));
	sleep(1);
	memset(reply,0,sizeof(reply));
	read(fd,reply,sizeof(reply));
	if (verbose)
			printf("%s",reply);

	if (strchr(reply,'\"')==NULL){
		printf("Activate code error.\n");
		return -2;
	}

	return 1;
}
