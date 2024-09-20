/* $begin tinymain */
/*
 * tiny.c - A simple, iterative HTTP/1.0 Web server that uses the
 *     GET method to serve static and dynamic content.
 *
 * Updated 11/2019 droh
 *   - Fixed sprintf() aliasing issue in serve_static(), and clienterror().
 */
#include "csapp.h"

void doit(int fd);
void read_requesthdrs(rio_t *rp);
int parse_uri(char *uri, char *filename, char *cgiargs);
void serve_static(int fd, char *filename, int filesize);
void get_filetype(char *filename, char *filetype);
void serve_dynamic(int fd, char *filename, char *cgiargs);
void clienterror(int fd, char *cause, char *errnum, char *shortmsg, char *longmsg);

/**
 * 목적 : 서버를 실행하고 클라이언트 요청을 받습니다.
 * 구성
 * - 영령줄 인자로 포트 번호를 받습니다. 이 포트는 서버가 클라이언트의 요청을 대기하는 데 사용됩니다.
 * - Open_listenfd를 사용해 소켓을 열고, 계속해서 클라이언트의 연결을 기다립니다.
 * - 클라이언트가 연결되면, 연결을 수락(Accept)하고 해당 클라이언트와 통신을 시작합니다.
 * - 각 클라이언트 요청은 doit 함수에서 처리되며, 처리 후 연결을 닫습니다.(Close).
 */
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

/**
 * 목적 : 클라이언트로부터 온 요청을 분석하고 적절히 처리합니다.
 * 구성
 * - 클라이언트의 요청을 읽고, 요청의 메소드(GET), URI, 버전을 파싱합니다.
 * - GET 메소드가 아닌 경우, 에러 메시지를 보내고 종료합니다.
 * - 요청 헤더를 읽고(read_requesthdrs), URI를 파싱하여 정적 또는 동적 콘텐츠인지 확인합니다.(parse_uri).
 * - 파일 정보(stat)를 확인하고, 정적 컨텐츠라면 serve_static, 동적 컨텐츠라면 serve_dynamic 함수를 호출합니다.
 */
void doit(int fd)
{
    printf("\n####### doit #######\n");
    int is_static;
    struct stat sbuf;
    char buf[MAXLINE], method[MAXLINE], uri[MAXLINE], version[MAXLINE];
    char filename[MAXLINE], cgiargs[MAXLINE];
    rio_t rio;

    Rio_readinitb(&rio, fd);    // 버퍼링된 읽기를 위한 Rio(Robust I/O) 패키지의 초기화 함수. rio_t 타입의 버퍼 구조체를 초기화.
    Rio_readlineb(&rio, buf, MAXLINE);
    printf("Request headers:\n");
    printf("%s", buf);
    // Rio_writen(fd, buf, strlen(buf));       // 요청 라인을 클라이언트에게 다시 돌려줌 (echo)
    sscanf(buf, "%s %s %s", method, uri, version);
    if (strcasecmp(method, "GET")) {
        clienterror(fd, method, "501", "Not implemented", "Tiny does not implement this method");
        return;
    }
    read_requesthdrs(&rio);
    is_static = parse_uri(uri, filename, cgiargs);
    if (stat(filename, &sbuf) < 0) {
        clienterror(fd, filename, "404", "Not found", "Tiny couldn't find this file");
        return;
    }

    if (is_static) {
        if (!(S_ISREG(sbuf.st_mode)) || !(S_IRUSR & sbuf.st_mode)) {
            clienterror(fd, filename, "403", "Forbidden", "Tiny couldn't read the file");
            return;
        }
        serve_static(fd, filename, sbuf.st_size);
    }
    else {
        if (!(S_ISREG(sbuf.st_mode)) || !(S_IXUSR & sbuf.st_mode)) {
            clienterror(fd, filename, "403", "Forbidden", "Tiny couldn't run the CGI program");
            return;
        }
        serve_dynamic(fd, filename, cgiargs);
    }
    printf("####### doit #######\n");
}

/**
 * 목적 : 클라이언트에게 에러 메시지를 전송합니다.
 * 구성
 * - 에러 메시지의 HTML 형식을 작성하고, 클라이언트에게 전송합니다.
 * - 에러 번호(예: 404), 에러 원인, 에러 메시지 등을 포함한 HTTP 응답을 생성합니다.
 */
void clienterror(int fd, char *cause, char *errnum, char *shortmsg, char *longmsg)
{
    printf("\n####### clienterror #######\n");
    char buf[MAXLINE], body[MAXBUF];

    sprintf(body, "<html><title>Tiny Error</title>");
    sprintf(body, "%s<body bgcolor=""ffffff"">\r\n", body);
    sprintf(body, "%s%s: %s\r\n", body, errnum, shortmsg);
    sprintf(body, "%s<p>%s: %s\r\n", body, longmsg, cause);
    sprintf(body, "%s<hr><em>The Tiny Web server</em>\r\n", body);

    sprintf(buf, "HTTP/1.0 %s %s\r\n", errnum, shortmsg);
    Rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "Content-type: text/html\r\n");
    Rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "Content-length: %d\r\n\r\n", (int)strlen(body));
    Rio_writen(fd, buf, strlen(buf));
    Rio_writen(fd, body, strlen(body));
    printf("####### clienterror #######\n");
}

/**
 * 목적 : 요청 헤더를 읽어들이는 함수입니다.
 * 구성
 * - 클라이언트가 보낸 요청 헤더를 한 줄씩 읽습니다.
 * - 개행(\r\n)이 나올 때까지 계속 읽어들이고, 출력해줍니다.
 */
