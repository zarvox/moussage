#ifndef __MOUSEWIDGET_H__
#define __MOUSEWIDGET_H__

#include <QWidget>
#include <QByteArray>

class QPaintEvent;
class QMouseEvent;

class MouseWidget : public QWidget {
	Q_OBJECT
public:
	MouseWidget(QWidget* parent = 0);
	~MouseWidget();
public slots:
	void updateImage(QByteArray data);
	void paintEvent(QPaintEvent* e);
signals:
	void clicked();
private:
	void mouseReleaseEvent(QMouseEvent* e);
	QByteArray lastImage;
};

#endif // __MOUSEWIDGET_H__