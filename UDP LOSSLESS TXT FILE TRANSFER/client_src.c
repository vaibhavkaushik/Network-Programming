#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<unistd.h>
#include<netinet/in.h>
#include<string.h>
#include<arpa/inet.h>
#include<time.h>


struct ack_packet
{
    int packet_no;
    int status;
    int base;
   
};

struct data_packet
{
    int packet_no;
    char content[1024];
    int base;
   	
};

void delay(int number_of_seconds)
{
    // Converting time into milli_seconds
    int milli_seconds = 10000 * number_of_seconds;
 
    // Stroing start time
    clock_t start_time = clock();
 
    // looping till required time is not acheived
    while (clock() < start_time + milli_seconds)
        ;
}



int main()
{
    //1. creating socket first

        int sock_val = socket(AF_INET,SOCK_DGRAM,0);

        //PF_INET it species the socket will follow the protocol from internet protocol family.
        //SOCK_DGRAM specifies that we will use datagram socket.
        //0 specifies that we will follow the default protocols as per the specified family.

        //sock_val will become our socket descriptor.

        if(sock_val<0)
        printf("Unable to create socket !\n");

        else
        printf("Socket created successfully !\n\n");
  
    //3. server address structure construction
  
    struct sockaddr_in server_address;
    memset(&server_address,0,sizeof(server_address));
  
    //intialzing parts of server_address
  
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(8859);
    server_address.sin_addr.s_addr = inet_addr("127.0.0.1");

    //while assigning s_addr, we can also use a particular server address using inet_addr("a.b.c.d");
  


    //5. File read
    FILE *fptr;
    char file_content,file_name[15];
    int file_size;

    printf("Enter txt file name:\t");
       scanf("%s",file_name);

    fptr = fopen(file_name,"r");

    if(fptr==NULL)
    { printf("File does not exist !\n");
      exit(0);
    }

   
    //finding file size
     fseek(fptr,0,SEEK_END);
     file_size = ftell(fptr);

     printf("File Size:%d\t\n\n",file_size);

    //6.Calculating number of packets
         int packets_count,buffer_size;
         printf("Enter buffer size:\t");
         scanf("%d",&buffer_size);

    if(file_size % buffer_size == 0)
     {
             packets_count = file_size/buffer_size;
    }   
     else
     {
           packets_count = (file_size/buffer_size)+1;       
     }   
   
     printf("Number of packets to be sent:\t%d\n\n",packets_count);

     //7.Decide the window size
     int window_size;
     printf("Enter window size:\t");
        scanf("%d",&window_size);
    char send_window_size[3];
    char send_packet_count[3];
    char send_buffer_size[3];

   
    sprintf(send_window_size,"%d",window_size);   
    sprintf(send_packet_count,"%d",packets_count);   
    sprintf(send_buffer_size,"%d",buffer_size);   
       

     sendto(sock_val, send_window_size, 3 , 0 ,(const struct sockaddr *)&server_address, sizeof(server_address)); //send window size
    sendto(sock_val, send_packet_count, 3 , 0 ,(const struct sockaddr *)&server_address, sizeof(server_address)); //send total packets
    sendto(sock_val, send_buffer_size, 3 , 0 ,(const struct sockaddr *)&server_address, sizeof(server_address));  //send buffer size


     char response[40];
     int length=sizeof(server_address),last_index;
    int x=0;
   

     x=recvfrom( sock_val , response, 40 , 0 ,(struct sockaddr *)&server_address, &length);
   
    printf("\n%s\n\n",response);

    printf("Client is now sending packets to server...\n\n");




     int total_packets=packets_count;
    
     //8. Creating the packets beforehand
   
     struct data_packet DATA[total_packets+2];
     // setting file pointer to start
     fseek(fptr,0,SEEK_SET);
     int packet_size = buffer_size,count=0,index=0;   
       
        while(file_size - ftell(fptr))
       {
         DATA[index].packet_no = count;
      
          //DATA[index].content = (char *)calloc(buffer_size+2,sizeof(char));
         fread(DATA[index].content,sizeof(char),buffer_size,fptr);
         count++;
        index++;
       }

    struct data_packet sender_window[window_size];
    struct ack_packet acknowledge;
   
    for(int i=0;i<window_size;i++)
    {
        sender_window[i].packet_no = DATA[i].packet_no;
    }
   
	
    

    int base=0,window,state=0,packets_sent=0,is_sent,is_recv,count_case0=0,count_case1=0;
    int i=1;
   	
	int received_arr[window_size],timed_out[window_size];
	for(int i=0;i<window_size;i++)
	{
		received_arr[i]=0;
		timed_out[i]=0;
	}
	
	index=0;
   
    while(packets_sent!=total_packets)
    {   
              
     
           switch(state)
           {
			case 0: 
		{
 			for(int i=0;i<window_size;i++)
		 	{
			   
			   printf("SEND PACKET %d BASE:%d\n",i,DATA[i].base);
			   sendto(sock_val, &DATA[i] , sizeof(DATA[i]) , 0 ,(const struct sockaddr*)&server_address, sizeof(server_address));
			   packets_sent++;
			}
            	}
            
            state=1;
            break;
            
            
                  case 1:
		{	   				
			   	recvfrom( sock_val , &acknowledge , sizeof(acknowledge) , 0 ,(struct sockaddr *)&server_address, &length);
			   
				if(acknowledge.packet_no == base) base++;
				  
				printf("RECEIVE ACK %d BASE:%d\n",acknowledge.packet_no,base);

				 
				 DATA[ base + window_size - 1].base = base; 
				  delay(100);
				  printf("SEND PACKET %d BASE:%d\n", base + window_size - 1,DATA[ base + window_size - 1].base);
				  sendto(sock_val, &DATA[ base  + window_size - 1] , sizeof(DATA[ base  + window_size - 1]) , 0 ,(const struct sockaddr*)&server_address, sizeof(server_address));
				  packets_sent++;	
		}
		state=1;
		break;
			      

	   }

	}           
       
    printf("\n\nAll packets transferred successfully !!\n");
   

    return 0;  
  
}


