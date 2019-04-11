#pragma once

#include "ProgressWidget.h"

#include <memory>

namespace APD
{

class ProgressEstimate : public ProgressWidget
{
    Q_OBJECT
public:
    explicit ProgressEstimate(QWidget* parent = nullptr);
    ~ProgressEstimate() override;

    enum TimeFormat
    {
        Approximate,
        Exact,
    };

    QString remainingTimeText() const;
    void setRemainingTimeText(const QString& text);

    bool isRemainingTimeHidden() const;
    void setRemainingTimeHidden(bool hide);

    TimeFormat remainingTimeFormat() const;
    void setRemainingTimeFormat(TimeFormat format) const;

    QString elapsedTimeText() const;
    void setElapsedTimeText(const QString& text);

    bool isElapsedTimeHidden() const;
    void setElapsedTimeHidden(bool hide);

    TimeFormat elapsedTimeFormat() const;
    void setElapsedTimeFormat(TimeFormat format) const;

public slots:
    void setValue(int value, const QVariant&, const TimeStamp& timeStamp) override;
    void setRange(int minimum, int maximum) override;

private:
    Q_DISABLE_COPY(ProgressEstimate)

    class Impl;
    std::unique_ptr<Impl> m_impl;
};

}
