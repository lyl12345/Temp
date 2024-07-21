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
	//ui.plot->axisRect()->setRangeZoom(Qt::Horizontal); // ֻ����ˮƽ����

	//������tick
	ui.plot->yAxis->grid()->setVisible(false);

	//�����᷶Χ
	ui.plot->xAxis->setRange(0, 80);
	ui.plot->yAxis->setRange(-5, 5);
	ui.plot->axisRect()->setupFullAxesBox();
	ui.plot->xAxis2->setTickLabels(false); // �����ϲ� x ��ı�ǩ
	ui.plot->xAxis2->setTicks(false);       // �����ϲ� x ��� ticks
	ui.plot->yAxis2->setTickLabels(false); // �����Ҳ� y ��ı�ǩ
	ui.plot->yAxis2->setTicks(false);       // �����Ҳ� y ��� ticks

	//��ӱ���
	ui.plot->plotLayout()->insertRow(0);
	QCPTextElement* title = new QCPTextElement(ui.plot, "Interaction Example", QFont("sans", 17, QFont::Bold));
	ui.plot->plotLayout()->addElement(0, 0, title);

	//�������ʶ
	ui.plot->xAxis->setLabel("x Axis");
	SlotCycleChanged(ui.cycleCount->currentText());
	//ui.plot->yAxis->setLabel("y Axis");
	//ui.plot->legend->setVisible(true);

	//ui.plot->yAxis->setTickLabels(false); // ���ؿ̶ȱ�ǩ
	//ui.plot->yAxis->setTicks(false);       // ���ؿ̶���

	//TODO���Marker
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

		// ����������
		QCPItemRect* rect = new QCPItemRect(ui.plot);
		QPen pen;
		pen.setStyle(Qt::DotLine);
		rect->setPen(pen); // ���α߿���ɫ
		//rect->setBrush(QBrush(Qt::blue)); // ���������ɫ
		QBrush brush;
		brush.setStyle(Qt::BDiagPattern);
		rect->setBrush(brush);
		rect->topLeft->setCoords(20, 1 - i); // �������Ͻ�����
		rect->bottomRight->setCoords(30, 0 - i); // �������½�����

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
