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
    bool isShowA0();
    bool isShowF0();
    bool isShowError();
    bool isShowTime();
    void setShowA0(QVariant);
    void setShowF0(QVariant);
    void setShowError(QVariant);
    void setShowTime(QVariant);
};

#endif // WEBWINDOW_H
