#ifndef HOMEPAGE_H
#define HOMEPAGE_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QProgressBar>
#include <QCheckBox>
#include "core/DataManager.h"

class HomePage : public QWidget
{
    Q_OBJECT
public:
    explicit HomePage(QWidget *parent = nullptr);

signals:
    void switchToTab(int index);  // 切换到其他页面

private slots:
    void onEditProfile();
    void onAddCourse();
    void onExportResume();
    void onViewAllProgress();
    void refreshUserInfo();

private:
    void setupUI();
    void loadUserInfo();      // 从 DataManager 读取
    void loadAcademicStats();
    void loadProgress();
    void loadTodos();
    void loadRecent();

    QLabel *m_nameLabel, *m_schoolLabel, *m_collegeLabel, *m_majorLabel, *m_gradeLabel, *m_degreeLabel;
    QLabel *m_gpaLabel, *m_creditsLabel, *m_passRateLabel;
    QVBoxLayout *m_progressLayout;
    QVBoxLayout *m_todoLayout;
    QHBoxLayout *m_recentLayout;
};

#endif // HOMEPAGE_H
