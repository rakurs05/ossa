#ifndef OSSA_MUSTHAVE_TYPES
#include "./mhs.h"
#endif

int plugin_connect();
int plugin_disconnect();
int plugin_state();
char* plugin_info();

int plugin_user_auth(char* username, char *pwd);
int plugin_user_oauth(char *token);
int plugin_user_exit();
int plugin_user_rename(char *newName);
int plugin_user_info();