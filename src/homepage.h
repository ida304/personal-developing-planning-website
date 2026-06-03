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

private:
    void setupUI();
    void loadUserInfo();      // 写死数据
    void loadAcademicStats(); // 写死数据
    void loadProgress();      // 写死进度条
    void loadTodos();         // 写死待办
    void loadRecent();        // 写死最近活动

    QLabel *m_nameLabel, *m_schoolLabel, *m_collegeLabel, *m_majorLabel, *m_gradeLabel, *m_degreeLabel;
    QLabel *m_gpaLabel, *m_creditsLabel, *m_passRateLabel;
    QVBoxLayout *m_progressLayout;
    QVBoxLayout *m_todoLayout;
    QHBoxLayout *m_recentLayout;
};

#endif // HOMEPAGE_H
