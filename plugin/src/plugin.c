#include <dlfcn.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "../../core/core.c"
#include "./plugin.h"

int unread = 0;

PLUGIN_NAME("ossa_interpreter\0");
#ifndef __fuck_with_it_with_yourself
char *help = "\n\
OSSA interpretor (OUKVp0.4-C) (plugin)\n\n\
Welcome to OSSA official interpretor. Here is simple fast guide\n\
For using commands of interpretor use prefix \':\'. If this command exsistn't in interpretor OSSA client tries to eexceute command by current plugin\n\n\
 ==> Messaging\n\
For messaging You should just type. Anyway, if you need to type a long message with a long amount of lines you can use \':long <stop>\' command. \'<stop>\' mean stop-character. This si terminator for\
typing. Only when you type it OSSA immidiatly send message. Recomindation: use \'`\' as stop-character\n\n\
 ==>Commands\n\
* :ls command\n\
-> :lschat\n\t list all chats, which is opened\n\
-> :lsuser\n\tShows all users in current chat\n\
-> :lsplugin\n\tList of all plugins in system\n\
-> Arguments:\n\
\t\tFor all of theese commands have common arguments set\n\
\t-mem\n\t\t\
Shows memory adress\n\
\t-plugin\n\
\t\tShow parent plugin\n\
\t-id\n\
\t\tNumirate list\n\
\t-state\n\t\tShow state of list's item\n\n\
* :flag command\n\
-> :flag <flag> set <true/false/1/0>\n\
\tSet flag's state\n\
-> :flag <flag> get\n\tGet value of flag\n\n\
* :plugin command\n\
-> :plugin install $<path>\n\t\
Install \'.ossa\' from computer by path\n\
-> :plugin install <name>\n\
\tDoanload and install plugin by name. You can configure sources via \':plugin source\' command\n\
-> :plugin load <path>\n\
\tLoad plugin manually by path. You can load a few same plugins\n\
-> :plugin unload <plugin ID>\n\
\tUnload plugin by plugin ID. You can see it via \':lsplugin -id\'. You cannot to unload system plugin (id.0)\n\
-> :plugin source <add/remove/list> <name/id> <url>\n\
\tWith \':plugin source add <name> <url>\' you can add new plugin repo or idividual link for plugin updating. Attention: list of sources have priority: last item is more significant than previos.\n\
-> :plugin source remove <id>\n\
-> :plugin state [ID of plugin, current by default]\n\
-> :plugin auth [ID of plugin, current by default]\n\
\tAuth in plugin via login and password\n\
-> :plugin oauth [ID of plugin, current by default]\n\
\tAuth in plugin via token via oauth2 technology\n\
\tRemove link for plugin sync\n\
-> :plugin chats <plugin ID>\n\
\tView all chats in <plugin ID> plugin and enumirate it\n\
-> :plugin list\n\
\tList all plugins repos\n\n\
* :set command\n\
\tUse it for setting values of enviroment variables values. Syntax: 'set <variable> <value>'. If variable is not exsisting it's returning error. Use 'let' to declire it before using. Also check type of variable via '::typeof(<var>)'\
 because OSSA interpretore use static typing.\n\
-> :set chat <chatID>\n\
\tSet current chat\n\n\
-> :mkchat <chat name>@<plugin>\n\
\tMake new chat with <plugin> parent plugin\n\n\
 ==> Checking messages and view messages\n\
-> :check\n\
\tTo look up for a new messages\n\
-> :view <last/unread(default)> <N>\n\
\tView last <N> messages or all unread (by default if additional argument is not given). If you viewing unread messages second argument isn't necessary\n\n\
Also you can use JS directly via '::' prefix (or ':js:'). This is advenced option, so API is located here: https://bitbucket.org/ossa-streaming/ossa/src/master/";
#else
    char *help = "Fuck with it by youslef. Good luck :D";
