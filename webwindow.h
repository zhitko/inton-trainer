#ifndef WEBWINDOW_H
#define WEBWINDOW_H

#include <QMainWindow>
#include <QUrl>
#include <QWebFrame>

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

public slots:
    void attachObject();
    QString getFiles();
};

#endif // WEBWINDOW_H
