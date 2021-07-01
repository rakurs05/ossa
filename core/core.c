#include "./core.h"
#include "dlist/list.h"
#include <cstdio>

/* internel */
char checkValidPlugin(struct ossaPlugin *plugin){
    return 1;
}
char find_settings(const void *target, void *current){

}
char find_user(const void *target, void *current){

}

/* export */
struct ossaChat makeChat(ossastr title, struct ossaPlugin *plugin){
    //Make chat object
    struct ossaChat chat;
    //Check for valid plugin and liinking to plugin
    if(checkValidPlugin(plugin)){
        chat.plugin = plugin;
    }else{
        return (struct ossaChat){0x0, lnothing, lnothing, 0x0};
    }
    //Copy title
    chat.title = (malloc(strlen(title)));
    strcpy(chat.title, title);
    //Making empty lists
    chat.messages = makeEmptyList();
    chat.userlist = makeEmptyList();
    chat.settings = makeEmptyList();
    return chat;
}

int setChatSettings(struct ossaChat* _this, ossastr field, ossastr data){
    // Not found settings field
    listRemove(&_this->settings, listFind(&_this->settings, field, find_settings));
    unsigned int size = strlen(field)+strlen(data);
    char *buffer = malloc(size);
    sprintf(buffer, "%s\r%s", field, data);
    listAppendLink(&_this->settings, buffer);
}

ossalist(ossastr) getChatSettings(struct ossaChat* _this){
    return _this->settings;
}

int inviteToChat(struct ossaChat* _this, ossastr globalUID){
    ossaUser newUser = _this->plugin->pcall.globalUIDInfo(globalUID);
    if(listFind(&_this->userlist, &newUser, find_user)){
        return OSSA_ALREADY;
    }
    int code = chatAction(_this, "invite", {globalUID, 0x0});
    if(code == OSSA_OK || code == OSSA_ACCPEPT){

    }else if(code == OSSA_WAIT){

    }else if(code == OSSA_DECLINE){

    }
}