#include "MainWindow.h"
#include <QTabWidget>
#include "profile/ProfileWidget.h"
#include "course/CourseWidget.h"
#include "resume/ResumeWidget.h"
#include "experience_achievement/ExpAchieveWidget.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    QTabWidget *tabWidget = new QTabWidget(this);
    tabWidget->addTab(new ProfileWidget, "个人资料");
    tabWidget->addTab(new CourseWidget, "课程管理");
    tabWidget->addTab(new ResumeWidget,"简历导出");
    tabWidget->addTab(new ExpAchieveWidget,"成就与经历");
    // 后续其他模块界面可以继续添加
    setCentralWidget(tabWidget);
    resize(1000, 700);
    setWindowTitle("个人发展规划系统");
}
