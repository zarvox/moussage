#ifndef __MOUSEMUX_H__
#define __MOUSEMUX_H__

#include <QObject>
#include <QByteArray>
#include <QMap>

class MouseWidget;
class QPushButton;
class SerialPort;

class MouseMux : public QObject {
	Q_OBJECT
public:
	MouseMux(QObject* parent = 0);
	~MouseMux();
public slots:
	void imageUpdate(qint64 id, QByteArray data);
private:
	QMap<qint64, MouseWidget*> widgetMap;
	QPushButton* pb;
	SerialPort* sp;
	bool serial_enabled;
};

#endif // __MOUSEMUX_H__

