#include <stdio.h>
#include <string.h>
#include "../../core/core.h"
#include "./plugin.h"
#include "../../core/elfloader/elf_loader.h"

//TODO: move to deafult plugin resource header
#define ossa_init_args(ptr, ...) \
    struct { \
        void *env; \
        __VA_ARGS__; \
    } *params = (struct { \
        void *env; \
        __VA_ARGS__; \
    } *)(ptr)


int unread = 0;

PLUGIN_NAME("plugman\0");

char *   iusername      =0x0,
     *   ipathToFile    =0x0,
         nullchar       =0x0;
FILE *   stream         =0x0;

struct _settings{
    char    *defaultUsercomsLocaton,
            *config,
            *plugins;
    unsigned int flags;           
} *settings;

struct ossaChat **currentChat = 0x0;

ossalist(struct ossaChat) *chats = 0x0;
ossalist(struct ossaPlugin) *plugins = 0x0;

int plugin_init(){
    printf("Plugin manager version: 1.3-A.\n");
    currentChat = ossaClientImport("currentChat");
    plugins = ossaClientImport("plugins");
    chats = ossaClientImport("chats");
    settings = ossaClientImport("settings");
    return OSSA_OK;
}

int plugin_connect(){
    // cinfo("OSSA User Kit Host");
    return 0;
}
int plugin_disconnect(){
    // printf("logger has been stopped.\n");
    return 0;
}
int plugin_state(){
    if(iusername != 0x0 && ipathToFile != 0x0){
        return OSSA_STATE_ACTIVE | OSSA_STATE_AUTHED | OSSA_STATE_ENABLE;
    }
    else return OSSA_STATE_ENABLE;
}

int plugin_user_auth(char* username, char *pwd){
    iusername = malloc(strlen(username));
    strcpy(iusername, username);
    ipathToFile = malloc(strlen(pwd));
    strcpy(ipathToFile, pwd);
    return OSSA_OK;
}
int plugin_user_oauth(char *token){
    return OSSA_DECLINE;
}
int plugin_user_exit(){
    free(ipathToFile);
    free(iusername);
    ipathToFile = iusername = 0x0;
    return OSSA_OK;
}
int plugin_user_rename(char *username){
    free(iusername);
    iusername = malloc(strlen(username));
    strcpy(iusername, username);
    return OSSA_OK;
}
ossaUser plugin_user_info(){
    ossaUser me;
    me.nickname = iusername;
    me.head = ipathToFile;
    return me;
}
ossaUser plugin_user_ginfo(ossastr guid){
    ossaUser me;
    me.nickname = iusername;
    me.head = ipathToFile;
    return me;
}

int plugin_message_send(ossaCID cid, ossaMessage mes){
    return OSSA_OK;
}
int plugin_message_edit(ossaCID cid, ossaMessage mes, ossaMID mid){
    return OSSA_OK;
}

ossaCID plugin_chat_makeChat(ossastr title){
    return 0;
}
ossastr plugin_chat_getprefs(ossaCID cid){
    return &nullchar;
}
int plugin_chat_setpref(ossaCID cid, ossastr fieldname, ossastr value){
    return OSSA_OK;
}
int plugin_chat_update(ossaCID cid){
    if(stream == 0x0) return -1;
    fprintf(stream, "(UPDATE) request\n");
    fflush(stream);
    return unread;
}
int plugin_chat_loadChat(ossaCID cid, ossastr location){
    fflush(stream);
    fclose(stream);
    char *kkk = malloc(1024);
    sprintf(kkk, "%s/%s", ipathToFile, location);
    stream = fopen(kkk, "w");
    free(kkk);
    if(stream == 0x0){
        return OSSA_DECLINE;
    }
    return OSSA_OK;
}
// ossastr plugin_chat_list(){
//     return &nullchar;
// }
ossastr plugin_chat_getGUIDs(ossaCID cid){
    return &nullchar;
}

//Read command word
int oreadWord(ossastr ptr, ossastr from){
    for(unsigned int i = 0; ; i++){
        if(from[i] == '\x02' || from[i] == '\x1c' || from[i] == '\x03' || from[i] == 0){
            ptr[i] = 0;
            return from[i];
        }else{
            ptr[i] = from[i];
        }
    }
    return 0;
}

char comparePluginNames(const void *a, void *b){
    char *k = (char*)a;
    char *l = ((struct ossaPlugin*)(b))->name;
    return !strcmp(k,l);
}

int pluginLoad(void *raw){
    ossa_init_args(raw,
                   ossastr path;
                    ossastr flags);
    if(loadPlugin((struct ossaPlugin *)listResolve(plugins, sizeof(struct ossaPlugin)), params->path) < 0){
        return -3;
    }else{
        ossaLog(OSSA_LOG_INFO, "< Plugin \"%s\" loaded successfully. Adress: %p\n", params->path, listGet(plugins, listLen(plugins)-1));
    }
    return OSSA_OK;
}

int currentPluginAuth(void *raw){

    ossa_init_args(raw,
                   ossastr login;
                    ossastr password;);
    if((*currentChat)->plugin->pcall.state() & OSSA_STATE_AUTHED)
        return OSSA_ALREADY;
    return ((*currentChat)->plugin->pcall.auth(params->login, params->password));
}

int currentPluginState(void *raw){
    ossa_init_args(raw, ossastr plugin);
    int plugID = listFind(plugins, params->plugin, comparePluginNames);
    if(plugID < 0)
        return OSSA_COM_INVALID_ARGS;
    struct ossaPlugin *pl = (struct ossaPlugin *)listGet(plugins, plugID);
    int state = pl->pcall.state();
    printf("State of plugin (%s): %i, %p, ", pl->name, state, (void*)((long long)state));
    for(int i = 4*8-1; i >= 0; i--)
        printf("%i", (state & (1<<i))>0);
    printf("\n");
    return OSSA_OK;
}

int webInstallPlugin(void *raw){
    ossa_init_args(raw, ossastr repo; ossastr type; ossastr plugname);
    return OSSA_COM_INVALID_ARGS;
}

int pluginUnload(void *raw){
    ossa_init_args(raw, ossastr plugin);
    int counter = 0;
    ossalist(struct ossaPlugin) *iter = plugins;
    while(iter->next != 0x0){
        if(!strcmp(((struct ossaPlugin*)(iter->data))->name, params->plugin)){
            unload_elf_module(((struct ossaPlugin*)(iter->data))->name);
            listRemove(plugins, counter);
            return OSSA_OK;
        }
        counter++;
    }
    return OSSA_COM_INVALID_ARGS;
}

ossastr plugin_chat_list(){
    return "system";
}