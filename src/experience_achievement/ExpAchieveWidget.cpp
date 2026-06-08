#include "ExpAchieveWidget.h"

ExpAchieveWidget::ExpAchieveWidget(QWidget *parent)
    : QWidget(parent)
    , m_currentExpId(-1)
    , m_currentAchId(-1)
{
    createTablesIfNotExists();
    setupUI();
    refreshExperienceTable();
    refreshAchievementTable();
}

void ExpAchieveWidget::createTablesIfNotExists()
{
    QSqlQuery q;
    q.exec("CREATE TABLE IF NOT EXISTS experience ("
           "id INTEGER PRIMARY KEY AUTOINCREMENT,"
           "type TEXT,"
           "name TEXT,"
           "startDate TEXT,"
           "endDate TEXT,"
           "role TEXT,"
           "description TEXT,"
           "tags TEXT,"
           "filePath TEXT)");

    q.exec("CREATE TABLE IF NOT EXISTS achievement ("
           "id INTEGER PRIMARY KEY AUTOINCREMENT,"
           "name TEXT,"
           "level TEXT,"
           "date TEXT,"
           "tags TEXT,"
           "filePath TEXT)");
}

void ExpAchieveWidget::setupUI()
{
    QTabWidget *tabWidget = new QTabWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(tabWidget);

    // 经历管理页面
    QWidget *expPage = new QWidget();
    QVBoxLayout *expLayout = new QVBoxLayout(expPage);

    QFormLayout *expForm = new QFormLayout();
    m_expTypeEdit = new QLineEdit();
    m_expNameEdit = new QLineEdit();
    m_expStartEdit = new QDateEdit(QDate::currentDate());
    m_expEndEdit = new QDateEdit(QDate::currentDate());
    m_expRoleEdit = new QLineEdit();
    m_expDescEdit = new QTextEdit();
    m_expTagsEdit = new QLineEdit();
    m_expFilePathEdit = new QLineEdit();

    expForm->addRow("类型:", m_expTypeEdit);
    expForm->addRow("名称:", m_expNameEdit);
    expForm->addRow("开始日期:", m_expStartEdit);
    expForm->addRow("结束日期:", m_expEndEdit);
    expForm->addRow("角色:", m_expRoleEdit);
    expForm->addRow("描述:", m_expDescEdit);
    expForm->addRow("标签:", m_expTagsEdit);
    expForm->addRow("文件路径:", m_expFilePathEdit);

    QHBoxLayout *expBtnLayout = new QHBoxLayout();
    m_expAddBtn = new QPushButton("添加");
    m_expUpdateBtn = new QPushButton("修改");
    m_expDelBtn = new QPushButton("删除");
    m_expOpenFileBtn = new QPushButton("选择文件");

    expBtnLayout->addWidget(m_expAddBtn);
    expBtnLayout->addWidget(m_expUpdateBtn);
    expBtnLayout->addWidget(m_expDelBtn);
    expBtnLayout->addWidget(m_expOpenFileBtn);

    m_expTable = new QTableWidget();
    m_expTable->setColumnCount(9);
    QStringList expHeaders;
    expHeaders << "ID" << "类型" << "名称" << "开始" << "结束" << "角色" << "描述" << "标签" << "文件";
    m_expTable->setHorizontalHeaderLabels(expHeaders);
    m_expTable->horizontalHeader()->setStretchLastSection(true);

    expLayout->addLayout(expForm);
    expLayout->addLayout(expBtnLayout);
    expLayout->addWidget(m_expTable);
    tabWidget->addTab(expPage, "经历管理");

    // 成就管理页面
    QWidget *achPage = new QWidget();
    QVBoxLayout *achLayout = new QVBoxLayout(achPage);

    QFormLayout *achForm = new QFormLayout();
    m_achNameEdit = new QLineEdit();
    m_achLevelEdit = new QLineEdit();
    m_achDateEdit = new QDateEdit(QDate::currentDate());
    m_achTagsEdit = new QLineEdit();
    m_achFilePathEdit = new QLineEdit();

    achForm->addRow("名称:", m_achNameEdit);
    achForm->addRow("等级:", m_achLevelEdit);
    achForm->addRow("日期:", m_achDateEdit);
    achForm->addRow("标签:", m_achTagsEdit);
    achForm->addRow("文件路径:", m_achFilePathEdit);

    QHBoxLayout *achBtnLayout = new QHBoxLayout();
    m_achAddBtn = new QPushButton("添加");
    m_achUpdateBtn = new QPushButton("修改");
    m_achDelBtn = new QPushButton("删除");
    m_achOpenFileBtn = new QPushButton("选择文件");

    achBtnLayout->addWidget(m_achAddBtn);
    achBtnLayout->addWidget(m_achUpdateBtn);
    achBtnLayout->addWidget(m_achDelBtn);
    achBtnLayout->addWidget(m_achOpenFileBtn);

    m_achTable = new QTableWidget();
    m_achTable->setColumnCount(6);
    QStringList achHeaders;
    achHeaders << "ID" << "名称" << "等级" << "日期" << "标签" << "文件";
    m_achTable->setHorizontalHeaderLabels(achHeaders);
    m_achTable->horizontalHeader()->setStretchLastSection(true);

    achLayout->addLayout(achForm);
    achLayout->addLayout(achBtnLayout);
    achLayout->addWidget(m_achTable);
    tabWidget->addTab(achPage, "成就管理");

    // 连接信号槽
    connect(m_expAddBtn, SIGNAL(clicked()), this, SLOT(addExperienceClicked()));
    connect(m_expUpdateBtn, SIGNAL(clicked()), this, SLOT(updateExperienceClicked()));
    connect(m_expDelBtn, SIGNAL(clicked()), this, SLOT(deleteExperienceClicked()));
    connect(m_expTable, SIGNAL(cellClicked(int, int)), this, SLOT(experienceCellClicked(int)));
    connect(m_expOpenFileBtn, SIGNAL(clicked()), this, SLOT(openExpFileClicked()));

    connect(m_achAddBtn, SIGNAL(clicked()), this, SLOT(addAchievementClicked()));
    connect(m_achUpdateBtn, SIGNAL(clicked()), this, SLOT(updateAchievementClicked()));
    connect(m_achDelBtn, SIGNAL(clicked()), this, SLOT(deleteAchievementClicked()));
    connect(m_achTable, SIGNAL(cellClicked(int, int)), this, SLOT(achievementCellClicked(int)));
    connect(m_achOpenFileBtn, SIGNAL(clicked()), this, SLOT(openAchFileClicked()));
}

