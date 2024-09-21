#include "csapp.h"

/* Recommended max cache and object sizes */
#define MAX_CACHE_SIZE 1049000
#define MAX_OBJECT_SIZE 102400

void trans(int client_proxy_fd);
void parse_uri(char *uri, char *path, char *hostname, char *port);

/* You won't lose style points for including this long line in your code */
static const char *user_agent_hdr =
    "User-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:10.0.3) Gecko/20120305 "
    "Firefox/10.0.3\r\n";

int main(int argc, char* argv[])
{
    if (argc != 2) {
        fprintf(stderr, "usage: %s <port>\n", argv[0]);
        exit(1);
    }
    
    int listenfd, connfd;
    struct sockaddr_storage clientaddr;     // 클라이언트의 주소 정보 저장 (ip주소, port번호)
    socklen_t clientlen;

    listenfd = Open_listenfd(argv[1]);
    while(1) {
        connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen);
        printf("========================\n");
        trans(connfd);
        printf("========================\n\n");
        Close(connfd);
    }

    return 0;
}

void trans(int client_proxy_fd)
{
    int proxyfd;
    int read_len;
    char buf[MAXBUF];
    char first_line[MAXLINE], method[MAXLINE], uri[MAXLINE], version[MAXLINE];
    char path[MAXLINE], hostname[100], port[10];
    rio_t rio_client, rio_server;

    // Client -> Proxy 헤더 첫번째 요청 라인                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        정보 받기
    Rio_readinitb(&rio_client, client_proxy_fd);
    Rio_readlineb(&rio_client, buf, MAXBUF);
    strcpy(first_line, buf);
    printf("client header : %s\n", first_line);

    // Client -> Proxy 첫번째 요청 라인 분석
    sscanf(first_line, "%s %s %s", method, uri, version);
    printf("method: %s\n", method);
    printf("uri: %s\n", uri);
    printf("version: %s\n", version);

    // Proxy 소켓 생성
    parse_uri(uri, path, hostname, port);
    proxyfd = Open_clientfd(hostname, port);

    // Proxy -> Server 요청 라인 전송
    sprintf(buf, "%s %s %s\r\n", method, path, version);
    Rio_writen(proxyfd, buf, strlen(buf));

    // Client -> Proxy 나머지 헤더 요청 라인 읽고 Proxy -> Server 요청 라인 전송
    printf("-------   client header   -------\n");
    while((read_len = Rio_readlineb(&rio_client, buf, MAXBUF)) > 0) {
        Rio_writen(proxyfd, buf, read_len);
        if (strcmp(buf, "\r\n") == 0) {
            break;
        }
        printf("%s", buf);
    }
    printf("------- client header end -------\n\n");

    // Proxy <- Server 응답 읽고 Client <- Proxy 응답
    Rio_readinitb(&rio_server, proxyfd);
    printf("-------   server response   -------\n");
    while((read_len = Rio_readlineb(&rio_server, buf, MAXBUF)) > 0) {
        Rio_writen(client_proxy_fd, buf, read_len);
        printf("%s", buf);
    }
    printf("\n------- server response end -------\n\n");
}

void parse_uri(char *uri, char *path, char *hostname, char *port) {
    // 포트가 있을 경우만 포트 추출
    if (strstr(uri, ":")) {
        sscanf(uri, "http://%99[^:]:%9[^/]/", hostname, port);
    } else {
        sscanf(uri, "http://%99[^/]/", hostname);
    }
    sscanf(uri, "http://%*[^/]%s", path);

    printf("\n-------   parse_uri   -------\n");
    printf("path: %s\n", path);
    printf("hostname: %s\n", hostname);
    printf("port: %s\n", port);
    printf("------- parse_uri end -------\n\n");
}