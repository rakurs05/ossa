#ifndef DATAMODELS_H
#define DATAMODELS_H

#include <QList>
#include <string>
#include <iostream>
using namespace std;




struct User {
    string nickName;
    unsigned int userId;
    struct Metadata {
        struct Avatar {
            QByteArray avatar;
            bool avaible;
        };
        struct Online {
            time_t date;
            bool isOnline;
            bool dateAccessible;
            bool avaible;
        };
        Online online;
        Avatar avatar;
        string otherMeta;
    };
    Metadata metadata;
};


struct Message {
    string body, attach;
    unsigned int userId;
};


struct Chat {
    string title;
    string chatId;
    struct UserList {
        unsigned short count;
        QList<User> list;

        bool avaible;
    };
    struct Avatar {
        QByteArray bytes;

        bool avaible;
    };
    struct LatestMessage {
        string value;
        bool avaible;
    };

    LatestMessage latestMessage;
    UserList userList;
    Avatar avatar;
    string otherMeta;
};

struct Content {
    string text;
    QByteArray bytes;
};


struct Attachment {
    int type;
    Content content;
    User owner;
};




#endif // DATAMODELS_H
/*
makeChat (title, {plugin-ptr, str{) -> str (JSON)
setChatSettings(str propName, str propValue) -> void
inviteToChat(Chat, string global_UID) -> void
delteUser(Chat, uint local_UID, string reason) -> void
sendMessage(Chat, Message) -> void
makeMessage(Chat, str body, list <attachment> attach) -> Message
editMessage(Chat, uint MessageID, Message) -> void
chatAction(Chat, str action_name) -> void
chatCheckEvent(Chat) -> bool (Is have event?)
ExportChat(Chat, str location_or_host) -> void
ImportChat(str location_or_host) -> Chat
*/
