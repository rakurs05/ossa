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
struct _Message{
    ossastr body;
    ossalist(ossastr) attach;
    ossaUID uid;
};
struct _User{
    ossastr nickname;
    ossastr head;
};
#define ossaMessage struct _Message
#define ossaUser struct _User
#endif

ossaUID getUidFromUser(ossaUser user);
ossastr getUsernameFromUser(ossaUser user);

ossastr getMessageBody(ossaMessage mes);
ossaUID getMessageSender(ossaMessage mes);
ossastr getMessageHead(ossaMessage mes);
ossaMessage makeMes(ossastr body, ossaUID sender, long long time);

struct Chat{
    ossastr title;
    ossalist(Message) messages;
    ossalist(User) userlist;
};

struct Plugin{
    //Shared data
    void *libEntity;
    struct __PLUGIN_CALLS__{
        //Zero-level
        int (*connect)();
        int (*disconnect)();
        int (*state)();
        ossastr (*info)();
        //First-level
        int (*auth)(ossastr,ossastr);
        int (*oauth)(ossastr);
        int (*exit)();
        int (*renameMe)(ossastr);
        ossaUser (*myInfo)();
        //Second-level
        int (*makeChat)(ossastr);
        ossastr (*getChatSettings)();
        int (*inviteToChat)(struct Chat*, ossastr);
        int (*deleteUser)(struct Chat*, ossaUID);
        int (*sendMes)(struct Chat*, ossaUID);
        int (*editMes)(struct Chat*, ossaMID);
        int (*chatAction)(struct Chat*, ossastr action); //сделаю как просто вызов void-функции, если это равно 0x0
        int (*checkChatEvent)(struct Chat*);
    }call;
    int (*init)();
    int (*destr)();
    //Metadata
    ossastr name;
    ossastr loaction;
    //Resolves
    ossalist(ossastr) resolved_net; //resolved network connections
    ossalist(ossastr) resolved_loc; //resolved local files access
};

#define lnothing (struct __list){0x0, 0x0}