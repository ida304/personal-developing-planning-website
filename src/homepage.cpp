#include "HomePage.h"
#include <QFormLayout>
#include <QDate>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QProgressBar>
#include <QCheckBox>
#include <QGroupBox>
#include "core/DataManager.h"

HomePage::HomePage(QWidget *parent) : QWidget(parent)
{
    setupUI();
    loadUserInfo();
    loadAcademicStats();
    loadProgress();
    loadTodos();
    loadRecent();

    // 监听个人信息变化，自动刷新
    connect(&DataManager::instance(), &DataManager::dataChanged,
            this, &HomePage::refreshUserInfo);
}

void HomePage::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(20);
    mainLayout->setContentsMargins(20, 20, 20, 20);

    // ========== 第一行 ==========
    QHBoxLayout *topRow = new QHBoxLayout;
    topRow->setSpacing(20);

    // 个人信息卡片
    QGroupBox *infoCard = new QGroupBox;
    infoCard->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    infoCard->setStyleSheet("QGroupBox { border: 1px solid #d9dbe8; border-radius: 16px; background-color: white; }");
    QVBoxLayout *infoLayout = new QVBoxLayout(infoCard);
    infoLayout->setContentsMargins(15, 15, 15, 15);
    infoLayout->setSpacing(10);
    QLabel *infoTitle = new QLabel("个人信息");
    infoTitle->setStyleSheet("font-size: 14pt; font-weight: bold; color: #6c63ff;");
    infoLayout->addWidget(infoTitle);
    QFormLayout *form = new QFormLayout;
    form->setLabelAlignment(Qt::AlignRight);
    m_nameLabel = new QLabel; m_schoolLabel = new QLabel; m_collegeLabel = new QLabel;
    m_majorLabel = new QLabel; m_gradeLabel = new QLabel; m_degreeLabel = new QLabel;
    form->addRow("姓名：", m_nameLabel);
    form->addRow("学校：", m_schoolLabel);
    form->addRow("学院：", m_collegeLabel);
    form->addRow("专业：", m_majorLabel);
    form->addRow("年级：", m_gradeLabel);
    form->addRow("学历：", m_degreeLabel);
    infoLayout->addLayout(form);
    QPushButton *editBtn = new QPushButton("编辑资料");
    connect(editBtn, &QPushButton::clicked, this, &HomePage::onEditProfile);
    infoLayout->addWidget(editBtn, 0, Qt::AlignRight);
    infoLayout->addStretch();   // 补：使卡片内容顶部对齐，底部自动填充

    // 学业概况卡片
    QGroupBox *academicCard = new QGroupBox;
    academicCard->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    academicCard->setStyleSheet("QGroupBox { border: 1px solid #d9dbe8; border-radius: 16px; background-color: white; }");
    QVBoxLayout *academicLayout = new QVBoxLayout(academicCard);
    academicLayout->setContentsMargins(15, 15, 15, 15);
    academicLayout->setSpacing(10);
    QLabel *academicTitle = new QLabel("学业概况");
    academicTitle->setStyleSheet("font-size: 14pt; font-weight: bold; color: #6c63ff;");
    academicLayout->addWidget(academicTitle);
    m_gpaLabel = new QLabel; m_creditsLabel = new QLabel; m_passRateLabel = new QLabel;
    QFont boldFont; boldFont.setBold(true); boldFont.setPointSize(14);
    m_gpaLabel->setFont(boldFont);
    academicLayout->addWidget(m_gpaLabel);
    academicLayout->addWidget(m_creditsLabel);
    academicLayout->addWidget(m_passRateLabel);
    academicLayout->addStretch();   // 补：使卡片内容顶部对齐，底部自动填充

    topRow->addWidget(infoCard);
    topRow->addWidget(academicCard);
    mainLayout->addLayout(topRow);

    // ========== 第二行 ==========
    QHBoxLayout *middleRow = new QHBoxLayout;
    middleRow->setSpacing(20);

    // 毕业进度卡片
    QGroupBox *progressCard = new QGroupBox;
    progressCard->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    progressCard->setStyleSheet("QGroupBox { border: 1px solid #d9dbe8; border-radius: 16px; background-color: white; }");
    QVBoxLayout *progressCardLayout = new QVBoxLayout(progressCard);
    progressCardLayout->setContentsMargins(15, 15, 15, 15);
    progressCardLayout->setSpacing(10);
    QLabel *progressTitle = new QLabel("毕业进度追踪");
    progressTitle->setStyleSheet("font-size: 14pt; font-weight: bold; color: #6c63ff;");
    progressCardLayout->addWidget(progressTitle);
    m_progressLayout = new QVBoxLayout;
    m_progressLayout->setSpacing(12);
    progressCardLayout->addLayout(m_progressLayout);
    QPushButton *viewAllBtn = new QPushButton("查看全部");
    connect(viewAllBtn, &QPushButton::clicked, this, &HomePage::onViewAllProgress);
    progressCardLayout->addWidget(viewAllBtn, 0, Qt::AlignRight);
    progressCardLayout->addStretch();   // 确保卡片内部拉伸

    // 待办事项卡片
    QGroupBox *todoCard = new QGroupBox;
    todoCard->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    todoCard->setStyleSheet("QGroupBox { border: 1px solid #d9dbe8; border-radius: 16px; background-color: white; }");
    QVBoxLayout *todoCardLayout = new QVBoxLayout(todoCard);
    todoCardLayout->setContentsMargins(15, 15, 15, 15);
    todoCardLayout->setSpacing(10);
    QLabel *todoTitle = new QLabel("待办事项");
    todoTitle->setStyleSheet("font-size: 14pt; font-weight: bold; color: #6c63ff;");
    todoCardLayout->addWidget(todoTitle);
    m_todoLayout = new QVBoxLayout;
    m_todoLayout->setSpacing(8);
    todoCardLayout->addLayout(m_todoLayout);
    todoCardLayout->addStretch();       // 确保卡片内部拉伸

    middleRow->addWidget(progressCard);
    middleRow->addWidget(todoCard);
    mainLayout->addLayout(middleRow);

    // ========== 第三行：最近活动 ==========
    QGroupBox *recentCard = new QGroupBox;
    recentCard->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    recentCard->setStyleSheet("QGroupBox { border: 1px solid #d9dbe8; border-radius: 16px; background-color: white; }");
    QVBoxLayout *recentCardLayout = new QVBoxLayout(recentCard);
    recentCardLayout->setContentsMargins(15, 15, 15, 15);
    recentCardLayout->setSpacing(10);
    QLabel *recentTitle = new QLabel("最近活动");
    recentTitle->setStyleSheet("font-size: 14pt; font-weight: bold; color: #6c63ff;");
    recentCardLayout->addWidget(recentTitle);
    m_recentLayout = new QHBoxLayout;
    m_recentLayout->setSpacing(15);
    recentCardLayout->addLayout(m_recentLayout);
    recentCardLayout->addStretch();   // 让最近活动卡片内部内容顶部对齐，底部留白
    mainLayout->addWidget(recentCard);

    // 底部按钮
    QHBoxLayout *btnLayout = new QHBoxLayout;
    btnLayout->addStretch();
    QPushButton *addCourseBtn = new QPushButton("添加课程");
    QPushButton *exportResumeBtn = new QPushButton("导出简历");
    connect(addCourseBtn, &QPushButton::clicked, this, &HomePage::onAddCourse);
    connect(exportResumeBtn, &QPushButton::clicked, this, &HomePage::onExportResume);
    btnLayout->addWidget(addCourseBtn);
    btnLayout->addWidget(exportResumeBtn);
    mainLayout->addLayout(btnLayout);
}

