namespace APD
{
/*!
  \mainpage

  \section intro Introduction

  Although computers today have a number of cores and allow efficient asynchonous programming,
  some software developers still tend to keep time consuming operations in the GUI thread. The reason
  for this may be lack of experience with multi-threading programming or just pure lazyness.
  As a result, their applications are not responsive, which has a great impact on the usability
  as the application seems frozen to the user. The aim of this project is to provide developers
  with a simple-to-use toolbox for doing computations and long operations in a worker thread
  and thus leaving GUI thread free for interaction. The toolbox provide a few basic progress
  widgets to inform the user about the state of the operation and a simple way to cancel the
  long operation at some point by the user.

  This is a typical (and wrong) example of synchonous operation and a progress dialog in Qt:

  \snippet mainwindow.cpp SynchronousExample

  We generally want to avoid such code as the application become unresponsive during the
  synchonous operation. Using the AsyncProgressDialog library, one can easily transform
  the code into the following asynchronous form:

  \snippet mainwindow.cpp SimpleAsynchronousExample

  As you may have noticed, the amount of code is similar in both examples whereas the latter
  example gains the benefit from being run asynchronously and thus not blocking the GUI thread.
  The trick is to move lengthy and possibly blocking code as well as the updates of the progress
  into a lambda function, which is run on the background. The lambda function has one input
  parameter, which can be used for sending updates to the progress widget and checking for cancel
  requests. Usage of this object is fully thread-safe as internally Qt::QueuedConnection is used
  to connect thread signals slots of progress widgets.

  \section io-values Input and output values

  Input values can be sent into the lambda function using the capture statemet. The user is required
  to handle possible race conditions. In the simple example with one GUI and one worker no race conditions
  are likely to happen as the GUI thread waits in QDialog::exec() method until the worker thread has finished.
  In case or multiple threads the user has to protect shared resources using mutexes or other synchronization
  technique. Another option is to simply pass copies into lambdas if the copy is cheap.

  Output values can be returned by return statement of the lambda function and accessed using APD::FunctionThread
  object returned from APD::AsyncProgressDialog::addTask() method, see the example below.

  \snippet mainwindow.cpp TaskResultExample

  \section task-thread Task thread

  APD::AsynchProgressDialog is capable of running mutiple threads. Each thread is associated with
  a progress widget or a collection of progress widgets and the progress is updated automatically
  from within the thread. User may supply their own implementation of APD::TaskThread or use
  the lambda functions as decribed above.

  \section progress-widgets Progress Widgets

  By default, the dialog displays a simple progress bar (APD::ProgressBar) for each task added to the dialog.
  However, AsynchProgressDialog offers multiple progress widgets, which are capable of displaying not only
  progress but also text (APD::ProgressLabel and APD::ProgressOutput), velocity plot (APD::ProgressVelocityPlot),
  or time estimates (APD::ProgressEstimate). The widgets can be combined together into a composite progress
  widget displaying progress from a single thread in APD::ProgressWidgetContainer. A factory is provided, which
  is capable of creating some popular combinations of progress widgets.

  \section examples Examples

  This section shows a few exmaples of what is the AsyncProgressDialog library capable of.

  \subsection example-progress-label Progress label

  \image html ProgressLabelExample.png

  The following code shows a progress dialog with a progress label. The label is set from within
  the task thread and may tell the user details about ongoing operation, for example, the name of the file,
  which is being copied.

  \snippet mainwindow.cpp ProgressLabelExample

  \subsection example-progress-estimate Progress estimate

  \image html ProgressEstimateExample.png

  Progress estimate widget is capable of automatically calculating and displaying estimated remaining time.
  The estimate works best if calls to setValue() method in the thread are evenly distributed.

  \snippet mainwindow.cpp ProgressEstimateExample

  \subsection example-velocity Velocity example

  \image html VelocityExample.png

  Velocity bar is somewhat similar to windows 7 copy file progress widget. It shows progress as well as
  current velocity and velocity history plot.

  \snippet mainwindow.cpp VelocityExample

  \subsection example-multi-progress Mutiple progress bars example

  \image html MultiProgressExample.png

  The following code shows how the progress dialog can cope with mutlitple threads showing a progress bar
  for each task as wel as a progress bar for total progress.

  \snippet mainwindow.cpp MultiProgressExample


  \copyright Copyright 2019 Ondrej Polacek. All rights reserved.
*/
}
