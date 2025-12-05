#ifndef CHATSERVER_H
#define CHATSERVER_H

#include <QTcpServer>
#include <QTcpSocket>
#include <QColor>
#include <QString>
#include <QList>

struct ServerClientInfo {
    int id;
    QString nickname;
    QColor color;
};
class ChatServer : public QTcpServer
{
    Q_OBJECT

public:
    explicit ChatServer(QObject *parent = nullptr);
    void startServer(); // starts tcp server

protected:
    void incomingConnection(qintptr socketDescriptor) override;

private:
    QList<QTcpSocket*> clients;
    QHash<QTcpSocket*, ServerClientInfo> clientInfos;

private slots:
    void handleClientMessage();
    void onClientDisconnected();

};

#endif // CHATSERVER_H
