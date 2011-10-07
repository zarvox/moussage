#ifndef __MOUSESOCKET_H__
#define __MOUSESOCKET_H__

#include <QObject>

class QTcpSocket;

class MouseSocket : public QObject {
	Q_OBJECT
public:
	MouseSocket(QObject* parent = 0);
	~MouseSocket();
public slots:
	void connected();
	void haveData();
	void socketError();
signals:
	void imageUpdate(qint64 mouseId, QByteArray image);
private:
	QTcpSocket* sock;
};

#endif