#include "chat_linker.h"

int startserver()
{
    int sd; // sd代表 "socket descriptor"
    int myport; // 伺服器port
    const char* myname; // 本地主機的名字

    char linktrgt[MAXNAMELEN];
    char linkname[MAXNAMELEN];

    // 使用socket函數創建tcp socket descriptor
    sd = socket(PF_INET, SOCK_STREAM, 0);

    // 使用bind函式將一個本地位址addr給socket
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    // 連接地址 INADDR_ANY 代表IP位址為0.0.0.0
    // 使用htonl這個函數就需要我們久遠的計算機概論的相關知識了
    // 與htonl具有類似功能的還包含 htons, ntohl, ntohs
    // 這幾個function都是在不同"big-endian" 與 "small-endian"之間做轉換
    // 最後kernel在socket被連接上之後選擇一個本地地址
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    // 指派伺服器端口為0
    server_address.sin_port = htons(0);

    // 使用bind函式，kernel將任意選擇一個可以使用的臨時port
    bind(sd, (struct sockaddr *) &server_address, sizeof(server_address));

    // 使用listen將server端socket descriptor 設定成被動監聽狀態，並可以接受等待佇列的長度為20
    // 等待佇列是指同時（其實不一定是同時，只要是server在處理封包的期間）不同client向server傳送訊息時，該信息停留在queue裡面等待被server接受
    listen(sd, 20);

    // 使用getsocketname, gethostname, 與gethostbyname 確定本地主機名字與server port

    char hostname[MAXNAMELEN];

    if(gethostname(hostname, sizeof hostname) != 0) perror("gethostname");

    struct hostent* h;
    // gethostbyname吃的是主機名或是一個域名
    h = gethostbyname(hostname);

    int len = sizeof(struct sockaddr);

    getsockname(sd, (struct sockaddr *) &server_address, &len);

    // 用gethostbyname回傳回來的hostent物件
    // 其中h_name代表該物件的"offical name"
    myname = h->h_name;
    myport = ntohs(server_address.sin_port);

    sprintf(linktrgt, "%s:%d", myname, myport);
    // 在"common.h"裡面有定義PORTLINK指向.chatport
    sprintf(linkname, "%s/%s", getenv("HOME"), PORTLINK);

    // 若嘗試連接到已存在的文件則不會連接並回傳-1
    if(symlink(linktrgt, linkname)!=0)
    {
        fprintf(stderr, "error : server already exist");
        return(-1);
    }
    
    // 準備開始接受client request
    printf("[ADMIN] Started Server on \'%s\' at \'%d\'\n", myname, myport);
    return(sd);
}

// 連接到server，成功連接則回傳socket descriptor，失敗則回傳-1
int hooktoserver()
{
    int sd;

    char linkname[MAXNAMELEN];
    char linktrgt[MAXNAMELEN];
    char* servhost;
    char* servport;
    int bytecnt;

    // 取得伺服器的地址
    sprintf(linkname, "%s/%s", getenv("HOME"), PORTLINK);
    bytecnt = readlink(linkname, linktrgt, MAXNAMELEN);
    if(bytecnt == -1)
    {
        fprintf(stderr, "error : no active chat server\n");
        return(-1);
    }

    linktrgt[bytecnt] = '\0';

    // 獲得伺服器IP位址與port
    servport = index(linktrgt, ":");
    *servport = '\0';
    servport ++;
    servhost = linktrgt;

    // 以unsigned short形式獲得server ip address
    unsigned short number = (unsigned short) strtoul(servport, NULL, 0);
}