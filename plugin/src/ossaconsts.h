#ifndef OSSACONSTS_H
#define OSSACONSTS_H
#define OSSA_OK             0x0
#define OSSA_WAIT           0x1
#define OSSA_ACCPEPT        0x2
#define OSSA_DECLINE        0x3
#define OSSA_ALREADY        0x4
#define OSSA_NOUSER         0x5
#define OSSA_BAD_LOGIN      0x6

#define OSSA_STATE_ENABLE   1<<0
#define OSSA_STATE_ACTIVE   1<<1
#define OSSA_STATE_HIDDEN   1<<2
#define OSSA_STATE_RESOVL   1<<3
#define OSSA_STATE_AUTHED   1<<4

#define OSSA_UPDATE_UNRESOLVED_SENT 1<<0
#define OSSA_UPDATE_KICK_OR_BAN     1<<1
#define OSSA_UPDATE_NEW_MESSAGE     1<<2
#define OSSA_UPDATE_UNRESOLVED_EDIT 1<<3
#define OSSA_UPDATE_EDITED_MESSAGE  1<<4

#define OSSA_AUTHTYPE_LOGIN    0x0
#define OSSA_AUTHTYPE_OAUTH    0x1



#endif