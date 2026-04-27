#include "CourseWidget.h"
#include "src/core/DataManager.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QMessageBox>
#include <QDebug>

CourseWidget::CourseWidget(QWidget *parent) : QWidget(parent), m_currentEditId(-1)
{
    // ========== 1. 创建表单区域 ==========
    QFormLayout *formLayout = new QFormLayout();

    m_nameEdit = new QLineEdit;
    m_creditSpin = new QDoubleSpinBox;
    m_creditSpin->setRange(0.5, 10.0);
    m_creditSpin->setSingleStep(0.5);
    m_creditSpin->setValue(3.0);

    m_scoreSpin = new QDoubleSpinBox;
    m_scoreSpin->setRange(0, 100);
    m_scoreSpin->setSingleStep(1);
    m_scoreSpin->setValue(85.0);

    m_courseTypeCombo = new QComboBox;
    m_courseTypeCombo->addItems({"学科基础必修", "学科基础选修", "专业方向必修",
                                "核心通识", "选修通识", "通修", "实验课", "暑课"});

    m_tagsEdit = new QLineEdit;

    // 学期下拉框：可手动输入，预设多个常用学期
    m_semesterCombo = new QComboBox;
    m_semesterCombo->setEditable(true);
    m_semesterCombo->addItems({
        "2024-2025-1", "2024-2025-2",
        "2025-2026-1", "2025-2026-2",
        "2026-2027-1", "2026-2027-2"
    });

    m_statusCombo = new QComboBox;
    m_statusCombo->addItems({"已修", "在修", "计划修"});

    formLayout->addRow("课程名:", m_nameEdit);
    formLayout->addRow("学分:", m_creditSpin);
    formLayout->addRow("成绩:", m_scoreSpin);
    formLayout->addRow("课程类型:", m_courseTypeCombo);
    formLayout->addRow("标签:", m_tagsEdit);
    formLayout->addRow("学期:", m_semesterCombo);
    formLayout->addRow("状态:", m_statusCombo);

    m_addBtn = new QPushButton("添加课程");
    m_updateBtn = new QPushButton("更新课程");
    m_updateBtn->setEnabled(false);

    QHBoxLayout *btnLayout = new QHBoxLayout;
    btnLayout->addWidget(m_addBtn);
    btnLayout->addWidget(m_updateBtn);

    // ========== 2. 搜索区域 ==========
    m_searchEdit = new QLineEdit;
    m_searchEdit->setPlaceholderText("输入关键词...");
    m_searchFieldCombo = new QComboBox;
    m_searchFieldCombo->addItems({"按课程名", "按标签", "按类型"});
    m_searchBtn = new QPushButton("搜索");

    QHBoxLayout *searchLayout = new QHBoxLayout;
    searchLayout->addWidget(new QLabel("搜索:"));
    searchLayout->addWidget(m_searchEdit);
    searchLayout->addWidget(m_searchFieldCombo);
    searchLayout->addWidget(m_searchBtn);
    searchLayout->addStretch();

    // ========== 3. 创建表格区域 ==========
    m_tableWidget = new QTableWidget;
    m_tableWidget->setColumnCount(8);
    m_tableWidget->setHorizontalHeaderLabels({"课程名", "学分", "成绩", "类型", "标签", "学期", "状态", "操作"});

    // ========== 4. GPA显示区域 ==========
    m_gpaLabel = new QLabel("当前加权GPA: 0.00");
    m_gpaLabel->setStyleSheet("font-size: 14px; font-weight: bold; color: #4caf50;");

    // ========== 5. 布局 ==========
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(formLayout);
    mainLayout->addLayout(btnLayout);
    mainLayout->addLayout(searchLayout);
    mainLayout->addWidget(m_tableWidget);
    mainLayout->addWidget(m_gpaLabel);

    // ========== 6. 连接信号 ==========
    connect(m_addBtn, &QPushButton::clicked, this, &CourseWidget::onAddClicked);
    connect(m_updateBtn, &QPushButton::clicked, this, &CourseWidget::onUpdateClicked);
    connect(m_searchBtn, &QPushButton::clicked, this, &CourseWidget::onSearchClicked);

    // ========== 7. 初始化 ==========
    refreshTable();
    updateGPA();
}

