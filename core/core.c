//#define DISABLE_ZIP
#include "./core.h"
#include "./parson/parson.h"
#include "dlist/list.h"
#include <stdio.h>
#include <string.h>
#ifndef DISABLE_ZIP
#include <zip.h>
#endif
#ifndef __DISABLE_DIRECT
#include <dlfcn.h>
#endif

#include "./elfloader/elf_loader.h"
#include <pthread.h>
#include <stdarg.h>
#include <sys/stat.h>

#define OSSA_CORE_VERSION 15

#ifndef OSSA_CORE_MAXHEADER
    #define OSSA_CORE_MAXHEADER 1024
#endif

struct ossaSymbol *heap;

/* local */
int importFromHeap(struct ossaPlugin *self, char *symname){
    struct elf_module *entity = self->libEntity;
    for(struct ossaSymbol *iter = heap;iter!=0x0; iter=iter->next){
        if(iter->type == 0x0) continue;
        if(!strcpy(iter->type, symname)){
            void **ptr2ptr = get_elf_module_symbol(entity, symname);
            if(ptr2ptr == 0x0) return -1;
            *ptr2ptr = iter->data;
            return 0;
        }
    }
    return -1;
}
int exportToHeap(struct ossaPlugin *self, char *symname, void *data){
    struct ossaSymbol *iter;
    for(iter = heap;iter->next!=0x0; iter=iter->next);
    iter->next = malloc(sizeof(struct ossaSymbol));
    (*iter->next) = (struct ossaSymbol){symname, data, 0x0};
    return 0;
}
extern int pushToHeap(char *symname, void *data){
    exportToHeap(0x0, symname, data);
}
int removeFromHeap(char *symname){
    struct ossaSymbol *iter = heap;
    while(1){
        if(iter->next == 0x0){
            return 0;
        }else{
            if(!strcmp(iter->next->type, symname)){
                void *ptr = iter->next->next;
                free(iter->next);
                iter->next = ptr;
                return 1;
            }
        }
    }
}
extern struct ossaSymbol getFromHeap(char *symname){
    for(struct ossaSymbol *iter = heap;iter!=0x0; iter=iter->next){
        if(!strcpy(iter->type, symname)){
            return *iter;
        }
    }
    return (struct ossaSymbol){0x0,0x0,0x0};
}
extern int initHeap(struct ossaSymbol *heapStorage){
    if(heapStorage == 0x0) heap = malloc(sizeof (struct ossaSymbol));
    else heap = heapStorage;
    *heap = (struct ossaSymbol){0x0,0x0,0x0};
    return 0;
}
// not ready
int clientRequireLibDirect(struct ossaPlugin *self, ossastr name){
    return 0;
}
int (*newMessageHandler)(ossaCID cid, ossaMessage mes);
int pluginChatAddMessage(ossaCID cid, ossaMessage mes){
    listAppend(&((struct ossaChat*)(cid))->messages, &mes, sizeof(ossaMessage));
    if(newMessageHandler != 0x0) newMessageHandler(cid, mes);
    return 0;
}int pluginChatAddUser(ossaCID cid, ossaUser mes){
    listAppend(&((struct ossaChat*)(cid))->userlist, &mes, sizeof(ossaUser));
    return 0;
}
char checkValidPlugin(struct ossaPlugin *plugin){
    return 1;
}
char find_settings(const void *target, void *current){

}
char find_user(const void *target, void *current){

}
void defaultLogFunction(char type, const char *format, ...){
    va_list list;
    va_start(list, format);
    vprintf(format, list);
    va_end(list);
}
int notifyProcess(void *pluginptr, ossaCID cid, ossaMessage message){
    struct ossaPlugin *pl = (struct ossaPlugin *)pluginptr;
    
}
// Local variables
void (*logFunction)(char type, const char *format, ...) = defaultLogFunction;
void (*notifyCallback)(struct ossaChat *where, ossaMessage incoming) = 0x0;
/* export */
int setNewMessageHandler(int (*clientNewMessageHandler)(ossaCID cid, ossaMessage mes)){
    newMessageHandler = clientNewMessageHandler;
    return 0;
}
int setNotifyCallback(void (*callback)(struct ossaChat *where, ossaMessage incoming)){
    notifyCallback = callback;
    return 0;
}
int setLogFunction(void (*ossaLog)(char type, const char *format, ...)){
    logFunction = ossaLog;
    return logFunction != 0x0;
}
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
    ossaUser zero = {"me", "{\"metadata\":{\"visual\":{\"pictype\":\"none\",\"picture\":\"none\"},\"text\":{\"name\":\"Default OSSA user (me)\",\"bio\":\"\"}},\"chat\":{\"name\":\"me\",\"ossauid\":0,\"roles\":[]}\"GID\":\"OUKVp0.4-C\"}"};
    listAppend(&chat.userlist, &zero, sizeof(ossaUser));
    chat.settings = makeEmptyList();
    chat.cid = plugin->pcall.makeChat(title);

    if(chat.cid < 0){
        fprintf(stderr, "[!!] OSSA Core: Fatal error: failed to make new chat\n");
    }

    return chat;
}

