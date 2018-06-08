#ifndef _env
#define _env
#include "chat_env.h"
#endif

// 透過聊天室名字找對應的ID
int find_group(char* name);

// 透過成員名字找到對應成員的物件
Member* find_member_by_name(char* name);

// 透過socket找到對應的成員物件
Member* find_member_by_socket(int socket);

// 成員退出前的清理資訊
void cleanup();

// 初始化聊天室
int initgroups(char* groupfile);

// 把聊天室的訊息傳送給客戶端
int listgroups(int socket);

// 加入聊天室
int joingroup(int socket, char* gname, char* mname);