// 经历相关函数
void ExpAchieveWidget::addExperienceClicked()
{
    QSqlQuery q;
    q.prepare("INSERT INTO experience (type,name,startDate,endDate,role,description,tags,filePath) VALUES (?,?,?,?,?,?,?,?)");
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
    DataManager::instance().notifyDataChanged();
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
    DataManager::instance().notifyDataChanged();
}

void ExpAchieveWidget::deleteExperienceClicked()
{
    if(m_currentExpId == -1) return;
    QSqlQuery q;
    q.prepare("DELETE FROM experience WHERE id=?");
    q.addBindValue(m_currentExpId);
    q.exec();
    refreshExperienceTable();
    clearExperienceForm();
    DataManager::instance().notifyDataChanged();
}

void ExpAchieveWidget::experienceCellClicked(int row)
{
    m_currentExpId = m_expTable->item(row, 0)->text().toInt();
    m_expTypeEdit->setText(m_expTable->item(row, 1)->text());
    m_expNameEdit->setText(m_expTable->item(row, 2)->text());
    m_expStartEdit->setDate(QDate::fromString(m_expTable->item(row, 3)->text(), "yyyy-MM-dd"));
    m_expEndEdit->setDate(QDate::fromString(m_expTable->item(row, 4)->text(), "yyyy-MM-dd"));
    m_expRoleEdit->setText(m_expTable->item(row, 5)->text());
    m_expDescEdit->setPlainText(m_expTable->item(row, 6)->text());
    m_expTagsEdit->setText(m_expTable->item(row, 7)->text());
    m_expFilePathEdit->setText(m_expTable->item(row, 8)->text());
}

