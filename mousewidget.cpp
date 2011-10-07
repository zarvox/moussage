#include "mousewidget.h"
#include <QPainter>
#include <QPaintEvent>
#include <QDebug>

MouseWidget::MouseWidget(QWidget* parent) : QWidget(parent) {
	lastImage.resize(195);
}

MouseWidget::~MouseWidget() {

}

void MouseWidget::paintEvent(QPaintEvent* e) {
	Q_UNUSED(e)
	QPainter p(this);
	if (lastImage.size() == 195) {
		for(int row = 0; row < 13; row++) {
			for(int col = 0; col < 15 ; col++) {
				unsigned char value = uchar(lastImage[row*15 + col]);
				QColor c(value, value, value);
				p.setBrush(c);
				p.drawRect(width() * col / 15, height() * row / 13, width() / 15, height() / 13);
			}
		}
	} else {
		qDebug() << "paintEvent, but no image yet";
	}
}

void MouseWidget::updateImage(QByteArray data) {
	lastImage = data;
	update();
}
