#include "output.h"
void cls(void)
{
	printf("\x1b[H\x1b[2J");
}

void *thread_printmonitor(void *data)
{

    unsigned int t=0;
    unsigned int i;
    while(1){
      //cls();
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
		 last[i]=last[i]+1;
	  }
	  sleep(1);
	  t++;
    }
}
