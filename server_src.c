#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
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
	//1. Socket creation
	int server_sock = socket(AF_INET,SOCK_DGRAM,0);
	
	if(server_sock<0)
	printf("Error creating socket !\n\n");
	else
	printf("Socket created successfully !\n\n");
	

	//2. Server address assignment

	struct sockaddr_in server_addr,client_addr;
	int addrlen = sizeof(server_addr);
	memset(&server_addr,'\0',sizeof(server_addr));
	memset(&client_addr,'\0',sizeof(client_addr));

	
	server_addr.sin_port = htons(8859);
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

	//3. Server binding
	int binder = bind(server_sock,(const struct sockaddr *)&server_addr, sizeof(server_addr));
	if(binder<0)
	printf("Error Binding !\n");
	else
	printf("Binding succesful !\n");

	//File Creation

	FILE *fp;
	fp = fopen("out.txt","w");
	

	printf("\nWaiting for client");	

	//4. Accepting
	
	socklen_t len;
	len = sizeof(client_addr);
	char window_size[3];
	char packets_count[3];
	char buffer_size[3];	

	char hello[41] ="Server has acknowledged your window size";
	
	recvfrom(server_sock, window_size, 3 , 0, ( struct sockaddr *)&client_addr, &len);
	recvfrom(server_sock,  packets_count , 3 , 0, ( struct sockaddr *)&client_addr, &len);
	recvfrom(server_sock,  buffer_size , 3 , 0, ( struct sockaddr *)&client_addr, &len);
    	
    	printf("\n\nClient's window size: %s\n", window_size);
    	printf("Total packets: %s\n", packets_count);
    	
	
	sendto(server_sock, hello, strlen(hello), 0, (const struct sockaddr *)&client_addr, sizeof(client_addr)); 
	

		
	struct data_packet data;
	struct ack_packet acknowledge;
	

	int packets_received=0,packets_tot,recv_len;
	int win_size,buff_size;	

	sscanf(packets_count,"%d",&packets_tot);
	sscanf(window_size,"%d",&win_size);	
	sscanf(buffer_size,"%d",&buff_size);	

	
	printf("\nServer is now receiving packets from client...\n\n");

	int state =0;

	while(packets_received!=packets_tot)
	{
		switch(state)
		{ 
		     case 0:
		     {
			
			    recvfrom(server_sock , &data, sizeof(data) , 0, (struct sockaddr *)&client_addr, &len);
			    
			  	
				printf("RECEIVE PACKET %d : ACCEPT BASE:%d\n",data.packet_no,data.base);
				    
				    acknowledge.packet_no = data.packet_no;
				    acknowledge.base = data.base;
				    fputs(data.content,fp);			    			    
	                	    packets_received++;
				delay(50);   
			    	    sendto(server_sock, &acknowledge, sizeof(acknowledge),0,(struct sockaddr*)&client_addr,sizeof(client_addr));
			    
			state=0;
			break;		
		     }
			    
			    	
			    	
		} 
	}

		printf("\nAll packets received successfully !\n");	
		fclose(fp);
		return 0;

}