int setChatSettings(struct ossaChat* _this, ossastr field, ossastr data){
    // Not found settings field
    listRemove(&_this->settings, listFind(&_this->settings, field, find_settings));
    unsigned int size = strlen(field)+strlen(data);
    char *buffer = malloc(size);
    sprintf(buffer, "%s\r%s", field, data);
    listAppendLink(&_this->settings, buffer);
    return OSSA_OK;
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
    int code = OSSA_OK;
//    int code = chatAction(_this, "invite", (ossalist(ossastr)){globalUID, 0x0});
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
//    int code = chatAction(_this, "userdel", argv);
    int code = OSSA_OK;
    if(code == OSSA_OK){
        listRemove(&_this->userlist, uid);
    }
    earaseList(&argv);
    return code;
}

int ossSetPluginRoutineDirect(void *pluginptr,void*(*routine)(void *args), void* args){
    pthread_t pid;
    pthread_create(&pid, 0x0, routine, args);
    pthread_detach(pid);
    listAppend(&((struct ossaPlugin*)(pluginptr))->threads, &pid, sizeof(pthread_t));
}

int sendMessage(struct ossaChat *_this, ossaMessage message){
    listAppend(&_this->messages, &message, sizeof(ossaMessage));
    if(!(_this->plugin->pcall.state() & (OSSA_STATE_AUTHED|OSSA_STATE_ENABLE)))
        return OSSA_BAD_LOGIN;
    _this->plugin->pcall.sendMes(_this->cid, message);
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
    char *argv = malloc(5120);
    memset(argv, 0, 5120);
    strcpy(argv, action_name);
    for(int i = 0; i < listLen(&args); i++){
        sprintf(argv, "%s %s", argv, (char*)listGet(&args, i));
        // strcat(argv, );
    }
    int ret = _this->plugin->pcall.chatAction(_this->cid, argv);
    free(argv);
    return ret;
}

int updateChat(struct ossaChat *_this){
    if( _this->plugin->pcall.updateChat(_this->cid) > 0){
        
    }
    return _this->plugin->pcall.updateChat(_this->cid);
}

#ifndef DISABLE_ZIP
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
//    struct zip_t *drop = zip_open(location, ZIP_DEFAULT_COMPRESSION_LEVEL, 'w');
    if(err != 0){
//        fprintf(stderr, "[!!] OSSA Core: Fatal error: failed to open \'%s\' for writing, %s", location, zip_strerror(drop));
        return err;
    }
    { //Writing header
        char headerdata[OSSA_CORE_MAXHEADER];
        sprintf(headerdata, "{\"desc\":\"OSSA Chat Archive Header File\",");
        sprintf(headerdata, "\"OSSA Core version\":\"%i\",", OSSA_CORE_VERSION);
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
//        zip_entry_open(drop, "header.json");
//        zip_entry_write(drop, headerdata,strlen(headerdata));
//        zip_entry_crc32(drop);
//        zip_entry_close(drop);
    }
