#include "MainWindow.h"
#include "HomePage.h"
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
#include <QMouseEvent>

#ifdef Q_OS_WIN
#include <windows.h>
#include <windowsx.h>
#endif

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), m_dragging(false)
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

    // 标题栏 (保持不变)
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

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    // 标题栏拖拽移动
    if (event->button() == Qt::LeftButton && event->pos().y() <= 60) {
        m_dragging = true;
        m_dragPos = event->globalPos() - frameGeometry().topLeft();
        event->accept();
    }
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    if (m_dragging && (event->buttons() & Qt::LeftButton)) {
        move(event->globalPos() - m_dragPos);
        event->accept();
    }
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_dragging = false;
        event->accept();
    }
}

#ifdef Q_OS_WIN
bool MainWindow::nativeEvent(const QByteArray &eventType, void *message, long *result)
{
    MSG *msg = static_cast<MSG*>(message);
    if (msg->message == WM_NCHITTEST) {
        int xPos = GET_X_LPARAM(msg->lParam);
        int yPos = GET_Y_LPARAM(msg->lParam);
        QPoint globalPos(xPos, yPos);
        QPoint localPos = mapFromGlobal(globalPos);
        const int borderWidth = 8;  // 可调，控制触发缩放的边缘厚度

        // 检测边缘
        if (localPos.x() <= borderWidth) {
            if (localPos.y() <= borderWidth) *result = HTTOPLEFT;
            else if (localPos.y() >= height() - borderWidth) *result = HTBOTTOMLEFT;
            else *result = HTLEFT;
            return true;
        }
        else if (localPos.x() >= width() - borderWidth) {
            if (localPos.y() <= borderWidth) *result = HTTOPRIGHT;
            else if (localPos.y() >= height() - borderWidth) *result = HTBOTTOMRIGHT;
            else *result = HTRIGHT;
            return true;
        }
        else if (localPos.y() <= borderWidth) {
            *result = HTTOP;
            return true;
        }
        else if (localPos.y() >= height() - borderWidth) {
            *result = HTBOTTOM;
            return true;
        }
    }
    return QMainWindow::nativeEvent(eventType, message, result);
}
#endif
