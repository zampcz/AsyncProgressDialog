#include "ProgressWidget.h"

namespace APD
{
/*!
    \class ProgressWidget
    \brief Base class for all widgets capable of displaying progress for a TaskThread.

    When added to AsyncProgressDialog together with a TaskThread, the progress widget is
    automatically connected to the associated signals of TaskThread. The inherited progress
    widgets override some or all of the virtaul slots defined in this class in order to
    update their progress.

    Progress widgets can be created using ProgressWidgetFactory.

    \sa ProgressBar, ProgressEstimate, ProgressLabel, ProgressOutput, ProgressVelocity, ProgressWidgetContainer
*/

/*!
    \fn ProgressWidget::ProgressWidget

    Constructs a progress widget with the given \a parent.
*/

/*!
    \fn void ProgressWidget::setValue(int value, const QVariant& userValue, const TimeStamp& timeStamp)

    A slot called when the progress of associated TaskThread is updated. The \a timeStamp
    value is added by the TaskThread::setValue() method. The time stamp can be used to
    display time estimate. It must be obtained in the worker thread in order to be accurate.

    \sa TaskThread::setValue()
*/

/*!
    \fn void ProgressWidget::setRange(int minimum, int maximum)

    A slot called when the range of associated TaskThread is updated.

    \sa TaskThread::setRange()
*/

/*!
    \fn void ProgressWidget::setText(int minimum, int maximum)

    A slot called when the progress text of associated TaskThread is updated.

    \sa TaskThread::setText()
*/

}
