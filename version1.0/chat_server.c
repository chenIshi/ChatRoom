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

    // idiot proof
    if(argc!=2)
    {
        fprintf(stderr, "[ERROR] The Command Should include \" ./chat_server [CHATROOM_TXT_NAME] \"\n");
        exit(1);
    }

    
}
