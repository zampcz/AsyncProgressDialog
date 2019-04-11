#pragma once

#include "ProgressWidget.h"

#include <memory>

class QProgressBar;

namespace APD
{

class ProgressBar : public ProgressWidget
{
    Q_OBJECT
public:
    explicit ProgressBar(QWidget *parent = nullptr);
    ~ProgressBar() override;

    QProgressBar* progressBar() const;

public slots:
    void setValue(int value, const QVariant&, const TimeStamp&) override;
    void setRange(int minimum, int maximum) override;

private:
    Q_DISABLE_COPY(ProgressBar)

    class Impl;
    std::unique_ptr<Impl> m_impl;
};

}