void HomePage::loadUserInfo()
{
    UserProfile profile = DataManager::instance().getUserProfile();
    m_nameLabel->setText(profile.name.isEmpty() ? "未填写" : profile.name);
    m_schoolLabel->setText(profile.school.isEmpty() ? "未填写" : profile.school);
    m_collegeLabel->setText(profile.college.isEmpty() ? "未填写" : profile.college);
    m_majorLabel->setText(profile.major.isEmpty() ? "未填写" : profile.major);
    m_gradeLabel->setText(profile.grade.isEmpty() ? "未填写" : profile.grade);
    m_degreeLabel->setText(profile.education.isEmpty() ? "未填写" : profile.education);
}

void HomePage::refreshUserInfo()
{
    loadUserInfo();   // 重新加载个人信息
    // 如果将来还需要刷新其他数据（如 GPA、进度等），也可以在这里调用
}

void HomePage::loadAcademicStats()
{
    m_gpaLabel->setText("GPA: 3.85");
    m_creditsLabel->setText("已修学分: 78 / 160");
    m_passRateLabel->setText("课程通过率: 92%");
}

void HomePage::loadProgress()
{
    QLayoutItem *child;
    while ((child = m_progressLayout->takeAt(0)) != nullptr) delete child;
    QList<QPair<QString, QPair<int,int>>> data = {
        {"学科基础必修", {12,24}}, {"专业方向必修", {10,26}},
        {"通修课程", {18,30}}, {"核心通识", {6,14}}
    };
    for (auto &d : data) {
        QHBoxLayout *row = new QHBoxLayout;
        QLabel *label = new QLabel(d.first);
        label->setFixedWidth(130);
        QProgressBar *bar = new QProgressBar;
        bar->setRange(0, d.second.second);
        bar->setValue(d.second.first);
        bar->setFormat(QString("%1 / %2").arg(d.second.first).arg(d.second.second));
        row->addWidget(label);
        row->addWidget(bar);
        m_progressLayout->addLayout(row);
    }
}

void HomePage::loadTodos()
{
    QLayoutItem *child;
    while ((child = m_todoLayout->takeAt(0)) != nullptr) delete child;
    auto addTodo = [this](const QString &text) {
        QCheckBox *cb = new QCheckBox(text);
        m_todoLayout->addWidget(cb);
    };
    addTodo("学科基础选修尚缺 8 学分，请选修课程");
    addTodo("您有 2 门课程尚未添加成绩");
    addTodo("距离挑战杯报名截止还有 3 天");
}

void HomePage::loadRecent()
{
    QLayoutItem *child;
    while ((child = m_recentLayout->takeAt(0)) != nullptr) delete child;
    QLabel *achLabel = new QLabel("🏆 最近成就：挑战杯（省级）2026-06-03");
    QLabel *courseLabel = new QLabel("📚 最近课程：数据结构（2024-2025-2）");
    m_recentLayout->addWidget(achLabel);
    m_recentLayout->addWidget(courseLabel);
    m_recentLayout->addStretch();
}

void HomePage::onEditProfile()   { emit switchToTab(1); }
void HomePage::onAddCourse()     { emit switchToTab(2); }
void HomePage::onExportResume()  { emit switchToTab(3); }
void HomePage::onViewAllProgress() { emit switchToTab(4); }
