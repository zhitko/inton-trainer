#ifndef WEBWINDOW_H
#define WEBWINDOW_H

#include "defines.h"

#include <QMainWindow>
#include <QUrl>
#include <QVariant>

namespace Ui {
class WebWindow;
}

class MainWindow;
class WebApi;

class WebWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit WebWindow(QWidget *parent = 0);
    ~WebWindow();

private:
    Ui::WebWindow *ui;
    MainWindow *mainWindow;
    WebApi *webApi;

    void initWeb();

public slots:
    void attachObject();
    bool isShowA0();
    bool isShowF0();
    bool isShowError();
    bool isShowTime();
    void setShowA0(QVariant);
    void setShowF0(QVariant);
    void setShowError(QVariant);
    void setShowTime(QVariant);
    void showSettings();
    void openFile(QVariant);
    void playFile(QVariant);
    void openApp();
};

#endif // WEBWINDOW_H
