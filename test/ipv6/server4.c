#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include <arpa/inet.h>

void error(char *msg) {
	perror(msg);
	exit(1);
}

int main(int argc, char *argv[]) {
	int sockfd, newsockfd, portno;
	socklen_t clilen;
	char buffer[256];
	struct sockaddr_in serv_addr, cli_addr;
	int n;


	if (argc < 2) {
		fprintf(stderr, "Usage: %s <listening port>\n", argv[0]);
		exit(0);
	}

	printf("\nIPv4 TCP Server Started...\n");

	//Sockets Layer Call: socket()	
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
		error("ERROR opening socket");
	bzero((char *) &serv_addr, sizeof(serv_addr));
	portno = atoi(argv[1]);

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno);

	
	//Sockets Layer Call: bind()
	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
		error("ERROR on binding");

	//Sockets Layer Call: listen()
	listen(sockfd, 5);
	clilen = sizeof(cli_addr);
	
	//Sockets Layer Call: accept()
	newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
	if (newsockfd < 0)
		error("ERROR on accept");

	//Sockets Layer Call: inet_ntoa()

	printf("Incoming connection from client having IPv4 address: %s\n",	inet_ntoa(cli_addr.sin_addr));

	memset(buffer,0, 256);
	
	//Sockets Layer Call: recv()
	n = recv(newsockfd, buffer, 255, 0);
	if (n < 0)
		error("ERROR reading from socket");

	printf("Message from client: %s\n", buffer);

	//Sockets Layer Call: send()
	n = send(newsockfd, "Server got your message", 23+1, 0);
	if (n < 0)
		error("ERROR writing to socket");
	
	//Sockets Layer Call: close()
	close(sockfd);
	close(newsockfd);
	
	return 0;
}
