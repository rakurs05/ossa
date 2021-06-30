#ifndef CHATWIDGET_H
#define CHATWIDGET_H

#include <QScrollArea>
#include <QVBoxLayout>
#include "datamodels.h"
#include "dummyplugin.h"
#include "core.h"
#include <QLabel>
class ChatWidget : public QScrollArea
{
    Q_OBJECT
public:
    explicit ChatWidget(QWidget *parent, Plugin *plugin);

private:
    Core *core;
};

class ChatItemWidget : public QWidget {
public:
    ChatItemWidget(Chat *chat);
};

#endif // CHATWIDGET_H
