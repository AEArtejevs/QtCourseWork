#include "chatserver.h"
#include <QDebug>

ChatServer::ChatServer(QObject *parent)
    : QTcpServer(parent)
{}

void ChatServer::startServer()
{
    int port = 12345;

    if (listen(QHostAddress::Any, port)) qDebug() << "Server started on" << port;
    else qDebug() << "Server failed to start";
}

void ChatServer::incomingConnection(qintptr socketDescriptor)
{
    QTcpSocket *client = new QTcpSocket(this);
    client->setSocketDescriptor(socketDescriptor);
    clients.append(client);

    connect(client, &QTcpSocket::readyRead, this, &ChatServer::handleClientMessage);
    connect(client, &QTcpSocket::disconnected, this, &ChatServer::onClientDisconnected);

    qDebug() << "Client connected:" << socketDescriptor;
}

void ChatServer::handleClientMessage()
{
    QTcpSocket *client = qobject_cast<QTcpSocket*>(sender());
    if (!client) return;

    // read only one line
    QString msg = QString::fromUtf8(client->readLine()).trimmed();
    qDebug() << "Received:" << msg;

    if (msg.startsWith("/ID|")) {

        QStringList p = msg.split("|");
        if (p.size() < 4) return;

        int id = p[1].toInt();
        QString nick = p[2];
        QColor color(p[3]);

        clientInfos[client] = { id, nick, color };

        // Send existing clients data to new one
        for (QTcpSocket *other : clients) {
            if (!clientInfos.contains(other)) continue;

            auto info = clientInfos[other];
            QString data = "/New|" + QString::number(info.id) + "|" + info.nickname + "|" + info.color.name();

            client->write(data.toUtf8() + "\n");
        }

        // new clients data is sent to other clients
        QString data = "/New|" + QString::number(id) + "|" + nick + "|" + color.name();
        for (QTcpSocket *other : clients) {
            if (other != client)
                other->write(data.toUtf8() + "\n");
        }

        return;
    }

    // Chat message
    if (msg.startsWith("MSG|")) {

        QStringList p = msg.split("|");
        if (p.size() < 4) return;

        for (QTcpSocket *other : clients) {
            if (other != client)
                other->write(msg.toUtf8() + "\n");  // appends newline
        }
        return;
    }

}

void ChatServer::onClientDisconnected()
{
    QTcpSocket *client = qobject_cast<QTcpSocket*>(sender());
    if (!client) return;

    clients.removeAll(client);
    clientInfos.remove(client);

    client->deleteLater();
    qDebug() << "Client disconnected";
}
