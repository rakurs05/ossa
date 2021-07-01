#pragma once
#ifndef OSSA_H
#define OSSA_H
#include "./types.h"
#ifndef OSSA_CPP
    #include "./dlist/list.h"
#endif
    struct ossaChat makeChat(ossastr title, struct ossaPlugin *plugin);
    int setChatSettings(struct ossaChat* _this, ossastr field, ossastr data);
    ossalist(ossastr) getChatSettings(struct ossaChat* _this);
    int inviteToChat(struct ossaChat* _this, ossastr globalUID);
    int deleteUser(struct ossaChat* _this, ossaUID uid, ossastr additional);
    int sendMessage(struct ossaChat *_this, ossaMessage message);
    ossaMessage makeMessage(struct ossaChat *_this, ossastr body, ossalist(ossastr) attachments);
    int editMessage(struct ossaChat *_this, ossaMID mid, ossaMessage edited);
    int chatAction(struct ossaChat *_this, ossastr action_name, ossalist(ossastr) args);
    int updateChat(struct ossaChat *_this);
    int exportChat(struct ossaChat *_this, ossastr location);
    int importChat(struct ossaChat *_this, ossastr location);

    int loadChatPlugin(struct ossaPlugin *_this, ossastr path);
#endif  