void CourseWidget::onAddClicked()
{
    QString name = m_nameEdit->text().trimmed();
    double credit = m_creditSpin->value();
    double score = m_scoreSpin->value();
    QString courseType = m_courseTypeCombo->currentText();
    QString tags = m_tagsEdit->text().trimmed();
    QString semester = m_semesterCombo->currentText();
    QString status = m_statusCombo->currentText();

    if (name.isEmpty()) {
        QMessageBox::warning(this, "错误", "课程名不能为空");
        return;
    }
    if (credit <= 0 || credit > 10) {
        QMessageBox::warning(this, "错误", "学分必须在0.5-10之间");
        return;
    }
    if (score < 0 || score > 100) {
        QMessageBox::warning(this, "错误", "成绩必须在0-100之间");
        return;
    }

    Course course;
    course.name = name;
    course.credit = credit;
    course.score = score;
    course.courseType = courseType;
    course.tags = tags;
    course.semester = semester;
    course.status = status;

    if (DataManager::instance().addCourse(course)) {
        QMessageBox::information(this, "成功", "课程添加成功");
        clearForm();
        refreshTable();
        updateGPA();
    } else {
        QMessageBox::critical(this, "错误", "保存失败，请重试");
    }
}

void CourseWidget::onUpdateClicked()
{
    if (m_currentEditId < 0) return;

    QString name = m_nameEdit->text().trimmed();
    double credit = m_creditSpin->value();
    double score = m_scoreSpin->value();
    QString courseType = m_courseTypeCombo->currentText();
    QString tags = m_tagsEdit->text().trimmed();
    QString semester = m_semesterCombo->currentText();
    QString status = m_statusCombo->currentText();

    if (name.isEmpty()) {
        QMessageBox::warning(this, "错误", "课程名不能为空");
        return;
    }
    if (credit <= 0 || credit > 10) {
        QMessageBox::warning(this, "错误", "学分必须在0.5-10之间");
        return;
    }
    if (score < 0 || score > 100) {
        QMessageBox::warning(this, "错误", "成绩必须在0-100之间");
        return;
    }

    Course course;
    course.id = m_currentEditId;
    course.name = name;
    course.credit = credit;
    course.score = score;
    course.courseType = courseType;
    course.tags = tags;
    course.semester = semester;
    course.status = status;

    if (DataManager::instance().updateCourse(course)) {
        QMessageBox::information(this, "成功", "课程更新成功");
        clearForm();
        setEditingMode(false);
        refreshTable();
        updateGPA();
    } else {
        QMessageBox::critical(this, "错误", "更新失败，请重试");
    }
}

void CourseWidget::onSearchClicked()
{
    QString keyword = m_searchEdit->text().trimmed();
    QString field = m_searchFieldCombo->currentText();

    QString searchField;
    if (field == "按课程名") searchField = "name";
    else if (field == "按标签") searchField = "tags";
    else searchField = "courseType";

    refreshTable(keyword, searchField);
}

void CourseWidget::refreshTable(const QString& keyword, const QString& field)
{
    QList<Course> allCourses = DataManager::instance().getAllCourses();
    QList<Course> courses;

    // 如果有搜索条件，进行过滤
    if (!keyword.isEmpty() && !field.isEmpty()) {
        for (const Course& c : allCourses) {
            bool match = false;
            if (field == "name") {
                match = c.name.contains(keyword, Qt::CaseInsensitive);
            } else if (field == "tags") {
                match = c.tags.contains(keyword, Qt::CaseInsensitive);
            } else if (field == "courseType") {
                match = c.courseType.contains(keyword, Qt::CaseInsensitive);
            }
            if (match) {
                courses.append(c);
            }
        }
    } else {
        courses = allCourses;
    }

    m_tableWidget->setRowCount(courses.size());
    for (int i = 0; i < courses.size(); ++i) {
        const Course& c = courses[i];
        m_tableWidget->setItem(i, 0, new QTableWidgetItem(c.name));
        m_tableWidget->setItem(i, 1, new QTableWidgetItem(QString::number(c.credit)));
        m_tableWidget->setItem(i, 2, new QTableWidgetItem(QString::number(c.score)));
        m_tableWidget->setItem(i, 3, new QTableWidgetItem(c.courseType));
        m_tableWidget->setItem(i, 4, new QTableWidgetItem(c.tags));
        m_tableWidget->setItem(i, 5, new QTableWidgetItem(c.semester));
        m_tableWidget->setItem(i, 6, new QTableWidgetItem(c.status));

        QWidget *btnWidget = new QWidget;
        QHBoxLayout *btnLayout = new QHBoxLayout(btnWidget);
        btnLayout->setContentsMargins(5, 0, 5, 0);

        QPushButton *editBtn = new QPushButton("编辑");
        QPushButton *delBtn = new QPushButton("删除");

        btnLayout->addWidget(editBtn);
        btnLayout->addWidget(delBtn);

        m_tableWidget->setCellWidget(i, 7, btnWidget);

        connect(editBtn, &QPushButton::clicked, this, [this, i]() {
            onEditClicked(i);
        });

        int courseId = c.id;
        connect(delBtn, &QPushButton::clicked, this, [this, courseId]() {
            onDeleteClicked(courseId);
        });
    }
}

