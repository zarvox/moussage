#ifndef __MOUSEMUX_H__
#define __MOUSEMUX_H__

#include <QObject>
#include <QByteArray>
#include <QMap>
#include <QList>
#include <QPair>
#include <QDateTime>

class MouseWidget;
class QPushButton;
class SerialPort;
class QTimer;

class MouseMux : public QObject {
	Q_OBJECT
public:
	MouseMux(QObject* parent = 0);
	~MouseMux();
public slots:
	void imageUpdate(qint64 id, QByteArray data);
	void timerTick();
	void reset();
private:
	void expireOlderThan(int msecs);
	void writeHistoryAverage();


	QMap<qint64, MouseWidget*> widgetMap;
	QPushButton* pb;
	SerialPort* sp;
	bool serial_enabled;
	QList<QPair<QDateTime, QByteArray> >history;
	QTimer* timer;
	QByteArray lastsent;
};

#endif // __MOUSEMUX_H__

