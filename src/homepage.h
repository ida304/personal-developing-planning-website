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
#include <QScrollArea>

class HomePage : public QWidget
{
    Q_OBJECT
public:
    explicit HomePage(QWidget *parent = nullptr);

signals:
    void switchToTab(int index);

private slots:
    void onEditProfile();
    void onAddCourse();
    void onExportResume();
    void onViewAllProgress();
    void onViewAllAchievements();
    void onViewAllCourses();
    void refreshAll();

private:
    void setupUI();
    void loadUserInfo();
    void loadAcademicStats();
    void loadProgress();
    void loadTodos();
    void loadRecent();

    QLabel *m_nameLabel, *m_schoolLabel, *m_collegeLabel, *m_majorLabel, *m_gradeLabel, *m_degreeLabel;
    QLabel *m_gpaLabel, *m_creditsLabel, *m_passRateLabel;
    QVBoxLayout *m_progressLayout;
    QVBoxLayout *m_todoLayout;
    QVBoxLayout *m_achievementLayout;   // 成就列表布局（放在滚动区域内）
    QVBoxLayout *m_courseLayout;        // 课程列表布局（放在滚动区域内）
};

#endif // HOMEPAGE_H
