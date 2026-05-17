#include "ExpAchieveWidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QTabWidget>
#include <QFileDialog>
#include <QHeaderView>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>

ExpAchieveWidget::ExpAchieveWidget(QWidget *parent)
    : QWidget(parent)
{
    createTablesIfNotExists();
    setupUI();
    refreshExperienceTable();
    refreshAchievementTable();
}

void ExpAchieveWidget::createTablesIfNotExists()
{
    QSqlDatabase db = QSqlDatabase::database();
    if (!db.isOpen()) return;

    QSqlQuery q;
    q.exec("CREATE TABLE IF NOT EXISTS experience ("
           "id INTEGER PRIMARY KEY AUTOINCREMENT,"
           "type TEXT, name TEXT, startDate TEXT, endDate TEXT,"
           "role TEXT, description TEXT, tags TEXT, filePath TEXT)");

    q.exec("CREATE TABLE IF NOT EXISTS achievement ("
           "id INTEGER PRIMARY KEY AUTOINCREMENT,"
           "name TEXT, level TEXT, date TEXT, tags TEXT, filePath TEXT)");
}

void ExpAchieveWidget::setupUI()
{
    m_currentExpId = -1;
    m_currentAchId = -1;

    auto *mainLayout = new QVBoxLayout(this);
    auto *tabWidget = new QTabWidget;

    auto *expTab = new QWidget;
    auto *expLayout = new QVBoxLayout(expTab);
    auto *expFormLayout = new QFormLayout;

    m_expTypeEdit = new QLineEdit;
    m_expNameEdit = new QLineEdit;
    m_expStartEdit = new QDateEdit(QDate::currentDate());
    m_expEndEdit = new QDateEdit(QDate::currentDate());
    m_expRoleEdit = new QLineEdit;
    m_expDescEdit = new QTextEdit;
    m_expTagsEdit = new QLineEdit;
    m_expFilePathEdit = new QLineEdit;

    expFormLayout->addRow("类型", m_expTypeEdit);
    expFormLayout->addRow("名称", m_expNameEdit);
    expFormLayout->addRow("开始时间", m_expStartEdit);
    expFormLayout->addRow("结束时间", m_expEndEdit);
    expFormLayout->addRow("角色", m_expRoleEdit);
    expFormLayout->addRow("描述", m_expDescEdit);
    expFormLayout->addRow("标签", m_expTagsEdit);
    expFormLayout->addRow("材料路径", m_expFilePathEdit);

    auto *expBtnLayout = new QHBoxLayout;
    m_expAddBtn = new QPushButton("添加");
    m_expUpdateBtn = new QPushButton("修改");
    m_expDelBtn = new QPushButton("删除");
    m_expOpenFileBtn = new QPushButton("选择文件");
    expBtnLayout->addWidget(m_expAddBtn);
    expBtnLayout->addWidget(m_expUpdateBtn);
    expBtnLayout->addWidget(m_expDelBtn);
    expBtnLayout->addWidget(m_expOpenFileBtn);

    m_expTable = new QTableWidget;
    m_expTable->setColumnCount(7);
    m_expTable->setHorizontalHeaderLabels({"ID","类型","名称","开始","结束","角色","标签"});
    m_expTable->horizontalHeader()->setStretchLastSection(true);

    expLayout->addLayout(expFormLayout);
    expLayout->addLayout(expBtnLayout);
    expLayout->addWidget(m_expTable);

    auto *achTab = new QWidget;
    auto *achLayout = new QVBoxLayout(achTab);
    auto *achFormLayout = new QFormLayout;

    m_achNameEdit = new QLineEdit;
    m_achLevelEdit = new QLineEdit;
    m_achDateEdit = new QDateEdit(QDate::currentDate());
    m_achTagsEdit = new QLineEdit;
    m_achFilePathEdit = new QLineEdit;

    achFormLayout->addRow("名称", m_achNameEdit);
    achFormLayout->addRow("级别", m_achLevelEdit);
    achFormLayout->addRow("获得时间", m_achDateEdit);
    achFormLayout->addRow("标签", m_achTagsEdit);
    achFormLayout->addRow("材料路径", m_achFilePathEdit);

    auto *achBtnLayout = new QHBoxLayout;
    m_achAddBtn = new QPushButton("添加");
    m_achUpdateBtn = new QPushButton("修改");
    m_achDelBtn = new QPushButton("删除");
    m_achOpenFileBtn = new QPushButton("选择文件");
    achBtnLayout->addWidget(m_achAddBtn);
    achBtnLayout->addWidget(m_achUpdateBtn);
    achBtnLayout->addWidget(m_achDelBtn);
    achBtnLayout->addWidget(m_achOpenFileBtn);

    m_achTable = new QTableWidget;
    m_achTable->setColumnCount(6);
    m_achTable->setHorizontalHeaderLabels({"ID","名称","级别","时间","标签","路径"});
    m_achTable->horizontalHeader()->setStretchLastSection(true);

    achLayout->addLayout(achFormLayout);
    achLayout->addLayout(achBtnLayout);
    achLayout->addWidget(m_achTable);

    tabWidget->addTab(expTab,"经历管理");
    tabWidget->addTab(achTab,"成就管理");
    mainLayout->addWidget(tabWidget);

    connect(m_expAddBtn, &QPushButton::clicked, this, &ExpAchieveWidget::addExperienceClicked);
    connect(m_expUpdateBtn, &QPushButton::clicked, this, &ExpAchieveWidget::updateExperienceClicked);
    connect(m_expDelBtn, &QPushButton::clicked, [this](){ deleteExperienceClicked(m_currentExpId); });
    connect(m_expOpenFileBtn, &QPushButton::clicked, this, &ExpAchieveWidget::openExpFileClicked);
    connect(m_expTable, &QTableWidget::cellClicked, this, &ExpAchieveWidget::experienceCellClicked);

    connect(m_achAddBtn, &QPushButton::clicked, this, &ExpAchieveWidget::addAchievementClicked);
    connect(m_achUpdateBtn, &QPushButton::clicked, this, &ExpAchieveWidget::updateAchievementClicked);
    connect(m_achDelBtn, &QPushButton::clicked, [this](){ deleteAchievementClicked(m_currentAchId); });
    connect(m_achOpenFileBtn, &QPushButton::clicked, this, &ExpAchieveWidget::openAchFileClicked);
    connect(m_achTable, &QTableWidget::cellClicked, this, &ExpAchieveWidget::achievementCellClicked);
}

