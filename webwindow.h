#ifndef WEBWINDOW_H
#define WEBWINDOW_H

#include <QMainWindow>
#include <QUrl>

namespace Ui {
class WebWindow;
}

class MainWindow;

class WebWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit WebWindow(QWidget *parent = 0);
    ~WebWindow();

private:
    Ui::WebWindow *ui;
    MainWindow *mainWindow;

    void initWeb();

private slots:
    void linkClickedWebView(QUrl);
};

#endif // WEBWINDOW_H
