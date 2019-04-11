#include "ProgressEstimate.h"

#include <QLabel>
#include <QGridLayout>
#include <QCoreApplication>

namespace APD
{

/*!
    \class DurationFormatter
    \brief Helper class which formats time duration into desired format.
*/

class DurationFormatter
{
    Q_DECLARE_TR_FUNCTIONS(DurationFormatter)
public:

    /*!
      Constructs a new duration formatter object.
    */
    template<class T>
    DurationFormatter(T dur)
    {
        using namespace std::chrono;
        using days = duration<long, std::ratio<3600 * 24>>;
        auto d = duration_cast<days>(dur);
        auto h = duration_cast<hours>(dur -= d);
        auto m = duration_cast<minutes>(dur -= h);
        auto s = duration_cast<seconds>(dur -= m);
        auto ms = duration_cast<milliseconds>(dur -= s);

        m_days = d.count();
        m_hours = h.count();
        m_minutes = m.count();
        m_seconds = static_cast<int>(s.count());
        m_milliseconds = static_cast<int>(ms.count());
    }

    QString approximate() const;
    QString exact() const;

private:
    QString fromatQuantity(int q, const QString& s) const { return QString("%1 %2%3").arg(q).arg(s).arg(q != 1 ? "s" : ""); }
    QString formatDays() const { return fromatQuantity(m_days, "day"); }
    QString formatHours() const { return fromatQuantity(m_hours, "hour"); }
    QString formatMinutes() const { return fromatQuantity(m_minutes, "minute"); }
    QString formatSeconds() const { return fromatQuantity(m_seconds, "second"); }
    QString formatMilliseconds() const { return fromatQuantity(m_milliseconds, "millisecond"); }

    int m_days = 0;
    int m_hours = 0;
    int m_minutes = 0;
    int m_seconds = 0;
    int m_milliseconds = 0;
};

/*!
    Return approximate representation of time (days to seconds). Lower resolution than seconds is ignored.
*/
QString DurationFormatter::approximate() const
{
    if (m_days > 0)
        return QString("About %1 and %2").arg(formatDays(), formatHours());
    else if (m_hours > 0)
        return QString("About %1 and %2").arg(formatHours(), formatMinutes());
    else if (m_minutes > 0)
    {
        auto str = QString("About %1").arg(formatMinutes());
        if (m_minutes < 5 && m_seconds > 30)
            str += tr(" and 30 seconds");
        return str;
    }
    else
    {
        if (m_seconds > 45)
            return tr("Less than 1 minute");
        else if (m_seconds > 30)
            return tr("Less than 45 seconds");
        else if (m_seconds > 15)
            return tr("Less than 30 seconds");
        else if (m_seconds > 10)
            return tr("Less than 15 seconds");
        else if (m_seconds > 5)
            return tr("Less than 10 seconds");
        else
            return tr("Less than 5 seconds");
    }
}

/*!
    Return exact time string (days to seconds). Lower resolution than seconds is ignored.
*/
QString DurationFormatter::exact() const
{
    QStringList str;
    if (m_days > 0)
        str << formatDays();
    if (m_hours > 0)
        str << formatHours();
    if (m_minutes > 0)
        str << formatMinutes();
    str << formatSeconds();
    return str.join(", ");
}


class ProgressEstimate::Impl
{
    friend class ProgressEstimate;
public:
    Impl(ProgressEstimate* parent);

    void setValue(int value, const TimeStamp& timeStamp);
    void setRange(int minimum, int maximum);

private:
    void updateWidgets();

private:
    std::chrono::time_point<std::chrono::steady_clock> m_firstTimeStamp;
    int m_firstValue;
    bool m_initialized = false;

    int m_minimum = 0;
    int m_maximum = 0;

    std::chrono::milliseconds m_elapsedTime;
    std::chrono::milliseconds m_remainingTime;

