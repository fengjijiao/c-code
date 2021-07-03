// Server side implementation of UDP client-server model
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define	PORT	(8000)
#define	MAX_UDP_PACKET_SIZE	(8192)
#define	DEBUG	(1)
  
// Driver code
int main() {
    int sockfd;
    char buffer[MAX_UDP_PACKET_SIZE];
    struct sockaddr_in servaddr, cliaddr;
      
    // Creating socket file descriptor
    if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }
      
    memset(&servaddr, 0, sizeof(servaddr));
    memset(&cliaddr, 0, sizeof(cliaddr));
      
    // Filling server information
    servaddr.sin_family    = AF_INET; // IPv4
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(PORT);
      
    // Bind the socket with the server address
    if ( bind(sockfd, (const struct sockaddr *)&servaddr, 
            sizeof(servaddr)) < 0 )
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
      
    int len;
  
    len = sizeof(cliaddr);  //len is value/resuslt

	while(1) {
		FILE* f = NULL;
		int n = MAX_UDP_PACKET_SIZE;
		int i = 0;
		int received_length = 0;
		while(n == MAX_UDP_PACKET_SIZE) {
			n = recvfrom(sockfd, (char *)buffer, MAX_UDP_PACKET_SIZE, MSG_WAITALL, ( struct sockaddr *) &cliaddr, &len);
			if (f == NULL) {
				if (n > 0) {
					f = fopen("./test2.jpeg", "wb");
				}else {
					printf("error\n");
					break;
				}
			}
			int wt = fwrite(buffer+2, 1, n-2, f);//len = buffers[capture_buf.index].length
			received_length += n;
			printf("%d bytes was be received.\n", received_length);
			i++;
		}
		printf("write complete.\n");
		fclose(f);
	}
	
    // sendto(sockfd, (const char *)hello, strlen(hello), 
    //     MSG_CONFIRM, (const struct sockaddr *) &cliaddr,
    //         len);
    // printf("Hello message sent.\n"); 
      
    return 0;
}