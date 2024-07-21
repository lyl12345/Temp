#include "TimingDiagram.h"
#include "qcustomplot.h"

TimingDiagram::TimingDiagram(QWidget* parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	InitUi();
	IintEvent();
}

TimingDiagram::~TimingDiagram()
{}

void TimingDiagram::InitUi()
{
	textTicker = QSharedPointer<QCPAxisTickerText>(new QCPAxisTickerText);
	ui.plot->yAxis->setTicker(textTicker);

	ui.plot->setInteractions(QCP::iRangeDrag /*| QCP::iRangeZoom*/ | QCP::iSelectAxes /*|
		QCP::iSelectLegend | QCP::iSelectPlottables*/);
	ui.plot->axisRect()->setRangeDrag(/*Qt::Horizontal |*/ Qt::Vertical);
	//ui.plot->axisRect()->setRangeZoom(Qt::Horizontal); // 只允许水平缩放

	//设置轴tick
	ui.plot->yAxis->grid()->setVisible(false);

	//设置轴范围
	ui.plot->xAxis->setRange(0, 80);
	ui.plot->yAxis->setRange(-5, 5);
	ui.plot->axisRect()->setupFullAxesBox();
	ui.plot->xAxis2->setTickLabels(false); // 隐藏上侧 x 轴的标签
	ui.plot->xAxis2->setTicks(false);       // 隐藏上侧 x 轴的 ticks
	ui.plot->yAxis2->setTickLabels(false); // 隐藏右侧 y 轴的标签
	ui.plot->yAxis2->setTicks(false);       // 隐藏右侧 y 轴的 ticks

	//添加标题
	ui.plot->plotLayout()->insertRow(0);
	QCPTextElement* title = new QCPTextElement(ui.plot, "Interaction Example", QFont("sans", 17, QFont::Bold));
	ui.plot->plotLayout()->addElement(0, 0, title);

	//绘制轴标识
	ui.plot->xAxis->setLabel("x Axis");
	SlotCycleChanged(ui.cycleCount->currentText());
	//ui.plot->yAxis->setLabel("y Axis");
	//ui.plot->legend->setVisible(true);

	//ui.plot->yAxis->setTickLabels(false); // 隐藏刻度标签
	//ui.plot->yAxis->setTicks(false);       // 隐藏刻度线

	//TODO添加Marker
	//ui.horizontalScrollBar->setRange(-500, 500);
	//ui.verticalScrollBar->setRange(-500, 500);
}

void TimingDiagram::IintEvent()
{
	connect(ui.cycleCount, &QComboBox::currentTextChanged, this, &TimingDiagram::SlotCycleChanged);
	connect(ui.clear, &QPushButton::clicked, this, &TimingDiagram::SlotClear);
	connect(ui.build, &QPushButton::clicked, this, &TimingDiagram::SlotBuild);
	connect(ui.horizontalScrollBar, SIGNAL(valueChanged(int)), this, SLOT(horzScrollBarChanged(int)));
	connect(ui.verticalScrollBar, SIGNAL(valueChanged(int)), this, SLOT(vertScrollBarChanged(int)));
	//connect(ui.plot->xAxis, SIGNAL(rangeChanged(QCPRange)), this, SLOT(xAxisChanged(QCPRange)));
	//connect(ui.plot->yAxis, SIGNAL(rangeChanged(QCPRange)), this, SLOT(yAxisChanged(QCPRange)));
}

void TimingDiagram::SlotCycleChanged(const QString &p_text)
{
	ui.plot->xAxis->setRange(0, p_text.toInt());
	ui.plot->replot();
	if (p_text.toInt() < 30)
	{
		ui.horizontalScrollBar->setVisible(true);
	}
	else
	{
		ui.horizontalScrollBar->setVisible(false);
	}
}

void TimingDiagram::SlotClear()
{
	ui.plot->clearGraphs();
	ui.plot->clearItems();
	textTicker->clear();
	ui.plot->replot();
}

