#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QLabel>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    client = new ChatClient(this);
    client->connectToHost();

    connect(client, &ChatClient::connectedToServer, this, &MainWindow::onConnectedToServer);
    connect(client, &ChatClient::chatMessageReceived, this, &MainWindow::onMessageReceived);
    connect(client, &ChatClient::newClientJoined, this, &MainWindow::onNewClientJoined);
    connect(client, &ChatClient::clientLeft, this, &MainWindow::removeClientFromList);
    connect(ui->sendButton, &QPushButton::clicked, this, &MainWindow::onSendClicked);

    ui->splitter->setSizes({200, 600});
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onConnectedToServer()
{
    addClientToList(client->getId(), client->getNickname(), client->getBubbleColor());
    addMessageBubble("Connected.", QColor("#888"));
}

void MainWindow::onMessageReceived(int senderId, QString nickname, QString text, QColor color)
{
    addMessageBubble(nickname + ": " + text, color);
}

void MainWindow::onSendClicked()
{
    QString text = ui->inputBox->text();
    if (text.isEmpty()) return;

    addMessageBubble("Me: " + text, client->getBubbleColor());
    client->sendMessage(text);
    ui->inputBox->clear();
}

void MainWindow::addMessageBubble(QString text, QColor color)
{
    QLabel* label = new QLabel(text);
    label->setWordWrap(true);
    label->setStyleSheet("background:" + color.name() + ";"
        "color:white;"
        "padding:6px;"
        "border-radius:6px;"
    );
    ui->messagesLayout->addWidget(label);
}
void MainWindow::addClientToList(int id, QString nickname, QColor color)
{
    QListWidgetItem* item = new QListWidgetItem();
    item->setText(nickname + " (" + QString::number(id) + ")");
    item->setBackground(color);
    ui->contactList->addItem(item);
}
void MainWindow::removeClientFromList(int id)
{
    for (int i = 0; i < ui->contactList->count(); i++) {
        QListWidgetItem *item = ui->contactList->item(i);
        if (item->text().contains(QString::number(id))) {
            delete ui->contactList->takeItem(i);
            break;
        }
    }
}
void MainWindow::closeEvent(QCloseEvent *event)
{
    client->sendDisconnectNotice();
    QMainWindow::closeEvent(event);
}
void MainWindow::onNewClientJoined(int id, QString nickname, QColor color)
{
    qDebug() << "NEW CLIENT:" << id << nickname << color;
    if (id == client->getId())
        return;
    addClientToList(id, nickname, color);
}