void CourseWidget::onEditClicked(int row)
{
    QList<Course> courses = DataManager::instance().getAllCourses();
    if (row < 0 || row >= courses.size()) return;

    const Course& c = courses[row];

    m_nameEdit->setText(c.name);
    m_creditSpin->setValue(c.credit);
    m_scoreSpin->setValue(c.score);
    m_courseTypeCombo->setCurrentText(c.courseType);
    m_tagsEdit->setText(c.tags);
    m_semesterCombo->setCurrentText(c.semester);
    m_statusCombo->setCurrentText(c.status);

    setEditingMode(true, c.id);
}

void CourseWidget::updateGPA()
{
    QList<Course> courses = DataManager::instance().getAllCourses();
    double totalPoints = 0.0;
    double totalCredits = 0.0;

    for (const Course& c : courses) {
        if (c.status != "已修") continue;

        double gp = 0.0;
        if (c.score >= 90) gp = 4.0;
        else if (c.score >= 85) gp = 3.7;
        else if (c.score >= 82) gp = 3.3;
        else if (c.score >= 78) gp = 3.0;
        else if (c.score >= 75) gp = 2.7;
        else if (c.score >= 72) gp = 2.3;
        else if (c.score >= 68) gp = 2.0;
        else if (c.score >= 64) gp = 1.5;
        else if (c.score >= 60) gp = 1.0;
        else gp = 0.0;

        totalPoints += gp * c.credit;
        totalCredits += c.credit;
    }

    double gpa = (totalCredits > 0) ? totalPoints / totalCredits : 0.0;
    m_gpaLabel->setText(QString("当前加权GPA: %1").arg(gpa, 0, 'f', 2));
}

void CourseWidget::clearForm()
{
    m_nameEdit->clear();
    m_creditSpin->setValue(3.0);
    m_scoreSpin->setValue(85.0);
    m_tagsEdit->clear();
    m_semesterCombo->setCurrentIndex(0);
    m_statusCombo->setCurrentIndex(0);
}

void CourseWidget::setEditingMode(bool editing, int id)
{
    m_currentEditId = editing ? id : -1;
    m_addBtn->setEnabled(!editing);
    m_updateBtn->setEnabled(editing);
}

void CourseWidget::onDeleteClicked(int courseId)
{
    QString courseName;
    QList<Course> courses = DataManager::instance().getAllCourses();
    for (const Course& c : courses) {
        if (c.id == courseId) {
            courseName = c.name;
            break;
        }
    }

    QMessageBox::StandardButton reply = QMessageBox::question(
        this,
        "确认删除",
        QString("确定要删除课程《%1》吗？").arg(courseName),
        QMessageBox::Yes | QMessageBox::No
    );

    if (reply == QMessageBox::Yes) {
        if (DataManager::instance().deleteCourse(courseId)) {
            QMessageBox::information(this, "成功", "课程删除成功");
            refreshTable();
            updateGPA();
        } else {
            QMessageBox::critical(this, "错误", "删除失败，请重试");
        }
    }
}
