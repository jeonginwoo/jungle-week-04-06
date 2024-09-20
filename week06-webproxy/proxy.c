#include "csapp.h"

/* Recommended max cache and object sizes */
#define MAX_CACHE_SIZE 1049000
#define MAX_OBJECT_SIZE 102400

void doit(int fd);
void read_requesthdrs(rio_t *rp);
void clienterror(int fd, char *cause, char *errnum, char *shortmsg, char *longmsg);
void parse_uri(char *uri, int *proxyfd);

/* You won't lose style points for including this long line in your code */
static const char *user_agent_hdr =
    "User-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:10.0.3) Gecko/20120305 "
    "Firefox/10.0.3\r\n";

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
        printf("\n\n\nAccepted connection from (%s, %s)\n", hostname, port);
        doit(connfd);  // line:netp:tiny:doit
        Close(connfd); // line:netp:tiny:close
    }
}

void doit(int fd)
{
    int proxyfd;
    int is_static;
    struct stat sbuf;
    char buf[MAXLINE], method[MAXLINE], uri[MAXLINE], version[MAXLINE];
    rio_t rio;

    Rio_readinitb(&rio, fd);    // 버퍼링된 읽기를 위한 Rio(Robust I/O) 패키지의 초기화 함수. rio_t 타입의 버퍼 구조체를 초기화.
    Rio_readlineb(&rio, buf, MAXLINE);
    printf("Request headers:\n");
    printf("%s", buf);
    sscanf(buf, "%s %s %s", method, uri, version);
    read_requesthdrs(&rio);

    parse_uri(uri, &proxyfd);
    Rio_readinitb(&rio, proxyfd);
    Rio_writen(proxyfd, buf, strlen(buf));
    Rio_readlineb(&rio, buf, MAXLINE);
    Fputs(buf, stdout);
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
    char port[10];
    
    sscanf(uri, "http://%99[^:]:%9[^/]/", host, port);
    printf("host: %s\n", host);
    printf("port: %s\n", port);
    *proxyfd = Open_clientfd(host, port);
}