void TimingDiagram::SlotBuild()
{
	for (int i = 0; i < 3; i = i + 2)
	{
		QVector<double> x(5), y(5);
		x[0] = 0;
		x[1] = 10;
		x[2] = 12;
		x[3] = 15;
		x[4] = 20;
		y[0] = 0 - i;
		y[1] = 1 - i;
		y[2] = 0.5 - i;
		y[3] = 0 - i;
		y[4] = 0 - i;
		QCPGraph* gr = ui.plot->addGraph();
		gr->setPen(QPen(Qt::red));
		ui.plot->graph()->setName(QString("New graph %1").arg(ui.plot->graphCount() - 1));
		ui.plot->graph()->setData(x, y);
		ui.plot->graph()->setLineStyle((QCPGraph::lsStepCenter));

		// 创建填充矩形
		QCPItemRect* rect = new QCPItemRect(ui.plot);
		QPen pen;
		pen.setStyle(Qt::DotLine);
		rect->setPen(pen); // 矩形边框颜色
		//rect->setBrush(QBrush(Qt::blue)); // 矩形填充颜色
		QBrush brush;
		brush.setStyle(Qt::BDiagPattern);
		rect->setBrush(brush);
		rect->topLeft->setCoords(20, 1 - i); // 矩形左上角坐标
		rect->bottomRight->setCoords(30, 0 - i); // 矩形右下角坐标

		//auto mLabel = new QCPItemText(ui.plot);
		//mLabel->setLayer("overlay");
		//mLabel->setText("PIN-I");
		//mLabel->setClipToAxisRect(false);
		//mLabel->setPadding(QMargins(3, 0, 3, 0));
		//mLabel->setBrush(QBrush(Qt::white));
		//mLabel->setPen(QPen(Qt::blue));
		//mLabel->setPositionAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		//mLabel->position->setCoords(QPointF(-16, 0.5 - i));
		//QCPAxisTicker* tick;
		//ui.plot->yAxis->setTicker()
		textTicker->addTick(-i, QString("PIN-%1").arg(i));
	}
	ui.horizontalScrollBar->setRange(0, 30);
	ui.verticalScrollBar->setRange(-5, 1);
	ui.plot->replot();
}

#include <QDebug>
void TimingDiagram::horzScrollBarChanged(int value)
{
	//qDebug() << value;
	qDebug() << ui.plot->xAxis->range().size();
	double len = ui.plot->xAxis->range().size();
	if (qAbs(ui.plot->xAxis->range().center() - value / 100.0) > 0.01) // if user is dragging plot, we don't want to replot twice
	{
		qDebug() << value / 100.0 * 30;
		ui.plot->xAxis->setRange(value / 100.0 * 30, ui.plot->xAxis->range().size(), Qt::AlignCenter);
		ui.plot->replot();
	}
}

void TimingDiagram::vertScrollBarChanged(int value)
{
	double len = ui.plot->yAxis->range().size();
	if (qAbs(ui.plot->yAxis->range().center() + value / 100.0) > 0.01) // if user is dragging plot, we don't want to replot twice
	{
		ui.plot->yAxis->setRange(value / 100.0 * len, ui.plot->yAxis->range().size(), Qt::AlignLeft);
		ui.plot->replot();
	}
}

void TimingDiagram::xAxisChanged(QCPRange range)
{
	ui.horizontalScrollBar->setValue(qRound(range.center() * 100.0)); // adjust position of scroll bar slider
	ui.horizontalScrollBar->setPageStep(qRound(range.size() * 100.0)); // adjust size of scroll bar slider
}

void TimingDiagram::yAxisChanged(QCPRange range)
{
	ui.verticalScrollBar->setValue(qRound(-range.center() * 100.0)); // adjust position of scroll bar slider
	ui.verticalScrollBar->setPageStep(qRound(range.size() * 100.0)); // adjust size of scroll bar slider
}
