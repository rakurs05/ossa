#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "chatwidget.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    delete ui->chatList->widget(0);
    ui->chatList->removeItem(0);

    delete ui->chatList->widget(1);
    ui->chatList->removeItem(1);

    Core core;
    for(Plugin *plugin: core.getPluginList()) {
        QWidget *container = new QWidget();
        ChatWidget *chatWidget = new ChatWidget(container, plugin);

        QVBoxLayout *layout = new QVBoxLayout(container);
        layout->addWidget(chatWidget);
        container->setLayout(layout);
        ui->chatList->addItem(container, "DummyPlugin");
    }
//    ui->chatList->addItem(new ChatWidget(self, ), "Dummy Plugin");
}

MainWindow::~MainWindow()
{
    delete ui;
}
