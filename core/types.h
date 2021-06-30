#pragma once

#define ossaUID unsigned long
#define ossaMID unsigned long
#define ossastr const char *

#define CLIST_CONST_BUFFER_SIZE           1024

#define astype(type) *(type*)
#define new(type) (type*)malloc(sizeof(type))
#define news(type,count) (type*)malloc(sizeof(type)*count)

#define CLIST_ERROR_ANY_ADDR              0x0
#define CLIST_ERROR_NULL_ARG              0x0
#define CLIST_ERROR_INDEX_OUT_OF_BOUND    0x0

struct __list{
    void *data;
    struct __list *next;
};

struct __list_booster{
    struct __list **points, *end;
    int pointsCount, len, info;
    void (*compare)(void *a, void *b);
    void (*destruct)(void *data);
};

struct __booster_info{
    void *target;
    int index, error;
};

#ifndef OSSA_STDLIST
    #define ossalist(type) struct __list
#else
    #define ossalist(type) std::list <type>
#endif

#ifdef OSSA_CTU
#define Message void *
#define User void *
#define Role void *

#else
struct Message{
    ossastr body;
    ossalist(ossastr) attach;
    ossaUID uid;
};
struct User{
    ossastr nickname;
    ossastr head;
};
#endif

ossaUID getUidFromUser(User user);
ossastr getUsernameFromUser(User user);

ossastr getMessageBody(Message mes);
ossaUID getMessageSender(Message mes);
ossastr getMessageHead(Message mes);
Message makeMes(ossastr body, ossaUID sender, long long time);

struct Chat{
    ossastr title;
    ossalist(Message) messages;
    ossalist(User) userlist;
};

#define lnothing (struct __list){0x0, 0x0}