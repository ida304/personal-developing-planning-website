#include "CourseWidget.h"
#include "../core/DataManager.h"
#include "GpaCalculator.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QTableWidget>
#include <QHeaderView>
#include <QPushButton>
#include <QMessageBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QDoubleSpinBox>

CourseWidget::CourseWidget(QWidget *parent) : QWidget(parent)
{
    setupUI();
    refreshTable();
    connect(&DataManager::instance(), &DataManager::dataChanged, this, &CourseWidget::refreshTable);
}

void CourseWidget::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(10, 10, 10, 10);
    mainLayout->setSpacing(6);

    // 筛选栏
    QHBoxLayout *filterLayout = new QHBoxLayout;
    filterLayout->addWidget(new QLabel("搜索:"));
    _searchEdit = new QLineEdit;
    _searchEdit->setPlaceholderText("课程名/课序号");
    filterLayout->addWidget(_searchEdit);

    filterLayout->addWidget(new QLabel("学期:"));
    _semesterCombo = new QComboBox;
    _semesterCombo->addItem("全部");
    _semesterCombo->addItems({"2024-2025-1", "2024-2025-2", "2025-2026-1", "2025-2026-2"});
    filterLayout->addWidget(_semesterCombo);

    filterLayout->addWidget(new QLabel("课程类型:"));
    _typeCombo = new QComboBox;
    _typeCombo->addItem("全部");
    _typeCombo->addItems({"学科基础必修", "学科基础选修", "专业方向必修", "核心通识", "选修通识", "通修", "实验课", "暑课"});
    filterLayout->addWidget(_typeCombo);

    filterLayout->addWidget(new QLabel("标签:"));
    _tagCombo = new QComboBox;
    _tagCombo->setEditable(true);
    _tagCombo->addItem("全部");
    filterLayout->addWidget(_tagCombo);

    _searchBtn = new QPushButton("查询");
    _resetBtn = new QPushButton("重置");
    filterLayout->addWidget(_searchBtn);
    filterLayout->addWidget(_resetBtn);
    filterLayout->addStretch();

    // 表格
    _table = new QTableWidget;
    _table->setColumnCount(8);
    _table->setHorizontalHeaderLabels({"ID", "课序号", "课程名", "学期", "学分", "成绩", "课程类型", "标签"});
    _table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    _table->setSelectionBehavior(QAbstractItemView::SelectRows);
    _table->horizontalHeader()->setStretchLastSection(true);
    _table->hideColumn(0);

    // 统计信息
    _statLabel = new QLabel;
    _statLabel->setStyleSheet("background-color: #e8f0fe; padding: 5px; border-radius: 3px;");

    // 按钮栏
    QHBoxLayout *btnLayout = new QHBoxLayout;
    _addBtn = new QPushButton("添加课程");
    _editBtn = new QPushButton("修改");
    _delBtn = new QPushButton("删除");
    btnLayout->addWidget(_addBtn);
    btnLayout->addWidget(_editBtn);
    btnLayout->addWidget(_delBtn);
    btnLayout->addStretch();

    mainLayout->addLayout(filterLayout);
    mainLayout->addWidget(_table, 1);
    mainLayout->addWidget(_statLabel);
    mainLayout->addLayout(btnLayout);

    connect(_searchBtn, &QPushButton::clicked, this, &CourseWidget::onSearchClicked);
    connect(_resetBtn, &QPushButton::clicked, this, &CourseWidget::onSearchClicked);
    connect(_addBtn, &QPushButton::clicked, this, &CourseWidget::onAddClicked);
    connect(_editBtn, &QPushButton::clicked, this, &CourseWidget::onEditClicked);
    connect(_delBtn, &QPushButton::clicked, this, &CourseWidget::onDeleteClicked);
    connect(_semesterCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &CourseWidget::onSearchClicked);
    connect(_typeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &CourseWidget::onSearchClicked);
    connect(_tagCombo, &QComboBox::currentTextChanged, this, &CourseWidget::onSearchClicked);
}

void CourseWidget::refreshTable()
{
    QList<Course> allCourses = DataManager::instance().getAllCourses();
    QStringList allTags;
    for (const Course& c : allCourses) {
        if (!c.tags.isEmpty()) {
            QStringList tags = c.tags.split(',', Qt::SkipEmptyParts);
            for (const QString& t : tags) allTags << t.trimmed();
        }
    }
    allTags.removeDuplicates();
    _tagCombo->clear();
    _tagCombo->addItem("全部");
    _tagCombo->addItems(allTags);
    onSearchClicked();
}

void CourseWidget::onSearchClicked()
{
    QString keyword = _searchEdit->text().trimmed();
    QString semester = _semesterCombo->currentText();
    if (semester == "全部") semester = "";
    QString courseType = _typeCombo->currentText();
    if (courseType == "全部") courseType = "";
    QString tagKeyword = _tagCombo->currentText();
    if (tagKeyword == "全部") tagKeyword = "";

    QList<Course> filtered = DataManager::instance().getCoursesByFilter(keyword, semester, courseType, tagKeyword);
    loadCoursesToTable(filtered);
    updateStatistics(filtered);
}

