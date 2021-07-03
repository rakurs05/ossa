#pragma once
#ifndef OSSA_H
#define OSSA_H
#include "./ossaconsts.h"
#include "./types.h"
#ifndef OSSA_CPP
    #include "./dlist/list.h"
#endif
    extern struct ossaChat makeChat(ossastr title, struct ossaPlugin *plugin);
    extern int setChatSettings(struct ossaChat* _this, ossastr field, ossastr data);
    extern ossalist(ossastr) getChatSettings(struct ossaChat* _this);
    extern int inviteToChat(struct ossaChat* _this, ossastr globalUID);
    extern int deleteUser(struct ossaChat* _this, ossaUID uid, ossastr additional);
    extern int sendMessage(struct ossaChat *_this, ossaMessage message);
    extern ossaMessage makeMessage(struct ossaChat *_this, ossastr body, ossalist(ossastr) attachments);
    extern int editMessage(struct ossaChat *_this, ossaMID mid, ossaMessage edited);
    extern int chatAction(struct ossaChat *_this, ossastr action_name, ossalist(ossastr) args);
    extern int updateChat(struct ossaChat *_this);
    extern int exportChat(struct ossaChat *_this, ossastr location);
    extern int importChat(struct ossaChat *_this, ossastr location);
    extern int loadChatPlugin(struct ossaPlugin *_this, ossastr path);
    extern int unloadChatPlugin(struct ossaPlugin *_this, ossastr path);
    extern int authPlugin(struct ossaPlugin *_this, ossastr username, ossastr passwd);
    extern int oauthPlugin(struct ossaPlugin *_this, ossastr token);

#endif  