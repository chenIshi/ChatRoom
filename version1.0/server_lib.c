#ifndef _env
#define _env
#include "chat_env.h"
#endif

#ifndef _common
#define _common
#include "common.h"
#endif

extern Group *group;
extern int ngroups;

int initgroups(char* groupsfile)
{
    FILE *fp;
    char name[MAXNAMELEN];
    int capa;
    int grid;

    fp = fopen(groupsfile, "r");
    if(!fp)
    {
        fprintf(stderr, "[ERROR] Unable to open file \"%s\" !\n", groupsfile);
        return(0);
    }

    // 聊天室的數量要放在文件的最上面一行
    fscanf(fp, "%d", &ngroups);
    group = (Group*) calloc(ngroups, sizeof(Group));
    if(!group)
    {
        fprintf(stderr, "[ERROR] Unable to calloc !\n");
        return(0);
    }

    for(grid=0; grid<ngroups; grid++)
    {
        // 理想的資料範本應該是 "[group的名字]+[該group所能容納的最大人數]"
        if(fscanf(fp, "%s %d", name, &capa) != 2)
        {
            fprintf(stderr, "[ERROR] Wrong Format om Group %d\n", grid+1);
            return(0);
        }

        // 初始化聊天室
        group[grid].name = strdup(name);
        group[grid].capa = capa;
        group[grid].occu = 0;
        group[grid].mems = NULL;
        
    }

    return(1);

}

// 由伺服器端接收的所有訊息再分給所有客戶端
int listgroups(int socket)
{
    int     grid;
    char    pktbufr[MAXPKTLEN];
    char*   bufrptr;
    long    bufrlen;

    bufrptr = pktbufr;
    for(grid=0; grid<ngroups; grid++)
    {
        // 取得聊天室的名字
        sprintf(bufrptr, "%s", group[grid].name);
        bufrptr += strlen(bufrptr) + 1;

        // 取得聊天室容量
        sprintf(bufrptr, "%d", group[grid].capa);
        bufrptr += strlen(bufrptr) + 1;

        // 取得聊天室佔有率
        sprintf(bufrptr, "%d", group[grid].occu);
        bufrptr += strlen(bufrptr) + 1;
    }
    bufrlen = bufrptr - pktbufr;

    // 拿到length之後開始傳packet給客戶端
    sendpkt(socket, LIST_GROUPS, bufrlen, pktbufr);
    return(1);
}

// 加入聊天室
int joingroup(int socket, char* gname, char* mname)
{
    int grid;
    Member* memb;

    // 根據聊天室名字取得對應的ID
    grid = find_group(gname);

    // 如果找不到對應的組別
    if(grid == -1)
    {
        char* errmsg = "no such group";
        // 發送拒絕的封包
        sendpkt(socket, JOIN_REJECTED, strlen(errmsg), errmsg);
        return(0);
    }

    // 檢查用戶想要使用的名字是不是已經被用了
    memb = find_member_by_name(mname);

    // 如果該名字已經被別人佔用了，則回傳錯誤訊息
    if(memb)
    {
        char* errmsg = "member name already exists";
        sendpkt(socket, JOIN_REJECTED, strlen(errmsg), errmsg);
        return(0);
    }

    // 檢查聊天室是不是已經滿了
    if(group[grid].capa == group[grid].occu)
    {
        char* errmsg = "room is full";
        sendpkt(socket, JOIN_REJECTED, strlen(errmsg), errmsg);
        return(0);
    }
    
    // 沒有問題之後就幫新成員分配記憶體空間
    memb = (Member*) calloc(1, sizeof(Member));
    if(!memb)
    {
        fprintf(stderr, "[ERROR] Unable to Calloc\n");
        cleanup();
    }

    // 把新加入的成員塞到Link List的頭
    memb->name = strdup(mname);
    memb->socket = socket;
    memb->grid = grid;
    memb->prev = NULL;
    memb->next = group[grid].mems;

    // 如果聊天室已經創建好，且已經有成員了，就更新group的成員列表(反正head的prev是沒有意義的)
    if(group[grid].mems)
    {
        group[grid].mems->prev = memb;
    }
    // 每次新增完新的成員，都把mems指到最新加入的成員上
    group[grid].mems = memb;
    prinf("[ADMIN] \'%s\' Has Joined \'%s\'\n", mname, gname);

    // 更新聊天室在線人數
    group[grid].occu ++;

    // 終於成功發送「接受成員」的訊息了
    sendpkt(socket, JOIN_ACCEPTED, 0, NULL);
    return(1);
}


// 離開聊天室
int leavegroup(int socket)
{
    Member *memb;

    memb = find_member_by_socket(socket);
    
    // 如果找不到對應的成員ID，就不做事裝死
    if(!memb) return(0);

    // 刪除該成員node前，先把鄰居的pointer處理一下
    if(memb->next) memb->next->prev = memb->prev;

    if(group[memb->grid].mems == memb)
    {
        group[memb->grid].mems = memb->next;
    }else
    {
        memb->prev->next = memb->next;
    }

    printf("[ADMIN] \'%s\' Left \'%s\'\n", memb->name, group[memb->grid].name);

    group[memb->grid].occu --;

    // 真正釋放該成員的記憶體空間
    free(memb->name);
    free(memb);
    return(1);
}

int relaymsg(int socket, char* text)
{
    Member* memb;
    Member* sender;
    char pktbufr[MAXNAMELEN];
    char* bufrptr;
    long bufrlen;

    // 根據socket找到對應的用戶資訊
    sender = find_member_by_socket(socket);

    // 找不到送訊息的人
    if(!sender)
    {
        fprintf(stderr, "strange: no corresponding member at %d\n", socket);
        return(0);
    }

    // 把發送者的姓名塞到封包前面
    bufrptr = pktbufr;
    strcpy(bufrptr, sender->name);
    bufrptr += strlen(bufrptr) + 1;
    strcpy(bufrptr, text);
    bufrptr += strlen(bufrptr) + 1;
    bufrlen = bufrptr - pktbufr;

    // 廣播該訊息給所有人
    for(memb = group[sender->grid].mems; memb; memb = memb->next)
    {
        // 發送者自己不用收到訊息（原文是這樣寫，不過我覺得至少要知會源發送者訊息已被送出）
        sendpkt(memb->socket, USER_TXT, bufrlen, pktbufr);
    }
    printf("%s: %s", sender->name, text);
    return(1);
}

// 如果有找到就回傳該index，沒有找到則回傳一個-1
int find_group(char* name)
{
    int grid;
    for(grid=0; grid<ngroups; grid++){
        if(strcmp(group[grid].name, name) == 0) return(grid);
    }
    return(-1);
}

Member* find_member_by_name(char* name)
{
    int grid;
    for(grid=0; grid<ngroups; grid++)
    {
        Member *memb;
        for(memb = group[grid].mems; memb; memb = memb->next)
        {
            if(strcmp(memb->name, name) == 0) return(memb);
        }
    }
    return(NULL);
}

Member* find_member_by_socket(int socket)
{
    int grid;
    for(grid=0; grid<ngroups; grid++)
    {
        Member* memb;
        for(memb=group[grid].mems; memb; memb = memb->next)
        {
            if(memb->socket == socket) return(memb);
        }
    }
    return(NULL);
}

void cleanup()
{
    char linkname[MAXNAMELEN];
    sprintf(linkname, "%s/%s", getenv("HOME"), PORTLINK);
    unlink(linkname);
    exit(0);
}