void ExpAchieveWidget::refreshExperienceTable()
{
    m_expTable->setRowCount(0);
    QSqlQuery q;
    q.exec("SELECT id,type,name,startDate,endDate,role,tags FROM experience");
    while (q.next()) {
        int row = m_expTable->rowCount();
        m_expTable->insertRow(row);
        m_expTable->setItem(row,0,new QTableWidgetItem(q.value(0).toString()));
        m_expTable->setItem(row,1,new QTableWidgetItem(q.value(1).toString()));
        m_expTable->setItem(row,2,new QTableWidgetItem(q.value(2).toString()));
        m_expTable->setItem(row,3,new QTableWidgetItem(q.value(3).toString()));
        m_expTable->setItem(row,4,new QTableWidgetItem(q.value(4).toString()));
        m_expTable->setItem(row,5,new QTableWidgetItem(q.value(5).toString()));
        m_expTable->setItem(row,6,new QTableWidgetItem(q.value(6).toString()));
    }
}

void ExpAchieveWidget::clearExperienceForm()
{
    m_expTypeEdit->clear();
    m_expNameEdit->clear();
    m_expStartEdit->setDate(QDate::currentDate());
    m_expEndEdit->setDate(QDate::currentDate());
    m_expRoleEdit->clear();
    m_expDescEdit->clear();
    m_expTagsEdit->clear();
    m_expFilePathEdit->clear();
    m_currentExpId = -1;
}

void ExpAchieveWidget::addExperienceClicked()
{
    QSqlQuery q;
    q.prepare("INSERT INTO experience (type,name,startDate,endDate,role,description,tags,filePath) "
              "VALUES (?,?,?,?,?,?,?,?)");
    q.addBindValue(m_expTypeEdit->text().trimmed());
    q.addBindValue(m_expNameEdit->text().trimmed());
    q.addBindValue(m_expStartEdit->date().toString("yyyy-MM-dd"));
    q.addBindValue(m_expEndEdit->date().toString("yyyy-MM-dd"));
    q.addBindValue(m_expRoleEdit->text().trimmed());
    q.addBindValue(m_expDescEdit->toPlainText().trimmed());
    q.addBindValue(m_expTagsEdit->text().trimmed());
    q.addBindValue(m_expFilePathEdit->text().trimmed());
    q.exec();

    refreshExperienceTable();
    clearExperienceForm();
}

void ExpAchieveWidget::updateExperienceClicked()
{
    if(m_currentExpId == -1) return;
    QSqlQuery q;
    q.prepare("UPDATE experience SET type=?,name=?,startDate=?,endDate=?,role=?,description=?,tags=?,filePath=? WHERE id=?");
    q.addBindValue(m_expTypeEdit->text().trimmed());
    q.addBindValue(m_expNameEdit->text().trimmed());
    q.addBindValue(m_expStartEdit->date().toString("yyyy-MM-dd"));
    q.addBindValue(m_expEndEdit->date().toString("yyyy-MM-dd"));
    q.addBindValue(m_expRoleEdit->text().trimmed());
    q.addBindValue(m_expDescEdit->toPlainText().trimmed());
    q.addBindValue(m_expTagsEdit->text().trimmed());
    q.addBindValue(m_expFilePathEdit->text().trimmed());
    q.addBindValue(m_currentExpId);
    q.exec();

    refreshExperienceTable();
    clearExperienceForm();
}

void ExpAchieveWidget::deleteExperienceClicked(int expId)
{
    if(expId == -1) return;
    QSqlQuery q;
    q.prepare("DELETE FROM experience WHERE id=?");
    q.addBindValue(expId);
    q.exec();
    refreshExperienceTable();
    clearExperienceForm();
}

