#ifndef CHATCLIENT_H
#define CHATCLIENT_H

#include <QObject>
#include <QTcpSocket>

using namespace std;

class ChatClient : public QObject
{
    Q_OBJECT
public:
    explicit ChatClient(QObject *parent = nullptr);
    void connectToHost();
    void sendMessage(QString text);
    int getId();
    QString getNickname();
    QColor getBubbleColor();

private:
    QTcpSocket* socket;
    int id;
    QString nickname;
    // QColor bubbleColor;

private slots: // Use slots when: A button triggers your function, The socket triggers your function; You want something to happen when an event occurs
    void connected();
    void handleServerMessage();

signals: // Use signals when: You want to tell the UI something happened; You want to tell another object something happened
    void connectedToServer();
    void messageReceived(QString);
    // void showClients();
};

#endif // CHATCLIENT_H
