#ifndef __MOUSEWIDGET_H__
#define __MOUSEWIDGET_H__

#include <QWidget>
#include <QByteArray>

class QPaintEvent;

class MouseWidget : public QWidget {
	Q_OBJECT
public:
	MouseWidget(QWidget* parent = 0);
	~MouseWidget();
public slots:
	void updateImage(QByteArray data);
	void paintEvent(QPaintEvent* e);
private:
	QByteArray lastImage;
};

#endif // __MOUSEWIDGET_H__