#include "csapp.h" // CS:APP 라이브러리 포함

int main(int argc, char **argv) {
    int clientfd;              // 클라이언트 소켓 파일 디스크립터
    char *host, *port, buf[MAXLINE]; // 서버 호스트와 포트 번호, 입력 버퍼
    rio_t rio;                 // Rio 패키지의 버퍼링된 I/O 구조체

    // 사용법이 맞지 않으면 에러 메시지 출력 후 종료
    if (argc != 3) {
        fprintf(stderr, "usage: %s <host> <port>\n", argv[0]);
        exit(0);
    }
    host = argv[1]; // 첫 번째 인자는 서버의 호스트 이름
    port = argv[2]; // 두 번째 인자는 포트 번호

    // 클라이언트 소켓을 열고 서버에 연결
    clientfd = Open_clientfd(host, port);

    // Rio 버퍼 초기화: 버퍼링된 읽기를 위해 파일 디스크립터를 설정
    Rio_readinitb(&rio, clientfd);

    // 표준 입력(stdin)으로부터 데이터를 읽어 서버로 전송하는 루프
    while (Fgets(buf, MAXLINE, stdin) != NULL) {
        // 입력 받은 데이터를 서버로 전송
        Rio_writen(clientfd, buf, strlen(buf));

        // 서버로부터 한 줄을 읽어들여 buf에 저장
        Rio_readlineb(&rio, buf, MAXLINE);

        // 서버의 응답을 표준 출력(stdout)으로 출력
        Fputs(buf, stdout);
    }

    // 클라이언트 소켓 종료
    Close(clientfd);
    exit(0); // 프로그램 정상 종료
}
