#pragma once

#include <QString>

namespace APD
{

class ProgressWidget;

enum class AdditionalWidget
{
    NoWidget = 0x00,
    Estimate = 0x01,
    Label = 0x02,
    Output = 0x04,
};
Q_DECLARE_FLAGS(AdditionalWidgets, AdditionalWidget)
Q_DECLARE_OPERATORS_FOR_FLAGS(AdditionalWidgets)

class ProgressWidgetFactory
{
public:
    static ProgressWidget* createProgressBar(const QString& label, AdditionalWidgets additionalWidgets = AdditionalWidget::NoWidget);
    static ProgressWidget* createProgressBar(AdditionalWidgets additionalWidgets = AdditionalWidget::NoWidget);
    static ProgressWidget* createVelocityBar(const QString& units, AdditionalWidgets additionalWidgets = AdditionalWidget::NoWidget);
    static ProgressWidget* createVelocityBar(AdditionalWidgets additionalWidgets = AdditionalWidget::NoWidget);
};

}
