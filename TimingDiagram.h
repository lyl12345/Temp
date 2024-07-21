#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_TimingDiagram.h"

class QCPAxisTickerText;

class TimingDiagram : public QMainWindow
{
	Q_OBJECT

public:
	TimingDiagram(QWidget* parent = nullptr);
	~TimingDiagram();

private:
	void InitUi();
	void IintEvent();
	void SlotCycleChanged(const QString& p_text);
	void SlotClear();
	void SlotBuild();

private slots:
	void horzScrollBarChanged(int value);
	void vertScrollBarChanged(int value);
	void xAxisChanged(QCPRange range);
	void yAxisChanged(QCPRange range);

private:
	QSharedPointer<QCPAxisTickerText> textTicker{ nullptr };

private:
	Ui::TimingDiagramClass ui;
};
