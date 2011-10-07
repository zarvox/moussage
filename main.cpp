#include <QApplication>
#include "mousesocket.h"
#include "mousemux.h"

int main(int argc, char** argv) {
	QApplication* app = new QApplication(argc, argv);
	MouseSocket* ms = new MouseSocket();
	MouseMux* mm = new MouseMux();
	QObject::connect(ms, SIGNAL(imageUpdate(qint64,QByteArray)),
		mm, SLOT(imageUpdate(qint64,QByteArray)));
	return app->exec();
}