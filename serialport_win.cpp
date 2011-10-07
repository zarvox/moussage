#include "serialport.h"
#include <QDebug>
#include <QString>
#include <QByteArray>
#include <QStringList>
#include <QApplication>

SerialPort::SerialPort(QObject* parent) : QObject(parent) {
	opened = false;
}

SerialPort::~SerialPort() {
	if (opened) {
		close();
	}
}

bool SerialPort::open() {
	qDebug() << "SerialPort::open() called";
	if (opened == true) {
		// Can't open an already-open serial port
		return false;
	}
	// Use COM1 by default.
	path = QString("\\\\.\\COM1");
	// Check command line for serial port override
	QStringList args = QApplication::arguments();
	for(int i = 0 ; i < args.size() ; i++) {
		if (args[i] == "--serial") {
			i++;
			if (i < args.size()) {
				path = args[i];
				// We expect either the canonical path "\\.\COM1"
				// or simply "COM1" here
				if (!path.startsWith("\\\\.\\")) {
					path = path.prepend("\\\\.\\");
				}
			}
		}
	}
	qDebug() << "Using serial port" << path;

	hSerial = CreateFile((const WCHAR*)(path.utf16()), GENERIC_READ | GENERIC_WRITE,
			0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (hSerial == INVALID_HANDLE_VALUE) {
		qDebug() << "Failed to open" << path;
		return false;
	}
	DCB dcbSerialParams = {0};
	dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
	if (!GetCommState(hSerial, &dcbSerialParams)) {
		qDebug() << "Failed to retrieve DCB for" << path;
		CloseHandle(hSerial);
		return false;
	}
	dcbSerialParams.BaudRate=CBR_115200;
	dcbSerialParams.ByteSize=8;
	dcbSerialParams.StopBits=ONESTOPBIT;
	dcbSerialParams.Parity=NOPARITY;
	if(!SetCommState(hSerial, &dcbSerialParams)){
		qDebug() << "Failed to set serial port speed for" << path;
		CloseHandle(hSerial);
		return false;
	}
	opened = true;
	qDebug() << "Serial port" << path << "opened successfully.";
	return true;
}

int SerialPort::write(QByteArray data) {
	if (!opened) {
		qDebug() << "SerialPort::write() called, but file not open!";
		return 0;
	}
	DWORD numwritten;
	WriteFile(hSerial, data.data(), data.length(), &numwritten, NULL);
	return int(numwritten);
}

void SerialPort::close() {
	if (opened) {
		CloseHandle(hSerial);
		opened = false;
	}
}


QByteArray SerialPort::read() {
// This is a blocking read.  It's not very useful.
	DWORD maxtoread = 10;
	DWORD numread = 0;
	char buffer[10];
	ReadFile(hSerial, buffer, maxtoread, &numread, NULL);
	QByteArray retval(buffer, numread);
	qDebug() << "Read" << numread << "bytes from serial";
	return retval;
}
