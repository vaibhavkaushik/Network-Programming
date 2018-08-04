#include<stdio.h>
#include<stdio.h>

#include<sys/types.h>
#include<sys/socket.h>

#include<netinet/in.h>
#include <unistd.h>

int main()
{
 	//create socket
	int network_socket;
	network_socket = socket(AF_INET,SOCK_STREAM,0);

	//specify an address for socket
	struct sockaddr_in   server_address;
        
	server_address.sin_family = AF_INET;
 	server_address.sin_port = htons(9002);
	server_address.sin_addr.s_addr = INADDR_ANY;

	int connection_status = connect(network_socket,(struct sockaddr *) &server_address, sizeof(server_address));

	//check status
	if(connection_status==-1)
	{ printf("ERROR");}
	
	//receive data from server
	char server_response[256];
	recv(network_socket,&server_response,sizeof(server_response) ,0);	

	//print server response
	printf("server response: %s",server_response);

	//close socket
	close(network_socket);


 	return 0;
}	
