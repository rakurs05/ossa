#include <stdio.h>
#include <string.h>
#include <time.h>
#include "./musthave.h"

char *   iusername      =0x0,
     *   ipathToFile    =0x0,
         nullchar       =0x0;
FILE *   stream         =0x0;

int plugin_init(){
    printf("logger has been Inited.\n");
    return OSSA_OK;
}

int plugin_connect(){
    printf("logger has been started.\n");
    return 0;
}
int plugin_disconnect(){
    printf("logger has been stopped.\n");
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
    sprintf(kkk, "%s/%s", ipathToFile, title);
    stream = fopen(kkk, "w");
    free(kkk);
    if(stream == 0x0){
        fprintf(stderr, "error on opening file\n");
        return -1*OSSA_DECLINE;
    }
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