#endif

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
    printf("OSSA User Kit Version: 0.5-C.\n");
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
    fprintf(stream, "(SEND) %s@%p (%lu): %s{%p}\n", iusername, cid, time(0x0), mes.body, &mes.attach);
    return OSSA_OK;
}
int plugin_message_editMes(ossaCID cid, ossaMessage mes, ossaMID mid){
    if(stream == 0x0) return -1;
    fprintf(stream, "(EDITED %lu) %s@%p (%lu): %s{%p}\n", mid, iusername, cid, time(0x0), mes.body, &mes.attach);
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
        return 0x0;
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
        printf("\n");
        if(!strcmp(argv[0]+2, "chat")){
            for(int i = 0; i < listLen(chats); i++){
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
        }else if(!strcmp(argv[0]+2, "user")){
            for(int i = 0; i < listLen(&(*currentChat)->userlist); i++){
                if(flags & (1<<0)){
                    printf("%i\t", i);
                }
                if(flags & (1<<1)){
                    printf("%p\t", listGet(&(*currentChat)->userlist, i));
                }
                printf("%s\t", ((ossaUser*)listGet(&(*currentChat)->userlist, i))->nickname);
                if(flags & (1<<4)){
                    printf("%p\t", ((ossaUser*)listGet(&(*currentChat)->userlist, i))->head);
                }
            }
            printf("\nTotal: %i\n", listLen(&(*currentChat)->userlist));
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

        }else if(!strcmp(argv[1], "state")){
            int plugID = 0;
            if(argc == 2)
                plugID = listFind(plugins, (*currentChat)->plugin, &comparePointers);
            else
                plugID = atoi(argv[2]);
            if(plugID > listLen(plugins))
                return OSSA_COM_INVALID_ARGS;
            struct ossaPlugin *pl = (struct ossaPlugin *)listGet(plugins, plugID);
            int state = pl->pcall.state();
            printf("State of plugin (%s): %i, %p, ", pl->name, state, (void*)((long long)state));
            for(int i = 4*8-1; i >= 0; i--)
                printf("%i", (state & (1<<i))>0);
            printf("\n");
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
        }else if(!strcmp(argv[1], "auth")){
            int plugID = 0;
            if(argc == 2)
                plugID = listFind(plugins, (*currentChat)->plugin, &comparePointers);
            else
                plugID = atoi(argv[2]);
            if(plugID > listLen(plugins))
                return OSSA_COM_INVALID_ARGS;
            struct ossaPlugin *pl = (struct ossaPlugin *)listGet(plugins, plugID);
            if(pl->pcall.state()&OSSA_STATE_AUTHED){
                printf("\t** You are already authed **\n");
            } else if(pl->pcall.auth != 0x0){
                printf("\t ** Auth for \'%s\' plugin **\nLogin:", pl->name);
                int i = 0;
                char *login = (char*)malloc(512);
                while(1){
                    char c = getchar();
                    if(c == '\n') break;
                    else if(c == 8) i--;
                    else login[i++] = c;
                }
                i = 0;
                printf("Password:");
                char *password = (char*)malloc(512);
                while(1){
                    char c = getchar();
                    if(c == '\n') break;
                    else if(c == 8) i--;
                    else password[i++] = c;
                }
                int codeOfAuth = pl->pcall.auth(login, password);
                if(codeOfAuth != OSSA_OK){
                    printf("Failed to auth\n");
                }
            }else{
                printf("\t ** Auth is disabled for this plugin. Try \'oauth\' **\n");
            }
        }else if(!strcmp(argv[1], "oauth")){
            int plugID = 0;
            if(argc == 2)
                plugID = listFind(plugins, (*currentChat)->plugin, &comparePointers);
            else
                plugID = atoi(argv[2]);
            if(plugID > listLen(plugins))
                return OSSA_COM_INVALID_ARGS;
            struct ossaPlugin *pl = (struct ossaPlugin *)listGet(plugins, plugID);
            if(pl->pcall.state()&OSSA_STATE_AUTHED){
                printf("\t** You are already authed **\n");
            } else if(pl->pcall.auth != 0x0){
                printf("\t ** OAuth for \'%s\' plugin **\nToken:", pl->name);
                int i = 0;
                char *token = (char*)malloc(512);
                while(1){
                    char c = getchar();
                    if(c == '\n') break;
                    else if(c == 8) i--;
                    else token[i++] = c;
                }
                int codeOfAuth = pl->pcall.oauth(token);
                if(codeOfAuth != OSSA_OK){
                    printf("Failed to auth\n");
                }
            }else{
                printf("\t ** OAuth is disabled for this plugin. Try \'auth\' **\n");
            }
        }
        else{
            free(argv);
            free(original);
            return -2;
        }
    } else if(!strcmp(argv[0], "help")){
        printf("%s\n", help);
    }else if(!strcmp(argv[0], "mkchat")){
        int term = 0;
        if(argc < 2){
            ossaLog(OSSA_LOG_CRIT, "Not enough arguments: you shold enter \'chatname@plugin\" for making new chat\n");
        }
        while(argv[1][term] != '@'){
            if(argv[1][term] == 0){
                ossaLog(OSSA_LOG_CRIT, "Not enough arguments: you shold enter \'chatname@plugin\" for making new chat\n");
                return OSSA_COM_INVALID_ARGS;
            }
            term++;
        }
        argv[1][term] = 0;
        struct ossaPlugin *plug = 0x0;
        { // searching for plugin
            struct __list *iter = plugins;
            while(iter != 0x0){
                struct ossaPlugin *piter = (struct ossaPlugin *)iter->data;
                if(!strcmp(argv[1]+term+1, piter->name)){
                    plug = piter;
                    break;
                }
                iter = iter->next;
            }
        }
        if(plug == 0x0){
            printf("< Failed to create new chat: \"%s\" plugin is not found.\n", argv[1]+term+1);
            free(argv);
            free(original);
            return OSSA_COM_INVALID_ARGS;
        }
        struct ossaChat newchat = makeChat(argv[1], plug);
        listAppend(chats, &newchat, sizeof(struct ossaChat));
    }else if(!strcmp(argv[0], "set")){
        if(!strcmp(argv[1], "chat")){
            if(atoi(argv[2]) >= listLen(chats)){
                free(argv);
                free(original);
                return OSSA_COM_INVALID_ARGS;
            } else *currentChat = listGet(chats, atoi(argv[2]));
        }else{
            //ENV
        }
    }else if(!strcmp(argv[0], "view")){
        int a = 0;
        if(argc == 1)
            a = (*currentChat)->plugin->pcall.updateChat((*currentChat));
        else if(argv[1] == "unread")
            a = (*currentChat)->plugin->pcall.updateChat((*currentChat));
        else if(argv[2] == 0) return OSSA_COM_INVALID_ARGS;
        a = listLen(&(*currentChat)->messages) - atoi(argv[2]);
        struct __list *frame = listFrame(&(*currentChat)->messages, a);
        while(frame != 0x0){
            printf("%s@%s: %s\n\t<meta: %s>\n", ((ossaUser*)listGet(&(*currentChat)->userlist, ((ossaMessage*)(frame->data))->uid))->nickname ,(*currentChat)->title, ((ossaMessage*)frame->data)->body, ((ossaUser*)listGet(&(*currentChat)->userlist, ((ossaMessage*)(frame->data))->uid))->head);
            frame = frame->next;
        }
    }else if(!strcmp(argv[0], "check-dev-pthread")){
        if(settings->flags & 1<<5)
            ossaClientSetRoutine((void*(*)(void*))(plugin_chat_update), 0x0);
        else
            ossaLog(OSSA_LOG_CRIT, "Access Fail: You should be developer for this action\n");
    }else if(!strcmp(argv[0], "addmes")){
        if((*currentChat)->plugin == self){
            ossaChatAddMessage(*currentChat, (ossaMessage){"Example message for test", lnothing, 0});
            unread++;
        }else{
            free(argv);
            free(original);
            return OSSA_COM_NOT_FOUND;
        }
    }
    else {
        free(argv);
        free(original);
        return OSSA_COM_NOT_FOUND;
    }
    free(argv);
    free(original);
    return OSSA_OK;
}
ossastr plugin_chat_list(){
    return "system";
}