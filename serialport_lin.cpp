#include "serialport.h"
#include <QDebug>
#include <QString>
#include <QByteArray>
#include <QApplication>
#include <QStringList>

#include <termios.h>
#include <fcntl.h>
#include <unistd.h>

#ifdef __linux__
#include <sys/signal.h>
#else // __APPLE__
#include <signal.h>
#endif

#include <sys/types.h>

SerialPort::SerialPort(QObject* parent) : QObject(parent) {
	opened = false;
	serial_handle = 0;
}

SerialPort::~SerialPort() {
	if (opened) {
		close();
	}
}

void signal_handler_IO(int status) {
	Q_UNUSED(status)
	// Do nothing.  We basically ignore SIGIO. :/
}

bool SerialPort::open() {
	qDebug() << "SerialPort::open() called";
	
#ifdef __linux__
	path = QString("/dev/ttyUSB0");
#endif

#ifdef APPLE
	path = QString("/dev/ttyUSB0");
#endif

	QStringList args = QApplication::arguments();
	for(int i = 0 ; i < args.size() ; i++) {
		if (args[i] == "--serial") {
			i++;
			if (i < args.size()) {
				// We expect the full path to the device node here.
				path = args[i];
			}
		}
	}
	qDebug() << "Using serial port" << path;



	/* try to open the serial port */
	struct termios oldtio, newtio;
	
	// Set up stuff so we ignore the SIGIO.
	struct sigaction saio;
	saio.sa_handler = signal_handler_IO;
	sigemptyset(&saio.sa_mask);
	saio.sa_flags = 0;
#ifdef __linux__
	saio.sa_restorer = NULL;
#endif
	sigaction(SIGIO, &saio, NULL);

	serial_handle = ::open(path.toLatin1(), O_RDWR | O_NOCTTY | O_NONBLOCK);
	if(serial_handle <= 0) {
		qDebug() << "Failed to open" << path;
		return false;
	}
	fcntl(serial_handle, F_SETFL, FASYNC);
	tcgetattr(serial_handle, &oldtio); // save current port settings.
	// set new port settings for canonical input processing.
	newtio.c_cflag = B115200 | CS8 ; // 8N1, no hardware flow control, no DTR/DTS or DCD
	newtio.c_iflag = IGNPAR;
	newtio.c_oflag = 0;
	newtio.c_lflag = 0;       //ICANON;
	newtio.c_cc[VMIN]=1;
	newtio.c_cc[VTIME]=0;
	tcflush(serial_handle, TCIFLUSH);
	tcsetattr(serial_handle,TCSANOW,&newtio);
	opened = true;
	qDebug() << "Serial port" << path << "opened successfully.";
	return true;
}

int SerialPort::write(QByteArray data) {
	int byteswritten = ::write(serial_handle, data.data(), data.length());
	return byteswritten;
}

void SerialPort::close() {
	if (opened) {
		tcflush(serial_handle, TCIFLUSH);
		tcsetattr(serial_handle, TCSANOW, &oldtio);
		::close(serial_handle);
		opened = false;
		qDebug() << "Closed" << path;
	}
}

QByteArray SerialPort::read() {
	// Unimplemented.
	QByteArray retval;
	return retval;
}
