#include "dummyplugin.h"


QList<Chat> DummyPlugin::getChatList() {
    QList<Chat> chatList;
    for(int i = 0; i < 10; i++) {
        Chat chat;
        chat.title = "test char #" + std::to_string(i);
        chat.chatId = std::to_string(i);
        chat.latestMessage.avaible = true;
        chat.latestMessage.value = "latest message, ok";
        if(i % 2 == 0) {
            QList<User> userList;
            for(int j=0; j <= i; j++) {
                User user;
                user.nickName = "test user #" + std::to_string(i);
                user.userId = i;
                user.metadata.online.avaible = true;
                user.metadata.online.dateAccessible = true;
                user.metadata.online.isOnline = true;
                user.metadata.online.date = chrono::system_clock::to_time_t(
                            chrono::system_clock::now());
                userList.append(user);
            }
            chat.userList.avaible = true;
            chat.userList.list = userList;
            chat.userList.count = static_cast<unsigned short>(userList.length());
            chatList.append(chat);
        }
    }
    return chatList;
}

QList<Message> DummyPlugin::getMessages(string chatId) {
    QList<Message> msgList;
    for(int i=0; i < (10 * stoi(chatId)); i++) {
        Message message;
        if(i % 2 == 0) {
            message.userId = 0;
            message.attach = "";
        } else {
            message.userId = 1;
            message.attach = "test attachment";
        }
        message.body = "test message, #" + to_string(i);
        msgList.append(message);
    }
    return msgList;
}
