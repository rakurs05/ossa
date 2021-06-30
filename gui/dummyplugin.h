#ifndef DUMMYPLUGIN_H
#define DUMMYPLUGIN_H

#include <iostream>
#include <chrono>
#include "datamodels.h"
#include "plugin.h"
#include <QString>
using namespace std;


class DummyPlugin : public Plugin {
public:
    QList<Chat> getChatList() override;
    QList<Message> getMessages(string chatId) override;
//    ~DummyPlugin() = 0;
};

#endif // DUMMYPLUGIN_H
