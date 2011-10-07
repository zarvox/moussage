#ifndef __SERIALPORT_H__
#define __SERIALPORT_H__

#include <QObject>
#ifdef __WIN32
#include <windows.h>
#endif
#ifdef __APPLE__
#include <termios.h>
#endif
#ifdef __linux__
#include <termios.h>
#endif


class SerialPort : public QObject {
	Q_OBJECT
public:
	SerialPort(QObject* parent = NULL);
	~SerialPort();
	bool open();
	void close();
public slots:
	int write(QByteArray data);
	QByteArray read(); // This is a blocking read.  It's not very useful.
private:


#ifdef __WIN32
	HANDLE hSerial;
#endif

#ifdef __APPLE__
	int serial_handle;
	struct termios oldtio;
	struct termios newtio;
#endif

#ifdef __linux__
	int serial_handle;
	struct termios oldtio;
	struct termios newtio;
#endif

	QString path;
	bool opened;
};

#endif // __SERIALPORT_H__
