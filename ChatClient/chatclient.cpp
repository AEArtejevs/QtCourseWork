// #include "../QtChat/chatclient.h"
#include "chatclient.h"

#include <QObject>
#include <QHostAddress>
#include <QDebug>

ChatClient::ChatClient(QObject *parent)
    : QObject{parent}
{
    socket=new QTcpSocket(this);
    connect(socket, &QTcpSocket::connected, this, &ChatClient::connected);
    connect(socket, &QTcpSocket::readyRead, this, &ChatClient::handleServerMessage);
}

void ChatClient::connectToHost() // connects client to server
{
    int port=12345;
    QHostAddress address(QHostAddress::LocalHost);
    socket->connectToHost(address, port);

}

void ChatClient::connected(){
    qDebug() << "client connected to server";
}

void ChatClient::handleServerMessage(){ // recieves messages from server

    QByteArray data = socket->readAll();
    qDebug() << "Received:" << data;

}

void ChatClient::sendMessage(QString text){ // Sends messages to server

    QByteArray data;
    data = text.toUtf8(); // Convert the QString to a QByteArray using UTF-8 encoding
    socket->write(data);

}