//    zip_close(drop);
    return 0;
}
#endif
int loadPlugin(struct ossaPlugin *_this, ossastr path){
    initHeap(heap);
    if(path == 0x0){
        return -1;
    }
    int err;
    zip_t *plug = zip_open(path, ZIP_RDONLY, &err);
    struct zip_stat_t *stat = calloc(256, sizeof(int));
    zip_stat_init(stat);
    zip_file_t *manifest, *elf;

    char *rawmanifest = malloc(1024); memset(rawmanifest, 0x0, 1024);
    char *rawElf = malloc(102400);  memset(rawElf, 0x0, 102400);

    manifest = zip_fopen(plug, "manifest.json", ZIP_FL_UNCHANGED);
    if(!manifest){
        printf("Failed to open manifest!\n");
    }
    int buftmp = 0;

    int ret = zip_fread(manifest, rawmanifest, 1024);
    zip_fclose(manifest);
    { //Prepsocessing
        char *tmp = malloc(10240), *includeName = malloc(128), *includeContent = malloc(10240);
        memset(tmp, 0 ,10240);
        strcpy(tmp, rawmanifest);
        memset(includeContent, 0, 10240);
        memset(includeName, 0, 128);
        char *i = strstr(tmp, "\"include\": \"");
        for(; i != 0x0; i = strstr(tmp, "\"include\": \"")){
            memcpy(includeName, i+12, (strstr(i+13, "\"")-i-12));
            sprintf(includeName, "%s", includeName);
            struct zip_file* include = zip_fopen(plug, includeName, ZIP_FL_UNCHANGED);
            zip_fread(include, includeContent, 10240);
            zip_fclose(include);
            char *len = includeName+strlen(includeName)+1+1;
            len[strlen(len)] = 0;
            *i = 0;
            includeContent[strlen(includeContent)] = 0;
            sprintf(tmp, "%s%s%s", tmp, includeContent+1, len);
            memset(includeContent, 0, 10240);
            memset(includeName, 0, 128);
        }
        free(rawmanifest);
        rawmanifest = malloc(strlen(tmp));
        strcpy(rawmanifest, tmp);
        free(tmp);
        free(includeName);
        free(includeContent);
        printf("Result after preprocessing is:\n==========\n%s\n==========\n", rawmanifest);
    }
    ossalist(ossastr) usercoms;
    char *name = 0x0, *execname = 0x0, *desc = 0x0;
    JSON_Value  *jmanifest = 0x0;
    { //parsing JSON
        jmanifest = json_parse_string(rawmanifest);
        JSON_Object *root = json_value_get_object(jmanifest);
        char *tmp = json_object_get_string(root, "name");
        name = malloc(strlen(tmp));
        sprintf(name, "%s", tmp);
        tmp = json_object_get_string(root, "main");
        execname = malloc(strlen(tmp));
        strcpy(execname, tmp);
    }
//    if(pluginHeader == 0x0){
//        logFunction(OSSA_BAD_LOGIN, "Failed to read plugin header data\n");
//        return -1;
//    }
//    char *name = getStringFromJson(pluginHeader, "name");
//    char *desc = getStringFromJson(pluginHeader, "description");
//    char *execname = getStringFromJson(pluginHeader, "main");
//    freeJson(pluginHeader);
    free(rawmanifest);
    elf = zip_fopen(plug, execname, ZIP_FL_UNCHANGED);
    if(!elf){
        logFunction(OSSA_BAD_LOGIN, "Failed to load executable file ($zip/%s) from %s plugin\n", execname, path);
        return -1;
    }
    zip_fread(elf, rawElf, 102400);
    struct elf_module *entity = load_elf_module(name, rawElf, 102400);
    if(entity == 0x0){
        logFunction(OSSA_BAD_LOGIN, "Failed to load executable file ($zip/%s) from %s plugin: ELF error\n", execname, path);
        return -1;
    }
    _this->libEntity = (void *) entity;
    _this->name = name;
    _this->loaction = desc;
    int nullCounter = 0;
    { //loading symbols
        _this->init = get_elf_module_symbol(entity, "plugin_init");
        _this->pcall.connect = get_elf_module_symbol(entity, "plugin_connect");
        _this->pcall.disconnect = get_elf_module_symbol(entity, "plugin_disconnect");
        _this->pcall.state = get_elf_module_symbol(entity, "plugin_state");


        if(get_elf_module_symbol(entity, "self") == 0x0){
            return -3;
        }
        *((void**)(get_elf_module_symbol(entity, "self"))) = (void*)_this;
        if(_this->name == 0x0) {
            fprintf(stderr, "[!!] OSSA Core: Fatal error: failed to load \'%s\' plugin: unnamed plugin.\n", path);
        }
        importFromHeap(_this, "ossaLog");
        importFromHeap(_this, "ossaClientSetRoutineDirect");
        importFromHeap(_this, "ossaClientNotifyDirect");
        importFromHeap(_this, "ossaClientEventDirect");
        importFromHeap(_this, "ossaClientImportDirect");
        importFromHeap(_this, "ossaClientExportDirect");
        importFromHeap(_this, "ossaClientRequireLibDirect");
        importFromHeap(_this, "ossaLog");
        _this->init = get_elf_module_symbol(entity, "plugin_init");
        _this->pcall.connect = (int(*)())get_elf_module_symbol(entity, "plugin_connect");
        _this->pcall.disconnect = (int(*)())get_elf_module_symbol(entity, "plugin_disconnect");
        _this->pcall.state = (int(*)())get_elf_module_symbol(entity, "plugin_state");

        _this->pcall.auth = (int(*)(ossastr, ossastr))get_elf_module_symbol(entity, "plugin_user_auth");
        _this->pcall.oauth = (int(*)(ossastr))get_elf_module_symbol(entity, "plugin_user_oauth");
        _this->pcall.exit = (int(*)())get_elf_module_symbol(entity, "plugin_user_exit");
        _this->pcall.renameMe = (int(*)(ossastr))get_elf_module_symbol(entity, "plugin_user_rename");
        _this->pcall.myInfo = (ossaUser(*)())get_elf_module_symbol(entity, "plugin_user_info");
        _this->pcall.globalUIDInfo = (ossaUser(*)(ossastr))get_elf_module_symbol(entity, "plugin_user_ginfo");

        _this->pcall.sendMes = (int(*)(ossaCID, ossaMessage))get_elf_module_symbol(entity, "plugin_message_send");
        _this->pcall.editMes = (int(*)(ossaCID, ossaMessage, ossaMID))get_elf_module_symbol(entity, "plugin_message_edit");

        _this->pcall.makeChat = (ossaCID(*)(ossastr))get_elf_module_symbol(entity, "plugin_chat_makeChat");
        _this->pcall.getChatSettings = (ossastr(*)(ossaCID))get_elf_module_symbol(entity, "plugin_chat_getprefs");
        _this->pcall.setChatSettings = (int(*)(ossaCID, ossastr, ossastr))get_elf_module_symbol(entity, "plugin_chat_setpref");
        _this->pcall.updateChat = (int(*)(ossaCID))get_elf_module_symbol(entity,"plugin_chat_update");
        _this->pcall.loadChat = (int(*)(ossaCID, ossastr))get_elf_module_symbol(entity,"plugin_chat_load");
        _this->pcall.getChatList = (ossastr(*)())get_elf_module_symbol(entity,"plugin_chat_list");
        _this->pcall.getChatGUIDs = (ossastr(*)(ossaCID))get_elf_module_symbol(entity, "plugin_chat_getGUIDs");
//        _this->pcall.chatAction = (int(*)(ossaCID, ossastr))get_elf_module_symbol(entity, "plugin_chat_action");
    }
    { //loading usercoms
        { //parsing usercoms
            struct json_array_t *obj = json_object_get_array(jmanifest, "commands");
            for(int i = 0; i < json_array_get_count(obj); i++){
                struct ossaUsercom current = {};
                struct json_object_t *currcom = json_object_get_value_at(obj, i);
                char *cname = json_object_get_string(currcom, "command");
                if(cname == 0x0)
                    continue;
                current.com = cname;
                char *descr = json_object_get_string(currcom, "description");
                current.descr = descr;
                struct json_array_t *jarg = json_object_get_array(currcom, "arguments");
                current.args = malloc(sizeof(ossastr) * json_array_get_count(jarg));
                for(int j = 0; j < json_array_get_count(jarg); j++){
                    struct json_object_t *currarg = json_array_get_object(jarg, j);
                    char *_argname = json_object_get_string(jarg, "format");
                    char *argname = malloc(strlen(_argname) + 5);
                    sprintf(argname, "%c%c%c%c%c%s", /* [required][multiply][couldBeNULL][order][additional][argformat] */
                            json_object_get_boolean(currarg, "require") == 1? 'r':'u',
                            json_object_get_boolean(currarg, "multiply") == 1? 'm':'s',
                            json_object_get_boolean(currarg, "nullable") == 1? 'n':'d',
                            strcmp(json_object_get_string(currarg, "order"), "any")? atoi(json_object_get_string(currarg, "order")):'o',
                            'a',
                            _argname
                    );
                    free(_argname);
                    char *callline = json_object_get_string(currarg, "call");
                    int lenOfFuncName = strstr(callline, "(") - callline;
                    char *funcName = malloc(lenOfFuncName);
                    strncpy(funcName, callline, lenOfFuncName);
                    
                }
            }
        }
    }
    _this->init();
    return nullCounter;
}

