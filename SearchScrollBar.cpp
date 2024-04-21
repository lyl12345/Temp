#include "SearchScrollBar.h"
#include <QPainter>
#include <QStyleOptionSlider>

SearchScrollBar::SearchScrollBar(QWidget*parent)
	: QScrollBar(parent)
{}

SearchScrollBar::~SearchScrollBar()
{}

void SearchScrollBar::paintEvent(QPaintEvent* e)
{
	QScrollBar::paintEvent(e);

	QStyleOptionSlider opt = qt_qscrollbarStyleOption(this);
	auto rect = this->style()->subControlRect(QStyle::CC_ScrollBar, &opt, QStyle::SC_SliderHandle, this);
	QStyleOptionSlider opt2 = qt_qscrollbarStyleOption(this);
	auto rect2 = this->style()->subControlRect(QStyle::CC_ScrollBar, &opt, QStyle::SC_ScrollBarGroove, this);
	auto rect3 = this->size();
	int xx = this->maximum();
	for (auto y : _ys)
	{
		QPainter painter(this);
		QPen pen = QPen(Qt::green);
		pen.setWidth(2);
		painter.setPen(pen);
		int offset = rect3.height() - rect2.height();
		double y1 = y * (rect2.height() - rect.height()) + offset;
		//int y2 = y * this->size().height() + rect3.height() - rect2.height();
		painter.drawLine(0, y1, 4, y1);
	}
}
