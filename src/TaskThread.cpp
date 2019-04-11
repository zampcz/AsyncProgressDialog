#include "TaskThread.h"
#include <QWidget>
#include <QComboBox>

namespace APD
{

struct TaskThread::Impl
{
    std::atomic<bool> m_canceled = false;
};

/*!
    \class TaskThread
    \brief Base class for all threads capable of reporting progress and cancelling the progress when requested.

    Task thread can be added to AsyncProgressDialog and associated with a progress widget,
    which displays its progress. Typically, a thread first sets range using setRange() method,
    and then sets progress values within this range as it runs using setValue() method.
    Alternatively, the thread may set a progress text using setText() method.
*/

/*!
    \fn void TaskThread::valueChanged(int value, const QVariant& userValue, const TimeStamp& timeStamp)

    This signal is emitted whenever progress value changes. Current time stamp is added to this signal.
    Time step must be acquired within the asynchronous thread in order to avoid possible lags in
    GUI thread and allow widgets to display time accurately.
*/

/*!
    \fn void TaskThread::rangeChanged(int minimum, int maximum)

    This signal is emitted when progress range changes.
*/

/*!
    \fn void TaskThread::textChanged(const QString& text)

    This signal is emitted whenever progress text changes.
*/


/*!
    Constructs a task thread with the given \a parent.
*/
TaskThread::TaskThread(QObject* parent)
 : QThread(parent)
 , m_impl(std::make_unique<TaskThread::Impl>())
{
    qRegisterMetaType<TimeStamp>("TimeStamp");
}

TaskThread::~TaskThread() = default;

/*!
    Sets minimum and maximum progress values to \a minimum and
    \a maximum respectively. This method can be used from within
    asynchronous computation. The method should be called before
    any call to setValue() method.

    The method emits rangeChanged() signal.
*/
void TaskThread::setRange(int minimum, int maximum)
{
    emit rangeChanged(minimum, maximum);
}

/*!
    Sets progress value to \a value. The value should be inside
    minimum - maximum range set by previous call to setRange()
    method. The \a userValue parameter is a user-defined value
    passed to the correpsonding ProgresWidget. This method can
    be used from within asynchronous computation.

    The method emits valueChanged() signal.
*/
void TaskThread::setValue(int value, const QVariant& userValue)
{
    emit valueChanged(value, userValue, std::chrono::steady_clock::now());
}

/*!
    Sets current progress text to \a text. This method can
    be used from within asynchronous computation.

    The method emits textChanged() signal.
*/
void TaskThread::setText(const QString& text)
{
    emit textChanged(text);
}

/*!
    Checks for a pending cancel request.

    This method is thread-safe.
*/
bool TaskThread::isCanceled() const
{
    return m_impl->m_canceled;
}

/*!
    Registers cancel request. It is up to thread implementer to
    check for cancel request using isCanceled() method in
    reasonable intervals and abort the execution of the thread
    if this request has been registered.

    This method is thread-safe.
*/
void TaskThread::cancel()
{
    m_impl->m_canceled = true;
}

}
