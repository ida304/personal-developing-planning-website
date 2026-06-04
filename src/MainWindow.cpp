#include "MainWindow.h"
#include "homepage.h"
#include "profile/ProfileWidget.h"
#include "course/CourseWidget.h"
#include "experience_achievement/ExpAchieveWidget.h"
#include "progress/ProgressWidget.h"
#include "resume/ResumeWidget.h"
#include <QSplitter>
#include <QVBoxLayout>
#include <QListWidget>
#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>
#include <QApplication>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    setWindowFlags(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setupUI();
    resize(1100, 750);
}

void MainWindow::setupUI()
{
    QWidget *central = new QWidget;
    central->setObjectName("centralWidget");
    central->setStyleSheet("#centralWidget { background-color: #f3f4f8; border-radius: 12px; }");
    QVBoxLayout *mainLayout = new QVBoxLayout(central);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // 标题栏
    QWidget *titleBar = new QWidget;
    titleBar->setFixedHeight(60);
    titleBar->setStyleSheet("background-color: #ffffff; border-bottom: 1px solid #eef0f7; border-top-left-radius: 12px; border-top-right-radius: 12px;");
    QHBoxLayout *titleLayout = new QHBoxLayout(titleBar);
    titleLayout->setContentsMargins(20, 0, 10, 0);
    titleLayout->setSpacing(6);

    QLabel *iconLabel = new QLabel("💜");
    iconLabel->setStyleSheet("font-size: 24px;");
    titleLayout->addWidget(iconLabel);

    QLabel *titleLabel = new QLabel("个人发展规划系统");
    titleLabel->setStyleSheet("font-size: 14pt; font-weight: bold; color: #6c63ff;");
    titleLayout->addWidget(titleLabel);
    titleLayout->addStretch();

    // 最小化按钮
    QPushButton *minBtn = new QPushButton("─");
    minBtn->setFixedSize(28, 28);
    minBtn->setStyleSheet(
        "QPushButton { background-color: transparent; border: none; border-radius: 4px; font-size: 14px; color: #6c63ff; }"
        "QPushButton:hover { background-color: #d9d7ff; color: #3a2fc2; }"
    );
    connect(minBtn, &QPushButton::clicked, this, &QMainWindow::showMinimized);

    // 最大化/恢复按钮
    QPushButton *maxBtn = new QPushButton("□");
    maxBtn->setFixedSize(28, 28);
    maxBtn->setStyleSheet(
        "QPushButton { background-color: transparent; border: none; border-radius: 4px; font-size: 14px; color: #6c63ff; }"
        "QPushButton:hover { background-color: #d9d7ff; color: #3a2fc2; }"
    );
    connect(maxBtn, &QPushButton::clicked, this, [this, maxBtn]() {
        if (isMaximized()) {
            showNormal();
            maxBtn->setText("□");
        } else {
            showMaximized();
            maxBtn->setText("❐");
        }
    });

    // 关闭按钮
    QPushButton *closeBtn = new QPushButton("✕");
    closeBtn->setFixedSize(28, 28);
    closeBtn->setStyleSheet(
        "QPushButton { background-color: transparent; border: none; border-radius: 4px; font-size: 14px; color: #6c63ff; }"
        "QPushButton:hover { background-color: #ff5f5f; color: white; }"
    );
    connect(closeBtn, &QPushButton::clicked, this, &QMainWindow::close);

    titleLayout->addWidget(minBtn);
    titleLayout->addWidget(maxBtn);
    titleLayout->addWidget(closeBtn);
    mainLayout->addWidget(titleBar);

    // 主体部分（导航栏 + 内容）
    QWidget *body = new QWidget;
    QHBoxLayout *bodyLayout = new QHBoxLayout(body);
    bodyLayout->setContentsMargins(0, 0, 0, 0);
    bodyLayout->setSpacing(0);

    m_nav = new QListWidget;
    m_nav->setFixedWidth(260);
    // 关键修改：上下边距设为 6px，产生间距并使第一个按钮下移
    QString navStyle = QString::fromUtf8(
        "QListWidget { background-color: #ffffff; border: none; outline: none; }"
        "QListWidget::item { background-color: #edeaff; padding: 14px 20px; border-radius: 12px; margin: 6px 12px; font-size: 13pt; color: #5a52d9; }"
        "QListWidget::item:selected { background-color: #d9d7ff; color: #3a2fc2; }"
        "QListWidget::item:hover { background-color: #ddd9ff; }"
    );
    m_nav->setStyleSheet(navStyle);
    m_nav->addItem("🏠 个人主页");
    m_nav->addItem("👤 个人资料");
    m_nav->addItem("📚 课程管理");
    m_nav->addItem("🏆 成就与经历");
    m_nav->addItem("📊 追踪培养");
    m_nav->addItem("📄 简历导出");

    m_stack = new QStackedWidget;
    m_home = new HomePage;
    m_profile = new ProfileWidget;
    m_course = new CourseWidget;
    m_exp = new ExpAchieveWidget;
    m_progress = new ProgressWidget;
    m_resume = new ResumeWidget;

    m_stack->addWidget(m_home);
    m_stack->addWidget(m_profile);
    m_stack->addWidget(m_course);
    m_stack->addWidget(m_exp);
    m_stack->addWidget(m_progress);
    m_stack->addWidget(m_resume);

    connect(m_nav, &QListWidget::currentRowChanged, m_stack, &QStackedWidget::setCurrentIndex);
    connect(m_home, &HomePage::switchToTab, m_stack, &QStackedWidget::setCurrentIndex);

    bodyLayout->addWidget(m_nav);
    bodyLayout->addWidget(m_stack, 1);
    mainLayout->addWidget(body);

    setCentralWidget(central);
}
