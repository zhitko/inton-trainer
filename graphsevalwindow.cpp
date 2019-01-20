#include "graphsevalwindow.h"
#include "ui_graphswindow.h"

#include <QDebug>
#include <QMessageBox>
#include <QDir>
#include <QDesktopServices>
#include <QUrl>

#include "graphswindow.h"

#include "drawerdp.h"
#include "soundplayer.h"
#include "settingsdialog.h"

#include "xlsxdocument.h"

#include <QSettings>

extern "C" {
    #include "./analysis/metrics.h"
}

GraphsEvalWindow::GraphsEvalWindow(QString path, Drawer * drawer, QWidget *parent) :
    GraphsWindow(parent)
{
    this->templatePath = path;
    this->path = path;
    this->drawer = drawer;
    this->QMGL->setDraw(this->drawer);
    this->drawFile(path);
    this->ui->playTemplateBtn->hide();
    this->ui->saveMetrics->hide();
    this->ui->openMetrics->hide();
    connect(this->ui->playTemplateBtn, SIGNAL(clicked()), this, SLOT(playTemplate()));
    connect(this->ui->showUMP, SIGNAL(clicked()), this, SLOT(showUMP()));
    connect(this->ui->saveMetrics, SIGNAL(clicked()), this, SLOT(saveMetrics()));
    connect(this->ui->openMetrics, SIGNAL(clicked()), this, SLOT(openMetrics()));


    SPTK_SETTINGS * sptk_settings = SettingsDialog::getSPTKsettings();

    this->drawer->showUMP = sptk_settings->dp->showPortr;
    if(this->drawer->showUMP)
        this->ui->showUMP->setText("Show original");
    else
        this->ui->showUMP->setText("Show Universal Melodic Portrait (UMP)");
}

GraphsEvalWindow::~GraphsEvalWindow()
{

}

Drawer * GraphsEvalWindow::createNewDrawer(QString path)
{
    this->ui->saveMetrics->show();
    this->ui->playTemplateBtn->show();
    this->drawer->Proc(path);
    this->path = path;

    this->ui->totalC->setText(QString::number(this->drawer->proximity_curve_correlation));
    this->ui->totalI->setText(QString::number(this->drawer->proximity_curve_integral));
    this->ui->totalL->setText(QString::number(this->drawer->proximity_curve_local));
    this->ui->totalA->setText(QString::number(this->drawer->proximity_average));
    this->ui->totalPr->setText(QString::number(this->drawer->proximity_range));

    SPTK_SETTINGS * sptk_settings = SettingsDialog::getSPTKsettings();

    switch (sptk_settings->dp->errorType) {
    case 0:
        this->ui->totalC->setStyleSheet("font-weight: bold");
        this->ui->totalCtitle->setStyleSheet("font-weight: bold");
        break;
    case 1:
        this->ui->totalI->setStyleSheet("font-weight: bold");
        this->ui->totalItitle->setStyleSheet("font-weight: bold");
        break;
    case 2:
        this->ui->totalL->setStyleSheet("font-weight: bold");
        this->ui->totalLtitle->setStyleSheet("font-weight: bold");
        break;
    case 3:
        this->ui->totalA->setStyleSheet("font-weight: bold");
        this->ui->totalAtitle->setStyleSheet("font-weight: bold");
        break;
    default:
        break;
    }

    return this->drawer;
}

void GraphsEvalWindow::playTemplate()
{
    SoundPlayer * player = new SoundPlayer(templatePath);
    player->start();
}

void GraphsEvalWindow::showUMP()
{
    if(this->drawer->showUMP)
        this->ui->showUMP->setText("Show Universal Melodic Portrait (UMP)");
    else
        this->ui->showUMP->setText("Show original");
    this->drawer->showUMP = !this->drawer->showUMP;
    this->QMGL->update();
}

