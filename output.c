#include "output.h"
#include <stdio.h>
#include <stdlib.h>

#define MAX_NODE_NUM 6
#define MAX_PHONE_NUM 5

#ifdef OLED
#include "SSD1306.h"
#include "Fonts.h"
#endif

#define TIMEOUT 4000 //time out time is 4000s

int sim900a_error;
void cls(void)
{
	printf("\x1b[H\x1b[2J");
}

void *thread_printmonitor(void *data)
{

    unsigned int t=0;
    unsigned int i;
    if (verbose)
    	printf("Enter display thread.\n");
    while(1){
     if (!verbose){
      cls();}
	  printf ("      Remote sensor monitor\n");
	  switch (t%4){
	    case 0:
	      printf(" Node_ID          Battery          Alarm         Interval        Last--\n");
	      break;
	    case 1:
	      printf(" Node_ID          Battery          Alarm         Interval        Last\\\n");
	      break;
	    case 2:
	      printf(" Node_ID          Battery          Alarm         Interval        Last|\n");
	      break;
	    case 3:
	      printf(" Node_ID          Battery          Alarm         Interval        Last/\n");
	      break;
	    default:
	      printf(" Node_ID          Battery          Alarm         Interval        Last--\n");
	      break;
	   }
	  for (i=0;i<6;i++){
		 printf("Node_ID(%d)     %3d        %3d        %3d        %4d\n",i+1,battery[i],alarm_dis[i],sample_interval[i],last[i]);
		 if (last[i]>=9999)
			 last[i]=9999;

		 else if (last[i]>=0)
			 last[i]=last[i]+1;
	  }
	  printf("Console last is %d.\n",console_last);
	  printf("Self Send Counter is %d.\n",check_counter);
	  if (console_last>=9999)
	  console_last=9999;

	  else if (console_last>=0)
	  console_last+=1;
	  if (verbose){
		 sleep(60);
	  }
	  else{
	  sleep(1);
	  }
	  t++;
    }
}
#ifdef OLED
void *thread_oled_display(void *arg){
		int i;
		unsigned char message[20];
		struct com_socket_fd *com_socket_fd_inst;
		com_socket_fd_inst=(struct com_socket_fd*) arg;
		char dispaly_buff[20];

		ssd1306_init();
		while (1){
	   ssd1306_clear_screen(0xFF);
	   sleep(1);
	   ssd1306_clear_screen(0x00);
	   if (sim900a_error==0)
	   ssd1306_display_string(0, 0, "GSM READY", 16, 1);
	   else if (sim900a_error==-1)
	   ssd1306_display_string(0, 0, "SIM ERROR", 16, 1);
	   else if (sim900a_error==-2)
	   ssd1306_display_string(0, 0, "NET ERROR", 16, 1);

	   sprintf(dispaly_buff,"Channel_ID:%d",com_socket_fd_inst->channel_id);
	   ssd1306_display_string(0, 16, dispaly_buff, 16, 1);


	   sprintf(dispaly_buff,"PANID:0x%x",com_socket_fd_inst->PANID);
	   ssd1306_display_string(0, 32, dispaly_buff, 16, 1);

	   if (com_socket_fd_inst->fd_sd<0)
		   ssd1306_display_string(0, 48, "Creat file error", 16, 1);
	   else
		   ssd1306_display_string(0, 48, "Creat file OK", 16, 1);

	   ssd1306_refresh_gram();

	   sleep(2);



	    ssd1306_clear_screen(0xFF);
	    sleep(1);
		ssd1306_clear_screen(0x00);
		sprintf(dispaly_buff,"R:%s",com_socket_fd_inst->serv_addr);
		ssd1306_display_string(0, 0, dispaly_buff, 16, 1);
		sprintf(dispaly_buff,"L:%s",com_socket_fd_inst->local_addr);
		ssd1306_display_string(0, 16, dispaly_buff, 16, 1);
		ssd1306_display_string(0, 32, "Remoter Sensor Gateway!", 16, 1);

		ssd1306_display_string(0, 48, "CSL last:", 16, 1);
		snprintf(message, 5, "%d", console_last);
		ssd1306_display_string(64, 48, message, 16, 1);


		ssd1306_refresh_gram();
		sleep(3);
		ssd1306_clear_screen(0x00);

		ssd1306_display_string(0, 0, "Phone Number", 16, 1);
		for(i=0;i<(com_socket_fd_inst->num_of_phone_number<3?com_socket_fd_inst->num_of_phone_number:3);i++){

			ssd1306_display_string(0, (i+1)*16, com_socket_fd_inst->phone_number[i], 16, 1);

		}
		ssd1306_refresh_gram();
		sleep(2);

		for(i=3;i<(com_socket_fd_inst->num_of_phone_number>3?com_socket_fd_inst->num_of_phone_number:3);i++){

					ssd1306_display_string(0, (i-2)*16, com_socket_fd_inst->phone_number[i], 16, 1);

				}


		ssd1306_clear_screen(0x00);
		for (i=0;i<6;i++){


		//display the battery
		 ssd1306_draw_bitmap(0, 4, &c_chBat816[0], 16, 8);
		 //if (last[i]<0)
		//	 ssd1306_display_string(18, 0, "null", 16, 1);
		 //else{
		 //snprintf(message, 4, "%d", battery[i]);
		 //ssd1306_display_string(18, 0, message, 16, 1);}

		 //display the alarm
		 ssd1306_draw_bitmap(64, 4, &c_chAlarm88[0], 8, 8);
		 /*if (last[i]<0)
			 ssd1306_display_string(74, 0, "null", 16, 1);
		 else{
		 snprintf(message, 5, "%d", alarm_dis[i]);
		 ssd1306_display_string(74, 0, message, 16, 1);}*/


		 if ((i+1)/10)
			 ssd1306_draw_3216char(0 ,16, '0'+(i+1)/10);

		 ssd1306_draw_3216char(16,16, '0'+(i+1)%10);
		//ssd1306_draw_3216char(16,16, '3');
		//ssd1306_draw_3216char(32,16, ':');
		//ssd1306_draw_3216char(48,16, '5');
		//ssd1306_draw_3216char(64,16, '6');
		//ssd1306_draw_1616char(80,32, ':');
		//ssd1306_draw_1616char(96,32, '4');
		//ssd1306_draw_1616char(112,32, '7');
		//ssd1306_draw_bitmap(87, 16, &c_chBmp4016[0], 40, 16);
		if (last[i]<0){
			ssd1306_display_string(58, 20, " UNKOW!", 16, 1);
			ssd1306_display_string(18, 0, "null", 16, 1);
			ssd1306_display_string(74, 0, "null", 16, 1);
		}
		else if (last[i]>TIMEOUT){
			ssd1306_display_string(58, 20, " LOSS! ", 16, 1);
			ssd1306_display_string(18, 0, "null", 16, 1);
			ssd1306_display_string(74, 0, "null", 16, 1);}
		else if (alarm_dis[i]>0){
			ssd1306_display_string(58, 20, " ALARM!", 16, 1);
			snprintf(message, 4, "%d", battery[i]);
			ssd1306_display_string(18, 0, "    ", 16, 1);
			ssd1306_display_string(18, 0, message, 16, 1);

			snprintf(message, 5, "%d", alarm_dis[i]);
			ssd1306_display_string(74, 0, "     ", 16, 1);
			ssd1306_display_string(74, 0, message, 16, 1);
		}
		else{
			ssd1306_display_string(58, 20, " I'M OK", 16, 1);

			snprintf(message, 4, "%d", battery[i]);
			ssd1306_display_string(18, 0, "    ", 16, 1);
			ssd1306_display_string(18, 0, message, 16, 1);

			snprintf(message, 5, "%d", alarm_dis[i]);
			ssd1306_display_string(74, 0, "     ", 16, 1);
			ssd1306_display_string(74, 0, message, 16, 1);

		}



		ssd1306_display_string(0, 52, "NODE ID", 12, 0);
		//ssd1306_display_string(52, 52, "MENU", 12, 0);
		ssd1306_display_string(80, 52, "STATUS", 12, 0);

		ssd1306_refresh_gram();
		if (last[i]>TIMEOUT||alarm_dis[i]>0)
		sleep(4);
		else
		sleep(2);
		//ssd1306_clear_screen(0x00);
		}
	}
}

