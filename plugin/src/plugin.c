#include <dlfcn.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "../../core/core.c"
#include "./musthave.h"

char *   iusername      =0x0,
     *   ipathToFile    =0x0,
         nullchar       =0x0;
FILE *   stream         =0x0;

struct _settings{
    char    *defaultUsercomsLocaton,
            *config;
    unsigned int flags;           
} *settings;

ossalist(struct ossaChat) *chats = 0x0;
ossalist(struct ossaPlugin) *plugins = 0x0;

int plugin_init(){
    printf("OSSA User Kit Version: 0.3-A.\n");
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
    if(stream == 0x0) return -1;
    fprintf(stream, "(SEND) %s@%i (%lu): %s{%p}\n", iusername, cid, time(0x0), mes.body, &mes.attach);
    return OSSA_OK;
}
int plugin_message_editMes(ossaCID cid, ossaMessage mes, ossaMID mid){
    if(stream == 0x0) return -1;
    fprintf(stream, "(EDITED %lu) %s@%i (%lu): %s{%p}\n", mid, iusername, cid, time(0x0), mes.body, &mes.attach);
    return OSSA_OK;
}

ossaCID plugin_chat_makeChat(ossastr title){
    char *kkk = malloc(1024);
    if(ipathToFile == 0x0){
        sprintf(kkk, "%s.ossalog", title);
    } else {
        sprintf(kkk, "%s/%s.ossalog", ipathToFile, title);
    }
    stream = fopen(kkk, "w");
    if(stream == 0x0){
        fprintf(stderr, "error on opening file \"%s\"\n", kkk);
        free(kkk);
        return -1*OSSA_DECLINE;
    }
    free(kkk);
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
    return OSSA_OK;
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
ossastr plugin_chat_list(){
    return &nullchar;
}
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

int plugin_chat_action(ossaCID chatid, ossastr action){
    char **argv, *original;
    int argc = 0;

    original = malloc(strlen(action));
    strcpy(original, action);

    { /* parsing */
        {
            int f = 0;
            for(int i = 0; action[i] != 0; i++){
                if(action[i] == ' ' || action[i] == '\t')
                    f++;
            }
            argv = news(char*, f);
        }
        argv[0] = original;
        for(int i = 0; original[i] != 0; i++){
            // printf("*");
            if(original[i] == ' '){
                original[i] = 0;
                argv[++argc] = original+(i+1);
            }
        }
        argc++;
    }
    // Debug
    // printf("argc is %i\n", argc);
    // for(int i = 0; i < argc; i++){
    //     printf("argv[%i] is \"%s\"\n", i, argv[i]);
    // }
    if(!strcmp(argv[0], "exit")){
        free(argv);
        free(original);
        return 0x6;
    }else if(!strncmp(argv[0], "ls", 2)){

        char flags = 0;
        struct ossaPlugin *opt = 0x0;
        for(int i = 1; i < argc; i++){
            if(!strcmp(argv[i], "-id")){
                flags |= 1 << 0;
            } else if(!strcmp(argv[i], "-mem")){
                flags |= 1 << 1;
            } else if(!strcmp(argv[i], "-plugin")){
                flags |= 1 << 2;
            } else if(!strcmp(argv[i], "-po")){
                flags |= 1 << 3;
                i++;
                for(int j = 0; j < listLen(plugins); j++){
                    if(!strcmp(((struct ossaPlugin*)listGet(plugins, j))->name, argv[i]))
                        {opt = listGet(plugins, j); break;}
                }
            } else if(!strcmp(argv[i], "-state")){
                flags |= 1 << 4;
            }
        }

        if(flags & (1<<0)){
            printf("ID\t");
        }
        if(flags & (1<<1)){
            printf("memory\t");
        }
        printf("Name\t");
        if(flags & (1<<2)){
            printf("parent\t");
        }
        if(flags & (1<<4)){
            printf("state\t");
        }

        if(!strcmp(argv[0]+2, "chat")){
            printf("\n");
            for(ossaCID i = 0; i < listLen(chats); i++){
                if(flags & (1<<3)){
                    if(((struct ossaChat*)listGet(chats, i))->plugin != opt)
                        continue;
                }
                if(flags & (1<<0)){
                    printf("%i\t", i);
                }
                if(flags & (1<<1)){
                    printf("%p\t", listGet(chats, i));
                }
                printf("%s\t", ((struct ossaChat*)listGet(chats, i))->title);
                if(flags & (1<<2)){
                    printf("%s\t", ((struct ossaChat*)listGet(chats, i))->plugin->name);
                }
                if(flags & (1<<4)){
                    printf("%i\t", ((struct ossaChat*)listGet(chats, i))->plugin->pcall.state());
                }
                printf("\n");
            }
            printf("Total: %i\n", listLen(chats));
        } else if(!strcmp(argv[0]+2, "plugin")){
            printf("\n");
            for(int i = 0; i < listLen(plugins); i++){
                if(flags & (1<<3)){
                    if(((struct ossaChat*)listGet(chats, i))->plugin != opt)
                        continue;
                }
                if(flags & (1<<0)){
                    printf("%i\t", i);
                }
                if(flags & (1<<1)){
                    printf("%p\t", listGet(plugins, i));
                }
                printf("%s\t", ((struct ossaPlugin*)listGet(plugins, i))->name);
                if(flags & (1<<4)){
                    printf("%i\t", ((struct ossaPlugin*)listGet(plugins, i))->pcall.state());
                }
                printf("\n");
            }
            printf("Total: %i\n", listLen(plugins));
        }
    }else if(!strcmp(argv[0], "flag")){
        int flagid = atoi(argv[1]);
        if(!strcmp(argv[2], "set")){
            if(!strcmp(argv[3], "true") || !strcmp(argv[3], "1") || !strcmp(argv[3], "enable"))
                settings->flags |= (1<<flagid);
            else if(!strcmp(argv[3], "false") || !strcmp(argv[3], "0") || !strcmp(argv[3], "disable"))
                settings->flags &= ~(1<<flagid);
            else
                printf("<! Invalid %s flag state\n", argv[3]);
        }else{
            printf("<* Flag %i state is %s\n", flagid, settings->flags & (1<<flagid) ? "true" : "false");
        }
    } else if(!strcmp(argv[0], "plugin")){
        if(!strcmp(argv[1], "load")){
            if(loadChatPlugin((struct ossaPlugin *)listResolve(plugins, sizeof(struct ossaPlugin)), argv[2]) < 0){
                free(argv);
                free(original);
                return -3;
            }else{
                printf("< Plugin \"%s\" loaded successfully. Adress: %p\n", argv[2], listGet(plugins, listLen(plugins)-1));
            }
        }else if(!strcmp(argv[1], "install")){

        }else if(!strcmp(argv[1], "unload")){
            void *ptr = 0x0;
            if(!strncmp(argv[2], "0x", 2)){
                //Using adress
                sscanf(argv[2]+2, "%x", (unsigned int*)&ptr);
                // printf("< Unloading %p plugin\n", ptr);
                if(ptr == listGet(plugins, 0)){
                    printf("< Failed to unload %p plugin: This is system plugin.\n", ptr);
                    return -4;
                }
                for(int i = 0; i < listLen(plugins); i++){
                    if(listGet(plugins, i) == ptr){
                        dlclose(((struct ossaPlugin*) ptr)->libEntity);
                        listRemove(plugins, i);
                        return OSSA_OK;
                    }
                }
                printf("< Failed to unload %p plugin: no such plugin in plugins list.\n", ptr);
                return -3;
            }else{
                unsigned int id = 0;
                id = atoi(argv[2]);
                // printf("Unloading #%i (%s) plugin\n", id, ((struct ossaPlugin*)listGet(plugins, id))->name);
                ptr = listGet(plugins, id);
                if(ptr == listGet(plugins, 0)){
                    printf("< Failed to unload %p plugin: This is system plugin.\n", ptr);
                    return -4;
                }
                dlclose(((struct ossaPlugin*) ptr)->libEntity);
                listRemove(plugins, id);
                return OSSA_OK;
                //Using ID
            }
        }else{
            free(argv);
            free(original);
            return -2;
        }
    }
    else {
        free(argv);
        free(original);
        return -1;
    }
    return OSSA_OK;
}