#pragma once

#include "ProgressWidget.h"

#include <memory>

class QPlainTextEdit;

namespace APD
{

class ProgressOutput : public ProgressWidget
{
    Q_OBJECT

public:
    explicit ProgressOutput(QWidget *parent = nullptr);
    ~ProgressOutput();

    QPlainTextEdit* plainTextEdit() const;

public slots:
    void setText(const QString& text) override;

private:
    Q_DISABLE_COPY(ProgressOutput)

    class Impl;
    std::unique_ptr<Impl> m_impl;
};

}
