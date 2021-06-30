#include "chatwidget.h"

ChatWidget::ChatWidget(QWidget *parent, Plugin *plugin) : QScrollArea(parent)
{
    QWidget *frame = new QWidget;
    setWidget(frame);
    QVBoxLayout *layout = new QVBoxLayout(frame);
//    frame->setLayout(layout);

    QList<Chat> chatList = plugin->getChatList();

    layout->addWidget(new QLabel("third test label"));
    for(int i=0; i < chatList.length(); i++) {
        ChatItemWidget *item = new ChatItemWidget(&chatList[i]);
        layout->addWidget(item);
    }
}


ChatItemWidget::ChatItemWidget (Chat *chat) {
    QGridLayout layout(this);

    if(chat->avatar.avaible) {
        QLabel *ava = new QLabel(chat->avatar.bytes);
        ava->setMaximumSize(64, 64);
        layout.addWidget(ava, 0, 0, 1, 0);
    }
    QLabel *chatTitle = new QLabel();
    layout.addWidget(chatTitle, 0, 1);
    if(chat->latestMessage.avaible) {
        QLabel *latestMsg = new QLabel;
        layout.addWidget(latestMsg, 1, 1);
    }
}
