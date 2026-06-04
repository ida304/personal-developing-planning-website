#include "HomePage.h"
#include "core/DataManager.h"
#include "core/models.h"                     // 新增：引入 Requirement 结构体
#include <QFormLayout>
#include <QDate>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QProgressBar>
#include <QCheckBox>
#include <QGroupBox>
#include <QScrollArea>
#include <algorithm>

HomePage::HomePage(QWidget *parent) : QWidget(parent)
{
    setupUI();
    connect(&DataManager::instance(), &DataManager::dataChanged,
            this, &HomePage::refreshAll);
    refreshAll();
}

void HomePage::refreshAll()
{
    loadUserInfo();
    loadAcademicStats();
    loadProgress();
    loadTodos();
    loadRecent();
}

void HomePage::setupUI()
{
    QScrollArea *scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea->setStyleSheet("QScrollArea { border: none; background-color: transparent; }");

    QWidget *contentWidget = new QWidget;
    QVBoxLayout *mainLayout = new QVBoxLayout(contentWidget);
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
    infoLayout->addStretch();

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
    academicLayout->addStretch();

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
    viewAllBtn->setStyleSheet("QPushButton { background-color: #8b85ff; color: white; border: none; padding: 6px 14px; border-radius: 6px; }");
    connect(viewAllBtn, &QPushButton::clicked, this, &HomePage::onViewAllProgress);
    progressCardLayout->addWidget(viewAllBtn, 0, Qt::AlignRight);
    progressCardLayout->addStretch();

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
    todoCardLayout->addStretch();

    middleRow->addWidget(progressCard);
    middleRow->addWidget(todoCard);
    mainLayout->addLayout(middleRow);

    // ========== 第三行：两个独立卡片（近期成就和近期课程） ==========
    QHBoxLayout *recentRow = new QHBoxLayout;
    recentRow->setSpacing(20);

    // ---- 成就卡片 ----
    QGroupBox *achievementCard = new QGroupBox;
    achievementCard->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    achievementCard->setStyleSheet("QGroupBox { border: 1px solid #d9dbe8; border-radius: 16px; background-color: white; }");
    QVBoxLayout *achievementLayout = new QVBoxLayout(achievementCard);
    achievementLayout->setContentsMargins(15, 15, 15, 15);
    achievementLayout->setSpacing(10);

    QLabel *achieveTitle = new QLabel("📌 近期成就与经历");
    achieveTitle->setStyleSheet("font-size: 14pt; font-weight: bold; color: #6c63ff;");
    achievementLayout->addWidget(achieveTitle);

    m_achievementLayout = new QVBoxLayout;
    m_achievementLayout->setSpacing(8);
    achievementLayout->addLayout(m_achievementLayout);

    // 弹性空间，将按钮推到底部
    achievementLayout->addStretch();

    QPushButton *manageAchieveBtn = new QPushButton("管理成就与经历");
    manageAchieveBtn->setStyleSheet("QPushButton { background-color: #8b85ff; color: white; border: none; padding: 6px 14px; border-radius: 6px; }");
    connect(manageAchieveBtn, &QPushButton::clicked, this, &HomePage::onViewAllAchievements);
    achievementLayout->addWidget(manageAchieveBtn, 0, Qt::AlignRight);

    // ---- 课程卡片 ----
    QGroupBox *courseCard = new QGroupBox;
    courseCard->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    courseCard->setStyleSheet("QGroupBox { border: 1px solid #d9dbe8; border-radius: 16px; background-color: white; }");
    QVBoxLayout *courseLayout = new QVBoxLayout(courseCard);
    courseLayout->setContentsMargins(15, 15, 15, 15);
    courseLayout->setSpacing(10);

    QLabel *courseTitle = new QLabel("📚 近期课程");
    courseTitle->setStyleSheet("font-size: 14pt; font-weight: bold; color: #6c63ff;");
    courseLayout->addWidget(courseTitle);

    m_courseLayout = new QVBoxLayout;
    m_courseLayout->setSpacing(8);
    courseLayout->addLayout(m_courseLayout);

    courseLayout->addStretch();

    QPushButton *manageCourseBtn = new QPushButton("管理课程");
    manageCourseBtn->setStyleSheet("QPushButton { background-color: #8b85ff; color: white; border: none; padding: 6px 14px; border-radius: 6px; }");
    connect(manageCourseBtn, &QPushButton::clicked, this, &HomePage::onViewAllCourses);
    courseLayout->addWidget(manageCourseBtn, 0, Qt::AlignRight);

    recentRow->addWidget(achievementCard);
    recentRow->addWidget(courseCard);
    mainLayout->addLayout(recentRow);

    // 底部按钮
    QHBoxLayout *btnLayout = new QHBoxLayout;
    btnLayout->addStretch();
    QPushButton *addCourseBtn = new QPushButton("添加课程");
    QPushButton *exportResumeBtn = new QPushButton("导出简历");
    QString btnStyle = "QPushButton { background-color: #8b85ff; color: white; border: none; padding: 6px 14px; border-radius: 6px; }";
    addCourseBtn->setStyleSheet(btnStyle);
    exportResumeBtn->setStyleSheet(btnStyle);
    connect(addCourseBtn, &QPushButton::clicked, this, &HomePage::onAddCourse);
    connect(exportResumeBtn, &QPushButton::clicked, this, &HomePage::onExportResume);
    btnLayout->addWidget(addCourseBtn);
    btnLayout->addWidget(exportResumeBtn);
    mainLayout->addLayout(btnLayout);

    scrollArea->setWidget(contentWidget);
    QVBoxLayout *outerLayout = new QVBoxLayout(this);
    outerLayout->setContentsMargins(0, 0, 0, 0);
    outerLayout->addWidget(scrollArea);
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
    // 清空原有待办
    QLayoutItem *child;
    while ((child = m_todoLayout->takeAt(0)) != nullptr) {
        if (child->widget()) delete child->widget();
        delete child;
    }

    auto& dm = DataManager::instance();
    bool hasTodo = false;

    // 1. 毕业要求缺口（已启用，需要 DataManager::getRequirements 实现）
    QList<Requirement> reqs = dm.getRequirements();
    for (const Requirement& req : reqs) {
        double shortage = req.requiredCredits - req.earnedCredits;
        if (shortage > 0.01) {
            QCheckBox *cb = new QCheckBox(QString("%1 尚缺 %2 学分").arg(req.category).arg(shortage, 0, 'f', 1));
            m_todoLayout->addWidget(cb);
            hasTodo = true;
        }
    }

    // 2. 未录入成绩的课程
    QList<Course> courses = dm.getAllCourses();
    int noGradeCount = 0;
    for (const Course& c : courses) {
        if (c.score <= 0.01 && c.status != "未修") noGradeCount++;
    }
    if (noGradeCount > 0) {
        QCheckBox *cb = new QCheckBox(QString("您有 %1 门课程尚未录入成绩").arg(noGradeCount));
        m_todoLayout->addWidget(cb);
        hasTodo = true;
    }

    // 3. 如果没有任何待办，显示鼓励信息
    if (!hasTodo) {
        QLabel *label = new QLabel("🎉 太棒了！当前没有待办事项。");
        label->setStyleSheet("color: #6c63ff;");
        m_todoLayout->addWidget(label);
    }
}

