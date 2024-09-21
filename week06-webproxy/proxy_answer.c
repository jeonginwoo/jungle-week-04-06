#include "csapp.h"

/* Recommended max cache and object sizes */
#define MAX_CACHE_SIZE 1049000
#define MAX_OBJECT_SIZE 102400

/* You won't lose style points for including this long line in your code */
static const char *user_agent_hdr =
    "User-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:10.0.3) Gecko/20120305 "
    "Firefox/10.0.3\r\n";

void doit(int fd);
void read_requesthdrs(rio_t *rp);
void clienterror(int fd, char *cause, char *errnum, char *shortmsg, char *longmsg);
void parse_uri(char *uri, int *proxyfd);

int main(int argc, char **argv)
{
    int listenfd, connfd;
    char hostname[MAXLINE], port[MAXLINE];
    socklen_t clientlen;
    struct sockaddr_storage clientaddr;

    /* Check command line args */
    if (argc != 2) {
        fprintf(stderr, "usage: %s <port>\n", argv[0]);
        exit(1);
    }

    listenfd = Open_listenfd(argv[1]);
    while (1) {
        clientlen = sizeof(clientaddr);
        connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen); // line:netp:tiny:accept
        Getnameinfo((SA *)&clientaddr, clientlen, hostname, MAXLINE, port, MAXLINE, 0);
        doit(connfd);
        Close(connfd);
    }
}

void doit(int fd)
{
    int proxyfd;
    char buf_client[MAXLINE], buf_server[MAXLINE], method[MAXLINE], uri[MAXLINE], version[MAXLINE];
    char hostname[MAXLINE], port[MAXLINE];
    rio_t rio_client, rio_server;

    // 클라이언트로부터 첫 번째 요청 라인 읽기
    Rio_readinitb(&rio_client, fd);
    Rio_readlineb(&rio_client, buf_client, MAXLINE);
    printf("%s", buf_client);

    // 요청 라인에서 메소드, URI, 버전 추출
    sscanf(buf_client, "%s %s %s", method, uri, version);
    printf("\n ####### header first line #######\n");
    printf("method: %s\n", method);
    printf("uri: %s\n", uri);
    printf("version: %s\n", version);

    // URI를 통해 proxyfd 초기화
    parse_uri(uri, &proxyfd);
    sprintf(buf_client, "%s %s %s", method, uri, version);

    // 서버에 요청 라인 전송
    Rio_writen(proxyfd, buf_client, strlen(buf_client));

    // 클라이언트로부터 나머지 헤더를 읽어서 서버로 전달
    while (strcmp(buf_client, "\r\n")) {
        Rio_readlineb(&rio_client, buf_client, MAXLINE); // 클라이언트로부터 헤더 읽기
        Rio_writen(proxyfd, buf_client, strlen(buf_client)); // 서버에 헤더 전송
    }
    // 헤더의 끝을 알리는 빈 줄 전달
    Rio_writen(proxyfd, "\r\n", 2);

    // 서버로부터 응답을 읽어 클라이언트로 전달
    Rio_readinitb(&rio_server, proxyfd);    // server->proxy rio 초기화
    ssize_t n;
    while ((n = Rio_readlineb(&rio_server, buf_server, MAXLINE)) > 0) {
        Rio_writen(fd, buf_server, n); // 서버 응답을 클라이언트로 전달
    }

    // 서버와의 연결 종료
    Close(proxyfd);
}


void read_requesthdrs(rio_t *rp)
{
    char buf[MAXLINE];

    Rio_readlineb(rp, buf, MAXLINE);
    while(strcmp(buf, "\r\n")) {
        Rio_readlineb(rp, buf, MAXLINE);
        printf("%s", buf);
    }
}

void parse_uri(char *uri, int *proxyfd) {
    char host[100];
    char port[10] = "80";  // 기본 포트는 80으로 설정

    // 포트가 있을 경우만 포트 추출
    if (strstr(uri, ":")) {
        sscanf(uri, "http://%99[^:]:%9[^/]/", host, port);
    } else {
        sscanf(uri, "http://%99[^/]/", host);
    }
    sscanf(uri, "http://%*[^/]%s", uri);

    printf("\n ####### parse_uri #######\n");
    printf("uri: %s\n", uri);
    printf("host: %s\n", host);
    printf("port: %s\n\n", port);
    *proxyfd = Open_clientfd(host, port);

    Rio_writen()
}

