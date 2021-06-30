#include "types.h"
#include <stdlib.h>
#include <string.h>

struct DynamicVariable{
    unsigned int size;
    char type;
    void *data;
};
struct NamedVariable{
    unsigned int size;
    char *name;
    void *data;
};

#define dvar void *
#define nvar void *
#define dvarinf struct DynamicVariable
#define nvarinf struct NamedVariable

void *makeNamed(const char *name, unsigned int size){
    void *data = malloc(strlen(name)+size+sizeof(unsigned int));
    memset(data, 0, size);
    astype(unsigned int)(data) = size;
    strcpy((char*)data+sizeof(unsigned int), name);
    return data;
}
struct NamedVariable descriptNamed(void *named){
    struct NamedVariable n = {*(unsigned int *)(named),\
        (char*)named+sizeof(int),\
        named+sizeof(int)+strlen((char*)(named+sizeof(int)))};
    return n;
}

void *makedyn(char type, void *data, unsigned int size){
    void *me = malloc(size+1);
    *(char*)(me) = type;
    for(unsigned int i = 1; i < size; i++){
        *(char*)(me+i) = *(char*)(data+(i-1));
    }
    return me;
}

struct DynamicVariable encrypt_dynamic(void *variable){
    struct DynamicVariable dv = {astype(unsigned int) (variable),\
            *(char*)(variable+sizeof(unsigned int)),\
            (variable+1+sizeof(unsigned int))};
    return dv;
}

void *lambda_float(float data){
    void *mem = malloc(sizeof(float));
    *(float*)mem = data;
    return makedyn('f', mem, sizeof(float));
}
void *lambda_int(int data){
    void *mem = malloc(sizeof(int));
    *(int*)mem = data;
    return makedyn('i', mem, sizeof(int));
}
void *labmda_long(long data){
    void *mem = malloc(sizeof(long));
    *(long*)mem = data;
    return makedyn('l', mem, sizeof(long));
}
void *lambda_string(const char *data){
    unsigned int size = strlen(data)+1+sizeof(unsigned int);
    void *mem = malloc(size);
    memset(mem, 0, size);
    astype(unsigned int)(mem) = size;
    *(char*)(mem+sizeof(unsigned int)) = 's';
    strcpy((char*)(mem+sizeof(unsigned int)+1),data);
    return mem;
}
void *lambda_named(const char *name, void *data, unsigned int datasize){
    char *str = (char*)malloc(strlen(name)+datasize);
    strcpy(str, name);
    str[strlen(name)] = 0;
    memcpy((str+strlen(name)+1), data, datasize);
    return str;
}
void *named_data(void *named){
    for(int i = 1; 1; i++){
        if(((char*)(named))[i] == 0)
            return (named+(i+1));
    }
}