#include <stdio.h>
#include <stdlib.h>
#include "../core/core.h"
#define OSSA_MUTUAL_CLI

#define OSSA_CLI_NOPLUGINS  1<<7
#define OSSA_CLI_USEBOIP    1<<6
#define OSSA_CLI_DEVMODE    1<<5
#define OSSA_CLI_ALLOWSHELL 1<<4
#define OSSA_CLI_SYSERNAME  1<<3
#define OSSA_CLI_USE_ETC    1<<2
#define OSSA_CLI_ALLOWSO    1<<1
#define OSSA_CLI_ANTIALIAS  1<<0

struct _settings{
    char    *defaultUsercomsLocaton,
            *config;
};

// void *cinfo(const char *e){
//     printf("Core info request: %s\n", e);
//     return 0x0;
// }

struct _usercom{
    ossastr name;
    int (*entity)(ossalist(ossastr));
};

char flags = OSSA_CLI_ALLOWSHELL | OSSA_CLI_ALLOWSO | OSSA_CLI_ANTIALIAS | OSSA_CLI_USE_ETC;

char *uname, *chatname;

ossalist(_usercom) comslist = lnothing;

struct ossaChat *currentChat = 0x0;

int input(ossalist(ossastr) *buffer){
    char *ibuf = news(char, 1024);
    memset(ibuf, 0, 1024);
    int i = 0;
    printf("%s@%s> ", uname, chatname);
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
        }else{
            ibuf[i++] = c;
        }
    }
    free(ibuf);
    return OSSA_OK;
}   

int main(int argc, char **argv){

    struct _settings settings;

    { /* args parsing */
        for(int i = 1; i < argc; i++){
            if(!strcmp(argv[i], "--noplug") || !strcmp(argv[i], "-P")){

            } else if(!strcmp(argv[i], "--useBOIP") || !strcmp(argv[i], "-b")){

            } else if(!strcmp(argv[i], "--devmode") || !strcmp(argv[i], "-D")){

            } else if(!strcmp(argv[i], "--fullacc") || !strcmp(argv[i], "-F")){

            } else if(!strcmp(argv[i], "--sysname") || !strcmp(argv[i], "-S")){

            } else if(!strcmp(argv[i], "--no-use-etc") || !strcmp(argv[i], "-e")){

            } else if(!strcmp(argv[i], "--allowonlyso") || !strcmp(argv[i], "-s")){

            } else if(!strcmp(argv[i], "--antialias") || !strcmp(argv[i], "-a")){

            } else if(!strcmp(argv[i], "--conf") || !strcmp(argv[i], "-E" )){
                settings.config = argv[++i];
            } else if(!strcmp(argv[i], "--usercoms") || !strcmp(argv[i], "-U" )){
                settings.defaultUsercomsLocaton = argv[++i];
            } else {
                printf("Usage here");
            }
        }
    }

    ossalist(struct ossaChat) chats = makeEmptyList();
    ossalist(struct ossaPlugin) plugins = makeEmptyList();
    ossalist(struct _usercom) usercoms = makeEmptyList();

    { /* loading user-space commands */
        loadChatPlugin((struct ossaPlugin *)listResolve(&plugins, sizeof(struct ossaPlugin)), settings.defaultUsercomsLocaton);
        if(listGet(&plugins, 0) == 0x0){
            //Failed to load sysplugin
            printf("[!!] OSSA Client: Failed to load system plugin at %s\n", settings.defaultUsercomsLocaton);
            return -1;
        }

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
                earaseList(&usercoms);
                return -1;
            }
        }
        listAppend(&chats, &syschat, sizeof(struct ossaChat));
        /* later */
    }
    currentChat = listGet(&chats, 0);

    ossalist(ossastr) ibuffer = makeEmptyList();

    while(1){
        earaseList(&ibuffer);
        ibuffer = makeEmptyList();
        input(&ibuffer);
        if(astype(char)listGet(&ibuffer, 0) == ':'){
            switch(chatAction((struct ossaChat*)listGet(&chats, 0), (char*)(listGet(&ibuffer, 0)+1), listFrame(&ibuffer, 1) != 0x0 ? *listFrame(&ibuffer, 1) : lnothing)){
                case OSSA_OK:
                    break;
                case OSSA_COM_NOT_FOUND:
                    switch(chatAction(currentChat, (char*)(listGet(&ibuffer, 0)+1), listFrame(&ibuffer, 1) != 0x0 ? *listFrame(&ibuffer, 1) : lnothing)){
                        case OSSA_OK:
                            break;
                        case OSSA_COM_NOT_FOUND:
                            printf("Command \"%s\" not found.\n", (char*)(listGet(&ibuffer, 0)+1));
                            break;
                        case OSSA_COM_INVALID_ARGS:
                            printf("Command \"%s\" called with invalid arguments.\n", (char*)(listGet(&ibuffer, 0)+1));
                            break;
                    }
                    break;
                case OSSA_COM_INVALID_ARGS:
                    printf("Command \"%s\" called with invalid arguments.\n", (char*)(listGet(&ibuffer, 0)+1));
                    break;
                // if()
                // printf("Command \"%s\" bot found.\n", (char*)(listGet(&ibuffer, 0)+1));
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
        }
    }
}