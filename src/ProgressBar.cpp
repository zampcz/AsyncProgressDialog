#include "ProgressBar.h"

#include <QProgressBar>
#include <QHBoxLayout>

namespace APD
{

class ProgressBar::Impl
{
    friend class ProgressBar;

public:
    Impl(ProgressBar* parent);

private:
    QProgressBar* m_progressBar;
};

ProgressBar::Impl::Impl(ProgressBar* parent)
{
    m_progressBar = new QProgressBar(parent);

    auto layout = new QHBoxLayout(parent);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(m_progressBar);
}

/*!
    \class ProgressBar
    \brief A wrapper around QProgressBar, which can be added to AsyncProgressDialog.

    The label shows current progress value as set by TaskThread::setValue() method.
*/

/*!
    Constructs a progress bar with the given \a parent.
*/
ProgressBar::ProgressBar(QWidget *parent)
    : ProgressWidget(parent)
    , m_impl(std::make_unique<Impl>(this))
{
}

ProgressBar::~ProgressBar() = default;

/*!
    Return the wrapped QProgressBar.
*/
QProgressBar* ProgressBar::progressBar() const
{
    return m_impl->m_progressBar;
}

/*!
    Reimplementation of ProgressWidget::setValue()
*/
void ProgressBar::setValue(int value, const QVariant&, const TimeStamp&)
{
    m_impl->m_progressBar->setValue(value);
}

/*!
    Reimplementation of ProgressWidget::setRange()
*/
void ProgressBar::setRange(int minimum, int maximum)
{
    m_impl->m_progressBar->setRange(minimum, maximum);
}

}
