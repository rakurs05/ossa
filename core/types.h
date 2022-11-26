#pragma once

#define ossaUID unsigned long
#define ossaMID unsigned long
#define ossaCID void *
#define ossastr char *

#ifdef OSSA_CTU
#define Message void *
#define User void *
#define Role void *

#ifndef pthread_t
    typedef unsigned long int pthread_t;
#endif

#else
#include "./mhs.h"
#define ossaMessage struct _Message
#define ossaUser struct _User
#endif

#ifndef ossalist
    #ifndef OSSA_STDLIST
        #define ossalist(type) struct __list
    #else
        #include <list>
        #define ossalist(type) std::list <type>
    #endif
#endif

struct ossaChat;

struct __PLUGIN_CALLS__{
    //Zero-level
    int (*connect)();
    int (*disconnect)();
    int (*state)();
    //First-level
    int (*auth)(ossastr,ossastr);
    int (*oauth)(ossastr);
    int (*exit)();
    int (*renameMe)(ossastr);
    ossaUser (*myInfo)();
    ossaUser (*globalUIDInfo)(ossastr);
    //Second-level
    // int (*inviteToChat)(struct Chat*, ossastr); // перемещено в chatAction
    // int (*deleteMes)(ossaCID, ossaMID); // перемещено в chatAction
    int (*sendMes)(ossaCID, ossaMessage);
    int (*editMes)(ossaCID, ossaMessage, ossaMID);
    //Third-level
    ossaCID (*makeChat)(ossastr);
    ossastr (*getChatSettings)(ossaCID);
    int (*setChatSettings)(ossaCID, ossastr, ossastr);
    int (*updateChat)(ossaCID);
    int (*chatAction)(ossaCID, ossastr); //(target chat, action) сделаю как просто вызов void-функции, если это равно 0x0
    int (*loadChat)(ossaCID, ossastr); //(target chat, src) if src in 0x0, read from Chat
    ossastr (*getChatList)();
    ossastr (*getChatGUIDs)(ossaCID);
};

struct ossaChat{
    ossastr title;
    ossalist(ossaMessage) messages;
    ossalist(ossaUser) userlist;
    ossalist(ossastr) settings;
    ossaCID cid;
    struct ossaPlugin *plugin;
};

struct SymbolList{
    char *type;
    void *data;
    struct SymbolList* next;
};

struct ossaPlugin{
    //Shared data
    void *libEntity;
    struct SymbolList sl;
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
    //Staff
    ossalist(pthread_t) threads;
};

#define lnothing (struct __list){0x0, 0x0}