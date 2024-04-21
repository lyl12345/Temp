#include "OutputWidget.h"
#include <QQueue>
#include <iostream>
#include <QTextBlock>
#include "SearchScrollBar.h"

OutputWidget::OutputWidget(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	InitUi();
	InitEvent();
}

OutputWidget::~OutputWidget()
{
}

void OutputWidget::InitUi()
{
	ui.errorEdit->setMaximumBlockCount(50000);
	ui.errorEdit->setUndoRedoEnabled(false);
	ui.standEdit->setMaximumBlockCount(50000);
	ui.standEdit->setUndoRedoEnabled(false);

	_scrollbar = new SearchScrollBar(this);
	ui.errorEdit->setVerticalScrollBar(_scrollbar);
	ui.errorEdit->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

	for (int i = 0; i < 50000; i++)
	{
		ui.errorEdit->appendPlainText(QString::number(i));
	}
}

void OutputWidget::InitEvent()
{
	connect(&_timer, &QTimer::timeout, this, &OutputWidget::SlotTimeout);
	connect(ui.search, &QLineEdit::textChanged, this, &OutputWidget::SlotSearch);
	connect(ui.errorEdit, &QPlainTextEdit::textChanged, this, &OutputWidget::SlotHighlightSearch);
	_timer.start(20);
}

void OutputWidget::SlotTimeout()
{
	const QQueue<QString>& error_queue = OutputManager::GetInstance()->GetErrorQueue();
	if (error_queue.size() > 0)
	{
		QString str = error_queue.join("");
		QTextCharFormat format;
		format.setForeground(Qt::red);
		ui.errorEdit->moveCursor(QTextCursor::End);
		ui.errorEdit->setCurrentCharFormat(format);
		ui.errorEdit->insertPlainText(str);
		OutputManager::GetInstance()->ClearError();
	}
	const QQueue<QString>& stand_queue = OutputManager::GetInstance()->GetStandQueue();
	if (stand_queue.size() > 0)
	{
		QString str = stand_queue.join("");
		ui.standEdit->moveCursor(QTextCursor::End);
		ui.standEdit->insertPlainText(str);
		OutputManager::GetInstance()->ClearStand();
	}
}

void OutputWidget::SlotSearch(const QString& p_text)
{
	HighlightSearch(ui.errorEdit);
	HighlightSearch(ui.standEdit);
}

void OutputWidget::SlotHighlightSearch()
{
	QPlainTextEdit* edit = dynamic_cast<QPlainTextEdit*>(this->sender());
	HighlightSearch(edit);
}

void OutputWidget::HighlightSearch(QPlainTextEdit* p_edit)
{
	if (p_edit)
	{
		QString text = ui.search->text();
		QTextDocument* document = p_edit->document();
		//OutputManager::GetInstance()->AppendError(QString::number(document->size().height()) + ";");
		QTextCursor cursor(document);
		QTextCharFormat format;
		format.setBackground(Qt::green);
		//清空高亮
		QList<QTextEdit::ExtraSelection> selections;
		p_edit->setExtraSelections(selections);
		if (p_edit==ui.errorEdit)
			_scrollbar->_ys.clear();

		QFontMetrics fm(document->defaultFont());
		double scale = 1.0;
		qreal margin = document->documentMargin();
		int content_hight = (fm.height() + margin) * document->size().height();
		if (content_hight < p_edit->height())
			scale = content_hight / (double)p_edit->height();

		QTextDocument::FindFlags flags;
		while (!cursor.isNull() && !cursor.atEnd())
		{
			cursor = document->find(text, cursor, flags);
			if (!cursor.isNull())
			{
				//std::cout << cursor.verticalMovementX() << std::endl;
				//QTextBlock *block = cursor.block();
				//OutputManager::GetInstance()->AppendError(QString::number(cursor.blockNumber()) + ";");
				//OutputManager::GetInstance()->AppendError(QString::number(document->size().height()) + ";");
				QTextEdit::ExtraSelection selection;
				selection.cursor = cursor;
				selection.format = format;
				selections.append(selection);
				int nn = cursor.blockNumber();
				int cc = document->size().height();
				double radio = document->size().height() == 0 ? 0 : (cursor.blockNumber()) / (double)(document->size().height());
				if (p_edit == ui.errorEdit)
					_scrollbar->_ys.append(scale * radio);

			}
		}
		//设置高亮
		p_edit->setExtraSelections(selections);
		if (p_edit == ui.errorEdit)
			_scrollbar->update();
	}
}
