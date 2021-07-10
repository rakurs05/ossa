#pragma once
#ifndef CORE_H
#include "./cssa.mutual.c"
#endif

#define clicom(name) int name(ossalist(ossastr) args)
#define clicom_t int(*)(ossalist(ossastr))
#define clicomreg(name) 

clicom(lschat){
    char flags, *plugname;
    for(unsigned int i = 0; i < listLen(&args); i++){
        if(!strcmp((char*)listGet(&args, i), "-plugin")){
            flags |= 1 << 0;
        }else if(!strcmp((char*)listGet(&args, i), "-cid")){
            flags |= 1 << 1;
        }else if(!strcmp((char*)listGet(&args, i), "-only-plugin")){
            flags |= 1 << 2;
            plugname = (char*)listGet(&args, ++i);
        }else if(!strcmp((char*)listGet(&args, i), "-mem")){
            flags |= 1 << 3;
        }else{

        }
    }
    { /* title */
        if(flags & (1<<1)){
            printf("CID\t");
        }
        printf("Title\t");
        if(flags & (1<<0)){
            printf("plugin\t");
        }
        if(flags & (1  << 3)){
            printf("Memory");
        }
        printf("\n");
    }
    for(unsigned int i = 0; i < listLen(&chats); i++){
        if(flags & (1 << 2)){
            if(strcmp(plugname, ((struct ossaChat*)(listGet(&chats, i)))->plugin->name)){
                continue;
            }
        }
        if(flags & (1<<1)){
            printf("#%i\t", i);
        }
        printf("%s\t", ((struct ossaChat*)(listGet(&chats, i)))->title);
        if(flags & (1<<0)){
            printf("%s\t", ((struct ossaChat*)(listGet(&chats, i)))->plugin->name);
        }
        if(flags & (1  << 3)){
            printf("%p\t", (listGet(&chats, i)));
        }
        printf("\n");
    }
    printf("Total: %i\n", listLen(&chats));
}