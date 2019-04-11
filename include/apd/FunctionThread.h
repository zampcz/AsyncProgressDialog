#pragma once

#include "TaskThread.h"

#include <QThread>
#include <QVariant>

#include <optional>

namespace APD
{

/*!
    \brief A thread class, which executes the function provided in the constructor
    of this class as soon as the thread is started.

    \tparam ResultType
        The return value of the function can be accessed using result() method.
        Note that the result is not ready until the thread finishes.
*/
template <class ResultType>
class FunctionThread : public TaskThread
{
public:
    //! Definition of a function to be passed to constructor of this class
    using Function = std::function<ResultType(TaskThread*)>;

    /*!
        Construct a new function thread. Function \a func is expected to accept one
        parameter - a pointer to TaskThread object. It can be used to setup progress
        range as well as current range value.
    */
    FunctionThread(Function func, QObject* parent = nullptr)
        : TaskThread(parent)
        , m_func(std::move(func))
    {}

    /*!
        Return an object, which has been previously returned by the function passed
        in the constructor, or std::nullopt if the thread has not finished yet.
    */
    std::optional<ResultType> result() const
    {
        std::lock_guard<std::mutex> lck (m_resultMutex);
        return m_result;
    }

protected:

    /*!
        Reimplementation of QThread::run()
    */
    void run() override
    {
        auto result = m_func(this);
        std::lock_guard<std::mutex> lck (m_resultMutex);
        m_result = std::move(result);
    }

private:
    Q_DISABLE_COPY(FunctionThread)

    Function m_func;
    std::optional<ResultType> m_result;
    mutable std::mutex m_resultMutex;
};

/*!
    \brief A thread class, which executes the function provided in the constructor
    of this class as soon as the thread is started.
*/
template<>
class  FunctionThread<void>
        : public TaskThread
{
public:
    //! Definition of a function to be passed to constructor of this class
    using Function = std::function<void(TaskThread*)>;

    /*!
        Construct a new function thread. Function \a func is expected to accept one
        parameter - a pointer to TaskThread object. It can be used to setup progress
        range as well as current range value.
    */
    FunctionThread(Function func, QObject* parent = nullptr)
        : TaskThread(parent)
        , m_func(std::move(func))
    {}

protected:
    /*!
        Reimplementation of QThread::run()
    */
    void run() override
    {
        m_func(this);
    }

private:
    Q_DISABLE_COPY(FunctionThread<void>)

    Function m_func;
};


};
