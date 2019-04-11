#pragma once

#include "ProgressWidget.h"

#include <memory>

class QLabel;

namespace APD
{

class ProgressLabel : public ProgressWidget
{
    Q_OBJECT

public:
    explicit ProgressLabel(QWidget *parent = nullptr);
    ~ProgressLabel() override;

    QLabel* label() const;

public slots:
    void setText(const QString& text) override;

private:
    Q_DISABLE_COPY(ProgressLabel)

    class Impl;
    std::unique_ptr<Impl> m_impl;
};

}
