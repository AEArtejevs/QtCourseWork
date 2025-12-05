#include "chatclient.h"
#include <QHostAddress>
#include <QRandomGenerator>
#include <QDebug>

// Generators
static int genRandomId() {
    return QRandomGenerator::global()->bounded(10000, 99999);
}
static QString genRandomNickname() {
    QStringList adjectives = { "Red", "Blue", "Fast", "Silent", "Wild", "Lazy" };
    QStringList animals = { "Fox", "Dog", "Wolf", "Bear", "Cat", "Hawk" };

    return adjectives[QRandomGenerator::global()->bounded(adjectives.size())] + animals[QRandomGenerator::global()->bounded(animals.size())];
}
static QColor genRandomColor() {
    QList<QColor> colors = {
        QColor("#D94545"), // soft red
        QColor("#3F70D3"), // medium blue
        QColor("#46A06D"), // medium green
        QColor("#A25AD6"), // soft purple
        QColor("#E28A33"), // warm orange
        QColor("#D9549E"), // rose
        QColor("#3BAFBF"), // teal
        QColor("#B16F3D"), // bronze
        QColor("#6270C2"), // lavender blue
        QColor("#5A8A8A")  // grey-teal
    };
    return colors[QRandomGenerator::global()->bounded(colors.size())];
}

// ChatClient

ChatClient::ChatClient(QObject *parent)
    : QObject(parent),
    socket(new QTcpSocket(this)),

    id(genRandomId()),
    nickname(genRandomNickname()),
    bubbleColor(genRandomColor())
{
    connect(socket, &QTcpSocket::connected, this, &ChatClient::connected);
    // connect(socket, &QTcpSocket::disconnected, this, &ChatClient::disconnected);
    connect(socket, &QTcpSocket::readyRead, this, &ChatClient::handleServerMessage);
}

void ChatClient::connectToHost()
{
    socket->connectToHost(QHostAddress::LocalHost, 12345);
}

void ChatClient::connected()
{
    QString identity = "/ID|" + QString::number(id) + "|" + nickname + "|" + bubbleColor.name();
    socket->write(identity.toUtf8() + "\n");
    emit connectedToServer();
}

void ChatClient::handleServerMessage()
{
    while (socket->canReadLine()) {
        QString msg = QString::fromUtf8(socket->readLine()).trimmed();

        // New client
        if (msg.startsWith("/New|")) {
            QStringList p = msg.split("|");
            if (p.size() < 4) continue;

            int senderId = p[1].toInt();
            QString name = p[2];
            QColor color(p[3]);
            knownClients[senderId] = { name, color };

            emit newClientJoined(senderId, name, color);
            continue;
        }

        // Chat message
        if (msg.startsWith("MSG|")) {
            QStringList p = msg.split("|");
            if (p.size() < 4) continue;

            int senderId = p[1].toInt();
            QString name = p[2];
            QString text = p[3];
            QColor color;

            if (senderId == id) // Message is from me
                color = bubbleColor;
            else
                color = knownClients[senderId].color; // Message is from another client

            emit chatMessageReceived(senderId, name, text, color);
            continue;
        }
        if (msg.startsWith("/LEFT|")) {
            QStringList p = msg.split("|");
            if (p.size() < 2) continue;

            int id = p[1].toInt();

            knownClients.remove(id); // remove from memory
            emit clientLeft(id); // update GUI

            continue;
        }
    }
}

void ChatClient::sendMessage(const QString &text)
{
    QString msg = "MSG|" + QString::number(id) + "|" + nickname + "|" + text;
    socket->write(msg.toUtf8() + "\n");
}


// void ChatClient::disconnected(){
//     QString msg = "/LEFT|" + QString::number(id);
//     socket->write(msg.toUtf8() + "\n");
// }

void ChatClient::sendDisconnectNotice()
{
    if (socket->state() == QAbstractSocket::ConnectedState) {
        QString msg = "/LEFT|" + QString::number(id);
        socket->write(msg.toUtf8() + "\n");
        socket->flush();
        socket->waitForBytesWritten(50);
    }
}

QColor ChatClient::getBubbleColor() const{ return bubbleColor; }
int ChatClient::getId() const { return id; }
QString ChatClient::getNickname() const { return nickname; }
