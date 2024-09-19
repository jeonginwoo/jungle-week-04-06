#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    int listenfd, connfd;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE] = {0};
    const char *response = "Echo: ";

    // 소켓 생성
    printf("소켓 생성\n");
    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // 소켓 옵션 설정
    printf("소켓 옵션 설정\n");
    if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        perror("setsockopt");
        close(listenfd);
        exit(EXIT_FAILURE);
    }

    // 주소 설정
    printf("주소 설정\n");
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // 소켓에 주소 바인딩
    printf("소켓에 주소 바인딩\n");
    if (bind(listenfd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        close(listenfd);
        exit(EXIT_FAILURE);
    }

    // 연결 요청 대기
    printf("연결 요청 대기\n");
    if (listen(listenfd, 3) < 0) {
        perror("listen");
        close(listenfd);
        exit(EXIT_FAILURE);
    }

    printf("Echo server is listening on port %d\n", PORT);

    // 클라이언트 연결 수락
    printf("클라이언트 연결 수락\n");
    if ((connfd = accept(listenfd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
        perror("accept");
        close(listenfd);
        exit(EXIT_FAILURE);
    }

    // 데이터 수신 및 송신
    printf("데이터 수신 및 송신\n");
    int valread;
    while ((valread = recv(connfd, buffer, BUFFER_SIZE, 0)) > 0) {
        send(connfd, response, strlen(response), 0);
        send(connfd, buffer, valread, 0);
        memset(buffer, 0, BUFFER_SIZE);
    }

    if (valread < 0) {
        perror("recv");
    }

    printf("끝\n");
    close(connfd);
    close(listenfd);
    return 0;
}