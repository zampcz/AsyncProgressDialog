#include "ProgressWidgetContainer.h"

#include <QGridLayout>

namespace APD
{

class ProgressWidgetContainer::Impl
{
    friend class ProgressWidgetContainer;

public:
    Impl(ProgressWidgetContainer* parent);

private:
    QGridLayout* m_layout;
    QList<ProgressWidget*> m_progressWidgets;
    ProgressWidgetContainer* m_parent;
};

ProgressWidgetContainer::Impl::Impl(ProgressWidgetContainer* parent)
    : m_parent(parent)
{
    m_layout = new QGridLayout(parent);
    m_layout->setContentsMargins(0, 0, 0, 0);
}


/*!
    \class ProgressWidgetContainer
    \brief A progress widget, which is a container for other progress widget.

    Internally, it creates a QGridLayout and places addition widgets there. If the
    used adds a progress widget, the corresponding signals and slots are connected
    in order to keep the child widgets updated. The ProgressWidgetContainer forwards
    those signals to its child progress widgets.
*/

/*!
    Constructs a progress widget container with the given \a parent.
*/
ProgressWidgetContainer::ProgressWidgetContainer(QWidget *parent)
    : ProgressWidget(parent)
    , m_impl(std::make_unique<Impl>(this))
{
}

ProgressWidgetContainer::~ProgressWidgetContainer() = default;

/*!
    Adds the given progress \a widget to the cell grid at \a row, \a column and
    starts forwarding progress events.
    See QGridLayout::addWidget() method for detailed description.

    Note that the given \a widget is reparented.

    To add a QWidget or QLayoutItem, which doesn't inherit from ProgressWidget,
    please use the layout() method and add the item directly into the layout.
*/
void ProgressWidgetContainer::addWidget(ProgressWidget* widget, int row, int column, Qt::Alignment alignment)
{
    addWidget(widget, row, column, 1, 1, alignment);
}

/*!
    \overload

    See QGridLayout::addWidget() method for detailed description.

    Note that the given \a widget is reparented.
 */
void ProgressWidgetContainer::addWidget(ProgressWidget* widget, int row, int column, int rowSpan, int columnSpan, Qt::Alignment alignment)
{
    assert(m_impl->m_layout->itemAtPosition(row, column) == nullptr);
    widget->setParent(this);
    m_impl->m_layout->addWidget(widget, row, column, rowSpan, columnSpan, alignment);
    m_impl->m_progressWidgets.push_back(widget);
}

/*!
    Removes a progress widget from the container and stop forwarding
    progress events.
*/
void ProgressWidgetContainer::removeWidget(ProgressWidget* widget)
{
    m_impl->m_progressWidgets.removeAll(widget);
    m_impl->m_layout->removeWidget(widget);
}

/*!
    Returns the underlying QGridLayout, which can be accessed to direcly
    to add layout items and widgets, which don't inherit from ProgressWidget.
*/
QGridLayout* ProgressWidgetContainer::gridLayout() const
{
    return m_impl->m_layout;
}

/*!
  This reimplemented method calls ProgressWidget::setValue() method of all contained progress widgets.
*/
void ProgressWidgetContainer::setValue(int value, const QVariant& userData, const TimeStamp& timeStamp)
{
    for (auto& widget : m_impl->m_progressWidgets)
        widget->setValue(value, userData, timeStamp);
}

/*!
  This reimplemented method calls ProgressWidget::setRange() method of all contained progress widgets.
*/
void ProgressWidgetContainer::setRange(int minimum, int maximum)
{
    for (auto& widget : m_impl->m_progressWidgets)
        widget->setRange(minimum, maximum);
}

/*!
  This reimplemented method calls ProgressWidget::setText() method of all contained progress widgets.
*/
void ProgressWidgetContainer::setText(const QString& text)
{
    for (auto& widget : m_impl->m_progressWidgets)
        widget->setText(text);
}


}
