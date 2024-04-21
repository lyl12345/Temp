#pragma once

#include <QWidget>
#include "ui_OutputWidget.h"
#include <QTimer>
#include "OutputManager.h"

class QPlainTextEdit;
class SearchScrollBar;

class OutputWidget : public QWidget
{
	Q_OBJECT

public:
	OutputWidget(QWidget* parent = nullptr);
	~OutputWidget();

private:
	void InitUi();
	void InitEvent();
	void SlotTimeout();
	void SlotSearch(const QString &p_text);
	void SlotHighlightSearch();
	void HighlightSearch(QPlainTextEdit *p_edit);

private:
	QTimer _timer;
	SearchScrollBar* _scrollbar{ nullptr };

private:
	Ui::OutputWidgetClass ui;
};
