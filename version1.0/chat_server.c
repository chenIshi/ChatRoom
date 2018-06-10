/*  
    2018.6.5 NCKU EE OS Project2 
    Chatroom implemented by C language
    reference to github prohject https://github.com/miluer/ChatRoom
*/

// local的一些環境定義

#ifndef _common
#define _common
#include "common.h"
#endif

#include "chat_server.h"
#include "chat_linker.c"

#ifndef _lib
#define _lib
#include "server_lib.c"
#endif

Group *group;
int ngroups;

int main(int argc, char* argv[])
{
    // 伺服器實施監聽的socket descriptor
    int     server_socket;
    // 客戶端可連接伺服器端的最大數量
    int     maxsd;
    fd_set  livesdset, tempset;

    // 防呆
    if(argc!=2)
    {
        fprintf(stderr, "[ERROR] The Command Should include \" ./chat_server [CHATROOM_TXT_NAME] \"\n");
        exit(1);
    }

    // 先初始化聊天室，如果初始化不成功則會回傳0，反則回傳1
    if(!initgroups(argv[1])) exit(1);

    // 設定訊號處理函數
    signal(SIGTERM, cleanup);
    signal(SIGINT, cleanup);

    // startserver定義在"chatlinker.c"中
    // 主要負責完成串建服務器服務，綁定socket，並設定為監聽狀態
    server_socket = startserver();

    // 如果回傳值為-1代表伺服器啟動失敗
    if(server_socket == -1) exit(1);

    // 初始化  maxsd
    maxsd = server_socket;

    // 初始化descriptor
    // 初始化livesdset, tempset所有的bit
    FD_ZERO(&livesdset);
    FD_ZERO(&tempset);
    // 啟動負責監聽client socket的server socket，descriptor與其對應的sd set
    FD_SET(server_socket, &livesdset);

    // 用一個無窮迴圈來接受client request
    while(1)
    {
        int socket;
    }

    
}
