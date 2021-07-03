#ifndef OSSA_MUSTHAVE_TYPES
#define OSSA_MUSTHAVE_TYPES
#ifndef ossalist
 #include "./list.h"
 #define ossalist(type) struct __list
#endif
#ifndef ossastr
    #define ossastr char *
#endif
#ifndef ossaMID
    #define ossaMID unsigned long
#endif
#ifndef ossaUID
    #define ossaUID unsigned long
#endif
#ifndef ossaCID
    #define ossaCID unsigned long
#endif
struct _Message{
    ossastr body;
    ossalist(ossastr) attach;
    ossaUID uid;
};
struct _User{
    ossastr nickname;
    ossastr head;
};
#define ossaMessage struct _Message
#define ossaUser struct _User
#endif