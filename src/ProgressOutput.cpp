#include "ProgressOutput.h"

#include <QPlainTextEdit>
#include <QHBoxLayout>

namespace APD
{

class ProgressOutput::Impl
{
    friend class ProgressOutput;

public:
    Impl(ProgressOutput* parent);

private:
    QPlainTextEdit* m_output;
};


ProgressOutput::Impl::Impl(ProgressOutput* parent)
{
    m_output = new QPlainTextEdit(parent);
    m_output->setReadOnly(true);

    auto layout = new QHBoxLayout(parent);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(m_output);
}

/*!
    \class ProgressOutput
    \brief A wrapper around QPlainTextEdit, which can be added to AsyncProgressDialog.

    The plain text edit shows progress text as set by TaskThread::setText() method. It keeps
    the previous texts in the widget and scrolls down as new text is added. The edit is read-only
    by default.
*/

/*!
    Constructs a progress output with the given \a parent.
*/
ProgressOutput::ProgressOutput(QWidget *parent)
    : ProgressWidget(parent)
    , m_impl(std::make_unique<Impl>(this))
{
}

ProgressOutput::~ProgressOutput() = default;

/*!
    Return the wrapped QLabel.
*/
QPlainTextEdit* ProgressOutput::plainTextEdit() const
{
    return m_impl->m_output;
}

/*!
    Reimplementation of ProgressWidget::setText()
*/
void ProgressOutput::setText(const QString& text)
{
    m_impl->m_output->appendPlainText(text);
}

}
