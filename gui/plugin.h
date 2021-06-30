#ifndef PLUGIN_H
#define PLUGIN_H
#include <QList>
#include "datamodels.h"

class Plugin {
public:
    virtual QList<Chat> getChatList() = 0;
    virtual QList<Message> getMessages(string chatId) = 0;
//    virtual ~Plugin() {}
};


#endif // PLUGIN_H
