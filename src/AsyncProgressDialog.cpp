#include "AsyncProgressDialog.h"
#include "ProgressWidget.h"

#include <QDialogButtonBox>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>

namespace APD
{

class AsyncProgressDialog::Impl : public QObject
{
    friend class AsyncProgressDialog;

public:
    Impl(AsyncProgressDialog* parent);

    void addTask(TaskThread* thread, ProgressWidget* widget);

    void setOverallProgress(bool enabled);
    bool hasOverallProgress() const { return m_overallProgressBar != nullptr; }

    void cancelAllTasks();

private:    // methods
    struct TaskData;

    void taskFinished(TaskThread* thread);
    void closeDialog();
    bool allTasksFinished() const;
    void updateOverallProgress();
    void updateProgressValue(TaskThread* thread, int value);
    void updateProgressRange(TaskThread* thread, int minimum, int maximum);
    TaskData* findTask(TaskThread* thread);

private:    // data
    struct TaskData
    {
        TaskThread* m_thread;
        ProgressWidget* m_widget;
        bool m_autoHide = false;
        std::pair<int, int> m_range = {0, 0};
        int m_value = 0;
    };
    QList<TaskData> m_tasks;

    AsyncProgressDialog* m_parent;
    QDialogButtonBox* m_buttonBox;
    ProgressWidget* m_overallProgressBar = nullptr;
    QLabel* m_label;
    bool m_autoClose = true;
    bool m_wasCanceled = false;

};



AsyncProgressDialog::Impl::Impl(AsyncProgressDialog* parent)
    : m_parent(parent)
{
    auto layout = new QVBoxLayout(parent);

    m_label = new QLabel(parent);
    m_label->hide();
    m_label->setAlignment(Qt::AlignCenter);

    m_buttonBox = new QDialogButtonBox(parent);
    m_buttonBox->setStandardButtons(QDialogButtonBox::Cancel);
    connect(m_buttonBox, &QDialogButtonBox::rejected, parent, &AsyncProgressDialog::reject);
    layout->addWidget(m_label);
    layout->addWidget(m_buttonBox);
}

void AsyncProgressDialog::Impl::addTask(TaskThread* thread, ProgressWidget* widget)
{
    assert(thread && widget);

    QObject::connect(thread, &QThread::finished, this,
            [this, thread](){ this->taskFinished(thread); });
    QObject::connect(thread, &TaskThread::valueChanged, this,
            [this, thread](int value){ updateProgressValue(thread, value); });
    QObject::connect(thread, &TaskThread::rangeChanged, this,
            [this, thread](int minimum, int maximum){ updateProgressRange(thread, minimum, maximum); });

    connect(thread, &TaskThread::valueChanged, widget, &ProgressWidget::setValue);
    connect(thread, &TaskThread::rangeChanged, widget, &ProgressWidget::setRange);
    connect(thread, &TaskThread::textChanged, widget, &ProgressWidget::setText);

    // insert widget to the layout
    widget->setParent(m_parent);
    auto boxLayout = qobject_cast<QVBoxLayout*>(m_parent->layout());
    assert(boxLayout);
    boxLayout->insertWidget((hasOverallProgress() ? 2 : 1) + m_tasks.size(), widget);

    m_tasks.push_back({thread, widget});

    thread->start();
}

void AsyncProgressDialog::Impl::taskFinished(TaskThread* thread)
{
    if (auto task = findTask(thread); task && task->m_autoHide)
        task->m_widget->hide();

    if (allTasksFinished())
    {
        if (m_autoClose)
            closeDialog();
        else
            m_buttonBox->setStandardButtons(QDialogButtonBox::Close);
    }
}

bool AsyncProgressDialog::Impl::allTasksFinished() const
{
    return std::all_of(m_tasks.begin(), m_tasks.end(),
                       [](const auto& task) { return task.m_thread->isFinished(); });
}

void AsyncProgressDialog::Impl::cancelAllTasks()
{
    m_wasCanceled = true;
    for (auto& task : m_tasks)
        task.m_thread->cancel();

    auto button = m_buttonBox->button(QDialogButtonBox::Cancel);
    assert(button);
    button->setText(tr("Canceling..."));
    button->setEnabled(false);
}

void AsyncProgressDialog::Impl::closeDialog()
{
    assert(allTasksFinished());
    m_parent->done(m_wasCanceled ? QDialog::Rejected : QDialog::Accepted);
}

void AsyncProgressDialog::Impl::setOverallProgress(bool enabled)
{
    if (enabled)
    {
        m_overallProgressBar = ProgressWidgetFactory::createProgressBar(tr("Total progress"));
        m_overallProgressBar->setParent(m_parent);

        auto boxLayout = qobject_cast<QVBoxLayout*>(m_parent->layout());
        assert(boxLayout);
        boxLayout->insertWidget(0, m_overallProgressBar);
    }
    else
    {
        m_parent->layout()->removeWidget(m_overallProgressBar);
        delete m_overallProgressBar;
    }
}

void AsyncProgressDialog::Impl::updateProgressValue(TaskThread* thread, int value)
{
    if (auto task = findTask(thread))
    {
        task->m_value = value;
        updateOverallProgress();
    }
}

void AsyncProgressDialog::Impl::updateProgressRange(TaskThread* thread, int minimum, int maximum)
{
    if (auto task = findTask(thread))
    {
        task->m_range = { minimum, maximum };
        updateOverallProgress();
    }
}

void AsyncProgressDialog::Impl::updateOverallProgress()
{
    if (!hasOverallProgress())
        return;

    int sumPercent = 0;
    for (auto& task : m_tasks)
    {
        int denom = task.m_range.second - task.m_range.first;
        if (denom == 0)
        {
            // Can't show overall progress as one of the tasks can't report progress
            m_overallProgressBar->setRange(0, 0);
            return;
        }
        int numer = task.m_value - task.m_range.first;
        sumPercent += (100 * numer) / denom;
    }

    m_overallProgressBar->setRange(0, 100);
    m_overallProgressBar->setValue(sumPercent / m_tasks.size(), QVariant(), TimeStamp());
}

AsyncProgressDialog::Impl::TaskData* AsyncProgressDialog::Impl::findTask(TaskThread* thread)
{
    auto ptr = std::find_if(m_tasks.begin(), m_tasks.end(),
                            [thread](const auto& task){ return task.m_thread == thread; });
    return ptr != m_tasks.end() ? &*ptr : nullptr;
}


/*!
    \class AsyncProgressDialog
    \brief The AsyncProgressDialog dialog is capable of running task threads and displaying
    a progress widget for each thread.

    Task threads and progress widgets are added to the dialog using addTask(). Newly added
    threads are started when added to the dialog and paired with the progress widget. The widget
    then shows progress as reported by the thread. If more than one tasks are added, a progress
    bar indicating overall progress can be shown (see setOverallProgress())

    There are two addTask() methods, which accept a function instead of a task thread object.
    They can be conveniently combined with lambda expressions, which avoids construction of
    a thread object on the caller side. The return value of the lambda expression can be safely
    accessed after the thread finishes.

    \snippet mainwindow.cpp TaskResultExample

    After a thread finishes, the associated widget can be kept visible or hidden depending on
    autoHideWidget. The thread object is deleted in destructor if the parent of the thread object
    is this dialog. If the thread is still running when the destructor is called, the thread object
    is scheduled to be deleted as soon as the thread finishes. Life time of thread object with
    different parent is not managed by this dialog.

    \sa TaskThread, ProgressWidget
*/


/*!
    Constructs a progress dialog.

    The \a parent and \a flags arguments are passed to the QDialog constructor.
*/
AsyncProgressDialog::AsyncProgressDialog(QWidget *parent, Qt::WindowFlags flags)
    : QDialog(parent, flags)
    , m_impl(std::make_unique<Impl>(this))
{
}

AsyncProgressDialog::~AsyncProgressDialog()
{
    // Check that threads owned by this class has finished.
    // If not, the thread parent must be reset and the thread object deleted later
    for (auto& task : m_impl->m_tasks)
        if (task.m_thread->parent() == this && !task.m_thread->isFinished())
        {
            task.m_thread->setParent(nullptr);
            connect(task.m_thread, &QThread::finished, task.m_thread, &QObject::deleteLater);
        }
}

/*!
    Add a task \a thread object and the associated progress \a widget.
    The widget displays the progress as reported by the thread.

    The ownership of \a widget is transferred to the dialog.

    The ownership of\a thread is not transferred. If the parent of the \a thread
    object is this dialog, the thread object is either deleted in destructor if finished,
    or scheduled to be deleted as soon as the thread finishes. Life time of thread object with
    different parent is not managed by this dialog.
*/
void AsyncProgressDialog::addTask(TaskThread* thread, ProgressWidget* widget)
{
    m_impl->addTask(thread, widget);
}

/*!
  Reimplemented from QDialog::reject()
*/
void AsyncProgressDialog::reject()
{
    if (m_impl->allTasksFinished())
        m_impl->closeDialog();
    else
        m_impl->cancelAllTasks();
}

/*!
    Set a flag whether the dialog gets hidden
    after all threads has finished.

    \sa autoClose()
*/
void AsyncProgressDialog::setAutoClose(bool close)
{
    m_impl->m_autoClose = close;
}

/*!
    Return a flag whether the dialog gets hidden
    after all threads has finished.

    The default is true.

    \sa setAutoClose()
*/
bool AsyncProgressDialog::autoClose() const
{
    return m_impl->m_autoClose;
}

/*!
    Set a flag whether the dialog shows total progress
    of all tasks.

    \sa hasOverallProgress()
*/
void AsyncProgressDialog::setOverallProgress(bool enabled)
{
    if (hasOverallProgress() != enabled)
        m_impl->setOverallProgress(enabled);
}

/*!
    Return a flag whether the dialog shows total progress
    of all tasks.

    The overal progress can be only shown if all running tasks
    report progress, i.e. their range (minimum and maximum progress
    values) is not empty.

    \sa setOverallProgress()
*/
bool AsyncProgressDialog::hasOverallProgress() const
{
    return m_impl->hasOverallProgress();
}

/*!
    Set the label's text.

    \sa labelText()
*/
void AsyncProgressDialog::setLabelText(const QString& labelText)
{
    m_impl->m_label->setText(labelText);
    m_impl->m_label->setHidden(labelText.isEmpty());
}

/*!
    Returns the label's text. Label is displayed as
    a top widget in the dialog. The label is hidden when empty.

    The default text is an empty string.

    \sa setLabelText()
*/
QString AsyncProgressDialog::labelText() const
{
    return m_impl->m_label->text();
}

/*!
    Returns number of threads in this dialog.

    \note Number of threads is always equal to number of widgets
    \sa threadAt(), widgetCount()
*/
int AsyncProgressDialog::threadCount() const
{
    return m_impl->m_tasks.count();
}

/*!
    Returns the thread object at given \a index.
    The index must be in the range [0, threadCount())

    \sa threadCount()
*/
TaskThread* AsyncProgressDialog::threadAt(int index) const
{
    return m_impl->m_tasks[index].m_thread;
}

/*!
    Returns number of progress widgets in this dialog.

    \note Number of threads is always equal to number of widgets
    \sa widgetAt(), threadCount()
*/
int AsyncProgressDialog::widgetCount() const
{
    return m_impl->m_tasks.count();
}

/*!
    Returns the progress widget at given \a index.
    The index must be in the range [0, widgetCount())

    \sa widgetCount()
*/
ProgressWidget* AsyncProgressDialog::widgetAt(int index) const
{
    return m_impl->m_tasks[index].m_widget;
}

/*!
    Sets the flag \a autoHide indicating whether the progress widget
    at given \a index should be automatically hidden after
    the associated thread finishes.

    The index must be in the range [0, widgetCount())

    \sa widgetAt(), widgetCount()
*/
void AsyncProgressDialog::setAutoHideWidget(int index, bool autoHide)
{
    m_impl->m_tasks[index].m_autoHide = autoHide;
}

/*!
    Return the flag indicating whether the progress widget
    at given \a index should be automatically hidden after
    the associated thread finishes.
    The index must be in the range [0, widgetCount())

    \sa widgetAt(), widgetCount()
*/
bool AsyncProgressDialog::autoHideWidget(int index) const
{
    return m_impl->m_tasks[index].m_autoHide;
}

}
