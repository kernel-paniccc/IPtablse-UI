#include "trafficwidget.h"

#include <QPainter>
#include <QPaintEvent>

TrafficWidget::TrafficWidget(QWidget *parent)
    : QWidget(parent)
{
    setMinimumHeight(210);
}

void TrafficWidget::addSample(double incomingMbps, double outgoingMbps)
{
    incoming_.append(incomingMbps);
    outgoing_.append(outgoingMbps);
    while (incoming_.size() > MaxSamples)
        incoming_.removeFirst();
    while (outgoing_.size() > MaxSamples)
        outgoing_.removeFirst();
    update();
}

void TrafficWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.fillRect(rect(), QColor("#ffffff"));
}