void GraphsEvalWindow::saveMetrics()
{
    qDebug() << "saveMetrics" << LOG_DATA;

    QXlsx::Document xlsx;

    QXlsx::Format format_title;
    format_title.setFontBold(true);
    format_title.setFontUnderline(QXlsx::Format::FontUnderlineSingle);

    QXlsx::Format format_subtitle;
    format_subtitle.setFontBold(true);

    xlsx.setColumnWidth(1, 40);
    xlsx.write(1, 1, "Template", format_subtitle);
    xlsx.write(2, 1, "Record", format_subtitle);

    xlsx.write(1, 2, this->templatePath);
    xlsx.write(2, 2, this->path);

    MetricsData data = this->drawer->getMetricData();

    xlsx.write(4, 1, "The data on the proximity", format_title);
    xlsx.write(5, 1, "Proximity curve correlation", format_subtitle);
    xlsx.write(5, 2, getMetric(data, METRIC_PROXIMITY_CURVE_CORRELATION));
    xlsx.write(6, 1, "Proximity curve integral", format_subtitle);
    xlsx.write(6, 2, getMetric(data, METRIC_PROXIMITY_CURVE_INTEGRAL));
    xlsx.write(7, 1, "Proximity curve local", format_subtitle);
    xlsx.write(7, 2, getMetric(data, METRIC_PROXIMITY_CURVE_LOCAL));
    xlsx.write(8, 1, "Average data on the proximity of curves", format_subtitle);
    xlsx.write(8, 2, getMetric(data, METRIC_PROXIMITY_AVERAGE));
    xlsx.write(9, 1, "Proximity shape", format_subtitle);
    xlsx.write(9, 2, getMetric(data, METRIC_PROXIMITY_CURVE_SHAPE));

    xlsx.write(11, 1, "Reference data on templates and records", format_title);
    xlsx.write(12, 1, "F0 max - Template", format_subtitle);
    xlsx.write(12, 2, getMetric(data, METRIC_TEMPLATE_F0_MAX));
    xlsx.write(13, 1, "F0 min - Template", format_subtitle);
    xlsx.write(13, 2, getMetric(data, METRIC_TEMPLATE_F0_MIN));
    xlsx.write(14, 1, "F0 max - Recorded", format_subtitle);
    xlsx.write(14, 2, getMetric(data, METRIC_RECORD_F0_MAX));
    xlsx.write(15, 1, "F0 min - Recorded", format_subtitle);
    xlsx.write(15, 2, getMetric(data, METRIC_RECORD_F0_MIN));
    xlsx.write(16, 1, "Mean Value UMP - Recorded", format_subtitle);
    xlsx.write(16, 2, getMetric(data, METRIC_MEAN_VALUE_UMP_RECORDED));
    xlsx.write(17, 1, "Mean Value UMP - Template", format_subtitle);
    xlsx.write(17, 2, getMetric(data, METRIC_MEAN_VALUE_UMP_TEMPLATE));
    xlsx.write(18, 1, "Root Mean Square UMP - Recorded", format_subtitle);
    xlsx.write(18, 2, getMetric(data, METRIC_RMS_UMP_RECORDED));
    xlsx.write(19, 1, "Root Mean Square UMP - Template", format_subtitle);
    xlsx.write(19, 2, getMetric(data, METRIC_RMS_UMP_TEMPLATE));
    xlsx.write(20, 1, "Center of Gravity UMP - Recorded", format_subtitle);
    xlsx.write(20, 2, getMetric(data, METRIC_CENTER_GRAVITY_UMP_RECORDED));
    xlsx.write(21, 1, "Center of Gravity UMP- Template", format_subtitle);
    xlsx.write(21, 2, getMetric(data, METRIC_CENTER_GRAVITY_UMP_TEMPLATE));

    xlsx.write(23, 1, "Relative data on templates and records", format_title);
    xlsx.write(24, 1, "Relative Diapason F0", format_subtitle);
    xlsx.write(24, 2, getMetric(data, METRIC_RELATIVE_DIAPASON_F0));
    xlsx.write(25, 1, "Relative Register F0", format_subtitle);
    xlsx.write(25, 2, getMetric(data, METRIC_RELATIVE_REGISTER_F0));
    xlsx.write(26, 1, "Relative Root Mean Square UMP(%)", format_subtitle);
    xlsx.write(26, 2, getMetric(data, METRIC_RELATIVE_RMS_UMP));
    xlsx.write(27, 1, "Relative Center of Gravity UMP", format_subtitle);
    xlsx.write(27, 2, getMetric(data, METRIC_RELATIVE_CENTER_GRAVITY_UMP));
    xlsx.write(28, 1, "Relative Root Mean Square sound level", format_subtitle);
    xlsx.write(28, 2, getMetric(data, METRIC_RELATEVE_RMS_VOLUME));
    xlsx.write(29, 1, "Relative voice sound duration", format_subtitle);
    xlsx.write(29, 2, getMetric(data, METRIC_RELATIVE_TEMPO));

    QDir templateDir(this->templatePath);
    QString templateName = templateDir.dirName().remove(".wav", Qt::CaseInsensitive);
    QString recordName = QDir(this->path).dirName().remove(".wav", Qt::CaseInsensitive);
    QString fileName  = templateName + " - " + recordName + ".xlsx";

    templateDir.cdUp();
    this->metricsFilePath = templateDir.absoluteFilePath(fileName);
    qDebug() << "save to" << this->metricsFilePath;

    bool isOk = xlsx.saveAs(this->metricsFilePath);

    if (isOk)
    {
        this->ui->openMetrics->show();
    }
}

void GraphsEvalWindow::openMetrics()
{
    QDesktopServices::openUrl(QUrl("file:///" + this->metricsFilePath, QUrl::TolerantMode));
}

