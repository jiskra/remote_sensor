#include "read_comment.h"
#define READ_NODE_DATA 0x02




void *thread_read_com(void *data){
	int com_fd,socket_fd;
	struct com_socket_fd *com_socket_fd_inst;
    int ret;
	unsigned char* serial_in_buff;
	unsigned char* frame_buff;
	int buff_ptr=0;
	int valid_data_length=0;
	unsigned char check_sum=0;
    struct upload_data upload_data_inst;
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
		printf("Read Length is %d.\n",ret);}
		else{
		usleep(50000);
		}
		valid_data_length+=ret;
		while (valid_data_length>=FRAME_LENGTH){
				upload_data_tmp_inst=(struct upload_data*)(serial_in_buff+buff_ptr);
			  //if ((serial_in_buff[buff_ptr+i]==0xfe&&serial_in_buff[buff_ptr+i+1]==0xfc&&serial_in_buff[buff_ptr+i+2]==FRAME_LENGTH) {//&&serial_in_buff[buff_ptr+i+FRAME_LENGTH-2]==0xfa&&serial_in_buff[buff_ptr+i+FRAME_LENGTH-1]){
				if (upload_data_tmp_inst->header1==0xfe&&upload_data_tmp_inst->header2==0xfc&&upload_data_tmp_inst->tail1==0xfb&&upload_data_tmp_inst->tail2==0xfa&&upload_data_tmp_inst->length==FRAME_LENGTH) {
				  printf("A frame check,and the checksum is correct.\n");
				  //memcpy(&upload_data_inst,serial_in_buff+buff_ptr+i,sizeof(upload_data_inst));
				  pthread_mutex_lock(&socket_lock);
				  ret=sendto(socket_fd, (void*)upload_data_tmp_inst, FRAME_LENGTH,0,(struct sockaddr*)&dist_addr,sizeof(dist_addr));
				  pthread_mutex_unlock(&socket_lock);

				 if (ret<0){
				    perror("Send File Name Failed:");
				    exit(1);
				   }

				  last[upload_data_tmp_inst->node_addr-1]=0;
				  if (upload_data_tmp_inst->commend_type==0x02){
					battery[upload_data_tmp_inst->node_addr-1]=upload_data_tmp_inst->addata[1]>>2;

				  }
				  if (upload_data_tmp_inst->commend_type==0x12){
					  alarm_dis[upload_data_tmp_inst->node_addr-1]=upload_data_tmp_inst->addata[0];
				  }
				  buff_ptr=buff_ptr+FRAME_LENGTH;
				  valid_data_length=valid_data_length-FRAME_LENGTH;
			    //}
			   /*else {
			    	printf("Get the header of the frame,but the checksum is not correct.\n");
			    	buff_ptr=buff_ptr+i;
			    	valid_data_length=valid_data_length-i;
			    }*/
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

    struct read_instru_packet read_instru_packet_inst;
    com_socket_fd_inst=(struct com_socket_fd *)data;
    com_fd=com_socket_fd_inst->fd_com;
    socket_fd=com_socket_fd_inst->fd_socket;
    while(1){
    	  /* 定义一个地址，用于捕获客户端地址 */
    	  struct sockaddr_in client_addr;
    	  socklen_t client_addr_length = sizeof(client_addr);

    	  if(recvfrom(socket_fd, (void*)&read_instru_packet_inst, sizeof(read_instru_packet_inst),0,(struct sockaddr*)&client_addr, &client_addr_length) == -1)
    	  {
    	  perror("Receive Data Failed:");
    	  exit(1);
          }
    	  if (read_instru_packet_inst.header1==0xfc&&read_instru_packet_inst.header2==0xfe&& //
    		  read_instru_packet_inst.tail1==0xfa&&read_instru_packet_inst.tail2==0xfb){
    		  console_last=0;
    		   pthread_mutex_lock(&uart_lock);
    	       ret = write(com_fd,&read_instru_packet_inst,sizeof(read_instru_packet_inst));
    	       pthread_mutex_unlock(&uart_lock);
    		   if(ret == -1){
    		   printf("Wirte com error./n");
    		   pthread_exit(0);
    		   }
    		   else{
    		   printf("data_send_successful",ret);
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
    while(1){
    	read_instru_packet_inst.header1=0xfc;
    	read_instru_packet_inst.header2=0xfe;
    	read_instru_packet_inst.length =sizeof(read_instru_packet_inst);
    	read_instru_packet_inst.instru=READ_NODE_DATA;
    	read_instru_packet_inst.commend1=READ_POWER;
    	read_instru_packet_inst.tail1=0xfa;
    	read_instru_packet_inst.tail2=0xfb;
    	for (i=0;i<6;i++){
    	read_instru_packet_inst.node_addr=i+1;
    	pthread_mutex_lock(&uart_lock);
    	ret=write(com_fd,&read_instru_packet_inst,sizeof(read_instru_packet_inst));
    	pthread_mutex_unlock(&uart_lock);
    	if (ret<0){
    		printf("Write Uart error.\n");
    		pthread_exit(0);
    	}
    	sleep(2000);
    	}

    }
}
