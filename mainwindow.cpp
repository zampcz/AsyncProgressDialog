#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "AsyncProgressDialog.h"

#include <cmath>

#include <QProgressDialog>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_test0_clicked()
{
    //! [SynchronousExample]
    QProgressDialog dlg(this);
    dlg.setLabelText("Progress");
    dlg.setRange(0, 20);
    dlg.setWindowModality(Qt::WindowModal);
    dlg.show();
    for (int i = 0; i < 20; i++)
    {
        dlg.setValue(i);
        if (dlg.wasCanceled())
            break;
        QThread::msleep(500);   // do a lengthy operation
    }
    //! [SynchronousExample]
}

void MainWindow::on_test1_clicked()
{
    //! [SimpleAsynchronousExample]
    APD::AsyncProgressDialog adlg;

    adlg.setLabelText("Progress");
    adlg.addTask([](auto thread)
    {
        thread->setRange(0, 10);
        for (int i = 0; i < 10; i++)
        {
            thread->setValue(i);
            if (thread->isCanceled())
                break;
            QThread::msleep(1000);  // do a lengthy operation
        }
        thread->setValue(10);
    });

    adlg.exec();
    //! [SimpleAsynchronousExample]
}

void MainWindow::on_test2_clicked()
{
    //! [ProgressLabelExample]
    APD::AsyncProgressDialog adlg;

    adlg.addTask([](APD::TaskThread* thread) {
        thread->setRange(0, 10);
        for (int i = 0; i < 10; i++)
        {
            thread->setValue(i);
            thread->setText(QString("Processing %1/10 ...").arg(i+1));
            if (thread->isCanceled())
                break;
            QThread::msleep(1000);
        }
        thread->setValue(10);
    }, APD::ProgressWidgetFactory::createProgressBar("Progress", APD::AdditionalWidget::Label));

    adlg.exec();
    //! [ProgressLabelExample]
}

void MainWindow::on_test3_clicked()
{
    //! [ProgressEstimateExample]
    APD::AsyncProgressDialog adlg;

    adlg.addTask([](APD::TaskThread* thread) {
        thread->setRange(0, 1000);
        for (int i = 0; i < 1000; i++)
        {
            thread->setValue(i);
            if (i % 10 == 0)
                thread->setText(QString("Processing %1/100 ...").arg(i/10 + 1));
            if (thread->isCanceled())
                break;
            QThread::msleep(100);
        }
        thread->setValue(10);
    }, APD::ProgressWidgetFactory::createProgressBar(APD::AdditionalWidget::Estimate | APD::AdditionalWidget::Output));

    adlg.exec();
    //! [ProgressEstimateExample]
}

void MainWindow::on_test4_clicked()
{
    //! [VelocityExample]
    APD::AsyncProgressDialog adlg;

    adlg.addTask([](APD::TaskThread* thread) {
        thread->setRange(0, 150);
        for (int i = 0; i < 150; i++)
        {
            thread->setValue(i, 2 + std::abs(2 * std::sin(i / 10.0)));
            if (thread->isCanceled())
                break;
            QThread::msleep(80);
        }
        thread->setValue(150);
    }, APD::ProgressWidgetFactory::createVelocityBar("kB"));

    adlg.exec();
    //! [VelocityExample]
}


class MyThread : public APD::TaskThread
{
public:
    MyThread(int steps, unsigned sleep, QObject* parent = nullptr)
        : TaskThread(parent)
        , m_steps(steps)
        , m_sleep(sleep)
    {}

protected:
    void run() override
    {
        setRange(0, m_steps);
        for (int i = 0; i < m_steps; i++)
        {
            setValue(i);
            if (isCanceled())
                break;
            QThread::msleep(m_sleep);
        }
        setValue(m_steps);
    }

private:
    Q_DISABLE_COPY(MyThread)

    int m_steps;
    unsigned m_sleep;
};


void MainWindow::on_test5_clicked()
{
    //! [MultiProgressExample]
    APD::AsyncProgressDialog adlg;

    adlg.addTask(new MyThread(10, 1000, &adlg), APD::ProgressWidgetFactory::createProgressBar("Progress 1"));
    adlg.addTask(new MyThread(20, 500, &adlg), APD::ProgressWidgetFactory::createProgressBar("Progress 2"));
    adlg.addTask(new MyThread(80, 100, &adlg), APD::ProgressWidgetFactory::createProgressBar("Progress 3"));
    adlg.addTask(new MyThread(120, 50, &adlg), APD::ProgressWidgetFactory::createProgressBar("Progress 4"));
    adlg.addTask(new MyThread(6, 2000, &adlg), APD::ProgressWidgetFactory::createProgressBar("Progress 5"));

    adlg.setOverallProgress(true);
    adlg.exec();
    //! [MultiProgressExample]
}


void MainWindow::on_pushButton_clicked()
{
    //! [TaskResultExample]
    APD::AsyncProgressDialog adlg;

    auto thread = adlg.addTask([](auto thread) {
        thread->setRange(0, 10);
        // The thread is going to calculate a summary and then return it
        int sum = 0;
        for (int i = 0; i < 10; i++)
        {
            thread->setValue(i);
            if (thread->isCanceled())
                break;
            sum += i;   // Calculate sum
            QThread::msleep(100);
        }
        thread->setValue(10);
        sum += 10;  // Calculate sum
        return sum; // Return sum variable, it can be later accesed using thread->result() method
    });

    // Task result is not ready yet.
    // Calling FunctionalThread::result() is thread safe.
    assert(thread->result() == std::nullopt);

    adlg.exec();

    // Now the task has finished, we can safely access its result
    QMessageBox::information(this, QString("Resut"), QString("Result is %1").arg(*thread->result()));
    //! [TaskResultExample]
}