void ExpAchieveWidget::openExpFileClicked()
{
    QString path = QFileDialog::getOpenFileName(this, "选择文件");
    if(!path.isEmpty()) {
        m_expFilePathEdit->setText(path);
    }
}

// 成就相关函数
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
    DataManager::instance().notifyDataChanged();
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
    DataManager::instance().notifyDataChanged();
}

void ExpAchieveWidget::deleteAchievementClicked()
{
    if(m_currentAchId == -1) return;
    QSqlQuery q;
    q.prepare("DELETE FROM achievement WHERE id=?");
    q.addBindValue(m_currentAchId);
    q.exec();
    refreshAchievementTable();
    clearAchievementForm();
    DataManager::instance().notifyDataChanged();
}

void ExpAchieveWidget::achievementCellClicked(int row)
{
    m_currentAchId = m_achTable->item(row, 0)->text().toInt();
    m_achNameEdit->setText(m_achTable->item(row, 1)->text());
    m_achLevelEdit->setText(m_achTable->item(row, 2)->text());
    m_achDateEdit->setDate(QDate::fromString(m_achTable->item(row, 3)->text(), "yyyy-MM-dd"));
    m_achTagsEdit->setText(m_achTable->item(row, 4)->text());
    m_achFilePathEdit->setText(m_achTable->item(row, 5)->text());
}

void ExpAchieveWidget::openAchFileClicked()
{
    QString path = QFileDialog::getOpenFileName(this, "选择文件");
    if(!path.isEmpty()) {
        m_achFilePathEdit->setText(path);
    }
}

// 刷新和清空函数
void ExpAchieveWidget::refreshExperienceTable()
{
    m_expTable->setRowCount(0);
    QSqlQuery q("SELECT * FROM experience");
    while(q.next()) {
        int row = m_expTable->rowCount();
        m_expTable->insertRow(row);
        m_expTable->setItem(row, 0, new QTableWidgetItem(QString::number(q.value(0).toInt())));
        m_expTable->setItem(row, 1, new QTableWidgetItem(q.value(1).toString()));
        m_expTable->setItem(row, 2, new QTableWidgetItem(q.value(2).toString()));
        m_expTable->setItem(row, 3, new QTableWidgetItem(q.value(3).toString()));
        m_expTable->setItem(row, 4, new QTableWidgetItem(q.value(4).toString()));
        m_expTable->setItem(row, 5, new QTableWidgetItem(q.value(5).toString()));
        m_expTable->setItem(row, 6, new QTableWidgetItem(q.value(6).toString()));
        m_expTable->setItem(row, 7, new QTableWidgetItem(q.value(7).toString()));
        m_expTable->setItem(row, 8, new QTableWidgetItem(q.value(8).toString()));
    }
}

void ExpAchieveWidget::refreshAchievementTable()
{
    m_achTable->setRowCount(0);
    QSqlQuery q("SELECT * FROM achievement");
    while(q.next()) {
        int row = m_achTable->rowCount();
        m_achTable->insertRow(row);
        m_achTable->setItem(row, 0, new QTableWidgetItem(QString::number(q.value(0).toInt())));
        m_achTable->setItem(row, 1, new QTableWidgetItem(q.value(1).toString()));
        m_achTable->setItem(row, 2, new QTableWidgetItem(q.value(2).toString()));
        m_achTable->setItem(row, 3, new QTableWidgetItem(q.value(3).toString()));
        m_achTable->setItem(row, 4, new QTableWidgetItem(q.value(4).toString()));
        m_achTable->setItem(row, 5, new QTableWidgetItem(q.value(5).toString()));
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

void ExpAchieveWidget::clearAchievementForm()
{
    m_achNameEdit->clear();
    m_achLevelEdit->clear();
    m_achDateEdit->setDate(QDate::currentDate());
    m_achTagsEdit->clear();
    m_achFilePathEdit->clear();
    m_currentAchId = -1;
}
