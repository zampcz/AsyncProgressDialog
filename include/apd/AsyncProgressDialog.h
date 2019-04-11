#pragma once

#include "ProgressWidgetFactory.h"
#include "ProgressBar.h"
#include "FunctionThread.h"

#include <QDialog>

#include <functional>


namespace APD
{

class TaskThread;
class ProgressWidget;

class AsyncProgressDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AsyncProgressDialog(QWidget *parent = nullptr, Qt::WindowFlags flags = Qt::WindowFlags());
    ~AsyncProgressDialog() override;

    void addTask(TaskThread* thread, ProgressWidget* widget);

    /*!
        Add task defined by a function \a func and the associated progress \a widget.
        The method creates a FunctionThread object internally. The widget displays the
        progress as reported by the function thread. The thread object is returned by
        this method.

        The ownership of \a widget is transferred to the dialog.

        The ownership of the function thread object is set to this dialog and is deleted
        in the destructor of the dialog.
    */
    template <typename F>
    auto addTask(F func, ProgressWidget* widget = ProgressWidgetFactory::createProgressBar())
        -> FunctionThread<decltype(func(static_cast<TaskThread*>(nullptr)))>*
    {
        using ResultType = decltype(func(static_cast<TaskThread*>(nullptr)));
        auto thread = new FunctionThread<ResultType>(std::move(func), this);
        addTask(thread, widget);
        return thread;
    }

    void setAutoClose(bool close);
    bool autoClose() const;

    void setOverallProgress(bool enabled);
    bool hasOverallProgress() const;

    void setLabelText(const QString& labelText);
    QString labelText() const;

    int threadCount() const;
    TaskThread* threadAt(int index) const;

    int widgetCount() const;
    ProgressWidget* widgetAt(int index) const;

    void setAutoHideWidget(int index, bool autoHide);
    bool autoHideWidget(int index) const;

public slots:
    void reject() override;

private:
    Q_DISABLE_COPY(AsyncProgressDialog)

    class Impl;
    std::unique_ptr<Impl> m_impl;
};


}

