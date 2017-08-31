#include "output.h"
#include <stdio.h>
#include <stdlib.h>

#ifdef OLED
#include "SSD1306.h"
#include "Fonts.h"
#endif

#define TIMEOUT 4000 //time out time is 4000s
void cls(void)
{
	printf("\x1b[H\x1b[2J");
}

void *thread_printmonitor(void *data)
{

    unsigned int t=0;
    unsigned int i;
    while(1){
      cls();
	  printf ("      Remote sensor monitor\n");
	  switch (t%4){
	    case 0:
	      printf(" Node_ID  Battery   Alarm    Last--\n");
	      break;
	    case 1:
	      printf(" Node_ID  Battery   Alarm    Last\\\n");
	      break;
	    case 2:
	      printf(" Node_ID  Battery   Alarm    Last|\n");
	      break;
	    case 3:
	      printf(" Node_ID  Battery   Alarm    Last/\n");
	      break;
	    default:
	      printf(" Node_ID  Battery   Alarm    Last--\n");
	      break;
	   }
	  for (i=0;i<6;i++){
		 printf("Node_ID(%d)     %d        %d        %d\n",i+1,battery[i],alarm_dis[i],last[i]);
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
	  sleep(1);
	  t++;
    }
}
#ifdef OLED
void *thread_oled_display(void *arg){
		int i;
		unsigned char message[20];
		ssd1306_init();
		while (1){
	    ssd1306_clear_screen(0xFF);
	    sleep(1);
		ssd1306_clear_screen(0x00);
		ssd1306_display_string(0, 0, "IP:192.168.1.104", 16, 1);
		ssd1306_display_string(0, 16, "S:192.168.1.105", 16, 1);
		ssd1306_display_string(0, 32, "Remoter Sensor Gateway!", 16, 1);

		ssd1306_display_string(0, 48, "CSL last:", 16, 1);
		snprintf(message, 5, "%d", console_last);
		ssd1306_display_string(64, 48, message, 16, 1);


		ssd1306_refresh_gram();
		sleep(3);
		ssd1306_clear_screen(0x00);

		for (i=0;i<6;i++){


		//display the battery
		 ssd1306_draw_bitmap(0, 4, &c_chBat816[0], 16, 8);
		 if (last[i]<0)
			 ssd1306_display_string(18, 0, "null", 16, 1);
		 else{
		 snprintf(message, 4, "%d", battery[i]);
		 ssd1306_display_string(18, 0, message, 16, 1);}

		 //display the alarm
		 ssd1306_draw_bitmap(64, 4, &c_chAlarm88[0], 8, 8);
		 if (last[i]<0)
			 ssd1306_display_string(74, 0, "null", 16, 1);
		 else{
		 snprintf(message, 5, "%d", alarm_dis[i]);
		 ssd1306_display_string(74, 0, message, 16, 1);}

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
		if (last[i]<0)
			ssd1306_display_string(58, 20, " UNKOW!", 16, 1);
		else if (last[i]>TIMEOUT)
			ssd1306_display_string(58, 20, " LOSS! ", 16, 1);
		else if (alarm_dis[i]>0)
			ssd1306_display_string(58, 20, " ALARM!", 16, 1);
		else
			ssd1306_display_string(58, 20, " I'M OK", 16, 1);



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
#endif
