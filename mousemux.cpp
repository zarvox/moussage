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
	serial_enabled = sp->open();
}

MouseMux::~MouseMux() {
}

static int region(QByteArray data, int rowbegin, int colbegin, int rowend, int colend) {
	// Note: end values are INCLUSIVE unlike most Qt classes
	int tally = 0;
	int pixels_used = 0;
	for(int row = rowbegin; row <= rowend; row++) {
		for(int col = colbegin ; col <= colend; col++) {
			tally += data[15 * row + col];
			pixels_used++;
		}
	}
	return (tally / pixels_used);
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
		QByteArray serialdata;
		serialdata.resize(12);
		// do something to map from image to LED brightness
		// Zones are numbered: 
		//       0 6
		//       1 7
		//       2 8
		//       3 9
		//   5   4 10  11		
		serialdata[0] = (char)region(data, 0, 3, 2, 7);
		serialdata[1] = (char)region(data, 3, 3, 5, 7);
		serialdata[2] = (char)region(data, 5, 3, 7, 7);
		serialdata[3] = (char)region(data, 7, 3, 9, 7);
		serialdata[4] = (char)region(data, 10, 3, 12, 7);
		serialdata[5] = (char)region(data, 10, 0, 12, 3);
		serialdata[6] = (char)region(data, 0, 7, 2, 11);
		serialdata[7] = (char)region(data, 3, 7, 5, 11);
		serialdata[8] = (char)region(data, 5, 7, 7, 11);
		serialdata[9] = (char)region(data, 7, 7, 9, 11);
		serialdata[10] = (char)region(data, 10, 7, 12, 11);
		serialdata[11] = (char)region(data, 10, 11, 12, 14);
		
		int written = sp->write(serialdata);
		qDebug() << written << "byte written";
		//qDebug() << sp->write("Test\n");
	}
}
