// 定義通用struct

// 定義聊天室成員
typedef struct _member
{
    // 成員名稱
    char* name;

    // socket descriptor
    int socket;

    // 成員所屬的聊天室
    int grid;

    // _member本身為double link-list架構，所以會有next 與prev 兩個node
    struct _member* next;
    struct _member* prev;
} Member;

// 定義聊天室環境
typedef struct _group
{
    // 聊天室名稱
    char* name;

    // 聊天室能夠容納的最大人數
    int capa;

    // 當前聊天室人數
    int occu;

    // 紀錄所有該聊天群組內成員的listed-list
    struct _member* mems;
} Group;


// 幾個常見的c lib
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

// 處理同步檔案io的部份，同時也是系統鎖的實現
#include <fcntl.h>

// linux底層基本的資料型態，像是當前行程碼pid_t
#include <sys/types.h>

// linux下socket的實現，port與address的關聯性
#include <sys/socket.h>
#include <netinet/in.h>

// 網路行為的定義，arpanet是網路的鼻祖
#include <arpa/inet.h>
#include <netdb.h>

// error發生時方便除錯的訊息
#include <errno.h>

// C語言不同行程間，訊號間的傳遞
#include <signal.h>

// 建立符號連接要用到(symlink)
#include <unistd.h>