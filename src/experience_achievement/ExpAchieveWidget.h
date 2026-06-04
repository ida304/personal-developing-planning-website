#ifndef EXPACHIEVEWIDGET_H
#define EXPACHIEVEWIDGET_H

#include <QWidget>
#include <QTableWidget>
#include <QLineEdit>
#include <QDateEdit>
#include <QPushButton>
#include <QTextEdit>
#include "../core/models.h"

class ExpAchieveWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ExpAchieveWidget(QWidget *parent = nullptr);

private slots:
    void addExperienceClicked();
    void updateExperienceClicked();
    void deleteExperienceClicked(int expId);
    void experienceCellClicked(int row);
    void openExpFileClicked();

    void addAchievementClicked();
    void updateAchievementClicked();
    void deleteAchievementClicked(int achId);
    void achievementCellClicked(int row);
    void openAchFileClicked();

private:
    void createTablesIfNotExists();

    void setupUI();
    void refreshExperienceTable();
    void refreshAchievementTable();
    void clearExperienceForm();
    void clearAchievementForm();

    QLineEdit *m_expTypeEdit;
    QLineEdit *m_expNameEdit;
    QDateEdit *m_expStartEdit;
    QDateEdit *m_expEndEdit;
    QLineEdit *m_expRoleEdit;
    QTextEdit *m_expDescEdit;
    QLineEdit *m_expTagsEdit;
    QLineEdit *m_expFilePathEdit;

    QLineEdit *m_achNameEdit;
    QLineEdit *m_achLevelEdit;
    QDateEdit *m_achDateEdit;
    QLineEdit *m_achTagsEdit;
    QLineEdit *m_achFilePathEdit;

    QPushButton *m_expAddBtn;
    QPushButton *m_expUpdateBtn;
    QPushButton *m_expDelBtn;
    QPushButton *m_expOpenFileBtn;

    QPushButton *m_achAddBtn;
    QPushButton *m_achUpdateBtn;
    QPushButton *m_achDelBtn;
    QPushButton *m_achOpenFileBtn;

    QTableWidget *m_expTable;
    QTableWidget *m_achTable;

    int m_currentExpId;
    int m_currentAchId;
};

#endif
