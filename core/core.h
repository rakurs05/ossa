#ifndef OSSA_H
#define OSSA_H
#include "./types.h"
#ifndef CORE_CPP
    #include "./dlist/list.h"

    int makeChat(ossastr title);
    int setChatSettings(struct Chat* _this, ossastr field, ossastr data);
    ossastr getChatSettings(struct Chat* _this);
    int inviteToChat(struct Chat* _this, ossastr globalUID);
    int deleteUser(struct Chat* _this, ossaUID uid, ossastr additional);
    int sendMessage(struct Chat *_this, ossaMessage message);
    ossaMessage makeMessage(struct Chat *_this, ossastr body, ossalist(ossastr) attachments);
    int editMessage(struct Chat *_this, ossaMID mid, ossaMessage edited);
    int chatAction(struct Chat *_this, ossastr action_name);
    int chechChatEvent(struct Chat *_this);
    int exportChat(struct Chat *_this, char _location_type, ossastr location);
    int importChat(struct Chat *_this, char _location_type, ossastr location);
    
#endif
#endif  