void HomePage::loadRecent()
{
    QLayoutItem *child;
    while ((child = m_achievementLayout->takeAt(0)) != nullptr) {
        if (child->widget()) delete child->widget();
        delete child;
    }
    while ((child = m_courseLayout->takeAt(0)) != nullptr) {
        if (child->widget()) delete child->widget();
        delete child;
    }

    // 成就列表
    QList<Achievement> achievements = DataManager::instance().getAllAchievements();
    std::sort(achievements.begin(), achievements.end(),
              [](const Achievement& a, const Achievement& b) { return a.obtainDate > b.obtainDate; });
    int achCount = std::min(5, achievements.size());
    for (int i = 0; i < achCount; ++i) {
        const Achievement& ach = achievements[i];
        QString display = QString("%1  %2").arg(ach.name).arg(ach.obtainDate.toString("yyyy-MM-dd"));
        if (!ach.level.isEmpty()) {
            display = QString("%1（%2）  %3").arg(ach.name).arg(ach.level).arg(ach.obtainDate.toString("yyyy-MM-dd"));
        }
        QLabel *label = new QLabel(display);
        label->setWordWrap(true);
        m_achievementLayout->addWidget(label);
    }
    if (achievements.isEmpty()) {
        QLabel *label = new QLabel("暂无成就记录");
        m_achievementLayout->addWidget(label);
    }

    // 课程列表
    QList<Course> courses = DataManager::instance().getAllCourses();
    std::sort(courses.begin(), courses.end(),
              [](const Course& a, const Course& b) { return a.id > b.id; });
    int courseCount = std::min(5, courses.size());
    for (int i = 0; i < courseCount; ++i) {
        const Course& c = courses[i];
        QLabel *label = new QLabel(QString("📚 %1（%2）").arg(c.name).arg(c.semester));
        label->setWordWrap(true);
        m_courseLayout->addWidget(label);
    }
    if (courses.isEmpty()) {
        QLabel *label = new QLabel("暂无课程");
        m_courseLayout->addWidget(label);
    }
}

void HomePage::onEditProfile()          { emit switchToTab(1); }
void HomePage::onAddCourse()            { emit switchToTab(2); }
void HomePage::onExportResume()         { emit switchToTab(3); }
void HomePage::onViewAllProgress()      { emit switchToTab(4); }
void HomePage::onViewAllAchievements()  { emit switchToTab(3); }
void HomePage::onViewAllCourses()       { emit switchToTab(2); }
