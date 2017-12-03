#include "read_comment.h"
#define READ_NODE_DATA 0x02
#define OK_COMMAND 0x21||0x22||0x23
#define POWER_REPORT 0x22
#define CONSOLE_TIME_OUT 7200
#define MAX_FILE_LENGTH 10000



void *thread_read_com(void *data){
	int com_fd,socket_fd;
	struct com_socket_fd *com_socket_fd_inst;
    int ret;
	unsigned char* serial_in_buff;
	unsigned char* frame_buff;
	int buff_ptr=0;
	int valid_data_length=0;
	//unsigned char check_sum=0;
    //struct upload_data upload_data_inst;
    struct upload_data *upload_data_tmp_inst;
	//int i;
	com_socket_fd_inst=(struct com_socket_fd*) data;
	com_fd=com_socket_fd_inst->fd_com;
	socket_fd=com_socket_fd_inst->fd_socket;
	if((serial_in_buff=malloc(FRAME_LENGTH*6*sizeof(char)))==NULL) {
		printf("Buff malloc error.\n");
		pthread_exit(0);
	}
	if ((frame_buff=malloc(FRAME_LENGTH*sizeof(char)))==NULL) {
		printf("Buff malloc error.\n");
		pthread_exit(0);

	}
	while(1){
		if (buff_ptr>=3*FRAME_LENGTH){
			memcpy(serial_in_buff,serial_in_buff+buff_ptr,valid_data_length*sizeof(char));
			buff_ptr=0;
		}
		ret=read(com_fd,serial_in_buff+buff_ptr+valid_data_length,FRAME_LENGTH);
		if (ret>0){
			if (verbose==2)
				printf("Read Length is %d.\n",ret);}
		else{
		usleep(50000);
		}
		valid_data_length+=ret;
		while (valid_data_length>=FRAME_LENGTH){
			int socket_send_key=0;
				upload_data_tmp_inst=(struct upload_data*)(serial_in_buff+buff_ptr);
			  //if ((serial_in_buff[buff_ptr+i]==0xfe&&serial_in_buff[buff_ptr+i+1]==0xfc&&serial_in_buff[buff_ptr+i+2]==FRAME_LENGTH) {//&&serial_in_buff[buff_ptr+i+FRAME_LENGTH-2]==0xfa&&serial_in_buff[buff_ptr+i+FRAME_LENGTH-1]){
				if (upload_data_tmp_inst->header1==0xfe&&upload_data_tmp_inst->header2==0xfc&&upload_data_tmp_inst->tail1==0xfb&&upload_data_tmp_inst->tail2==0xfa&&upload_data_tmp_inst->length==FRAME_LENGTH) {
				  printf("A frame check,and the checksum is correct.\n");
				  //memcpy(&upload_data_inst,serial_in_buff+buff_ptr+i,sizeof(upload_data_inst));
				 // if (check_counter==0){
				      socket_send_key=1;
					  //pthread_mutex_lock(&socket_lock);
					  //ret=sendto(socket_fd, (void*)upload_data_tmp_inst, FRAME_LENGTH,0,(struct sockaddr*)&dist_addr,sizeof(dist_addr));
					  //pthread_mutex_unlock(&socket_lock);
					  //printf("Send Success %d.\n",ret);
					  //if (ret<0){
					 //perror("Send SOCKET Failed:");
					 //exit(1);}
					  time_t timer;
					  struct tm *tblock;
					  timer = time(NULL);
					  tblock = localtime(&timer);

					  if (console_last>CONSOLE_TIME_OUT){
						  if (com_socket_fd_inst->fd_sd<0){
							  char filename_buff[200];
							  char filename_console_buff[200]="echo Create file > ";
							  sprintf(filename_buff,"/tmp/mounts/SD-P1/%d", timer);
							  //strcat(filename_console_buff,asctime(tblock));
							  strcat(filename_console_buff,filename_buff);
							  system(filename_console_buff);
							  com_socket_fd_inst->fd_sd=open(filename_buff,O_WRONLY | O_CREAT);
							  if (verbose)
							      perror("Creat the datalog file.\n");

						  if (com_socket_fd_inst->fd_sd=open(filename_buff,O_WRONLY | O_CREAT)){
							 if (verbose)
							      perror("Creat the datalog file.\n");
							 write(com_socket_fd_inst->fd_sd,asctime(tblock),strlen(asctime(tblock)));
							 if (verbose)
							 	perror("Write file.\n");
							 file_counter++;
							 if (file_counter>MAX_FILE_LENGTH){
								 close(com_socket_fd_inst->fd_sd);
								 if (verbose)
								 perror("Close SD file.\n");
							 }
							 write(com_socket_fd_inst->fd_sd,upload_data_tmp_inst,sizeof(upload_data_tmp_inst));
							 if (verbose)
								 perror("Write file.\n");
							 file_counter++;
							 if (file_counter>MAX_FILE_LENGTH){
							 	close(com_socket_fd_inst->fd_sd);
							 	if (verbose)
							 	perror("Close SD file.\n");
							 							 }
						   }
						  }
						else{
						 write(com_socket_fd_inst->fd_sd,asctime(tblock),strlen(asctime(tblock)));
						 if (verbose)
							perror("Write file.\n");
						  write(com_socket_fd_inst->fd_sd,upload_data_tmp_inst,sizeof(upload_data_tmp_inst));
						 if (verbose)
						    perror("Write file.\n");
						  }
					  }

				 // }
				 /* else{
					  pthread_mutex_lock(&check_lock);
					  check_counter-=1;
					  pthread_mutex_unlock(&check_lock);
				  }*/


				  if (((upload_data_tmp_inst->commend_type== POWER_REPORT)||
					   (upload_data_tmp_inst->commend_type== AD_REPORT)||
					   (upload_data_tmp_inst->commend_type== UID_REPORT))
						  && (upload_data_tmp_inst->node_addr<=MAX_NODE_NUM)){

					  last[upload_data_tmp_inst->node_addr-1]=0;
					  socket_send_key=1;

				  }
				  if ((upload_data_tmp_inst->commend_type==POWER_REPORT)&& (upload_data_tmp_inst->node_addr<=MAX_NODE_NUM)){
					 socket_send_key=1;
					 battery[upload_data_tmp_inst->node_addr-1]=upload_data_tmp_inst->commend1;
					 if (check_counter>0){
						 pthread_mutex_lock(&check_lock);
						 check_counter-=1;
						 pthread_mutex_unlock(&check_lock);
					 }

				  }

				  if ((upload_data_tmp_inst->commend_type==EXCEPTION_REPORT)&&
					  (upload_data_tmp_inst->node_addr<=MAX_NODE_NUM)&&
					  (upload_data_tmp_inst->commend3==LOST_STATUS)){
					  if (last[upload_data_tmp_inst->node_addr-1]<=4000){ //long time for check power
						  last[upload_data_tmp_inst->node_addr-1]=9997;
						  socket_send_key=0;
					  }
					  else if (last[upload_data_tmp_inst->node_addr-1]<9999){
						  last[upload_data_tmp_inst->node_addr-1]+=1;
						  socket_send_key=0;
					  }
					  else {
						  last[upload_data_tmp_inst->node_addr-1]=9999;
						  socket_send_key=1;}

					  if (check_counter>0){
						  pthread_mutex_lock(&check_lock);
						  check_counter-=1;
						  pthread_mutex_unlock(&check_lock);
					  }
				  }


				  //if (upload_data_tmp_inst->commend_type==0x12){9
					//  alarm_dis[upload_data_tmp_inst->node_addr-1]=upload_data_tmp_inst->addata[0];
				 //}
				  buff_ptr=buff_ptr+FRAME_LENGTH;
				  valid_data_length=valid_data_length-FRAME_LENGTH;
			    //}
			   /*else {
			    	printf("Get the header of the frame,but the checksum is not correct.\n");
			    	buff_ptr=buff_ptr+i;
			    	valid_data_length=valid_data_length-i;
			    }*/
				 if (socket_send_key==1){
					 pthread_mutex_lock(&socket_lock);
					 ret=sendto(socket_fd, (void*)upload_data_tmp_inst, FRAME_LENGTH,0,(struct sockaddr*)&dist_addr,sizeof(dist_addr));
					 pthread_mutex_unlock(&socket_lock);
					 printf("Send Success %d.\n",ret);
					 if (ret<0){
						 perror("Send SOCKET Failed:");
						 exit(1);}
				 	}
				}
			  else {
				  buff_ptr=buff_ptr+1;
				  valid_data_length=valid_data_length-1;
			  }

		}
	}

}
void *thread_read_socket(void* data){
	int com_fd,socket_fd;
	int ret;
    struct com_socket_fd *com_socket_fd_inst;
    int packet_length;
    struct read_instru_packet read_instru_packet_inst;
    struct manage_packet manage_packet_inst;
    char socket_rec_ptr[500];
    char *phone_number[5];
    char *message;
    com_socket_fd_inst=(struct com_socket_fd *)data;
    com_fd=com_socket_fd_inst->fd_com;
    socket_fd=com_socket_fd_inst->fd_socket;
    int i;
    for (i=0;i<5;i++){
    	phone_number[i]=malloc(sizeof(char)*12);
    }
    	message=malloc(sizeof(char)*100);
    if (verbose)
    	printf("Enter Socket received thread.\n");
    while(1){
    	  /* 定义一个地址，用于捕获客户端地址 */
    	  struct sockaddr_in client_addr;
    	  socklen_t client_addr_length = sizeof(client_addr);

    	  if(recvfrom(socket_fd, (void*)socket_rec_ptr, (sizeof(read_instru_packet_inst)>sizeof(manage_packet_inst))?sizeof(read_instru_packet_inst):sizeof(manage_packet_inst),0,(struct sockaddr*)&client_addr, &client_addr_length) == -1)
    	  {
    	  exit(1);}
    	  perror("Receive Data Failed:");
    	  memcpy((void*)&read_instru_packet_inst,(void*)socket_rec_ptr,sizeof(read_instru_packet_inst));
    	  memcpy((void*)&manage_packet_inst,(void*)socket_rec_ptr,sizeof(manage_packet_inst));
    	  if (manage_packet_inst.head==0xf0f0){

    	     		  if (verbose)
    	     			  printf("Received the Manage packet.\n");
    	     		  console_last=0;
    	     		  com_socket_fd_inst->num_of_phone_number=manage_packet_inst.num_of_phone_number;
    	     		  if (verbose)
    	     			  printf("Num of Phone Number is %d.\n",manage_packet_inst.num_of_phone_number);
    	     		  for (i=0;i<com_socket_fd_inst->num_of_phone_number;i++){
    	     		  com_socket_fd_inst->phone_number[i]=phone_number[i];
    	     		  memcpy((void*)phone_number[i],(void*)manage_packet_inst.phone_number[i],sizeof(char)*12);
    	     		  //com_socket_fd_inst->phone_number[i]=manage_packet_inst.phone_number[i];
    	     		  if (verbose){
    	     			  printf("The %d's PHONE NUMBER is %s.\n",i,com_socket_fd_inst->phone_number[i]);
    	     		  }
    	     		  }
    	     		  com_socket_fd_inst->short_message=message;
    	     		  memcpy((void*)message,(void*)manage_packet_inst.message,sizeof(char)*100);
    	     		  if (verbose){
    	     			  printf("Send MESSAGE: %s\n",manage_packet_inst.message);
    	     		  }
    	     		 pthread_mutex_lock(&message_lock);
    	     		 send_short_message=1;
    	     		 pthread_mutex_unlock(&message_lock);
    	     	  }
    	  if (read_instru_packet_inst.header1==0xfc&&read_instru_packet_inst.header2==0xfe&& //
    		  read_instru_packet_inst.tail1==0xfa&&read_instru_packet_inst.tail2==0xfb){
    		  console_last=0;
    	       if((read_instru_packet_inst.instru==READ_THRESHOLD)&&(read_instru_packet_inst.node_addr<=MAX_NODE_NUM&&read_instru_packet_inst.node_addr>0 )){
    	    	   alarm_dis[read_instru_packet_inst.node_addr-1]=read_instru_packet_inst.commend2;
    	    	   if (verbose)
    	    		   printf("Received Set Threshold and alarm packet.\n");
    	       }

    	       if((read_instru_packet_inst.instru==SET_SAMPLE_RATE)&&(read_instru_packet_inst.node_addr<=MAX_NODE_NUM&&read_instru_packet_inst.node_addr>0 )){
    	    	   if (verbose)
    	    		   printf("Received Set Sensor Sample rate and interval packet.\n");
    	    	   if (read_instru_packet_inst.commend3>0)
    	    	   sample_interval[read_instru_packet_inst.node_addr-1]=read_instru_packet_inst.commend3;
    	       }

    	       if ((read_instru_packet_inst.instru==READ_AD)&&(read_instru_packet_inst.node_addr<=MAX_NODE_NUM&&read_instru_packet_inst.node_addr>0 )){
    	         	pthread_mutex_lock(&write_ad_lock);
    	         	if (verbose)
    	         		printf("[debug]Read AD data lock com.\n");
    	         	pthread_mutex_lock(&uart_lock);
    	         	ret = write(com_fd,&read_instru_packet_inst,sizeof(read_instru_packet_inst));
    	         	pthread_mutex_unlock(&uart_lock);
    	         	sleep(60);
    	         	pthread_mutex_unlock(&write_ad_lock);
    	         	if (verbose)
    	         	    printf("[debug]Read AD data unlock com.\n");
    	         		  }
    	         else if (read_instru_packet_inst.instru!=READ_THRESHOLD){
    	        	 pthread_mutex_lock(&uart_lock);
    	        	 ret = write(com_fd,&read_instru_packet_inst,sizeof(read_instru_packet_inst));
    	        	 pthread_mutex_unlock(&uart_lock);}

    		   if(ret == -1){
    		   printf("Wirte com error.\n");
    		   pthread_exit(0);}
    		   else{
    		   printf("Data_send_successful %d\n",ret);
    		   }
    	  }

         }
}


