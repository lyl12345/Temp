#pragma once
#include <QObject>
#include <QTimer>
#include <QQueue>

class OutputWidget;

class OutputWidgetPrivate : QObject
{
	Q_OBJECT
public:
	OutputWidgetPrivate(OutputWidget* p_ptr);
	~OutputWidgetPrivate();

private:
	void SlotTimeout();

protected:
	QTimer timer;
	QQueue<QString> queue;

	OutputWidget* q_ptr{ nullptr };
};