void *thread_message_alarm(void *data){
	int fd_sim900a;
	int alarm_register[MAX_NODE_NUM];
	char *phone_number[5];
	char num_of_phone_number;
	char *hub_id;
	char alarm_message[100]="ALARM!!! Hub ID is ";
	const char const_alarm_message[]="ALARM!!! Hub ID is ";
	char alarm_value_message[100];
	int error;
	int i,j;
	struct com_socket_fd *com_socket_fd_inst;
	com_socket_fd_inst=(struct com_socket_fd*) data;
	fd_sim900a=com_socket_fd_inst->fd_sim900a;
	//for (i=0;i<MAX_PHONE_NUM;i++)
	//  phone_number[i]=com_socket_fd_inst->phone_number[i];
	num_of_phone_number=com_socket_fd_inst->num_of_phone_number;//NUM of Phone Number
	hub_id=com_socket_fd_inst->hub_id;

    strcat(alarm_message,hub_id);
	if (verbose){
		printf("Enter send message thread.\n");
		printf("ALARM Message is %s\n PHONE NUMBER is %s\n",alarm_message,com_socket_fd_inst->phone_number[0]);
		printf("FD_SIM900A is %x",fd_sim900a);
	}

	error=init_sim900a(fd_sim900a);
	sim900a_error=error;
	if (error<0) {
		printf("SIM900A init ERROR.\n");
		//exit(-1);
	}
    if (verbose&&error>0){
    	printf("Send a test message.\n");
    	for (j=0;j<num_of_phone_number;j++)
    	send_message (fd_sim900a,com_socket_fd_inst->phone_number[j],"This is a test message!");
    }


    while(1){
       if (send_short_message!=0&&error>0){
    	num_of_phone_number=com_socket_fd_inst->num_of_phone_number;//NUM of Phone Number

    	printf("Send a Alarm message.\n");
    	for (j=0;j<num_of_phone_number;j++){
    	send_message (fd_sim900a,com_socket_fd_inst->phone_number[j],com_socket_fd_inst->short_message);
    	if (verbose)
    	send_message (fd_sim900a,com_socket_fd_inst->phone_number[j],"This is a ALARM TEST Message!!!");
    	}

    	pthread_mutex_lock(&message_lock);
    	send_short_message=0;
    	pthread_mutex_unlock(&message_lock);}

		sleep(60);
	}

   exit(1);



}
#endif
