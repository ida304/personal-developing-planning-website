#ifndef PROGRESSWIDGET_H
#define PROGRESSWIDGET_H

#include <QWidget>
#include <QMap>
#include <QStringList>
#include <QProgressBar>
#include <QLabel>
#include <QPushButton>
#include <QGridLayout>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include "src/core/DataManager.h"
#include <QStandardPaths>

struct ProgressSummary {
    QString category;
    double required;
    double earned;
    double gap;
    double completionRate;
};

class ProgressWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ProgressWidget(QWidget *parent = nullptr);
    ~ProgressWidget();

public slots:
    void refreshData();

private:
    void setupUI();
    void loadGraduationRequirements();
    QMap<QString, QStringList> loadCategoryMapping();
    QMap<QString, double> calculateEarnedCredits();
    void updateProgressDisplay();
    void generateHTMLReport();
    void showManualCreditDialog();
    bool loadGraduationReqsWithFallback();

    // UI Components
    QGridLayout *mainLayout;
    QMap<QString, QProgressBar*> progressBars;
    QMap<QString, QLabel*> creditLabels;
    QMap<QString, QLabel*> gapLabels;
    QPushButton *exportButton;

    // Data
    QStringList categories;
    QMap<QString, double> requiredCredits;
    QMap<QString, QStringList> categoryMapping;
    QMap<QString, ProgressSummary> progressSummary;
};

#endif // PROGRESSWIDGET_H
