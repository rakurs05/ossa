#pragma once

#define OSSA_OK         0x0
#define OSSA_WAIT       0x1
#define OSSA_ACCPEPT    0x2
#define OSSA_DECLINE    0x3
#define OSSA_ALREADY    0x4

#define ossaUID unsigned long
#define ossaMID unsigned long
#define ossastr char *

#define CLIST_CONST_BUFFER_SIZE           1024

#ifndef __cplusplus
#define astype(type) *(type*)
#define new(type) (type*)malloc(sizeof(type))
#define news(type,count) (type*)malloc(sizeof(type)*count)
#endif

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

#ifndef ossalist
    #ifndef OSSA_STDLIST
        #define ossalist(type) struct __list
    #else
        #include <list>
        #define ossalist(type) std::list <type>
    #endif
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
    ossaUser (*globalUIDInfo)(ossastr)
    //Second-level
    int (*inviteToChat)(struct Chat*, ossastr); // перемещено в chatAction
    int (*deleteUser)(struct Chat*, ossaUID);
    int (*sendMes)(struct Chat*, ossaUID);
    int (*editMes)(struct Chat*, ossaMID);
    //Third-level
    int (*makeChat)(ossastr);
    ossastr (*getChatSettings)();
    int (*setChatSettings)(struct Chat*, ossastr, ossastr);
    int (*checkChatEvent)(struct Chat*);
    int (*chatAction)(struct Chat*, ossastr); //(target chat, action) сделаю как просто вызов void-функции, если это равно 0x0
    int loadChat(struct Chat*, ossastr); //(target chat, src) if src in 0x0, read from Chat
    int saveChat(struct Chat*, ossastr); //Same as upper
};

ossaUID getUidFromUser(ossaUser user);
ossastr getUsernameFromUser(ossaUser user);

ossastr getMessageBody(ossaMessage mes);
ossaUID getMessageSender(ossaMessage mes);
ossastr getMessageHead(ossaMessage mes);
ossaMessage makeMes(ossastr body, ossaUID sender, long long time);

struct ossaChat{
    ossastr title;
    ossalist(ossaMessage) messages;
    ossalist(ossaUser) userlist;
    ossalist(ossastr) settings;
    struct ossaPlugin *plugin;
};

struct ossaPlugin{
    //Shared data
    void *libEntity;
    struct __PLUGIN_CALLS__ pcall;
    //Structure calls
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