void CourseWidget::loadCoursesToTable(const QList<Course>& courses)
{
    _table->setRowCount(courses.size());
    for (int i = 0; i < courses.size(); ++i) {
        const Course& c = courses[i];
        _table->setItem(i, 0, new QTableWidgetItem(QString::number(c.id)));
        _table->setItem(i, 1, new QTableWidgetItem(c.courseCode));
        _table->setItem(i, 2, new QTableWidgetItem(c.name));
        _table->setItem(i, 3, new QTableWidgetItem(c.semester));
        _table->setItem(i, 4, new QTableWidgetItem(QString::number(c.credit)));
        _table->setItem(i, 5, new QTableWidgetItem(QString::number(c.score)));
        _table->setItem(i, 6, new QTableWidgetItem(c.courseType));
        _table->setItem(i, 7, new QTableWidgetItem(c.tags));
    }
    _table->resizeColumnsToContents();
}

void CourseWidget::updateStatistics(const QList<Course>& courses)
{
    double avgScore = GPACalculator::calculateAverageScore(courses);
    double gpa = GPACalculator::calculateGpa(courses);
    _statLabel->setText(QString("当前显示 %1 门课程 | 平均成绩: %2  |  GPA: %3")
                        .arg(courses.size())
                        .arg(avgScore, 0, 'f', 2)
                        .arg(gpa, 0, 'f', 2));
}

void CourseWidget::showCourseDialog(Course* course)
{
    QDialog dialog(this);
    dialog.setWindowTitle(course ? "修改课程" : "添加课程");
    QFormLayout *form = new QFormLayout(&dialog);

    QLineEdit *codeEdit = new QLineEdit;
    QLineEdit *nameEdit = new QLineEdit;
    QComboBox *semesterCombo = new QComboBox;
    semesterCombo->addItems({"2024-2025-1", "2024-2025-2", "2025-2026-1", "2025-2026-2"});
    QDoubleSpinBox *creditSpin = new QDoubleSpinBox;
    creditSpin->setRange(0, 10);
    creditSpin->setSingleStep(0.5);
    QDoubleSpinBox *scoreSpin = new QDoubleSpinBox;
    scoreSpin->setRange(0, 100);
    scoreSpin->setSuffix(" 分");
    QComboBox *typeCombo = new QComboBox;
    typeCombo->addItems({"学科基础必修", "学科基础选修", "专业方向必修", "核心通识", "选修通识", "通修", "实验课", "暑课"});
    QLineEdit *tagsEdit = new QLineEdit;
    tagsEdit->setPlaceholderText("多个标签用逗号分隔");

    if (course) {
        codeEdit->setText(course->courseCode);
        nameEdit->setText(course->name);
        semesterCombo->setCurrentText(course->semester);
        creditSpin->setValue(course->credit);
        scoreSpin->setValue(course->score);
        typeCombo->setCurrentText(course->courseType);
        tagsEdit->setText(course->tags);
    }

    form->addRow("课序号(可选):", codeEdit);
    form->addRow("课程名 *:", nameEdit);
    form->addRow("学期 *:", semesterCombo);
    form->addRow("学分 *:", creditSpin);
    form->addRow("成绩 *:", scoreSpin);
    form->addRow("课程类型 *:", typeCombo);
    form->addRow("自定义标签:", tagsEdit);

    QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
    form->addRow(buttons);

    if (dialog.exec() == QDialog::Accepted) {
        if (nameEdit->text().trimmed().isEmpty()) {
            QMessageBox::warning(this, "提示", "课程名不能为空");
            return;
        }
        Course newCourse;
        newCourse.courseCode = codeEdit->text().trimmed();
        newCourse.name = nameEdit->text().trimmed();
        newCourse.semester = semesterCombo->currentText();
        newCourse.credit = creditSpin->value();
        newCourse.score = scoreSpin->value();
        newCourse.courseType = typeCombo->currentText();
        newCourse.tags = tagsEdit->text().trimmed();

        if (course) {
            newCourse.id = course->id;
            if (DataManager::instance().updateCourse(course->id, newCourse))
                QMessageBox::information(&dialog, "成功", "修改成功");
            else
                QMessageBox::warning(&dialog, "错误", "修改失败");
        } else {
            if (DataManager::instance().addCourse(newCourse))
                QMessageBox::information(&dialog, "成功", "添加成功");
            else
                QMessageBox::warning(&dialog, "错误", "添加失败");
        }
        refreshTable();
    }
}

void CourseWidget::onAddClicked()
{
    showCourseDialog(nullptr);
}

void CourseWidget::onEditClicked()
{
    int row = _table->currentRow();
    if (row < 0) {
        QMessageBox::information(this, "提示", "请先选择要修改的课程");
        return;
    }
    int id = _table->item(row, 0)->text().toInt();
    QList<Course> all = DataManager::instance().getAllCourses();
    for (Course& c : all) {
        if (c.id == id) {
            showCourseDialog(&c);
            break;
        }
    }
}

void CourseWidget::onDeleteClicked()
{
    int row = _table->currentRow();
    if (row < 0) {
        QMessageBox::information(this, "提示", "请先选择要删除的课程");
        return;
    }
    int id = _table->item(row, 0)->text().toInt();
    if (QMessageBox::question(this, "确认", "确定删除该课程吗？") == QMessageBox::Yes) {
        if (DataManager::instance().deleteCourse(id))
            refreshTable();
        else
            QMessageBox::warning(this, "错误", "删除失败");
    }
}
