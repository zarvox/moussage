#include "mousemux.h"
#include "mousewidget.h"
#include <QDebug>
#include <QPushButton>
#include <QApplication>
#include "serialport.h"

MouseMux::MouseMux(QObject* parent) : QObject(parent) {
	pb = new QPushButton("Quit");
	pb->show();
	QObject::connect(pb, SIGNAL(clicked()),
				qApp, SLOT(quit()));
	
	// Init the serial port we'll talk to the Arduino over.
	sp = new SerialPort();
	serial_enabled = sp->open(3);
}

MouseMux::~MouseMux() {
}

void MouseMux::imageUpdate(qint64 id, QByteArray data) {
	if (pb) {
		pb->hide();
		pb->deleteLater();
		pb = NULL;
	}
	if (! widgetMap.contains(id)) {
		qDebug() << "Adding widget for mouse id" << id;
		widgetMap.insert(id, new MouseWidget());
		widgetMap[id]->resize(15 * 16, 13 * 16);
		widgetMap[id]->show();
	}
	widgetMap[id]->updateImage(data);
	if (serial_enabled) {
		qDebug() << "Writing to serial";
		qDebug() << sp->write("Test\n");
	}
}
