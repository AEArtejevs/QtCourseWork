#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "../QtChat/chatclient.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    ChatClient* client;

private slots:
    void onConnectedToServer();
    void onMessageReceived(int senderId, QString nickname, QString text, QColor color);
    void onSendClicked();
    void onNewClientJoined(int id, QString nickname, QColor color);
    void addClientToList(int id, QString nickname, QColor color);

private:
    void addMessageBubble(QString text, QColor color);
};

#endif
