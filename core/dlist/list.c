#include "./list.h"

char states[16] = "staest point";

struct __list makeEmptyList(){
    struct __list me = {states,0x0};
    return me;
}

char comparePointers(const void *a, void *b){
    return (a == b);
}
char compareInts(const void *a, void *b){
    return (*((int*)a) == *((int*)b));
}
char compareFloats(const void *a, void *b){
    return (*((int*)a) == *((int*)b));
}

void *listResolve(struct __list *_this, unsigned int size){
    if(_this == 0x0)
        return CLIST_ERROR_NULL_ARG;
    if(_this->data == states){
        struct __list *_new = _this;
        _new->data = malloc(size);
        _new->next = 0x0;
        return _new->data;
    }
    if(_this->next != 0x0) return listResolve(_this->next, size);
    else{
        _this->next = (struct __list *)(malloc(sizeof(struct __list)));
        struct __list *_new = _this->next;
        _new->data = malloc(size);
        _new->next = 0x0;
        return _new->data;
    }
}

int listAppend(struct __list *_this, void *data, unsigned int size){
    if(_this == 0x0)
        return -1;
    if(_this->data == states){
        struct __list *_new = _this;
        _new->data = malloc(size);
        memcpy(_new->data, data, size);
        _new->next = 0x0;
        return 1;
    }
    if(_this->next != 0x0) return listAppend(_this->next, data, size)+1;
    else{
        _this->next = (struct __list *)(malloc(sizeof(struct __list)));
        struct __list *_new = _this->next;
        _new->data = malloc(size);
        memcpy(_new->data, data, size);
        _new->next = 0x0;
        return 1;
    }
}
int listAppendLink(struct __list *_this, void *data){
    if(_this == 0x0)
        return -1;
    if(data == states){
        struct __list *_new = _this;
        _new->data = data;
        _new->next = 0x0;
        return 1;
    }
    if(_this->next != 0x0) return listAppendLink(_this->next, data)+1;
    else{
        _this->next = (struct __list *)(malloc(sizeof(struct __list)));
        struct __list *_new = _this->next;
        _new->data = data;
        _new->next = 0x0;
        return 1;
    }
}
void *listGet(struct __list *_this, int index){
    if(_this == 0x0)
        return CLIST_ERROR_NULL_ARG;
    else{
        if(index == 0)
            return _this->data;
        else if(_this->next != 0x0)
            return listGet(_this->next, index-1);
        else return CLIST_ERROR_INDEX_OUT_OF_BOUND;
    }
}
int listLen(struct __list *_this){
    if(_this->next == 0x0) return 1;
    else return listLen(_this->next)+1;
}

struct __list_booster makeEmptyBooster(unsigned int pointRate){
    struct __list_booster me;
    me.pointsCount = pointRate;
    me.points = news(struct __list*, pointRate);
    for(int i = 0; i < pointRate; i++)
        me.points[i] = 0x0;
    me.len = 0;
    me.end = 0x0;
    me.compare = 0x0;
    return me;
}

struct __booster_info __booster_len(struct __list_booster *_this){
    struct __booster_info me;
    me.error = -1;
    me.index = _this->len;
    me.target = _this->end;
    return me;
}

struct __booster_info __booster_append(struct __list_booster *_this, int size){
    struct __booster_info me;
    if(_this->end == 0x0){ /* if end is undefined, define it */
        _this->points[0] = new(struct __list);
        *_this->points[0] = (struct __list){0x0,0x0};
        for(int i = 1; i < _this->pointsCount; i++){
            _this->points[i] = _this->points[0];
        }
        _this->end = _this->points[0];
        _this->len++;
        _this->points[0]->data = malloc(size);
    
        me.target = _this->points[0]->data;
        me.error = 1;
        me.index = 0;
        return me;
    }
    void *ret = listResolve(_this->end, size);
    _this->len++;
    _this->end = _this->end->next;
    __booster_assamble(_this);
    me.error = 0;
    me.index = _this->len-1;
    me.target = ret;
    return me;
}

struct __booster_info __booster_assamble(struct __list_booster *_this){
    struct __booster_info me;
    me.error = me.index = 0;
    me.target = 0x0;
    int __len = _this->len;
    for(int i = 1; i < _this->pointsCount; i++){
        _this->points[i] = listFrame(_this->points[i-1], __len/_this->pointsCount);
    }
    return me;
}

struct __list *listFrame(struct __list *_this, int index){
    if(_this == 0x0) return 0x0;
    else if(index == 0) return _this;
    else return listFrame(_this->next, index-1);
}

struct __booster_info __booster_get(struct __list_booster *_this, int index){
    struct __booster_info me;
    // unsigned int frame = _this->pointsCount * (index - phi) / _this->len;
    unsigned int frame = 0;
    if(_this->len / _this->pointsCount>=1)
        frame = index / (_this->len / _this->pointsCount);
    else 
        frame = 0;
    // printf("(Debug): frame: %i\n", frame);
    if(_this->len / _this->pointsCount>=1)
        me.target = listGet(_this->points[frame], index % (_this->len / _this->pointsCount));
    else
        me.target = listGet(_this->points[frame], index);
    me.index = index;
    me.error = 0;
    return me;
}
int earaseList(struct __list *_this){
    while (listLen(_this) > 1) {
        listRemove(_this, 0);
    }
    free(_this->next);
    *_this = (struct __list) {0x0, 0x0};
    return 0;
}
int listRemove(struct __list *_this, int index){
    if(index < 0)
        return -2;
    if(index == 0){
        if(_this->next == 0x0){
            if(_this->data != 0x0)
                free(_this->data);
            _this->data = 0x0;
            return -1;
        }
        else{
            if(_this->data != 0x0)
                free(_this->data);
            _this->data = _this->next->data;
            void *next = _this->next->next;
            free(_this->next);
            _this->next = (struct __list*)next;
        }
        return listLen(_this);
    }
    if(index > listLen(_this)){
        return -1;
    }
    struct __list *prev = listFrame(_this, index-1),
    *next = listFrame(_this, index+1);

    free(prev->next->data);
    free(prev->next);
    prev->next = next;
    return listLen(_this);
}
long listFind(struct __list *_this, const void *data, char (*compare)(const void *target, void *object)){
    unsigned int len = listLen(_this);
    for(int i = 0; i < len; i++){
        if(compare(data, listGet(_this, i)))
            return i;
    }
    return -1;
}