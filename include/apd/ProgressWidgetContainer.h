#pragma once

#include "ProgressWidget.h"

#include <memory>

class QGridLayout;

namespace APD
{

class ProgressWidgetContainer : public ProgressWidget
{
    Q_OBJECT

public:
    explicit ProgressWidgetContainer(QWidget* parent = nullptr);
    ~ProgressWidgetContainer();

    void addWidget(ProgressWidget* widget, int row, int column, Qt::Alignment alignment = 0);
    void addWidget(ProgressWidget* widget, int row, int column, int rowSpan, int columnSpan, Qt::Alignment alignment = 0);
    void removeWidget(ProgressWidget* widget);

    QGridLayout* gridLayout() const;

public slots:
    void setValue(int value, const QVariant& userData, const TimeStamp& timeStamp) override;
    void setRange(int minimum, int maximum) override;
    void setText(const QString& text) override;

private:
    Q_DISABLE_COPY(ProgressWidgetContainer)

    class Impl;
    std::unique_ptr<Impl> m_impl;
};

}
