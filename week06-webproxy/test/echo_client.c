#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    int clientfd = 0;
    struct sockaddr_in serv_addr;
    char *message = "Hello, Server!";
    char buffer[BUFFER_SIZE] = {0};

    // 소켓 생성
    printf("소켓 생성\n");
    if ((clientfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // IPv4 주소 변환
    printf("IPv4 주소 변환\n");
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0) {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    // 서버에 연결
    printf("서버에 연결\n");
    if (connect(clientfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnection Failed \n");
        return -1;
    }

    // 데이터 송신
    printf("데이터 송신\n");
    send(clientfd, message, strlen(message), 0);
    printf("Message sent: %s\n", message);

    // 데이터 수신
    printf("데이터 수신\n");
    int valread = recv(clientfd, buffer, BUFFER_SIZE, 0);
    if (valread > 0) {
        buffer[valread] = '\0';
        printf("Message received: %s\n", buffer);
    } else {
        printf("No message received or error occurred.\n");
    }

    close(clientfd);
    return 0;
}