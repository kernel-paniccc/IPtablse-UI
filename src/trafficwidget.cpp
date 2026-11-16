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

    const QRectF chart = rect().adjusted(52, 18, -18, -34);
    painter.setPen(QPen(QColor("#e2e8f0"), 1));
    for (int step = 0; step <= 4; ++step) {
        const qreal y = chart.top() + chart.height() * step / 4.0;
        painter.drawLine(QPointF(chart.left(), y), QPointF(chart.right(), y));
    }
    for (int step = 0; step <= 6; ++step) {
        const qreal x = chart.left() + chart.width() * step / 6.0;
        painter.drawLine(QPointF(x, chart.top()), QPointF(x, chart.bottom()));
    }

    painter.setPen(QColor("#64748b"));
    painter.drawText(QRectF(4, chart.top() - 8, 44, 20), Qt::AlignRight, "Mbps");
    painter.drawText(QRectF(chart.left(), chart.bottom() + 7, chart.width(), 20),
                     Qt::AlignCenter, "последние 60 секунд");
}

