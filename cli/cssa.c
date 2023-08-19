#include <dlfcn.h>
#include <errno.h>
#include <stdio.h>
#define MAX_USERID_LENGTH 32
#include <stdlib.h>
#include "../core/elfloader/elf_loader.h"
#include "../core/core.h"
#ifndef __WIN32
    #include <unistd.h>
    void playsound(const char *path){

    }
#else
    #include <windows.h>
    void playsound(const char *path){
        // PlaySound(path, NULL, SND_ASYNC);
        printf("\033[s\n* NEW MESSAGE *\033[u");
    }
#endif
#define OSSA_MUTUAL_CLI

#define OSSA_CLI_ALLOW_KFP  1<<8
#define OSSA_CLI_NOPLUGINS  1<<7
#define OSSA_CLI_USEBOIP    1<<6
#define OSSA_CLI_DEVMODE    1<<5
#define OSSA_CLI_ALLOWSHELL 1<<4
#define OSSA_CLI_SYSERNAME  1<<3
#define OSSA_CLI_USE_ETC    1<<2
#define OSSA_CLI_ALLOWSO    1<<1
#define OSSA_CLI_ANTIALIAS  1<<0

int messageHandler(ossaCID cid, ossaMessage mes){
    playsound("message.mp3");
    printf("\033[s\n* NEW MESSAGE *\033[u");
}

struct _settings{
    char    *defaultUsercomsLocation,
            *config,
            *plugins,
            *defaultPlugmanLocation;
    unsigned int flags;
};

void *cinfo(const char *e){
    printf("Core info request: %s\n", e);
    return 0x0;
}

struct _usercom{
    ossastr name;
    int (*entity)(ossalist(ossastr));
};

char flags = OSSA_CLI_ALLOWSHELL | OSSA_CLI_ALLOWSO | OSSA_CLI_ANTIALIAS | OSSA_CLI_USE_ETC;

char *uname, *chatname;

struct ossaChat *currentChat = 0x0;

int input(ossalist(ossastr) *buffer){
    char *ibuf = news(char, 1024);
    memset(ibuf, 0, 1024);
    int i = 0;
    printf("\033[s\n\033[u%s@%s> ", ((ossaUser*)listGet(&currentChat->userlist, 0))->nickname, currentChat->title);
    while(1){
        char c = getchar();
        if(c == ' '){
            ibuf[i] = 0;
            listAppend(buffer, ibuf, strlen(ibuf)+1);
            memset(ibuf, 0, 1024);
            i = 0;
        }else if(c == '\n'){
            ibuf[i] = 0;
            listAppend(buffer, ibuf, strlen(ibuf)+1);
            break;
        }else if(c == 8){
            if(i == 0){
                strcpy(ibuf, (char*)listGet(buffer, listLen(buffer)));
                listRemove(buffer, listLen(buffer));
            }else{
                ibuf[--i] = 0;
            }
        }else{
            ibuf[i++] = c;
        }
    }
    free(ibuf);
    return OSSA_OK;
}

int parseSettings(int argc, char **argv, struct _settings *settings){
    for(int i = 0; i < argc; i++){
        if(!strcmp(argv[i], "--noplug") || !strcmp(argv[i], "-P")){

        } else if(!strcmp(argv[i], "--useBOIP") || !strcmp(argv[i], "-b")){

        } else if(!strcmp(argv[i], "--devmode") || !strcmp(argv[i], "-D")){
            settings->flags |= OSSA_CLI_DEVMODE;
        } else if(!strcmp(argv[i], "--fullacc") || !strcmp(argv[i], "-F")){

        } else if(!strcmp(argv[i], "--sysname") || !strcmp(argv[i], "-S")){
            uname = malloc(MAX_USERID_LENGTH);
            // cuserid(uname);
        } else if(!strcmp(argv[i], "--no-use-etc") || !strcmp(argv[i], "-e")){

        } else if(!strcmp(argv[i], "--allowonlyso") || !strcmp(argv[i], "-s")){

        } else if(!strcmp(argv[i], "--antialias") || !strcmp(argv[i], "-a")){

        } else if(!strcmp(argv[i], "--conf") || !strcmp(argv[i], "-E" )){
            if(strcmp(settings->config, argv[i+1])){
                settings->config = argv[++i];

                FILE *cfg = fopen(settings->config, "r");
                char *argss = (char*)malloc(2048);
                fread(argss, 1, 2048, cfg);
                char **args_structed = (char**)(malloc(sizeof(char*) * 32));
                args_structed[0] = argss;
                int current_Arg = 0;
                for(int i = 0; argss[i] != 0; i++){
                    if(argss[i] == ' '){
                        argss[i] = 0;
                        args_structed[++current_Arg] = argss+(++i);
                    }
                }
                fclose(cfg);

                parseSettings(current_Arg, args_structed, settings);
            }
        } else if(!strcmp(argv[i], "--usercoms") || !strcmp(argv[i], "-U" )){
            settings->defaultUsercomsLocation = argv[++i];
        } else if(!strcmp(argv[i], "--allow-kfp") || !strcmp(argv[i], "-K" )) {
            settings->flags |= OSSA_CLI_ALLOW_KFP;
        } else if(!strcmp(argv[i], "--plugpath") || !strcmp(argv[i], "-p")){
            settings->plugins = argv[++i];
        } else if(!strcmp(argv[i], "--plugman") || !strcmp(argv[i], "-M")){
            settings->defaultPlugmanLocation = argv[++i];
        }
        else {
            printf("Usage here\n");
        }
    }
}

