#pragma once

#include "TimeStamp.h"

#include <QWidget>

namespace APD
{

class ProgressWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ProgressWidget(QWidget *parent = nullptr)
        : QWidget(parent)
    {}

public slots:
    virtual void setValue(int /*value*/, const QVariant& /*userValue*/, const TimeStamp& /*timeStamp*/) {}
    virtual void setRange(int /*minimum*/, int /*maximum*/) {}
    virtual void setText(const QString& /*text*/) {}

private:
    Q_DISABLE_COPY(ProgressWidget)
};

}
