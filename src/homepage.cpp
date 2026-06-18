#include "HomePage.h"
#include "core/DataManager.h"
#include "core/models.h"
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
#include <QDebug>

HomePage::HomePage(QWidget *parent) : QWidget(parent)
{
    setupUI();
    // 连接数据变化信号，刷新所有数据
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

// 单独刷新个人信息（供队友调用）
void HomePage::refreshUserInfo()
{
    loadUserInfo();
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

    // ========== 第三行：两个独立卡片 ==========
    QHBoxLayout *recentRow = new QHBoxLayout;
    recentRow->setSpacing(20);

    // 成就卡片
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
    achievementLayout->addStretch();
    QPushButton *manageAchieveBtn = new QPushButton("管理成就与经历");
    manageAchieveBtn->setStyleSheet("QPushButton { background-color: #8b85ff; color: white; border: none; padding: 6px 14px; border-radius: 6px; }");
    connect(manageAchieveBtn, &QPushButton::clicked, this, &HomePage::onViewAllAchievements);
    achievementLayout->addWidget(manageAchieveBtn, 0, Qt::AlignRight);

    // 课程卡片
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
    auto& dm = DataManager::instance();
    QList<Course> allCourses = dm.getAllCourses();
    int totalCourses = allCourses.size();
    qDebug() << "数据库中的课程总数:" << totalCourses;  // 需要 #include <QDebug>

    double totalPoints = 0.0, totalCredits = 0.0, earnedCredits = 0.0;
    int passed = 0;

    for (const Course& c : allCourses) {
        // 临时去掉状态限制，只要成绩≥60就算
        if (c.score < 60) continue;
        passed++;
        earnedCredits += c.credit;
        totalCredits += c.credit;
        double point = 0.0;
        if (c.score >= 90) point = 4.0;
        else if (c.score >= 85) point = 3.7;
        else if (c.score >= 82) point = 3.3;
        else if (c.score >= 78) point = 3.0;
        else if (c.score >= 75) point = 2.7;
        else if (c.score >= 72) point = 2.3;
        else if (c.score >= 68) point = 2.0;
        else if (c.score >= 64) point = 1.5;
        else if (c.score >= 60) point = 1.0;
        totalPoints += point * c.credit;
    }

    double gpa = (totalCredits == 0) ? 0.0 : (totalPoints / totalCredits);
    double passRate = (totalCourses == 0) ? 0.0 : (100.0 * passed / totalCourses);
    // 毕业要求总学分硬编码160避免文件问题
    double totalRequired = 160.0;

    m_gpaLabel->setText(QString("GPA: %1").arg(gpa, 0, 'f', 2));
    m_creditsLabel->setText(QString("已修学分: %1 / %2").arg(earnedCredits, 0, 'f', 1).arg(totalRequired, 0, 'f', 1));
    m_passRateLabel->setText(QString("课程通过率: %1%").arg(passRate, 0, 'f', 1));
}

void HomePage::loadProgress()
{
    QLayoutItem *child;
    while ((child = m_progressLayout->takeAt(0)) != nullptr) {
        if (child->widget()) delete child->widget();
        delete child;
    }

    QList<Requirement> reqs = DataManager::instance().getRequirements();

    if (reqs.isEmpty()) {
        // 无数据时显示一条提示（不硬编码具体值）
        QLabel *emptyLabel = new QLabel("请填写个人资料（专业、年级）以显示毕业要求");
        emptyLabel->setStyleSheet("color: #999; font-style: italic;");
        m_progressLayout->addWidget(emptyLabel);
        return;
    }

    for (const Requirement& req : reqs) {
        QLabel *nameLabel = new QLabel(req.category);
        nameLabel->setFixedWidth(130);

        QProgressBar *bar = new QProgressBar;
        // 关键：即使总学分为0，也设置合理范围并显示文本，确保条目可见
        if (req.requiredCredits == 0) {
            bar->setRange(0, 1);
            bar->setValue(0);
            bar->setFormat("0.0 / 0.0");
        } else {
            int maxVal = static_cast<int>(req.requiredCredits);
            int curVal = static_cast<int>(req.earnedCredits);
            // 如果已修学分超出总学分，进度条显示为100%，但数值仍显示实际值
            if (curVal > maxVal) curVal = maxVal;
            bar->setRange(0, maxVal);
            bar->setValue(curVal);
            bar->setFormat(QString("%1 / %2")
                           .arg(req.earnedCredits, 0, 'f', 1)
                           .arg(req.requiredCredits, 0, 'f', 1));
        }

        // 确保进度条高度足够显示文本
        bar->setMinimumHeight(25);

        QHBoxLayout *row = new QHBoxLayout;
        row->setSpacing(10);
        row->addWidget(nameLabel);
        row->addWidget(bar, 1);
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

    // 毕业要求缺口
    QList<Requirement> reqs = dm.getRequirements();
    for (const Requirement& req : reqs) {
        double shortage = req.requiredCredits - req.earnedCredits;
        if (shortage > 0.01) {
            QCheckBox *cb = new QCheckBox(QString("%1 尚缺 %2 学分").arg(req.category).arg(shortage, 0, 'f', 1));
            m_todoLayout->addWidget(cb);
            hasTodo = true;
        }
    }

    // 未录入成绩的课程
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

    QList<Achievement> achievements = DataManager::instance().getAllAchievements();
    std::sort(achievements.begin(), achievements.end(),
              [](const Achievement& a, const Achievement& b) { return a.obtainDate > b.obtainDate; });
    int achCount = std::min(5, static_cast<int>(achievements.size()));
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

    QList<Course> courses = DataManager::instance().getAllCourses();
    std::sort(courses.begin(), courses.end(),
              [](const Course& a, const Course& b) { return a.id > b.id; });
    int courseCount = std::min(5, static_cast<int>(courses.size()));
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
