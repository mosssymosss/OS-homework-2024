#include <iostream>
#include <fstream>
#include <sstream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>

const std::string SERVER_IP = "217.113.2.233"; // IP address of w
const int SERVER_PORT = 80;
const std::string FILENAME = "ysu.html";
const std::string serverAddress = "www.ysu.am";


int main()
{
	std::string request = "GET / HTTP/1.1\r\n";
	request += "Host: www.ysu.am\r\n";
	request += "Connection: close\r\n\r\n";

	int client_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (client_socket == -1) 
	{
		std::perror("Socket creation failed");
		exit(errno);
	}

		
	struct sockaddr_in server_address;
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(SERVER_PORT);
	server_address.sin_addr.s_addr = inet_addr(SERVER_IP.c_str());
	if (inet_pton(AF_INET, serverAddress.c_str(), &server_address.sin_addr) < 0) 
	{
		std::perror("inet_pton error");
		close(client_socket);
		exit(EXIT_FAILURE);
	}

	if (connect(client_socket, (struct sockaddr*)&server_address, sizeof(server_address)) == -1) 
	{
		std::perror("Connection failed");
		close(client_socket);
		exit(errno);
	}

	if (send(client_socket, request.c_str(), request.length(), 0) == -1) 
	{
		std::perror("Send failed");
		close(client_socket);
		exit(errno);
	}

	std::stringstream response_buffer;
	char buffer[4096];
	int bytes_received;
	while ((bytes_received = recv(client_socket, buffer, sizeof(buffer), 0)) > 0) 
	{
		response_buffer.write(buffer, bytes_received);
	}
	if (bytes_received == -1) 
	{
		std::perror("Receive failed");
		close(client_socket);
		exit(errno);
	}
	std::cout << response_buffer.str() << std::endl;

	std::ofstream outfile(FILENAME);
	if (!outfile) 
	{
		std::cerr << "Error opening file for writing" << std::endl;
		close(client_socket);
		exit(EXIT_FAILURE);
	}
	outfile << response_buffer.str();
	outfile.close();
	close(client_socket);


	return 0;
}

