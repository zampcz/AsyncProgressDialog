#include "ProgressVelocityPlot.h"

#include <QChart>
#include <QChartView>
#include <QLineSeries>
#include <QAreaSeries>
#include <QGraphicsSimpleTextItem>
#include <QHBoxLayout>

namespace APD
{

class ProgressVelocityPlot::Impl
{
    friend class ProgressVelocityPlot;

public:
    Impl(const QString& quantityUnits, ProgressVelocityPlot* parent);

    void setValue(int value, const QVariant& userData, const TimeStamp& timeStamp);
    void setRange(int minimum, int maximum);

private:
    std::chrono::time_point<std::chrono::steady_clock> m_lastTimeStamp;
    bool m_initialized = false;

    QtCharts::QChart* m_chart;
    QtCharts::QAreaSeries* m_progressSeries;
    QtCharts::QAreaSeries* m_velocitySeries;
    QtCharts::QLineSeries* m_currentVelocitySeries;
    QGraphicsSimpleTextItem* m_currentVelocity;
    double m_maxVelocity = 0;

    int m_minimum = 0;
    int m_maximum = 0;
    QString m_quantityUnits;
};

ProgressVelocityPlot::Impl::Impl(const QString& quantityUnits, ProgressVelocityPlot* parent)
    : m_quantityUnits(quantityUnits)
{
    using namespace QtCharts;

    static QColor s_green(167, 229, 145);
    static QColor s_darkGreen(6, 176, 37);

    m_progressSeries = new QAreaSeries(new QLineSeries());
    m_progressSeries->setPen(QPen(s_green, 0));
    m_progressSeries->setBrush(QBrush(s_green));

    m_velocitySeries = new QAreaSeries(new QLineSeries());
    m_velocitySeries->setPen(QPen(s_darkGreen, 0));
    m_velocitySeries->setBrush(QBrush(s_darkGreen));

    m_currentVelocitySeries = new QLineSeries();
    m_currentVelocitySeries->setPen(QPen(Qt::black, 0));

    m_chart = new QChart();
    m_chart->addSeries(m_progressSeries);
    m_chart->addSeries(m_velocitySeries);
    m_chart->addSeries(m_currentVelocitySeries);
    m_chart->legend()->setVisible(false);
    m_chart->createDefaultAxes();
    m_chart->axes(Qt::Horizontal).first()->setVisible(false);
    m_chart->axes(Qt::Vertical).first()->setVisible(false);
    m_chart->setMargins(QMargins(0,0,0,0));
    m_chart->setBackgroundPen(QPen(Qt::black, 0));
    m_chart->setBackgroundRoundness(0);

    m_currentVelocity = new QGraphicsSimpleTextItem(m_chart);
    m_currentVelocity->setZValue(10);

    auto *chartView = new QChartView(m_chart, parent);

    auto layout = new QHBoxLayout(parent);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(chartView);
}

void ProgressVelocityPlot::Impl::setValue(int value, const QVariant& userData, const TimeStamp& timeStamp)
{
    using namespace std::chrono;

    if (!m_initialized)
    {
        m_lastTimeStamp = timeStamp;
        m_initialized = true;
        return;
    }

    bool ok;
    auto quantity = userData.toDouble(&ok);
    auto elapsedTime = duration_cast<milliseconds>(timeStamp - m_lastTimeStamp);
    if (elapsedTime.count() > 0 && ok)
    {
        // compute the velocity, multiplied 1000 to convert from milliseconds to seconds
        auto velocity = (1000 * quantity) / elapsedTime.count();
        m_velocitySeries->upperSeries()->append(value, velocity);
        if (velocity > m_maxVelocity)
        {
            m_maxVelocity = velocity;
            m_chart->axes(Qt::Vertical).first()->setRange(0, m_maxVelocity * 1.1);
        }

        if (m_velocitySeries->upperSeries()->count() > 1)
        {
            // update progress series
            m_progressSeries->upperSeries()->clear();
            m_progressSeries->upperSeries()->append(m_minimum, m_maxVelocity * 1.1);
            m_progressSeries->upperSeries()->append(value, m_maxVelocity * 1.1);

            // update current velocity series
            m_currentVelocitySeries->clear();
            m_currentVelocitySeries->append(m_minimum, velocity);
            m_currentVelocitySeries->append(m_maximum, velocity);

            QString units;
            if (!m_quantityUnits.isEmpty())
                units = QString(" %1/s").arg(m_quantityUnits);
            m_currentVelocity->setText(QString("%1%2").arg(velocity, 0, 'g', 3).arg(units));
            QFontMetrics fm(m_currentVelocity->font());

            auto point = m_chart->mapToPosition(QPointF(m_maximum, velocity), m_currentVelocitySeries);
            auto pos = point - QPointF(fm.width(m_currentVelocity->text()) + 3, fm.height());
            if (pos.y() < 10)
                pos.ry() += fm.height();
            m_currentVelocity->setPos(pos);
        }
    }

    m_lastTimeStamp = timeStamp;
}

void ProgressVelocityPlot::Impl::setRange(int minimum, int maximum)
{
    m_minimum = minimum;
    m_maximum = maximum;
    m_chart->axes(Qt::Horizontal).first()->setRange(minimum + 1, maximum - 1);
}


/*!
    \class ProgressVelocityPlot
    \brief A widget capable of showing current velocity as well as the velocity history.

    The widget displays a plot, which shows current velocity, velocity history and current
    progress. Note that the velocity can be shown after the second update to progress
    value as the time is measured from the first call to setValue().

    The velocity is computed from the quantity passed as the second parameter in the
    setValue() method and from the elapsed time between this and the previous call to
    setValue() method. The velocity unit is displayed in the form numerator/denominator,
    where denominator is always seconds (s) and numerator is set by the quantityUnits()
    method. If quantity units are empty, no velocity unit is shown.
*/

/*!
    Constructs a progress velocity plot with the given \a quantityUnits and \a parent.
*/
ProgressVelocityPlot::ProgressVelocityPlot(const QString& quantityUnits, QWidget *parent)
    : ProgressWidget(parent)
    , m_impl(std::make_unique<Impl>(quantityUnits, this))
{
}

/*!
    Constructs a progress velocity plot with the given \a parent.
*/
ProgressVelocityPlot::ProgressVelocityPlot(QWidget *parent)
    : ProgressVelocityPlot(QString(), parent)
{
}

ProgressVelocityPlot::~ProgressVelocityPlot() = default;


/*!
    Reimplementation of ProgressWidget::setValue()
*/
void ProgressVelocityPlot::setValue(int value, const QVariant& userData, const TimeStamp& timeStamp)
{
    m_impl->setValue(value, userData, timeStamp);
}

/*!
    Reimplementation of ProgressWidget::setRange()
*/
void ProgressVelocityPlot::setRange(int minimum, int maximum)
{
    m_impl->setRange(minimum, maximum);
}

/*!
    Return quantity units. The velocity units
    are the composed from the quantity units and per second
    suffix. For instance, if the quantity units are Mb, the
    velocity units are Mb/s.

    The default is an empty string and thus no velocity units
    are displayed by default.

    \sa setQuantityUnits()
*/
QString ProgressVelocityPlot::quantityUnits() const
{
    return m_impl->m_quantityUnits;
}

/*!
    Set quantity units
    \sa quantityUnits()
*/
void ProgressVelocityPlot::setQuantityUnits(const QString& quantityUnits)
{
    m_impl->m_quantityUnits = quantityUnits;
}

/*!
    Return the flag whether the progress visualization should be displayed.

    The default is false.
    \sa setProgressHidden()
*/
bool ProgressVelocityPlot::isProgressHidden() const
{
    return !m_impl->m_progressSeries->isVisible();
}

/*!
    Set the progress hidden flag.
    \sa isProgressHidden()
*/
void ProgressVelocityPlot::setProgressHidden(bool hide)
{
    m_impl->m_progressSeries->setVisible(!hide);
}

/*!
    Return the flag whether the velocity history
    visualization should be displayed.

    The default is false.
    \sa setVelocityHistoryHidden
*/
bool ProgressVelocityPlot::isVelocityHistoryHidden() const
{
    return !m_impl->m_velocitySeries->isVisible();
}

/*!
    Set the velocity history hidden flag.

    The default is false.
    \sa isVelocityHistoryHidden
*/
void ProgressVelocityPlot::setVelocityHistoryHidden(bool hide)
{
    m_impl->m_velocitySeries->setVisible(!hide);
}

/*!
    Return the flag whether the current velocity
    visualization should be displayed.

    The default is false.
    \sa setCurrentVelocityHidden()
*/
bool ProgressVelocityPlot::isCurrentVelocityHidden() const
{
    return !m_impl->m_currentVelocitySeries->isVisible();
}

/*!
    Set the current velocity hidden flag.

    The default is false.
    \sa isCurrentVelocityHidden()
*/
void ProgressVelocityPlot::setCurrentVelocityHidden(bool hide)
{
    m_impl->m_currentVelocity->setVisible(!hide);
    m_impl->m_currentVelocitySeries->setVisible(!hide);
}

/*!
    Return the pen of progress visualization.

    The default is Qt::green.
    \sa setProgressPen()
*/
QPen ProgressVelocityPlot::progressPen() const
{
    return m_impl->m_progressSeries->pen();
}

/*!
    Set progress pen.
    \sa progressPen()
*/
void ProgressVelocityPlot::setProgressPen(const QPen& pen)
{
    m_impl->m_progressSeries->setPen(pen);
}

/*!
    Return the brush of progress visualization.

    The default is Qt::green.
    \sa setProgressBrush()
*/
QBrush ProgressVelocityPlot::progressBrush() const
{
    return m_impl->m_progressSeries->brush();
}

/*!
    Set the progress brush.
    \sa progressBrush()
*/
void ProgressVelocityPlot::setProgressBrush(const QBrush& brush)
{
    m_impl->m_progressSeries->setBrush(brush);
}

/*!
    Return the pen of current velocity visualization.

    The default is Qt::black.
    \sa setCurrentVelocityPen()
*/
QPen ProgressVelocityPlot::currentVelocityPen() const
{
    return m_impl->m_currentVelocitySeries->pen();
}

/*!
    Set the current velocity pen.
    \sa currentVelocityPen()
*/
void ProgressVelocityPlot::setCurrentVelocityPen(const QPen& pen)
{
    m_impl->m_currentVelocitySeries->setPen(pen);
    m_impl->m_currentVelocity->setPen(pen);
}

/*!
    Return the pen of velocity history visualization.

    The default is Qt::darkGreen.
    \sa setVelocityHistoryPen()
*/
QPen ProgressVelocityPlot::velocityHistoryPen() const
{
    return m_impl->m_velocitySeries->pen();
}

/*!
    Set the velocity history pen.
    \sa velocityHistoryPen()
*/
void ProgressVelocityPlot::setVelocityHistoryPen(const QPen& pen)
{
    return m_impl->m_velocitySeries->setPen(pen);
}

/*!
    Return the brush of velocity history visualization.

    The default is Qt::darkGreen.
    \sa setVelocityHistoryBrush()
*/
QBrush ProgressVelocityPlot::velocityHistoryBrush() const
{
    return m_impl->m_velocitySeries->brush();
}

/*!
    Set the velocity history brush.
    \sa velocityHistoryBrush()
*/
void ProgressVelocityPlot::setVelocityHistoryBrush(const QBrush& brush)
{
    m_impl->m_velocitySeries->setBrush(brush);
}


}
