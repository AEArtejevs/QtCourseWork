#ifndef CHATCLIENT_H
#define CHATCLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <QColor>
#include <QMap>

struct ClientInfo {
    QString nickname;
    QColor color;
};

class ChatClient : public QObject
{
    Q_OBJECT

public:
    explicit ChatClient(QObject *parent = nullptr);

    void connectToHost();
    void sendMessage(const QString &text);

    int getId() const; // const cause getter most of the time should be const
    QString getNickname() const;
    QColor getBubbleColor() const;

private:
    QTcpSocket* socket;
    int id;
    QString nickname;
    QColor bubbleColor;
    QMap<int, ClientInfo> knownClients;

private slots: // Use slots when: A button triggers your function, The socket triggers your function; You want something to happen when an event occurs
    void connected();
    void handleServerMessage();

signals: // Use signals when: You want to tell the UI something happened; You want to tell another object something happened
    void connectedToServer();
    void chatMessageReceived(int senderId, const QString &nickname, const QString &text, const QColor &color);
    void newClientJoined(int id, const QString &nickname, const QColor &color);
};

#endif // CHATCLIENT_H
