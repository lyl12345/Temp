#pragma once

#include <QObject>
#include <QScrollBar>

class SearchScrollBar  : public QScrollBar
{
	Q_OBJECT

public:
	SearchScrollBar(QWidget *parent);
	~SearchScrollBar();

protected:
	void paintEvent(QPaintEvent*) override;

public:
	QVector<double> _ys;
};