//deprecated! Use loadPlugin(_this, path_to_zip_pluin) instead
int loadChatPlugin(struct ossaPlugin *_this, ossastr path){
#if (OSSA_CORE_VERSION > 14)
    return -OSSA_DEPRECATED;
#else
    if(path == 0x0) {
        return -1;
    }
    void *entity = _this->libEntity = dlopen(path, RTLD_LAZY);
    if(_this->libEntity == 0x0){
        fprintf(stderr, "[!!] OSSA Core: Fatal error: failed to open \'%s\' plugin: \n\t%s\n", path, dlerror());
        return -1;
    }
    unsigned int nullCounter = 0;
    _this->loaction = malloc(strlen(path));
    strcpy(_this->loaction, path);
    _this->name = *((char**)(dlsym(entity, "plugin_name")));
    if(_this->name == 0x0){
        return -2;
    }
    for(int i = 0; _this->name[i] != 0; i++){
        if(((_this->name[i] > 'z' || _this->name[i] < 'a')&&
            (_this->name[i] > 'Z' || _this->name[i] < 'A')&&
            (_this->name[i] > '9' || _this->name[i] < '0'))&&
            (_this->name[i] != '_' && _this->name[i] != '-')){
                fprintf(stderr, "Unregular name: \'%s\', \'%c\'\n", _this->name, _this->name[i]);
                return -2;
            }
    }
    if(dlsym(entity, "self") == 0x0){
        return -3;
    }
    *((void**)(dlsym(entity, "self"))) = (void*)_this;
    if(_this->name == 0x0) {
        fprintf(stderr, "[!!] OSSA Core: Fatal error: failed to load \'%s\' plugin: unnamed plugin.\n", path);
    }
    if(dlsym(entity, "ossaLog") != 0x0){
        *((void(**)(char, const char*, ...))dlsym(entity, "ossaLog")) = logFunction;
    }
    _this->init = dlsym(entity, "plugin_init");
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

    _this->pcall.makeChat = (ossaCID(*)(ossastr))dlsym(entity, "plugin_chat_makeChat");
    _this->pcall.getChatSettings = (ossastr(*)(ossaCID))dlsym(entity, "plugin_chat_getprefs");
    _this->pcall.setChatSettings = (int(*)(ossaCID, ossastr, ossastr))dlsym(entity, "plugin_chat_setpref");
    _this->pcall.updateChat = (int(*)(ossaCID))dlsym(entity,"plugin_chat_update");
    _this->pcall.loadChat = (int(*)(ossaCID, ossastr))dlsym(entity,"plugin_chat_load");
    _this->pcall.getChatList = (ossastr(*)())dlsym(entity,"plugin_chat_list");
    _this->pcall.getChatGUIDs = (ossastr(*)(ossaCID))dlsym(entity, "plugin_chat_getGUIDs");
    _this->pcall.chatAction = (int(*)(ossaCID, ossastr))dlsym(entity, "plugin_chat_action");

    { /* check for NULL functions */
        if(_this->init == 0x0) nullCounter++;

        if(_this->pcall.connect == 0x0) nullCounter++;
        if(_this->pcall.disconnect == 0x0) nullCounter++;
        if(_this->pcall.state == 0x0) nullCounter++;
        
        if(_this->pcall.auth == 0x0) nullCounter++;
        if(_this->pcall.oauth == 0x0) nullCounter++;
        if(_this->pcall.exit == 0x0) nullCounter++;
        if(_this->pcall.renameMe == 0x0) nullCounter++;
        if(_this->pcall.myInfo == 0x0) nullCounter++;
        if(_this->pcall.globalUIDInfo == 0x0) nullCounter++;

        if(_this->pcall.sendMes == 0x0) nullCounter++;
        if(_this->pcall.editMes == 0x0) nullCounter++;

        if(_this->pcall.makeChat == 0x0) nullCounter++;
        if(_this->pcall.getChatSettings == 0x0) nullCounter++;
        if(_this->pcall.setChatSettings == 0x0) nullCounter++;
        if(_this->pcall.updateChat == 0x0) nullCounter++;
        if(_this->pcall.loadChat == 0x0) nullCounter++;
        if(_this->pcall.getChatList == 0x0) nullCounter++;
        if(_this->pcall.getChatGUIDs == 0x0) nullCounter++;
    }
    { //exporting
        void *ptr = 0x0;
        //ossaClientSetRoutineDirect
        ptr = dlsym(entity, "ossaClientSetRoutineDirect");
        if(ptr != 0x0){
            *((int(**)(void*,void*(*)(void*), void*))(ptr)) = ossSetPluginRoutineDirect;
        }
        ptr = 0x0;
        ptr = dlsym(entity, "ossaChatAddMessage");
        if(ptr != 0x0){
            *((int(**)(ossaCID cid, ossaMessage))(ptr)) = pluginChatAddMessage;
        }
        ptr = dlsym(entity, "ossaChatAddUser");
        if(ptr != 0x0){
            *((int(**)(ossaCID cid, ossaUser))(ptr)) = pluginChatAddUser;
        }
    }

    if(_this->init == 0x0){
        fprintf(stderr, "[!!] OSSA Core: Fatal error: failed to load init\n");
        return -2;
    }else{
        int code = _this->init();
        if(code != OSSA_OK){
            fprintf(stderr, "[!!] OSSA Core: Fatal error: init failed with code %i\n", code);
            return -3;
        }
    }

    return nullCounter;
#endif
}

#ifndef COMPILE_STATIC
    // #warning "USING DYNAMIC COMPILATION"
    int main(){

    }
#else
    // #warning "USING STATIC COMPILATION"
#endif

ossastr getUsernameFromUser(ossaUser user){
    return user.nickname;
}