void read_requesthdrs(rio_t *rp)
{
    printf("\n####### read_requesthdrs #######\n");
    char buf[MAXLINE];

    Rio_readlineb(rp, buf, MAXLINE);
    while(strcmp(buf, "\r\n")) {
        Rio_readlineb(rp, buf, MAXLINE);
        printf("%s", buf);
        // Rio_writen(rp->rio_fd, buf, strlen(buf)); // 클라이언트에 헤더를 돌려줌 (echo)
    }
    printf("####### read_requesthdrs #######\n");
}

/**
 * 목적 : 클라이언트가 요청한 URI를 파싱하여 파일 이름과 CGI 인자를 구분합니다.
 * 구성
 * - URI가 "cgi-bin"을 포함하고 있지 않으면 정적 컨텐츠로 간주하고 파일명을 생성합니다. 이 경우, URI의 끝이 /이면 home.html을 반환합니다.
 * - "cgi-bin"을 포함한 URI는 동적 컨텐츠로 처리하며, cgiargs에 인자를 저장하고 파일명을 생성합니다.
 */
int parse_uri(char *uri, char *filename, char *cgiargs)
{
    printf("\n####### parse_uri #######\n");
    char *ptr;

    if (!strstr(uri, "cgi-bin")) {
        strcpy(cgiargs, "");
        strcpy(filename, ".");
        strcat(filename, uri);
        if (uri[strlen(uri)-1] == '/')
            strcat(filename, "home.html");
        printf("####### parse_uri static #######\n");
        return 1;
    }
    else {
        ptr = index(uri, '?');
        if (ptr) {
            strcpy(cgiargs, ptr+1);
            *ptr = '\0';
        }
        else
            strcpy(cgiargs, "");
        strcpy(filename, ".");
        strcat(filename, uri);
        printf("####### parse_uri dynamic #######\n");
        return 0;
    }
}

/**
 * 목적 : 정적 컨텐츠(파일)를 클라이언트에게 제공하는 함수입니다.
 * 구성
 * - 파일의 종류를 확인하고(get_filetype), HTTP 응답 헤더를 생성하여 클라이언트에게 보냅니다.
 * - 파일을 메모리로 매핑(Mmap)하고, 그 내용을 클라이언트에게 전송합니다.
 * - 전송 후, 매핑된 메모리를 해제(Munmap)합니다.
 */
void serve_static(int fd, char *filename, int filesize)
{
    printf("\n####### serve_static #######\n");
    int srcfd;
    char *srcp, filetype[MAXLINE], buf[MAXBUF];

    get_filetype(filename, filetype);
    sprintf(buf, "HTTP/1.0 200 OK\r\n");
    sprintf(buf, "%sServer: Tiny Web Server\r\n", buf);
    sprintf(buf, "%sConnection: close\r\n", buf);
    sprintf(buf, "%sContent-length: %d\r\n", buf, filesize);
    sprintf(buf, "%sContent-type: %s\r\n\r\n", buf, filetype);
    Rio_writen(fd, buf, strlen(buf));
    printf("Response headers:\n");
    printf("%s", buf);

    srcfd = Open(filename, O_RDONLY, 0);
    // srcp = Mmap(0, filesize, PROT_READ, MAP_PRIVATE, srcfd, 0);
    srcp = malloc(filesize);
    Rio_readn(srcfd, srcp, filesize);
    Close(srcfd);
    Rio_writen(fd, srcp, filesize);
    free(srcp);
    printf("####### serve_static #######\n");
}

/**
 * 목적 : 파일의 확장자를 확인하고, 그에 맞는 MIME 타입을 설정합니다.
 * 구성
 * - 파일이 .html, .gif, .png, .jpg로 끝나는지 확인하여 각각 적절한 MIME 타입을 설정합니다.
 */
void get_filetype(char *filename, char *filetype)
{
    printf("\n####### get_filetype #######\n");
    if (strstr(filename, ".html"))
        strcpy(filetype, "text/html");
    else if (strstr(filename, ".gif"))
        strcpy(filetype, "image/gif");
    else if (strstr(filename, ".png"))
        strcpy(filetype, "image/png");
    else if (strstr(filename, ".jpg"))
        strcpy(filetype, "image/jpeg");
    else if (strstr(filename, ".mp4"))
        strcpy(filetype, "video/mp4");      // MP4 파일
    else
        strcpy(filetype, "text/plain");
    printf("####### get_filetype #######\n");
}

/**
 * 목적 : 동적 컨텐츠(CGI 스크립트)를 클라이언트에게 제공하는 함수입니다.
 * 구성
 * - HTTP 응답 헤더를 생성하여 클라이언트에게 보낸 후, CGI 프로그램을 실행합니다.
 * - 자식 프로세스를 생성하여(Fork), 클라이언트가 보낸 인자를 환경 변수로 설정하고(setenv), 해당 CGI 프로그램을 실행(Execve)합니다.
 * - 자식 프로세스가 종료될 때까지 부모 프로세스는 대기(Wait)합니다.
 */
void serve_dynamic(int fd, char *filename, char *cgiargs)
{
    printf("\n####### serve_dynamic #######\n");
    char buf[MAXLINE], *emptylist[] = { NULL };

    sprintf(buf, "HTTP/1.0 200 OK\r\n");
    Rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "Server: Tiny Web Server\r\n");
    Rio_writen(fd, buf, strlen(buf));

    if (Fork() == 0) {
        setenv("QUERY_STRING", cgiargs, 1);
        Dup2(fd, STDOUT_FILENO);
        Execve(filename, emptylist, environ);
    }
    Wait(NULL);
    printf("####### serve_dynamic #######\n");
}