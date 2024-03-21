#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <vector>


void* handle_client(void* data)
{
	int client_socket = *(static_cast<int*>(data));
	char buffer[3001];
	// Receive message from client
	while(true)
	{
		int rs = recv(client_socket, buffer, 3000, 0);
        if (rs < 0)
	    {
	       	std::perror("client socket connection error");
			close(client_socket);
            return nullptr;
		}
		else if (rs > 0)
		{
			std::cout << "Got message:\n";
			buffer[rs] = '\0';
			std::cout << buffer << "\n";
		}
		else
		{
			std::cout<<"Disconnected client \n";
			break;
		}
	}
	close(client_socket);
	return nullptr;
}

int main()
{
	// create socket
	int server_socket = socket(AF_INET, SOCK_STREAM, 0);
	if(server_socket == -1)
	{
		std::perror("socket");
		exit(errno);
	}

	// create an endpoint
	
	// socket address
	struct sockaddr_in server_address;

	// internet protocol = AF_INET
	server_address.sin_family = AF_INET;
	// accept or any address (bind the socket to all available interfaces)
	server_address.sin_addr.s_addr = htonl(INADDR_ANY);
	// port
	server_address.sin_port = htons(9888);

	// Bind server_socket to server_address
	if (bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) 
	{
		std::perror("bind failed");
		exit(errno);
	}

	if (listen(server_socket, 1024) < 0) 
	{
		std::perror("listen failed");
		exit(errno);
       	}
	std::cout << "Waiting for connection\n";
	
	std::vector<pthread_t> threads;

	while(true)
       	{
		int client_socket;
	        struct sockaddr_in client_address;
		unsigned int client_addr_len = sizeof(client_address);
		// Accept incoming connection
		if ((client_socket = accept(server_socket, (struct sockaddr*) &client_address, &client_addr_len)) < 0) 
		{
			std::perror("accept failed");
		        exit(errno);
		}
		std::cout << "Connected client with address: " << inet_ntoa(client_address.sin_addr) << "\n";

		pthread_t thread;
		if(pthread_create(&thread, nullptr, handle_client, &client_socket) != 0)
		{
			std::perror("pthread");
			exit(EXIT_FAILURE);
		}
		threads.push_back(thread);
	}
	for(auto& x : threads)
	{
		if(pthread_join(x, nullptr) != 0)
		{
			std::perror("join");
			exit(EXIT_FAILURE);
		}
	}

	// close
	close(server_socket);
	return 0;
}
