#include "ProgressWidgetFactory.h"
#include "ProgressWidgetContainer.h"
#include "ProgressBar.h"
#include "ProgressEstimate.h"
#include "ProgressLabel.h"
#include "ProgressOutput.h"
#include "ProgressVelocityPlot.h"

#include <QLabel>
#include <QGridLayout>

namespace APD
{

ProgressWidget* createProgressWidgetImpl(ProgressWidget* bar, const QString& label, AdditionalWidgets additionalWidgets)
{
    if (static_cast<int>(additionalWidgets) == static_cast<int>(AdditionalWidget::NoWidget) && label.isEmpty())
        return bar;

    auto container = new ProgressWidgetContainer();

    int row = 0;
    int colSpan = label.isEmpty() ? 1 : 2;

    if (additionalWidgets.testFlag(AdditionalWidget::Estimate))
        container->addWidget(new ProgressEstimate(), row++, 0, 1, colSpan);

    if (label.isEmpty())
        container->addWidget(bar, row++, 0, 1, colSpan);
    else
    {
        container->gridLayout()->addWidget(new QLabel(label, container), row, 0);
        container->addWidget(bar, row++, 1);
    }

    if (additionalWidgets.testFlag(AdditionalWidget::Label))
        container->addWidget(new ProgressLabel(), row++, 0, 1, colSpan);

    if (additionalWidgets.testFlag(AdditionalWidget::Output))
        container->addWidget(new ProgressOutput(), row++, 0, 1, colSpan);

    return container;
}


/*!
    \class ProgressWidgetFactory
    \brief A convenience class which contains factory methods for creating progress and velocity bars

    A progress bar as well as velocity bar widget can be composed of multitude of additional widgets
    for displaying progress, current text, etc.

    \sa AdditionalWidget
*/


/*!
    Create and return a new progress bar widget. The progress bar has an adjanced \a label and possibly
    number of \a additionalWidgets.

    \sa AdditionalWidgets
*/
ProgressWidget* ProgressWidgetFactory::createProgressBar(const QString& label, AdditionalWidgets additionalWidgets)
{
    return createProgressWidgetImpl(new ProgressBar(), label, additionalWidgets);
}

/*!
    Create and return a new progress bar widget. The progress bar may have number of adjanced \a additionalWidgets.

    \sa AdditionalWidgets
*/
ProgressWidget* ProgressWidgetFactory::createProgressBar(AdditionalWidgets additionalWidgets)
{
    return createProgressBar(QString(), additionalWidgets);
}

/*!
    Create and return a new velocity bar widget. The velocity bar has an adjanced \a label and possibly
    number of \a additionalWidgets.

    \sa AdditionalWidgets
*/
ProgressWidget* ProgressWidgetFactory::createVelocityBar(const QString& units, AdditionalWidgets additionalWidgets)
{
    return createProgressWidgetImpl(new ProgressVelocityPlot(units), QString(), additionalWidgets);
}

/*!
    Create and return a new velocity bar widget. The velocity bar may have number of adjanced \a additionalWidgets.

    \sa AdditionalWidgets
*/
ProgressWidget* ProgressWidgetFactory::createVelocityBar(AdditionalWidgets additionalWidgets)
{
    return createVelocityBar(QString(), additionalWidgets);
}

}
