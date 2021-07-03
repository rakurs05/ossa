#include "./core.h"
#include "dlist/list.h"
#include <stdio.h>
#include <string.h>
#include <zip.h>
#include <dlfcn.h>
#ifndef  __WIN32
    // #include <unistd.h>
    #include <sys/stat.h>
#endif

#define OSSA_CORE_VERSION "0.3-AS(D)"

#ifndef OSSA_CORE_MAXHEADER
    #define OSSA_CORE_MAXHEADER 1024
#endif

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
        return (struct ossaChat){0x0, lnothing, lnothing, lnothing, 0x0};
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
    if(newUser.nickname == 0x0){
        //No user
        return OSSA_NOUSER;
    }
    if(listFind(&_this->userlist, &newUser, find_user)){
        return OSSA_ALREADY;
    }
    int code = chatAction(_this, "invite", (ossalist(ossastr)){globalUID, 0x0});
    if(code == OSSA_OK || code == OSSA_ACCPEPT){
        listAppend(&_this->userlist, &newUser, sizeof(ossaUser));
    }
    return code;
}

int deleteUser(struct ossaChat* _this, ossaUID uid, ossastr additional){
    ossalist(ossastr) argv = makeEmptyList();
    int charscount = 1024; // ((ceil(log10(uid))+1)*sizeof(char)); /* ибо пошли нахуй все эти "умные" формулы, сюка" */
    char *j = malloc(charscount+1);
    sprintf(j, "%lu", uid);
    listAppend(&argv, j, charscount+1);
    listAppend(&argv, additional, strlen(additional));
    int code = chatAction(_this, "userdel", argv);
    if(code == OSSA_OK){
        listRemove(&_this->userlist, uid);
    }
    earaseList(&argv);
    return code;
}

int sendMessage(struct ossaChat *_this, ossaMessage message){
    listAppend(&_this->messages, &message, sizeof(ossaMessage));
    return updateChat(_this);
}

ossaMessage makeMessage(struct ossaChat *_this, ossastr body, ossalist(ossastr) attachments){
    ossaMessage me;
    me.uid = astype(ossaUID) listGet(&_this->userlist, 0); //0th user is always 'me'
    me.body = body;
    me.attach = attachments;
    return me;
}

int editMessage(struct ossaChat *_this, ossaMID mid, ossaMessage edited){
    astype(ossaMessage) listGet(&_this->userlist, mid) = edited;
    return updateChat(_this);
}

int chatAction(struct ossaChat *_this, ossastr action_name, ossalist(ossastr) args){
    char argv[5120];
    memset(argv, 0, 5120);
        strcat(argv, "\2");
    strcpy(argv, action_name);
    for(int i = 0; i < listLen(&args); i++){
        strcat(argv, (char*)listGet(&args, i));
        strcat(argv, "\28");
    }
        strcat(argv, "\3");
    return _this->plugin->pcall.chatAction(0, argv);
}

int updateChat(struct ossaChat *_this){
    return _this->plugin->pcall.updateChat(0);
}

