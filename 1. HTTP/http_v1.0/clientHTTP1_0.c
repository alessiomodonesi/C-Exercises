#include<stdio.h>
#include<string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>

int main(){
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in address;

    address.sin_family = AF_INET;
    address.sin_port = htons(80); //big endian di 8080 (network byte order)

    char* ip = (char *)&address.sin_addr.s_addr;

    ip[0] = 142, ip[1] = 251, ip[2] = 153, ip[3] = 119;

    int c = connect(sockfd,(struct sockaddr*) &address, sizeof(address));

    if(c == 0){
        printf("connessione stabilita\n");
    } else {
        perror("connessione fallita");
        exit(1);
    }

    char buffer[] = "GET /imghp?hl=it&ogbl HTTP/1.0\r\n\r\n";

    int byteScritti = 0;
    int m =0;
   while(byteScritti < strlen(buffer)){
       m = write(sockfd, buffer + byteScritti, strlen(buffer)- byteScritti);
       byteScritti += m;
   }

    printf("ho scritto %d bytes\n", byteScritti);

    char response[1000000];

    int n = 0;
    int byteLetti = 0;
    while( (n = read(sockfd, response + byteLetti, sizeof(response)- byteLetti)) > 0){
        //printf("numero caratteri letti:%d\n", n);
        byteLetti += n;
        //printf("numero caratteri letti finora:%d\n", byteLetti);
    }

    printf("il server dice:%s\n", response);
}