void ExpAchieveWidget::experienceCellClicked(int row)
{
    m_currentExpId = m_expTable->item(row,0)->text().toInt();
    QSqlQuery q;
    q.prepare("SELECT * FROM experience WHERE id=?");
    q.addBindValue(m_currentExpId);
    q.exec();
    if(q.next()){
        m_expTypeEdit->setText(q.value("type").toString());
        m_expNameEdit->setText(q.value("name").toString());
        m_expStartEdit->setDate(QDate::fromString(q.value("startDate").toString(),"yyyy-MM-dd"));
        m_expEndEdit->setDate(QDate::fromString(q.value("endDate").toString(),"yyyy-MM-dd"));
        m_expRoleEdit->setText(q.value("role").toString());
        m_expDescEdit->setPlainText(q.value("description").toString());
        m_expTagsEdit->setText(q.value("tags").toString());
        m_expFilePathEdit->setText(q.value("filePath").toString());
    }
}

void ExpAchieveWidget::openExpFileClicked()
{
    QString path = QFileDialog::getOpenFileName(this,"选择文件");
    if(!path.isEmpty()) m_expFilePathEdit->setText(path);
}

void ExpAchieveWidget::refreshAchievementTable()
{
    m_achTable->setRowCount(0);
    QSqlQuery q;
    q.exec("SELECT id,name,level,date,tags,filePath FROM achievement");
    while (q.next()) {
        int row = m_achTable->rowCount();
        m_achTable->insertRow(row);
        m_achTable->setItem(row,0,new QTableWidgetItem(q.value(0).toString()));
        m_achTable->setItem(row,1,new QTableWidgetItem(q.value(1).toString()));
        m_achTable->setItem(row,2,new QTableWidgetItem(q.value(2).toString()));
        m_achTable->setItem(row,3,new QTableWidgetItem(q.value(3).toString()));
        m_achTable->setItem(row,4,new QTableWidgetItem(q.value(4).toString()));
        m_achTable->setItem(row,5,new QTableWidgetItem(q.value(5).toString()));
    }
}

void ExpAchieveWidget::clearAchievementForm()
{
    m_achNameEdit->clear();
    m_achLevelEdit->clear();
    m_achDateEdit->setDate(QDate::currentDate());
    m_achTagsEdit->clear();
    m_achFilePathEdit->clear();
    m_currentAchId = -1;
}

void ExpAchieveWidget::addAchievementClicked()
{
    QSqlQuery q;
    q.prepare("INSERT INTO achievement (name,level,date,tags,filePath) VALUES (?,?,?,?,?)");
    q.addBindValue(m_achNameEdit->text().trimmed());
    q.addBindValue(m_achLevelEdit->text().trimmed());
    q.addBindValue(m_achDateEdit->date().toString("yyyy-MM-dd"));
    q.addBindValue(m_achTagsEdit->text().trimmed());
    q.addBindValue(m_achFilePathEdit->text().trimmed());
    q.exec();

    refreshAchievementTable();
    clearAchievementForm();
}

void ExpAchieveWidget::updateAchievementClicked()
{
    if(m_currentAchId == -1) return;
    QSqlQuery q;
    q.prepare("UPDATE achievement SET name=?,level=?,date=?,tags=?,filePath=? WHERE id=?");
    q.addBindValue(m_achNameEdit->text().trimmed());
    q.addBindValue(m_achLevelEdit->text().trimmed());
    q.addBindValue(m_achDateEdit->date().toString("yyyy-MM-dd"));
    q.addBindValue(m_achTagsEdit->text().trimmed());
    q.addBindValue(m_achFilePathEdit->text().trimmed());
    q.addBindValue(m_currentAchId);
    q.exec();

    refreshAchievementTable();
    clearAchievementForm();
}

void ExpAchieveWidget::deleteAchievementClicked(int achId)
{
    if(achId == -1) return;
    QSqlQuery q;
    q.prepare("DELETE FROM achievement WHERE id=?");
    q.addBindValue(achId);
    q.exec();
    refreshAchievementTable();
    clearAchievementForm();
}

void ExpAchieveWidget::achievementCellClicked(int row)
{
    m_currentAchId = m_achTable->item(row,0)->text().toInt();
    QSqlQuery q;
    q.prepare("SELECT * FROM achievement WHERE id=?");
    q.addBindValue(m_currentAchId);
    q.exec();
    if(q.next()){
        m_achNameEdit->setText(q.value("name").toString());
        m_achLevelEdit->setText(q.value("level").toString());
        m_achDateEdit->setDate(QDate::fromString(q.value("date").toString(),"yyyy-MM-dd"));
        m_achTagsEdit->setText(q.value("tags").toString());
        m_achFilePathEdit->setText(q.value("filePath").toString());
    }
}

void ExpAchieveWidget::openAchFileClicked()
{
    QString path = QFileDialog::getOpenFileName(this,"选择文件");
    if(!path.isEmpty()) m_achFilePathEdit->setText(path);
}