void *thread_period_read_power(void* data){
	int com_fd,socket_fd;
	int ret;
	int i;
    struct com_socket_fd *com_socket_fd_inst;
    //struct instru_packet instru_packet_inst;
    struct read_instru_packet read_instru_packet_inst;
    com_socket_fd_inst=(struct com_socket_fd *)data;
    com_fd=com_socket_fd_inst->fd_com;
    socket_fd=com_socket_fd_inst->fd_socket;
    read_instru_packet_inst.header1=0xfc;
    read_instru_packet_inst.header2=0xfe;

    read_instru_packet_inst.tail1=0xfa;
    read_instru_packet_inst.tail2=0xfb;

    read_instru_packet_inst.length =sizeof(read_instru_packet_inst);
    read_instru_packet_inst.instru=SET_ZIGBEE;
    read_instru_packet_inst.commend1=(unsigned char)(com_socket_fd_inst->channel_id);
    read_instru_packet_inst.commend2=(unsigned char)(com_socket_fd_inst->PANID>>8);
    if (verbose)
    	printf("[debug]PAIN ID high byte is %x.\n",read_instru_packet_inst.commend2);
    read_instru_packet_inst.commend3=(unsigned char)(com_socket_fd_inst->PANID&0xff);
    if (verbose)
        printf("[debug]PAIN ID low byte is %x.\n",read_instru_packet_inst.commend3);
    pthread_mutex_lock(&uart_lock);
    ret=write(com_fd,&read_instru_packet_inst,sizeof(read_instru_packet_inst));
    pthread_mutex_unlock(&uart_lock);
    if (verbose)
    	perror("[debug]Set Channel and PANID:");

    read_instru_packet_inst.instru=READ_POWER;
        	//read_instru_packet_inst.commend1=READ_POWER;

        	//period read battery
     for (i=0;i<MAX_NODE_NUM;i++){
        read_instru_packet_inst.node_addr=i+1;
        pthread_mutex_lock(&write_ad_lock);
        if (verbose)
            printf("[debug]Read power lock com.\n");
        pthread_mutex_lock(&uart_lock);
        ret=write(com_fd,&read_instru_packet_inst,sizeof(read_instru_packet_inst));
        pthread_mutex_unlock(&uart_lock);
        if (verbose)
        	printf("[debug] Read power.\n ");
        sleep(10);
        pthread_mutex_unlock(&write_ad_lock);
        if (verbose)
            printf("[debug]Read power unlock com.\n");

        if (verbose)
        	printf("[debug]Read Power.\n");
        	pthread_mutex_lock(&check_lock);
        	check_counter+=1;
        	pthread_mutex_unlock(&check_lock);
        if (ret<0){
        	printf("[Error]Write Uart error.\n");
        	pthread_exit(0);
        	}
        	sleep(5);
        	}

    while(1){


    	read_instru_packet_inst.instru=READ_POWER;
    	//read_instru_packet_inst.commend1=READ_POWER;

    	//period read battery
    	for (i=0;i<MAX_NODE_NUM;i++){
    		read_instru_packet_inst.node_addr=i+1;
    		pthread_mutex_lock(&write_ad_lock);
    		if (verbose)
    		  printf("[debug]Read power lock com.\n");
    		pthread_mutex_lock(&uart_lock);
    		ret=write(com_fd,&read_instru_packet_inst,sizeof(read_instru_packet_inst));
    		pthread_mutex_unlock(&uart_lock);
    		sleep(10);
    		pthread_mutex_unlock(&write_ad_lock);
    		if (verbose)
    		  printf("[debug]Read power unlock com.\n");

    		if (verbose)
    			printf("Read Power.\n");
    		pthread_mutex_lock(&check_lock);
    		check_counter+=1;
    		pthread_mutex_unlock(&check_lock);
    		if (ret<0){
    			printf("Write Uart error.\n");
    			pthread_exit(0);
    		}
    		sleep(60);
    		}
    }
}
void *thread_period_heartbeat(void* data){
	int com_fd;
	int socket_fd;
	int ret;
	int i,j;
    int sample_counter[6];
    struct com_socket_fd *com_socket_fd_inst;
    //struct instru_packet instru_packet_inst;
    struct read_instru_packet read_instru_packet_inst;
    com_socket_fd_inst=(struct com_socket_fd *)data;
    com_fd=com_socket_fd_inst->fd_com;
    socket_fd=com_socket_fd_inst->fd_socket;
    read_instru_packet_inst.header1=0xfc;
    read_instru_packet_inst.header2=0xfe;

    read_instru_packet_inst.tail1=0xfa;
    read_instru_packet_inst.tail2=0xfb;

    read_instru_packet_inst.length =sizeof(read_instru_packet_inst);
    read_instru_packet_inst.instru=READ_AD;
    read_instru_packet_inst.commend1=0x00;
    read_instru_packet_inst.commend2=0x00;
    read_instru_packet_inst.commend3=0x00;
    if (verbose)
    	printf("Enter the period READ AD thread.\n");
    while(1){
    	//period read battery
    	if (verbose!=3){
    	for (i=0;i<MAX_NODE_NUM;i++){
    		if (sample_counter[i]>5*sample_interval[i]){
    			sample_counter[i]=0;
    			read_instru_packet_inst.node_addr=i+1;
    			pthread_mutex_lock(&write_ad_lock);
    			if (verbose)
    			    printf("[debug]Read AD data lock com.\n");
    			pthread_mutex_lock(&uart_lock);
    			ret=write(com_fd,&read_instru_packet_inst,sizeof(read_instru_packet_inst));
    			pthread_mutex_unlock(&uart_lock);
    			sleep(60);
    			pthread_mutex_unlock(&write_ad_lock);
    			if (verbose)
    			    printf("[debug]Read AD data unlock com.\n");

    			if (ret<0){
    				printf("Write UDP error.\n");
    				pthread_exit(0);}
    			}
    		sleep(60);
    		for(j=0;j<MAX_NODE_NUM;j++){
    		    sample_counter[i]=sample_counter[i]+1;
    		    }

    	}
    	}
    }
}
void *thread_console_heartbeat(void* data){
	int com_fd;
	int socket_fd;
	int ret;
	int i,j;
    int sample_counter[6];
    struct com_socket_fd *com_socket_fd_inst;
    struct heart_beat heart_beat_inst;
    unsigned char heart_beat_status[6]={0xff,0xff,0xff,0xff,0xff,0xff};
    com_socket_fd_inst=(struct com_socket_fd*) data;
    com_fd=com_socket_fd_inst->fd_com;
    socket_fd=com_socket_fd_inst->fd_socket;
    heart_beat_inst.header1=0xf0;
    heart_beat_inst.header2=0xf0;
    heart_beat_inst.length=0x0b;
    heart_beat_inst.counter=0;
    heart_beat_inst.tail=0xff;
    while(1){
    	for (i=0;i<6;i++){
    		if (last[i]==-1)
    			heart_beat_status[i]=255;
    		else if (last[i]==9999){
    			heart_beat_status[i]=254;
    		}
    		else {
    			heart_beat_status[i]=battery[i];
    		}

    	}
    	heart_beat_inst.status0=heart_beat_status[0];
    	heart_beat_inst.status1=heart_beat_status[1];
    	heart_beat_inst.status2=heart_beat_status[2];
    	heart_beat_inst.status3=heart_beat_status[3];
    	heart_beat_inst.status4=heart_beat_status[4];
    	heart_beat_inst.status5=heart_beat_status[5];


    	if (verbose){
    		printf("[debug] send heart_beat.\n");
    	}
    	pthread_mutex_lock(&socket_lock);
    	ret=sendto(socket_fd, (void*)com_socket_fd_inst, sizeof(heart_beat_inst),0,(struct sockaddr*)&dist_addr,sizeof(dist_addr));
    	pthread_mutex_unlock(&socket_lock);
    	printf("Send Success %d.\n",ret);
    	if (ret<0){
    		perror("Send SOCKET Failed:");
    		exit(1);}
    	heart_beat_inst.counter=heart_beat_inst.counter+1;
    	sleep(30);

    }
}