    QLabel* m_elapsedTimeLabel;
    QLabel* m_remainingTimeLabel;
    QLabel* m_elapsedTimeText;
    QLabel* m_remainingTimeText;
    TimeFormat m_elapsedTimeFormat = Exact;
    TimeFormat m_remainingTimeFormat = Approximate;
};

ProgressEstimate::Impl::Impl(ProgressEstimate* parent)
{
    m_elapsedTimeLabel = new QLabel(tr("Elapsed time"), parent);
    m_remainingTimeLabel = new QLabel(tr("Estimated remaining time"), parent);
    m_elapsedTimeText = new QLabel(tr("..."), parent);
    m_remainingTimeText = new QLabel(tr("..."), parent);

    // QFormLayout cannot be used here as it keeps spacing of hidden rows
    auto layout = new QGridLayout(parent);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(m_elapsedTimeLabel, 0, 0);
    layout->addWidget(m_elapsedTimeText, 0, 1);
    layout->addWidget(m_remainingTimeLabel, 1, 0);
    layout->addWidget(m_remainingTimeText, 1, 1);
}

void ProgressEstimate::Impl::setValue(int value, const TimeStamp& timeStamp)
{
    using namespace std::chrono;

    if (!m_initialized)
    {
        m_firstTimeStamp = timeStamp;
        m_firstValue = value;
        m_initialized = true;
        return;
    }

    m_elapsedTime = duration_cast<milliseconds>(timeStamp - m_firstTimeStamp);
    if (m_maximum - m_minimum > 0)
    {
        auto stepDelta = value - m_firstValue;
        auto fullRange = m_maximum - m_firstValue;
        if (fullRange == 0)
            return;

        auto totalTime = (m_elapsedTime * fullRange) / stepDelta;
        m_remainingTime = totalTime - m_elapsedTime;
    }
    updateWidgets();
}

void ProgressEstimate::Impl::setRange(int minimum, int maximum)
{
    m_minimum = minimum;
    m_maximum = maximum;
}

void ProgressEstimate::Impl::updateWidgets()
{
    DurationFormatter elapsed(m_elapsedTime);
    DurationFormatter remaining(m_remainingTime);
    m_elapsedTimeText->setText(m_elapsedTimeFormat == Approximate ? elapsed.approximate() : elapsed.exact());
    m_remainingTimeText->setText(m_remainingTimeFormat == Approximate ? remaining.approximate() : remaining.exact());
}


/*!
    \class ProgressEstimate
    \brief A widget capable of showing elapsed and remaining time.

    The widget calculates elapsed and remaining time based on progress range and current
    progress value. Note that both times can be shown after the second update to progress
    value as the time is measured from the first call to setValue().

    Elapsed and remaining time can be shown or hidden by calling setElapsedTimeHidden() and
    setRemainingTimeHidden() respectively.

    The reporting of elapsed and remaining time can be either exact or approximate. TimeFormat::Exact
    shows time in HH:MM:SS format whereas TimeFormat::Approximate format depends on the time itself
    (e.g. Less than 30 seconds). Default format for elapsed time is TimeFormat::Exact and for remaining
    time it is TimeFormat::Approximate.

    \enum ProgressEstimate::TimeFormat
    Specifies how the elapsed and remaining time should be displayed.

    \var ProgressEstimate::TimeFormat ProgressEstimate::Exact
    The time is shown in HH:MM:SS format
    \var ProgressEstimate::TimeFormat ProgressEstimate::Approximate
    The time is shown in an approximate format, e.g. \a Less \a than \a 30 \a seconds
*/

/*!
    Constructs a progress estimate widget with the given \a parent.
*/
ProgressEstimate::ProgressEstimate(QWidget *parent)
    : ProgressWidget(parent)
    , m_impl(std::make_unique<Impl>(this))
{
}

ProgressEstimate::~ProgressEstimate() = default;

/*!
    Return label for the remaining time estimate.

    The default is "Estimated remaining time".

    \sa setRemainingTimeText()
*/
QString ProgressEstimate::remainingTimeText() const
{
    return m_impl->m_remainingTimeLabel->text();
}

/*!
    Set label for the remaining time estimate.

    \sa remainingTimeText()
*/
void ProgressEstimate::setRemainingTimeText(const QString& text)
{
    m_impl->m_remainingTimeLabel->setText(text);
}

/*!
    Returns the flag whether the remaining time estimate should be displayed.

    The default is false.

    \sa setRemainingTimeHidden()
*/
bool ProgressEstimate::isRemainingTimeHidden() const
{
    return m_impl->m_remainingTimeLabel->isHidden();
}

/*!
    Set the flag whether the remaining time estimate should be displayed.

    \sa isRemainingTimeHidden()
*/
void ProgressEstimate::setRemainingTimeHidden(bool hide)
{
    m_impl->m_remainingTimeLabel->setHidden(hide);
    m_impl->m_remainingTimeText->setHidden(hide);
}

/*!
    Return format of remaining time estimate.

    The default is TimeFormat::Approximate

    \sa setRemainingTimeFormat(), TimeFormat
*/
ProgressEstimate::TimeFormat ProgressEstimate::remainingTimeFormat() const
{
    return m_impl->m_remainingTimeFormat;
}

/*!
    Set format of remaining time estimate.

    \sa remainingTimeFormat(), TimeFormat
*/
void ProgressEstimate::setRemainingTimeFormat(ProgressEstimate::TimeFormat format) const
{
    if (m_impl->m_remainingTimeFormat != format)
    {
        m_impl->m_remainingTimeFormat = format;
        m_impl->updateWidgets();
    }
}

/*!
    Return label for elapsed time.

    The default is "Elapsed time".

    \sa setElapsedTimeText()
*/
QString ProgressEstimate::elapsedTimeText() const
{
    return m_impl->m_elapsedTimeLabel->text();
}

/*!
    Set label for elapsed time.

    \sa elapsedTimeText()
*/
void ProgressEstimate::setElapsedTimeText(const QString& text)
{
    m_impl->m_elapsedTimeLabel->setText(text);
}

/*!
    Return flag whether the elapsed time should be displayed.

    The default is false.

    \sa setElapsedTimeHidden()
*/
bool ProgressEstimate::isElapsedTimeHidden() const
{
    return m_impl->m_remainingTimeLabel->isHidden();
}

/*!
    Set flag whether the elapsed time should be displayed.

    \sa isElapsedTimeHidden()
*/
void ProgressEstimate::setElapsedTimeHidden(bool hide)
{
    m_impl->m_elapsedTimeLabel->setHidden(hide);
    m_impl->m_elapsedTimeText->setHidden(hide);
}

/*!
    Return format of elapsed time.

    The default is TimeFormat::Exact

    \sa setElapsedTimeFormat(), TimeFormat
*/
ProgressEstimate::TimeFormat ProgressEstimate::elapsedTimeFormat() const
{
    return m_impl->m_elapsedTimeFormat;
}

/*!
    Set format of elapsed time.

    \sa elapsedTimeFormat(), TimeFormat
*/
void ProgressEstimate::setElapsedTimeFormat(ProgressEstimate::TimeFormat format) const
{
    if (m_impl->m_elapsedTimeFormat != format)
    {
        m_impl->m_elapsedTimeFormat = format;
        m_impl->updateWidgets();
    }
}

/*!
    Reimplementation of ProgressWidget::setValue()
*/
void ProgressEstimate::setValue(int value, const QVariant&, const TimeStamp& timeStamp)
{
    m_impl->setValue(value, timeStamp);
}

/*!
    Reimplementation of ProgressWidget::setRange()
*/
void ProgressEstimate::setRange(int minimum, int maximum)
{
    m_impl->setRange(minimum, maximum);
}


};
