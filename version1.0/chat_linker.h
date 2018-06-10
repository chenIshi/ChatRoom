#ifndef _common
#define _common
#include "common.h"
#endif

#ifndef _env
#define _env
#include "chat_env.h"
#endif

// 為開始監聽client request做準備
// 成功的話回傳 socket descriptor, 錯誤則回傳-1
int startserver();

// 與服務器建立連線
// 成功的話回傳socket descriptor，失敗也是一樣回傳-1
int hooktoserver();

// 從kernel讀取一個socket的訊息
int readn(int sd, char* buf, int n);

// 接收packet
Packet* recvpkt(int sd);

// 送出packet
int sendpkt(int sd, char typ, long len, char* buf);

// 釋放掉packet佔用的記憶體空間
void freepkt(Packet *pkt);
