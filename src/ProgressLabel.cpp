#include "ProgressLabel.h"

#include <QLabel>
#include <QHBoxLayout>

namespace APD
{

class ProgressLabel::Impl
{
    friend class ProgressLabel;

public:
    Impl(ProgressLabel* parent);

private:
    QLabel* m_label;
};

ProgressLabel::Impl::Impl(ProgressLabel* parent)
{
    m_label = new QLabel(parent);

    auto layout = new QHBoxLayout(parent);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(m_label);
}

/*!
    \class ProgressLabel
    \brief A wrapper around QLabel, which can be added to AsyncProgressDialog.

    The label shows current progress text as set by TaskThread::setText() method.
*/

/*!
    Constructs a progress label with the given \a parent.
*/
ProgressLabel::ProgressLabel(QWidget *parent)
    : ProgressWidget(parent)
    , m_impl(std::make_unique<Impl>(this))
{
}

ProgressLabel::~ProgressLabel() = default;

/*!
    Return the wrapped QLabel.
*/
QLabel* ProgressLabel::label() const
{
    return m_impl->m_label;
}

/*!
    Reimplementation of ProgressWidget::setText()
*/
void ProgressLabel::setText(const QString& text)
{
    m_impl->m_label->setText(text);
}

}
