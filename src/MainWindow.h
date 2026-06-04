#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidget>
#include <QStackedWidget>

class HomePage;
class ProfileWidget;
class CourseWidget;
class ExpAchieveWidget;
class ProgressWidget;
class ResumeWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);

private:
    void setupUI();
    QListWidget *m_nav;
    QStackedWidget *m_stack;
    HomePage *m_home;
    ProfileWidget *m_profile;
    CourseWidget *m_course;
    ExpAchieveWidget *m_exp;
    ProgressWidget *m_progress;
    ResumeWidget *m_resume;
};

#endif // MAINWINDOW_H
