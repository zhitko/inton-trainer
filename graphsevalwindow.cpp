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
    this->ui->playBtn->hide();
    this->ui->playTemplateBtn->show();
    this->ui->playRecordBtn->hide();
    this->ui->saveMetrics->show();
    this->ui->openMetrics->hide();
    connect(this->ui->playTemplateBtn, SIGNAL(clicked()), this, SLOT(playTemplate()));
    connect(this->ui->playRecordBtn, SIGNAL(clicked()), this, SLOT(playRecord()));
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
    this->ui->playRecordBtn->show();
    this->ui->openMetrics->hide();
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

    QXlsx::Format format_value;
    format_value.setHorizontalAlignment(QXlsx::Format::AlignLeft);

    QXlsx::Format format_title;
    format_title.setFontBold(true);
    format_title.setFontUnderline(QXlsx::Format::FontUnderlineSingle);

    QXlsx::Format format_subtitle;
    format_subtitle.setFontBold(true);
    format_subtitle.setVerticalAlignment(QXlsx::Format::AlignTop);

    QXlsx::Format format_file_subtitle;
    format_file_subtitle.setVerticalAlignment(QXlsx::Format::AlignTop);
    format_file_subtitle.setFontItalic(true);
    format_file_subtitle.setTextWarp(true);

    xlsx.setColumnWidth(1, 40);
    xlsx.setColumnWidth(2, 20);
    xlsx.setColumnWidth(3, 20);
    xlsx.mergeCells(QXlsx::CellRange(1,2,1,6));
    xlsx.mergeCells(QXlsx::CellRange(2,2,2,6));
    xlsx.setRowHeight(1, 50);
    xlsx.setRowHeight(2, 50);

    int row = 1;
    xlsx.write(row,   1, "Template", format_subtitle);
    xlsx.write(row++, 2, this->templatePath, format_file_subtitle);
    xlsx.write(row,   1, "Record", format_subtitle);
    xlsx.write(row++, 2, this->path, format_file_subtitle);

    MetricsData data = this->drawer->getMetricData();

    row++;
    xlsx.write(row,   1, "The data on the proximity", format_title);
    xlsx.write(row,   2, "Proximity", format_title);
    xlsx.write(row++, 3, "Distance", format_title);
    if (hasMetric(data, METRIC_PROXIMITY_CURVE_CORRELATION))
    {
        xlsx.write(row,   1, "Proximity thru curve 'Correlation'", format_subtitle);
        xlsx.write(row,   2, getMetric(data, METRIC_PROXIMITY_CURVE_CORRELATION), format_value);
        xlsx.write(row++, 3, 100.0 - getMetric(data, METRIC_PROXIMITY_CURVE_CORRELATION), format_value);
    }
    if (hasMetric(data, METRIC_PROXIMITY_CURVE_INTEGRAL))
    {
        xlsx.write(row,   1, "Proximity thru curve 'Integral'", format_subtitle);
        xlsx.write(row,   2, getMetric(data, METRIC_PROXIMITY_CURVE_INTEGRAL), format_value);
        xlsx.write(row++, 3, 100.0 - getMetric(data, METRIC_PROXIMITY_CURVE_INTEGRAL), format_value);
    }
    if (hasMetric(data, METRIC_PROXIMITY_CURVE_LOCAL))
    {
        xlsx.write(row,   1, "Proximity thru curve 'Local'", format_subtitle);
        xlsx.write(row,   2, getMetric(data, METRIC_PROXIMITY_CURVE_LOCAL), format_value);
        xlsx.write(row++, 3, 100.0 - getMetric(data, METRIC_PROXIMITY_CURVE_LOCAL), format_value);
    }
    if (hasMetric(data, METRIC_PROXIMITY_AVERAGE))
    {
        xlsx.write(row,   1, "Average of the three above proximities ", format_subtitle);
        xlsx.write(row,   2, getMetric(data, METRIC_PROXIMITY_AVERAGE), format_value);
        xlsx.write(row++, 3, 100.0 - getMetric(data, METRIC_PROXIMITY_AVERAGE), format_value);
    }
    if (hasMetric(data, METRIC_PROXIMITY_RANGE))
    {
        xlsx.write(row,   1, "Proximity thru 'Range'", format_subtitle);
        xlsx.write(row,   2, getMetric(data, METRIC_PROXIMITY_RANGE), format_value);
        xlsx.write(row++, 3, 100.0 - getMetric(data, METRIC_PROXIMITY_RANGE), format_value);
    }

    row++;
    xlsx.write(row++, 1, "Reference data on templates", format_title);
    if (hasMetric(data, METRIC_TEMPLATE_F0_MAX))
    {
        xlsx.write(row,   1, "F0 max [Hz]", format_subtitle);
        xlsx.write(row++, 2, getMetric(data, METRIC_TEMPLATE_F0_MAX), format_value);
    }
    if (hasMetric(data, METRIC_TEMPLATE_F0_MIN))
    {
        xlsx.write(row,   1, "F0 min  [Hz]", format_subtitle);
        xlsx.write(row++, 2, getMetric(data, METRIC_TEMPLATE_F0_MIN), format_value);
    }
    if (hasMetric(data, METRIC_DIAPASON_F0_TEMPLATE))
    {
        xlsx.write(row,   1, "Diapason F0", format_subtitle);
        xlsx.write(row++, 2, getMetric(data, METRIC_DIAPASON_F0_TEMPLATE), format_value);
    }
    if (hasMetric(data, METRIC_REGISTER_F0_TEMPLATE))
    {
        xlsx.write(row,   1, "Register F0 [Hz]", format_subtitle);
        xlsx.write(row++, 2, getMetric(data, METRIC_REGISTER_F0_TEMPLATE), format_value);
    }
    if (hasMetric(data, METRIC_MEAN_VALUE_UMP_TEMPLATE))
    {
        xlsx.write(row,   1, "Mean Value of the  curve  NMP", format_subtitle);
        xlsx.write(row++, 2, getMetric(data, METRIC_MEAN_VALUE_UMP_TEMPLATE), format_value);
    }
    if (hasMetric(data, METRIC_CENTER_GRAVITY_UMP_TEMPLATE_MID))
    {
        xlsx.write(row,   1, "Center of the curve  NMP", format_subtitle);
        xlsx.write(row++, 2, getMetric(data, METRIC_CENTER_GRAVITY_UMP_TEMPLATE_MID), format_value);
    }
    if (hasMetric(data, METRIC_CENTER_GRAVITY_UMP_TEMPLATE_LENGHT))
    {
        xlsx.write(row,   1, "Width of the curve  NMP", format_subtitle);
        xlsx.write(row++, 2, getMetric(data, METRIC_CENTER_GRAVITY_UMP_TEMPLATE_LENGHT), format_value);
    }
    if (hasMetric(data, METRIC_MEAN_VALUE_UMP_DERIVATIVE_TEMPLATE))
    {
        xlsx.write(row,   1, "Mean Value of the Derivative curve NMP", format_subtitle);
        xlsx.write(row++, 2, getMetric(data, METRIC_MEAN_VALUE_UMP_DERIVATIVE_TEMPLATE), format_value);
    }
    if (hasMetric(data, METRIC_CENTER_GRAVITY_UMP_DERIVATIVE_TEMPLATE_MID))
    {
        xlsx.write(row,   1, "Center of the Derivative curve NMP", format_subtitle);
        xlsx.write(row++, 2, getMetric(data, METRIC_CENTER_GRAVITY_UMP_DERIVATIVE_TEMPLATE_MID), format_value);
    }
    if (hasMetric(data, METRIC_CENTER_GRAVITY_UMP_DERIVATIVE_TEMPLATE_LENGHT))
    {
        xlsx.write(row,   1, "Width of the Derivative curve NMP", format_subtitle);
        xlsx.write(row++, 2, getMetric(data, METRIC_CENTER_GRAVITY_UMP_DERIVATIVE_TEMPLATE_LENGHT), format_value);
    }
    if (hasMetric(data, METRIC_MEAN_VOLUME_TEMPLATE))
    {
        xlsx.write(row,   1, "Voiced Sounds Level", format_subtitle);
        xlsx.write(row++, 2, getMetric(data, METRIC_MEAN_VOLUME_TEMPLATE), format_value);
    }
    if (hasMetric(data, METRIC_TEMPO_TEMPLATE))
    {
        xlsx.write(row,   1, "Voiced Sounds Duration", format_subtitle);
        xlsx.write(row++, 2, getMetric(data, METRIC_TEMPO_TEMPLATE), format_value);
    }

    row++;
    xlsx.write(row++, 1, "Reference data on Records", format_title);
    if (hasMetric(data, METRIC_RECORD_F0_MAX))
    {
        xlsx.write(row,   1, "F0 max [Hz]", format_subtitle);
        xlsx.write(row++, 2, getMetric(data, METRIC_RECORD_F0_MAX), format_value);
    }
    if (hasMetric(data, METRIC_RECORD_F0_MIN))
    {
        xlsx.write(row,   1, "F0 min  [Hz]", format_subtitle);
        xlsx.write(row++, 2, getMetric(data, METRIC_RECORD_F0_MIN), format_value);
    }
    if (hasMetric(data, METRIC_DIAPASON_F0_RECORDED))
    {
        xlsx.write(row,   1, "Diapason F0", format_subtitle);
        xlsx.write(row++, 2, getMetric(data, METRIC_DIAPASON_F0_RECORDED), format_value);
    }
    if (hasMetric(data, METRIC_REGISTER_F0_RECORDED))
    {
        xlsx.write(row,   1, "Register F0 [Hz]", format_subtitle);
        xlsx.write(row++, 2, getMetric(data, METRIC_REGISTER_F0_RECORDED), format_value);
    }
    if (hasMetric(data, METRIC_MEAN_VALUE_UMP_RECORDED))
    {
        xlsx.write(row,   1, "Mean Value of the  curve  NMP", format_subtitle);
        xlsx.write(row++, 2, getMetric(data, METRIC_MEAN_VALUE_UMP_RECORDED), format_value);
    }
    if (hasMetric(data, METRIC_CENTER_GRAVITY_UMP_RECORDED_MID))
    {
        xlsx.write(row,   1, "Center of the curve  NMP", format_subtitle);
        xlsx.write(row++, 2, getMetric(data, METRIC_CENTER_GRAVITY_UMP_RECORDED_MID), format_value);
    }
    if (hasMetric(data, METRIC_CENTER_GRAVITY_UMP_RECORDED_LENGHT))
    {
        xlsx.write(row,   1, "Width of the curve  NMP", format_subtitle);
        xlsx.write(row++, 2, getMetric(data, METRIC_CENTER_GRAVITY_UMP_RECORDED_LENGHT), format_value);
    }
    if (hasMetric(data, METRIC_MEAN_VALUE_UMP_DERIVATIVE_RECORDED))
    {
        xlsx.write(row,   1, "Mean Value of the Derivative curve NMP", format_subtitle);
        xlsx.write(row++, 2, getMetric(data, METRIC_MEAN_VALUE_UMP_DERIVATIVE_RECORDED), format_value);
    }
    if (hasMetric(data, METRIC_CENTER_GRAVITY_UMP_DERIVATIVE_RECORDED_MID))
    {
        xlsx.write(row,   1, "Center of the Derivative curve NMP", format_subtitle);
        xlsx.write(row++, 2, getMetric(data, METRIC_CENTER_GRAVITY_UMP_DERIVATIVE_RECORDED_MID), format_value);
    }
    if (hasMetric(data, METRIC_CENTER_GRAVITY_UMP_DERIVATIVE_RECORDED_LENGHT))
    {
        xlsx.write(row,   1, "Width of the Derivative curve NMP", format_subtitle);
        xlsx.write(row++, 2, getMetric(data, METRIC_CENTER_GRAVITY_UMP_DERIVATIVE_RECORDED_LENGHT), format_value);
    }
    if (hasMetric(data, METRIC_MEAN_VOLUME_RECORDED))
    {
        xlsx.write(row,   1, "Voiced Sounds Level", format_subtitle);
        xlsx.write(row++, 2, getMetric(data, METRIC_MEAN_VOLUME_RECORDED), format_value);
    }
    if (hasMetric(data, METRIC_TEMPO_RECORDED))
    {
        xlsx.write(row,   1, "Voiced Sounds Duration", format_subtitle);
        xlsx.write(row++, 2, getMetric(data, METRIC_TEMPO_RECORDED), format_value);
    }

    row++;
    xlsx.write(row,   1, "Relative data on templates and records", format_title);
    xlsx.write(row++, 2, "Relation", format_title);
    if (hasMetric(data, METRIC_RELATIVE_F0_MAX))
    {
        xlsx.write(row,   1, "F0 max [Hz]", format_subtitle);
        xlsx.write(row++, 2, getMetric(data, METRIC_RELATIVE_F0_MAX), format_value);
    }
    if (hasMetric(data, METRIC_RELATIVE_F0_MIN))
    {
        xlsx.write(row,   1, "F0 min  [Hz]", format_subtitle);
        xlsx.write(row++, 2, getMetric(data, METRIC_RELATIVE_F0_MIN), format_value);
    }
    if (hasMetric(data, METRIC_RELATIVE_DIAPASON_F0))
    {
        xlsx.write(row,   1, "Diapason F0", format_subtitle);
        xlsx.write(row++, 2, getMetric(data, METRIC_RELATIVE_DIAPASON_F0), format_value);
    }
    if (hasMetric(data, METRIC_RELATIVE_REGISTER_F0))
    {
        xlsx.write(row,   1, "Register F0 [Hz]", format_subtitle);
        xlsx.write(row++, 2, getMetric(data, METRIC_RELATIVE_REGISTER_F0), format_value);
    }
    if (hasMetric(data, METRIC_RELATIVE_MEAN_UMP))
    {
        xlsx.write(row,   1, "Mean Value of the  curve  NMP", format_subtitle);
        xlsx.write(row++, 2, getMetric(data, METRIC_RELATIVE_MEAN_UMP), format_value);
    }
    if (hasMetric(data, METRIC_RELATIVE_CENTER_GRAVITY_UMP_MID))
    {
        xlsx.write(row,   1, "Center of the curve  NMP", format_subtitle);
        xlsx.write(row++, 2, getMetric(data, METRIC_RELATIVE_CENTER_GRAVITY_UMP_MID), format_value);
    }
    if (hasMetric(data, METRIC_RELATIVE_CENTER_GRAVITY_UMP_LENGHT))
    {
        xlsx.write(row,   1, "Width of the curve  NMP", format_subtitle);
        xlsx.write(row++, 2, getMetric(data, METRIC_RELATIVE_CENTER_GRAVITY_UMP_LENGHT), format_value);
    }
    if (hasMetric(data, METRIC_RELATIVE_MEAN_VALUE_UMP_DERIVATIVE))
    {
        xlsx.write(row,   1, "Mean Value of the Derivative curve NMP", format_subtitle);
        xlsx.write(row++, 2, getMetric(data, METRIC_RELATIVE_MEAN_VALUE_UMP_DERIVATIVE), format_value);
    }
    if (hasMetric(data, METRIC_RELATIVE_CENTER_GRAVITY_UMP_DERIVATIVE_MID))
    {
        xlsx.write(row,   1, "Center of the Derivative curve NMP", format_subtitle);
        xlsx.write(row++, 2, getMetric(data, METRIC_RELATIVE_CENTER_GRAVITY_UMP_DERIVATIVE_MID), format_value);
    }
    if (hasMetric(data, METRIC_RELATIVE_CENTER_GRAVITY_UMP_DERIVATIVE_LENGHT))
    {
        xlsx.write(row,   1, "Width of the Derivative curve NMP", format_subtitle);
        xlsx.write(row++, 2, getMetric(data, METRIC_RELATIVE_CENTER_GRAVITY_UMP_DERIVATIVE_LENGHT), format_value);
    }
    if (hasMetric(data, METRIC_RELATEVE_MEAN_VOLUME))
    {
        xlsx.write(row,   1, "Voiced Sounds Level", format_subtitle);
        xlsx.write(row++, 2, getMetric(data, METRIC_RELATEVE_MEAN_VOLUME), format_value);
    }
    if (hasMetric(data, METRIC_RELATIVE_TEMPO))
    {
        xlsx.write(row,   1, "Voiced Sounds Duration", format_subtitle);
        xlsx.write(row++, 2, getMetric(data, METRIC_RELATIVE_TEMPO), format_value);
    }

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