int main(int argc, char **argv){

    struct _settings settings = {0x0,0x0,0x0, 0x0};

    { //Default settings
        #ifndef __WIN32

            char *f = "/.config/ossa/cssa.cfg";
            char *ff = (char*)malloc(strlen(getenv("HOME")) + strlen(f) + 1);
            strcat(ff, getenv("HOME"));
            strcat(ff, f);
            FILE *cfg = fopen(ff, "r");
            if(cfg == 0x0){
                cfg = fopen("/etc/ossa/cssa.cfg", "r");
                if(cfg == 0x0){
                    cfg = fopen(ff, "w");
                    if(cfg == 0x0) {
                        printf("[!!] OSSA Client: Unable to read or rewrite config. Please, reinstall CSSA or allow \'+w+r\' for \"~/.config/ossa/cssa.cfg\"\n");
                        printf("[!!] OSSA Client: I/O Error: %s\n", strerror(errno));
                        return -1;
                    }
                    cuserid(uname);
                    fprintf(cfg, "--sysname --allowonlyso --usercoms %s/.config/ossa/interpreter.so --plugpath %s/.share/ossa/plugins/", getenv("HOME"), getenv("HOME"));
                    fflush(cfg);
                    fclose(cfg);
                    cfg = fopen(ff, "r");
                    if(cfg == 0x0) {
                        printf("[!!] OSSA Client: Unable to read or rewrite config. Please, reinstall CSSA or allow \'+w+r\' for \"~/.config/ossa/cssa.cfg\"\n");
                        return -1;
                    }
                }
            }
            char *argss = (char*)malloc(2048);
            fread(argss, 1, 2048, cfg);
            char **args_structed = (char**)(malloc(sizeof(char*) * 32));
            args_structed[0] = argss;
            int current_Arg = 0;
            for(int i = 0; argss[i] != 0; i++){
                if(argss[i] == ' '){
                    argss[i] = 0;
                    args_structed[++current_Arg] = argss+(++i);
                }
            }
            parseSettings(current_Arg+1, args_structed, &settings);
            // free(args_structed);
            // free(argss);
            fclose(cfg);
        #else

        #endif
    }
    parseSettings(argc, argv, &settings);

    setNewMessageHandler(messageHandler);

    ossalist(struct ossaChat) chats = makeEmptyList();
    ossalist(struct ossaPlugin) plugins = makeEmptyList();

    initHeap(0x0);
    pushToHeap("chats", &chats);
    pushToHeap("plugins", &plugins);
    pushToHeap("settings", &settings);
    pushToHeap("currentChat", &currentChat);

    { /* loading user-space commands */
        if(loadPlugin((struct ossaPlugin *)listResolve(&plugins, sizeof(struct ossaPlugin)), settings.defaultUsercomsLocation) == -1 ||
            listGet(&plugins, 0) == 0x0){
            //Failed to load sysplugin
            printf("[!!] OSSA Client: Failed to load system plugin at %s\n", settings.defaultUsercomsLocation);
            return -1;
        }
        if(loadPlugin((struct ossaPlugin *)listResolve(&plugins, sizeof(struct ossaPlugin)), settings.defaultPlugmanLocation) == -1 ||
           listGet(&plugins, 0) == 0x0){
            //Failed to load sysplugin
            printf("[!!] OSSA Client: Failed to load system plugin at %s\n", settings.defaultPlugmanLocation);
            return -1;
        }

        if(uname == 0x0){
            uname = malloc(strlen("user"));
            strcpy(uname, "user");
        }
        //Deprecated
//        *((ossalist(struct ossaChat)**)(dlsym(((struct ossaPlugin*)listGet(&plugins, 0))->libEntity, "chats"))) = &chats;
//        *((ossalist(struct ossaPlugin)**)(dlsym(((struct ossaPlugin*)listGet(&plugins, 0))->libEntity, "plugins"))) = &plugins;
//        *((struct _settings**)(dlsym(((struct ossaPlugin*)listGet(&plugins, 0))->libEntity, "settings"))) = &settings;

        struct ossaChat syschat = makeChat("sys", (struct ossaPlugin*)listGet(&plugins, 0));

        if(syschat.plugin == 0x0){
            //Failed to start syschat
            printf("[!!] OSSA Client: Failed to load system chat at %p\n", &syschat);
            printf("\tDo you want to continue with Minimal CLI Coms kit? [y/N]");
            char c = getchar();
            if(c == 'y' || c == 'Y'){

            }else{
                earaseList(&chats);
                earaseList(&plugins);
                return -1;
            }
        }
        listAppend(&chats, &syschat, sizeof(struct ossaChat));
        // listAppend(&currentChat->userlist, (uname, strlen(uname));
        /* later */
    }
    currentChat = listGet(&chats, 0);
    ossalist(ossastr) ibuffer = makeEmptyList();

    while(1){
        earaseList(&ibuffer);
        ibuffer = makeEmptyList();
        input(&ibuffer);
        if(astype(char)listGet(&ibuffer, 0) == ':'){
            switch (chatAction(listGet(&chats, 0),\
                (char*)(listGet(&ibuffer, 0)+1),\
                listFrame(&ibuffer, 1) != 0x0 ? *listFrame(&ibuffer, 1) : lnothing)){
                case OSSA_ORDER_KILL_CLIENT:
                    earaseList(&chats);
                    earaseList(&plugins);
                    return OSSA_OK;
                case OSSA_OK:
                    break;
                case OSSA_COM_NOT_FOUND:
                    switch(chatAction(currentChat,\
                    (char*)(listGet(&ibuffer, 0)+1),\
                    listFrame(&ibuffer, 1) != 0x0 ? *listFrame(&ibuffer, 1) : lnothing)){
                        case OSSA_ORDER_KILL_CLIENT:
                            if(settings.flags & OSSA_CLI_ALLOW_KFP){
                                earaseList(&chats);
                                earaseList(&plugins);
                                return OSSA_OK;
                            } else {
                                printf("Command tries to kill client, but KFP (flag 8) is not allowed.\
 Do you want to exit? [y/N]: ");
                                if(getchar() == 'y'){
                                    earaseList(&chats);
                                    earaseList(&plugins);
                                    return OSSA_OK;
                                }
                            }
                            break;
                        case OSSA_COM_NOT_FOUND:
                            printf("< Command \"%s\" not found\n", (char*)(listGet(&ibuffer, 0)+1));
                        case OSSA_OK:
                            break;
                        default:
                            break;
                    }
                    break;
                default:
                    break;
            }
        }else if(astype(char)listGet(&ibuffer, 0) == '$'){
            if(flags & OSSA_CLI_ALLOWSHELL){
                char *ff = malloc(10240);
                memset(ff, 0, 10240);
                sprintf(ff, "%s", (char*)(listGet(&ibuffer, 0)+1));
                for(int i = 1; i < listLen(&ibuffer); i++){
                    sprintf(ff, "%s %s", ff, (char*)listGet(&ibuffer, i));
                }
                system(ff);
                free(ff);
            }else{
                printf("Failed to execute SHELL command: not permited\n");
            }
        }else{
            //Send message
            ossaMessage mes;
            mes.attach = makeEmptyList();
            mes.uid = 0;
            mes.body = (char*)malloc(1024*4);
            memset(mes.body, 0, 1024*4);
            for(int i = 0; i < listLen(&ibuffer); i++){
                sprintf(mes.body, "%s %s", mes.body, (char*)listGet(&ibuffer, i));
            }
            int sendcode = sendMessage(currentChat, mes) < 0;
            if(sendcode != OSSA_OK){
                printf("<<ERROR>>\n");
            }
            // free(mes.body);
        }
    }
}