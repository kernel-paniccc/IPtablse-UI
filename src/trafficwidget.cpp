#include "trafficwidget.h"

#include <QPainter>
#include <QPaintEvent>
#include <QPainterPath>

#include <algorithm>

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

    double maximum = 1.0;
    for (double value : incoming_)
        maximum = std::max(maximum, value);
    for (double value : outgoing_)
        maximum = std::max(maximum, value);
    maximum *= 1.15;

    const auto drawSeries = [&](const QList<double> &samples, const QColor &color) {
        if (samples.isEmpty())
            return;
        QPainterPath path;
        for (int index = 0; index < samples.size(); ++index) {
            const qreal x = chart.right() - chart.width() * (samples.size() - 1 - index) / (MaxSamples - 1.0);
            const qreal y = chart.bottom() - chart.height() * samples.at(index) / maximum;
            index == 0 ? path.moveTo(x, y) : path.lineTo(x, y);
        }
        painter.setPen(QPen(color, 2.4, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        painter.drawPath(path);
    };

    drawSeries(incoming_, QColor("#2563eb"));
    drawSeries(outgoing_, QColor("#f97316"));

    painter.setPen(QColor("#64748b"));
    painter.drawText(QRectF(4, chart.top(), 44, 20), Qt::AlignRight,
                     QString::number(maximum, 'f', 1));
    painter.drawText(QRectF(4, chart.bottom() - 10, 44, 20), Qt::AlignRight, "0");
}

