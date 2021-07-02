#pragma once

#define OSSA_OK             0x0
#define OSSA_WAIT           0x1
#define OSSA_ACCPEPT        0x2
#define OSSA_DECLINE        0x3
#define OSSA_ALREADY        0x4
#define OSSA_NOUSER         0x5

#define OSSA_STATE_ENABLE   1<<0
#define OSSA_STATE_ACTIVE   1<<1
#define OSSA_STATE_HIDDEN   1<<2
#define OSSA_STATE_RESOVL   1<<3
#define OSSA_STATE_AUTHED   1<<4

#define OSSA_UPDATE_UNRESOLVED_SENT 1<<0
#define OSSA_UPDATE_KICK_OR_BAN     1<<1
#define OSSA_UPDATE_NEW_MESSAGE     1<<2
#define OSSA_UPDATE_UNRESOLVED_EDIT 1<<3
#define OSSA_UPDATE_EDITED_MESSAGE  1<<4

#define ossaUID unsigned long
#define ossaMID unsigned long
#define ossaCID unsigned int
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

struct ossaChat;

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
    ossaUser (*globalUIDInfo)(ossastr);
    //Second-level
    // int (*inviteToChat)(struct Chat*, ossastr); // перемещено в chatAction
    // int (*deleteUser)(struct Chat*, ossaUID, ossastr); // перемещено в chatAction
    int (*sendMes)(ossaCID, ossaMessage);
    int (*editMes)(ossaCID, ossaMessage, ossaMID);
    //Third-level
    int (*makeChat)(ossastr);
    ossastr (*getChatSettings)();
    int (*setChatSettings)(struct ossaChat*, ossastr, ossastr);
    int (*updateChat)(struct ossaChat*);
    int (*chatAction)(struct ossaChat*, ossastr); //(target chat, action) сделаю как просто вызов void-функции, если это равно 0x0
    int (*loadChat)(struct ossaChat*, ossastr); //(target chat, src) if src in 0x0, read from Chat
    int (*saveChat)(struct ossaChat*, ossastr); //Same as upper
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
    // //Constants as NULL
    // ossaUser noUser;
    // ossaMessage noMessage; //deleted message, for example
};

#define lnothing (struct __list){0x0, 0x0}