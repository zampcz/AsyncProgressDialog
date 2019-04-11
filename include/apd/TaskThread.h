#pragma once

#include "TimeStamp.h"

#include <QThread>
#include <QVariant>

namespace APD
{

class TaskThread : public QThread
{
    Q_OBJECT

public:
    explicit TaskThread(QObject* parent = nullptr);
    ~TaskThread();

    void setRange(int minimum, int maximum);
    void setValue(int value, const QVariant& userValue = QVariant());
    void setText(const QString& text);

    bool isCanceled() const;

public slots:
    void cancel();

signals:
    void valueChanged(int value, const QVariant& userValue, const TimeStamp& timeStamp);
    void rangeChanged(int minimum, int maximum);
    void textChanged(const QString& text);

private:
    Q_DISABLE_COPY(TaskThread)

    struct Impl;
    std::unique_ptr<Impl> m_impl;
};

}
