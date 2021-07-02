#ifndef OSSA_MUSTHAVE_TYPES
#define OSSA_MUSTHAVE_TYPES
#ifndef ossalist
 #include "./dlist/list.h"
 #define ossalist(type) struct __list
#endif
#ifndef ossastr
    #define ossastr char *
#endif
#ifndef ossaUID
    #define ossaUID unsigned long
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
#endif