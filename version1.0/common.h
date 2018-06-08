// 定義server port
#define PORTLINK ".chatport"

// 定義封包空間限制
#define MAXNAMELEN 256
#define MAXPKTLEN 2048

// 定義封包類型
#define LIST_GROUPS     0
#define JOIN_GROUP      1
#define LEAVE_GROUP     2
#define USER_TXT        3
#define JOIN_REJECTED   4
#define JOIN_ACCEPTED   5

// 封包架構
typedef struct _packet
{
    // 封包類型
    char    type;

    // 封包內容長度
    long    length;

    // 封包內容
    char*   txt;
} Packet;

extern int startserver();
extern int locateserver();

extern Packet *recvpkt(int sd);
extern int sendpkt(int sd, char typm, long len, char* buf);
extern void freepkt(Packet* msg);