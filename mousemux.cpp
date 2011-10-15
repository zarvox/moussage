#include "mousemux.h"
#include "mousewidget.h"
#include <QDebug>
#include <QPushButton>
#include <QApplication>
#include "serialport.h"
#include <QTimer>

#define AVG_PERIOD (100)

MouseMux::MouseMux(QObject* parent) : QObject(parent) {
	pb = new QPushButton("Quit");
	pb->show();
	QObject::connect(pb, SIGNAL(clicked()),
				qApp, SLOT(quit()));

	timer = new QTimer(this);
	QObject::connect(timer, SIGNAL(timeout()),
				this, SLOT(timerTick()));
	timer->start(8);
	// Init the serial port we'll talk to the Arduino over.
	sp = new SerialPort();
	serial_enabled = sp->open();
}

MouseMux::~MouseMux() {
}


void MouseMux::timerTick() {
	// Prune history older than AVG_PERIOD msec
	expireOlderThan(AVG_PERIOD);
	// Take average of the (remaining) history, write to serial port
	writeHistoryAverage();
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
		QObject::connect(widgetMap[id], SIGNAL(clicked()),
				this, SLOT(reset()));
	}
	widgetMap[id]->updateImage(data);
	if (serial_enabled) {
		QByteArray motorthisframe;
		motorthisframe.resize(12);
		// do something to map from image to LED brightness
		// Zones are numbered: 
		//       0 6
		//       1 7
		//       2 8
		//       3 9
		//   5   4 10  11		
		motorthisframe[0] = (char)region(data, 0, 3, 2, 7);
		motorthisframe[1] = (char)region(data, 3, 3, 5, 7);
		motorthisframe[2] = (char)region(data, 5, 3, 7, 7);
		motorthisframe[3] = (char)region(data, 7, 3, 9, 7);
		motorthisframe[4] = (char)region(data, 10, 3, 12, 7);
		motorthisframe[5] = (char)region(data, 10, 0, 12, 3);
		motorthisframe[6] = (char)region(data, 0, 7, 2, 11);
		motorthisframe[7] = (char)region(data, 3, 7, 5, 11);
		motorthisframe[8] = (char)region(data, 5, 7, 7, 11);
		motorthisframe[9] = (char)region(data, 7, 7, 9, 11);
		motorthisframe[10] = (char)region(data, 10, 7, 12, 11);
		motorthisframe[11] = (char)region(data, 10, 11, 12, 14);
		QDateTime now = QDateTime::currentDateTime();
		history.append(qMakePair(now, motorthisframe));
	}
}

void MouseMux::expireOlderThan(int msecs) {
	QDateTime now = QDateTime::currentDateTime();
	QList<int> to_remove;
	for (int i = 0 ; i < history.size(); i++) {
		if(history[i].first.msecsTo(now) > msecs)
			to_remove.append(i);
	}
	for (int i = to_remove.size() - 1 ; i >= 0 ; i--) {
		history.removeAt(to_remove[i]);
	}
}

void MouseMux::writeHistoryAverage() {
	QByteArray serialdata;
	serialdata.resize(12);
	for (int motor = 0; motor < 12; motor++ ) {
		int total = 0;
		for(int i = 0; i < history.size() ; i++) {
			total += history[i].second[motor];
		}
		if (history.size() > 0)
			total /= history.size();
		int CLAMP = 127;
		serialdata[motor] = total > CLAMP ? (char)(CLAMP): (char)total;
	}
	if (serialdata != lastsent) {
		lastsent = serialdata;
		qDebug() << "taking average of" << history.size() << "samples";
		serialdata = serialdata.prepend((char)0);
		int written = sp->write(serialdata);
		qDebug() << serialdata.toHex();
		//qDebug() << written << "bytes written";
	}
}

void MouseMux::reset() {
	qDebug() << "resetting power supply";
	QByteArray ba;
	ba.resize(1);
	ba[0] = 1;
	sp->write(ba);
}
