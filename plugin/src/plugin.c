#include <stdio.h>
#include <string.h>
#include <time.h>
#include "./musthave.h"
#include "mhs.h"

char *   iusername      =0x0,
     *   ipathToFile    =0x0,
         nullchar       =0x0;
FILE *   stream         =0x0;

int plugin_init(){
    printf("OSSA User Kit Version: 0.3-A.\n");
    return OSSA_OK;
}

int plugin_connect(){
    // printf("OSSA User Kit Host \n");
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
    fprintf(stream, "(SEND) %s@%lu (%lu): %s{%p}\n", iusername, cid, time(0x0), mes.body, &mes.attach);
    return OSSA_OK;
}
int plugin_message_editMes(ossaCID cid, ossaMessage mes, ossaMID mid){
    if(stream == 0x0) return -1;
    fprintf(stream, "(EDITED %lu) %s@%lu (%lu): %s{%p}\n", mid, iusername, cid, time(0x0), mes.body, &mes.attach);
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
    char word[512];
    int index = 0;
    oreadWord(word, action);
    if(!strcmp(word, "info")){
        fprintf(stderr, "OSSA User Kit Version: 0.3-A.\n\tOperating system is ");
        #ifdef __linux
            fprintf(stderr, "linux");
        #endif
        #ifdef __WIN32
            fprintf(stderr, "Windows");
        #endif
        #ifdef __APPLE
            fprintf(stderr, "macOS (OSX)");
        #endif
        fprintf(stderr, "\n\tTotal commands (except MCCK): %i\n\tLog file: (%p)\n", 0, stream);
    }else if(!strcmp(word, "ffmeg")){

    }else{
        fprintf(stderr, "\"%s\" is not found", word);
        return -1;
    }
    return OSSA_OK;
}