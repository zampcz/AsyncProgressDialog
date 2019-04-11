#pragma once

#include "ProgressWidget.h"

#include <QPen>
#include <QBrush>

#include <memory>

namespace APD
{

class ProgressVelocityPlot : public ProgressWidget
{
    Q_OBJECT

public:
    explicit ProgressVelocityPlot(QWidget* parent = nullptr);
    explicit ProgressVelocityPlot(const QString& quantityUnits, QWidget* parent = nullptr);
    ~ProgressVelocityPlot() override;

    QString quantityUnits() const;
    void setQuantityUnits(const QString& quantityUnits);

    bool isProgressHidden() const;
    void setProgressHidden(bool hide);
    bool isVelocityHistoryHidden() const;
    void setVelocityHistoryHidden(bool hide);
    bool isCurrentVelocityHidden() const;
    void setCurrentVelocityHidden(bool hide);
    QPen progressPen() const;
    void setProgressPen(const QPen& pen);
    QBrush progressBrush() const;
    void setProgressBrush(const QBrush& brush);
    QPen currentVelocityPen() const;
    void setCurrentVelocityPen(const QPen& pen);
    QPen velocityHistoryPen() const;
    void setVelocityHistoryPen(const QPen& pen);
    QBrush velocityHistoryBrush() const;
    void setVelocityHistoryBrush(const QBrush& brush);

public slots:
    void setValue(int value, const QVariant& userData, const TimeStamp& timeStamp) override;
    void setRange(int minimum, int maximum) override;

private:
    Q_DISABLE_COPY(ProgressVelocityPlot)

    class Impl;
    std::unique_ptr<Impl> m_impl;
};

}
