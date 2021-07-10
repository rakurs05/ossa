#ifndef OSSA_MUTUAL_CLI
#pragma OSSA_DECLINE
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

struct _usercom{
    ossastr name;
    int (*entity)(ossalist(ossastr));
};

char flags = OSSA_CLI_ALLOWSHELL | OSSA_CLI_ALLOWSO | OSSA_CLI_ANTIALIAS | OSSA_CLI_USE_ETC;

char *uname, *chatname;

ossalist(_usercom) comslist = lnothing;

struct ossaChat *currentChat = 0x0;

ossalist(struct ossaChat) chats = lnothing;
ossalist(struct ossaPlugin) plugins = lnothing;

#include "./cssa.usercoms.c"
#endif