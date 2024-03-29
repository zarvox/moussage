#include "mousesocket.h"

#include <QTcpSocket>
#include <QtEndian>
#include <QApplication>
#include <QStringList>
#include <QString>
#include <QDebug>

MouseSocket::MouseSocket(QObject* parent) : QObject(parent) {
	sock = new QTcpSocket();
	QString host = "127.0.0.1";
	QStringList args = QApplication::arguments();
	for(int i = 0 ; i < args.size() ; i++) {
		if (args[i] == "--server") {
			i++;
			if (i < args.size()) {
				host = args[i];
				qDebug() << "Using server" << host;
			}
		}
	}
	sock->connectToHost(host, 33333);
	QObject::connect(sock, SIGNAL(readyRead()),
		this, SLOT(haveData()));
	QObject::connect(sock, SIGNAL(connected()),
		this, SLOT(connected()));
}

MouseSocket::~MouseSocket() {

}

void MouseSocket::socketError() {
	qDebug() << "Socket error occured, aborting!";
	qApp->exit();
}

void MouseSocket::connected() {
	qDebug() << "Connected!";
}

void MouseSocket::haveData() {
	//qDebug() << "bytes available:" << sock->bytesAvailable();
	while (sock->bytesAvailable() >= 203) {
		qint64 id_data;
		QByteArray data;
		sock->read((char*)(&id_data), 8);
		id_data = qFromLittleEndian(id_data);
		data = sock->read(195);
		emit imageUpdate(id_data, data);
		//qDebug() << id_data << ":" <<  int(data[191]) <<  int(data[192]) <<  int(data[193]) <<  int(data[194]);
	}
}