int exportChat(struct ossaChat *_this, ossastr location){
    struct stat stats;
    ossastr final = location;

    if(stat(location, &stats) == 0){
        if(S_ISDIR(stats.st_mode)){
            //if it directory, we should place
            final = malloc(strlen(location)+strlen("/drop.ossadrop"));
            sprintf(final, "%s/drop.ossadrop", location);
        }
    }else{
        // fprintf(stderr, "[!!] OSSA Core: Fatal error: failed to stat \'%s\'!\n");
    }
    int err = 0;
    zip_t *drop = zip_open(location, ZIP_CREATE, &err);
    if(err != 0){
        fprintf(stderr, "[!!] OSSA Core: Fatal error: failed to open \'%s\' for writing, %s", location, zip_strerror(drop));
        return err;
    }
    { //Writing header
        char headerdata[OSSA_CORE_MAXHEADER];
        sprintf(headerdata, "{\"desc\":\"OSSA Chat Archive Header File\",");
        sprintf(headerdata, "\"OSSA Core version\":\"%s\",", OSSA_CORE_VERSION);
        sprintf(headerdata, "\"messages_count\":%i,", listLen(&_this->messages));
        sprintf(headerdata, "\"users_count\":%i,", listLen(&_this->userlist));
        sprintf(headerdata, "\"plugin\":{");
        sprintf(headerdata, "\"name\":\"%s\",", _this->plugin->name);
        sprintf(headerdata, "\"location\":\"%s\",", _this->plugin->loaction);
        sprintf(headerdata, "},");
        sprintf(headerdata, "\"settings\":[");
        for(int i = 0; i < listLen(&_this->settings); i++){
            sprintf(headerdata, "\"%s\",", (char*)listGet(&_this->settings, i));
        }
        sprintf(headerdata, "]}");
        zip_source_t *s;
        if((s = zip_source_buffer(drop, headerdata, sizeof(headerdata), 0)) == 0x0||
        zip_file_add(drop, "header.json", s, ZIP_FL_ENC_UTF_8) < 0){
            zip_source_free(s);
            fprintf(stderr, "[!!] OSSA Core: Fatal error: failed to write header: %s",\
                zip_strerror(drop));
        }
    }
    zip_close(drop);
}

int loadChatPlugin(struct ossaPlugin *_this, ossastr path){
    void *entity = _this->libEntity = dlopen(path, RTLD_LAZY);
    if(_this->libEntity == 0x0){
        fprintf(stderr, "[!!] OSSA Core: Fatal error: failed to open \'%s\' plugin: %s", path, dlerror());
        return -1;
    }
    _this->loaction = malloc(strlen(path));
    strcpy(_this->loaction, path);
    _this->init = dlsym(entity, "init");
    _this->pcall.connect = (int(*)())dlsym(entity, "plugin_connect");
    _this->pcall.disconnect = (int(*)())dlsym(entity, "plugin_disconnect");
    _this->pcall.state = (int(*)())dlsym(entity, "plugin_state");

    _this->pcall.auth = (int(*)(ossastr, ossastr))dlsym(entity, "plugin_user_auth");
    _this->pcall.oauth = (int(*)(ossastr))dlsym(entity, "plugin_user_oauth");
    _this->pcall.exit = (int(*)())dlsym(entity, "plugin_user_exit");
    _this->pcall.renameMe = (int(*)(ossastr))dlsym(entity, "plugin_user_rename");
    _this->pcall.myInfo = (ossaUser(*)())dlsym(entity, "plugin_user_info");
    _this->pcall.globalUIDInfo = (ossaUser(*)(ossastr))dlsym(entity, "plugin_user_ginfo");

    _this->pcall.sendMes = (int(*)(ossaCID, ossaMessage))dlsym(entity, "plugin_message_send");
    _this->pcall.editMes = (int(*)(ossaCID, ossaMessage, ossaMID))dlsym(entity, "plugin_message_edit");

    _this->pcall.makeChat = (int(*)(ossastr, ossaCID))dlsym(entity, "plugin_chat_makeChat");
    _this->pcall.getChatSettings = (ossastr(*)(ossaCID))dlsym(entity, "plugin_chat_getprefs");
    _this->pcall.setChatSettings = (int(*)(ossaCID, ossastr, ossastr))dlsym(entity, "plugin_chat_setpref");
    _this->pcall.updateChat = (int(*)(ossaCID))dlsym(entity,"plugin_chat_update");
    _this->pcall.loadChat = (int(*)(ossaCID, ossastr))dlsym(entity,"plugin_chat_load");
    _this->pcall.getChatList = (ossastr(*)())dlsym(entity,"plugin_chat_list");
    _this->pcall.getChatGUIDs = (ossastr(*)(ossaCID))dlsym(entity, "plugin_chat_getGUIDs");

}

int main(){

}