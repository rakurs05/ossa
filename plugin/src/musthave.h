#ifndef OSSA_MUSTHAVE_TYPES
#include "./mhs.h"
#endif
#include "./ossaconsts.h"

int plugin_connect();
int plugin_disconnect();
int plugin_state();

int plugin_user_auth(char* username, char *pwd);
int plugin_user_oauth(char *token);
int plugin_user_exit();
int plugin_user_rename(char *newName);
ossaUser plugin_user_info();
ossaUser plugin_user_ginfo(ossastr globalUID);

int plugin_message_send(ossaCID cid, ossaMessage mes);
int plugin_message_editMes(ossaCID cid, ossaMessage mes, ossaMID mid);

int plugin_chat_makeChat(ossastr title, ossaCID cid);
ossastr plugin_chat_getprefs(ossaCID cid);
int plugin_chat_setpref(ossaCID cid, ossastr fieldname, ossastr value);
int plugin_chat_update(ossaCID cid);
int plugin_chat_loadChat(ossaCID cid, ossastr location);
ossastr plugin_chat_list();
ossastr plugin_chat_getGUIDs(ossaCID cid);