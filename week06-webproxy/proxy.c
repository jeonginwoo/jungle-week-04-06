#include "csapp.h"
#include "cache.h"

void *thread(void *vargp);
void trans(int client_proxy_fd);
void parse_uri(char *uri, char *path, char *hostname, char *port);

/* You won't lose style points for including this long line in your code */
static const char *user_agent_hdr =
    "User-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:10.0.3) Gecko/20120305 "
    "Firefox/10.0.3\r\n";

Dll *dll;

int main(int argc, char* argv[])
{
    if (argc != 2) {
        fprintf(stderr, "usage: %s <port>\n", argv[0]);
        exit(1);
    }
    
    int listenfd, *connfdp;
    struct sockaddr_storage clientaddr;     // 클라이언트의 주소 정보 저장 (ip주소, port번호)
    socklen_t clientlen;
    pthread_t tid;
    dll = newDll();

    listenfd = Open_listenfd(argv[1]);
    while(1) {
        printf("\n========================\n");
        clientlen = sizeof(struct sockaddr_storage);
        connfdp = Malloc(sizeof(int));
        *connfdp = Accept(listenfd, (SA *) &clientaddr, &clientlen);
        Pthread_create(&tid, NULL, thread, connfdp);
        printf("========================\n\n");
    }
    // while(1) {
    //     connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen);
    //     printf("========================\n");
    //     trans(connfd);
    //     printf("========================\n\n");
    //     Close(connfd);
    // }

    return 0;
}

void *thread(void *vargp)
{
    int connfd = *((int *)vargp);
    Pthread_detach(pthread_self());
    Free(vargp);
    trans(connfd);
    Close(connfd);
    return NULL;
}

void trans(int client_proxy_fd)
{
    int proxyfd;
    int read_len;
    char buf[MAXBUF];
    char first_line[MAXLINE], method[MAXLINE], uri[MAXLINE], version[MAXLINE];
    char path[MAXLINE], hostname[100], port[10];
    rio_t rio_client, rio_server;
    node *dll_node;
    cacheKey cache_key;

    printf("\n=== dll ===\n");
    printDll(dll);
    printf("=== dll ===\n\n");

    // Client -> Proxy 헤더 첫번째 요청 라인                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        정보 받기
    Rio_readinitb(&rio_client, client_proxy_fd);
    Rio_readlineb(&rio_client, buf, MAXBUF);
    strcpy(first_line, buf);
    printf("client header : %s\n", first_line);

    printf("\n=== dll ===\n");
    printDll(dll);
    printf("=== dll ===\n\n");

    // Client -> Proxy 첫번째 요청 라인 분석
    printf("\n=== first line ===\n");
    sscanf(first_line, "%s %s %s", method, uri, version);
    printf("method: %s\n", method);
    printf("uri: %s\n", uri);
    printf("version: %s\n", version);
    printf("=== first line ===\n\n");

    printf("\n=== dll ===\n");
    printDll(dll);
    printf("=== dll ===\n\n");

    // Proxy 소켓 생성
    parse_uri(uri, path, hostname, port);

    printf("\n=== dll ===\n");
    printDll(dll);
    printf("=== dll ===\n\n");
    strcpy(cache_key.method, method);
    cache_key.path = path;
    
    printf("\n=== key compare ===\n");
    printf("method: %s\n", cache_key.method);
    printf("path: %s\n", cache_key.path);
    printf("=== key compare ===\n\n");

    char proxy_to_client_header[MAXLINE];
    if ((dll_node = search(dll, cache_key)) != NULL) {
        sprintf(proxy_to_client_header, "HTTP/1.0 200 OK\r\n");
        sprintf(proxy_to_client_header, "%sServer: Proxy Server\r\n", proxy_to_client_header);
        sprintf(proxy_to_client_header, "%sConnection: close\r\n", proxy_to_client_header);
        snprintf(proxy_to_client_header + strlen(proxy_to_client_header), sizeof(proxy_to_client_header) - strlen(proxy_to_client_header),
         "Content-length: %d\r\n\r\n", (int)strlen(dll_node->data));
        // sprintf(proxy_to_client_header, "%sContent-type: %s\r\n\r\n", proxy_to_client_header, );

        moveFront(dll, dll_node);
        strcpy(buf, dll_node->data);
        Rio_writen(client_proxy_fd, proxy_to_client_header, strlen(proxy_to_client_header));
        Rio_writen(client_proxy_fd, buf, strlen(dll_node->data));
        return;
    }

    proxyfd = Open_clientfd(hostname, port);

    // Proxy -> Server 요청 라인 전송
    sprintf(buf, "%s %s %s\r\n", method, path, version);
    Rio_writen(proxyfd, buf, strlen(buf));

    // Client -> Proxy 나머지 헤더 요청 라인 읽고 Proxy -> Server 요청 라인 전송
    printf("\n-------   client header   -------\n");
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
    char res_header[MAXLINE] = "";
    char *data;
    char *file_size_str;
    long file_size;

    // read header
    printf("\n=== read header ===\n");
    while((read_len = Rio_readlineb(&rio_server, buf, MAXBUF)) > 0) {
        sprintf(res_header, "%s%s", res_header, buf);
        if (strstr(buf, "Content-length")) {
            sscanf(buf, "Content-length: %ld", &file_size);
        }
        if (strcmp(buf, "\r\n") == 0) {
            break;
        }
        printf("%s", buf);
    }
    printf("\n=== read header ===\n\n");

    // read body
    data = (char *)malloc(file_size);
    strcpy(data, "");
    printf("\n=== read body ===\n");
    while((read_len = Rio_readnb(&rio_server, buf, file_size)) > 0) {
        strncat(data, buf, read_len);
        printf("%s", buf);
    }
    printf("\n=== read body ===\n\n");
    printf("=== data ===\n");
    printf("%s", data);
    printf("\n=== data ===\n\n");
    
    if (file_size <= MAX_OBJECT_SIZE){
        // 캐시에 넣기
        dll_node = (node *)malloc(sizeof(node));
        dll_node->data = data;
        dll_node->data_len = strlen(data);
        strcpy(dll_node->cache_key.method, cache_key.method);

        dll_node->cache_key.path = (char *)malloc(sizeof(path));
        strcpy(dll_node->cache_key.path, path);
        
        pushFront(dll, dll_node);
    } else {
        // 캐시 패스
        free(data);
    }
    Rio_writen(client_proxy_fd, res_header, strlen(res_header));
    Rio_writen(client_proxy_fd, data, file_size);
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