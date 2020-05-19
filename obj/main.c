// UDP to serial communicaton bridge for raspbrerry pi
// Program inspired on the project ... https://github.com/vlaate/SkyWatcherWiFi
//rigelinorion 05.2020
#include "includes.inc"

int DEBUG = 1;
#define PORT	 	11880 
#define MAXLINE 	1024 
#define timeOut 	20
#define bufferSize 	1024

char serialBuffer[bufferSize];
uint8_t serialIndex = 0;
uint8_t ignore = 1; 

int serial_port;
int sockfd; 
struct sockaddr_in servaddr, cliaddr;


int main( int argc, char *argv[], char *envp[] ) {
	if(argc>1)DEBUG=1;
	// Creating socket file descriptor 
	if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) { 
		perror("socket creation failed"); 
		exit(EXIT_FAILURE); 
	} 
	
	memset(&servaddr, 0, sizeof(servaddr)); 
	memset(&cliaddr, 0, sizeof(cliaddr)); 
	
	// Filling server information 
	servaddr.sin_family = AF_INET; // IPv4 
	servaddr.sin_addr.s_addr = INADDR_ANY; 
	servaddr.sin_port = htons(PORT); 
	
	// Bind the socket with the server address 
	if ( bind(sockfd, (const struct sockaddr *)&servaddr, 
			sizeof(servaddr)) < 0 ) 
	{ 
		perror("bind failed"); 
		exit(EXIT_FAILURE); 
	} 
	
  	
  	if ((serial_port = serialOpen ("/dev/serial0", 9600)) < 0){
    	fprintf (stderr, "Unable to open serial device: %s\n", strerror (errno)) ;
  	}

	while(1)
    {
     char dat;
     char buffer[MAXLINE];
     int len, n;
	 len = sizeof(cliaddr); //len is value/resuslt

     n = recvfrom(sockfd, (char *)buffer, MAXLINE , MSG_DONTWAIT, ( struct sockaddr *) &cliaddr, &len);
		if(n>0){
			for(int i = 0; i<n;i++){
				serialPutchar(serial_port, buffer[i]);
				if(DEBUG)printf("program-->telescope %c", buffer[i]);	
			}
			if(DEBUG)printf("\n", ' ');
			ignore = 1;
		} 
		
		// 
		if(serialDataAvail (serial_port)){
			dat = serialGetchar (serial_port);
			while(serialDataAvail (serial_port)){ 
				dat = serialGetchar (serial_port);
				if((dat==61) || (dat==33)){
					ignore = 0;
				}
				if(!ignore){
					serialBuffer[serialIndex] = dat; 
          			if (serialIndex < bufferSize - 1){
            			serialIndex++;
          			}
				}
				usleep(timeOut*100);
			}
			sendto(sockfd, (char *)serialBuffer, serialIndex,  MSG_DONTWAIT, (const struct sockaddr *) &cliaddr, len);
			if(DEBUG)printf("telescope-->program %c", serialBuffer);
			serialIndex = 0;
		}
	}
	return 0; 
} 
