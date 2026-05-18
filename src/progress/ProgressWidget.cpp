#include "ProgressWidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QDesktopServices>
#include <QUrl>
#include <QDateTime>
#include <QFile>
#include <QStandardPaths>
#include <QDoubleSpinBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QDebug>

ProgressWidget::ProgressWidget(QWidget *parent)
    : QWidget(parent)
{
    // 初始化八大类别
    categories << "学科基础必修" << "学科基础选修" << "专业方向必修"
               << "核心通识" << "选修通识" << "通修" << "实验课" << "暑课";

    setupUI();
    categoryMapping = loadCategoryMapping();

    // 连接DataManager的信号
    connect(&DataManager::instance(), &DataManager::dataChanged,
            this, &ProgressWidget::refreshData);

    // 初始加载数据
    refreshData();
}

ProgressWidget::~ProgressWidget()
{
}

void ProgressWidget::setupUI()
{
    mainLayout = new QGridLayout(this);
    mainLayout->setSpacing(20);
    mainLayout->setContentsMargins(20, 20, 20, 20);

    // 创建标题
    QLabel *titleLabel = new QLabel("培养进度追踪", this);
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(18);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    titleLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(titleLabel, 0, 0, 1, 4, Qt::AlignCenter);

    // 添加空白行
    mainLayout->addItem(new QSpacerItem(20, 20), 1, 0);

    // 创建2x4网格的进度显示
    int row = 2;
    int col = 0;

    for (const QString &category : categories) {
        // 类别名称
        QLabel *categoryLabel = new QLabel(category, this);
        categoryLabel->setMinimumWidth(120);
        categoryLabel->setMaximumWidth(150);
        categoryLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        mainLayout->addWidget(categoryLabel, row, col * 3, Qt::AlignLeft);

        // 进度条
        QProgressBar *progressBar = new QProgressBar(this);
        progressBar->setRange(0, 100);
        progressBar->setValue(0);
        progressBar->setMinimumHeight(25);
        progressBar->setFormat("%p%");
        progressBars[category] = progressBar;
        mainLayout->addWidget(progressBar, row, col * 3 + 1);

        // 学分显示标签
        QLabel *creditLabel = new QLabel("0.0 / 0.0", this);
        creditLabel->setMinimumWidth(100);
        creditLabel->setAlignment(Qt::AlignCenter | Qt::AlignVCenter);
        creditLabels[category] = creditLabel;
        mainLayout->addWidget(creditLabel, row, col * 3 + 2);

        // 缺口显示标签
        QLabel *gapLabel = new QLabel("", this);
        gapLabel->setMinimumWidth(80);
        gapLabel->setAlignment(Qt::AlignCenter | Qt::AlignVCenter);
        gapLabels[category] = gapLabel;
        mainLayout->addWidget(gapLabel, row, col * 3 + 3);

        col++;
        if (col > 1) {
            col = 0;
            row++;
            mainLayout->addItem(new QSpacerItem(20, 10), row - 1, 0);
        }
    }

    // 添加弹性空间
    mainLayout->setRowStretch(row + 1, 1);

    // 导出按钮
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();

    exportButton = new QPushButton("导出进度报告", this);
    exportButton->setMinimumWidth(150);
    exportButton->setMinimumHeight(35);
    exportButton->setStyleSheet(
        "QPushButton { background-color: #4CAF50; color: white; border-radius: 5px; padding: 8px; }"
        "QPushButton:hover { background-color: #45a049; }"
    );
    connect(exportButton, &QPushButton::clicked, this, &ProgressWidget::generateHTMLReport);
    buttonLayout->addWidget(exportButton);
    buttonLayout->addStretch();

    mainLayout->addLayout(buttonLayout, row + 2, 0, 1, 4);
}

QMap<QString, QStringList> ProgressWidget::loadCategoryMapping()
{
    QMap<QString, QStringList> mapping;

    QFile file(":/data/course_category_mapping.json");
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
        QJsonObject obj = doc.object();

        for (auto it = obj.begin(); it != obj.end(); ++it) {
            QStringList targets;
            QJsonArray arr = it.value().toArray();
            for (const QJsonValue &val : arr) {
                targets.append(val.toString());
            }
            mapping[it.key()] = targets;
        }
        file.close();
    }

    return mapping;
}

bool ProgressWidget::loadGraduationReqsWithFallback()
{
    QFile reqFile(":/data/graduation_reqs.json");
    if (reqFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QJsonDocument doc = QJsonDocument::fromJson(reqFile.readAll());
        QJsonObject obj = doc.object();
        reqFile.close();

        UserProfile profile = DataManager::instance().getUserProfile();
        QString major = profile.major;
        QString grade = profile.grade;

        if (obj.contains(major)) {
            QJsonObject majorObj = obj[major].toObject();
            if (majorObj.contains(grade)) {
                QJsonObject gradeObj = majorObj[grade].toObject();

                requiredCredits.clear();
                for (const QString &cat : categories) {
                    if (gradeObj.contains(cat)) {
                        requiredCredits[cat] = gradeObj[cat].toDouble();
                    } else {
                        requiredCredits[cat] = 0.0;
                    }
                }
                return true;
            }
        }
    }

    showManualCreditDialog();
    return false;
}

void ProgressWidget::showManualCreditDialog()
{
    QDialog dialog(this);
    dialog.setWindowTitle("手动录入应修学分");
    dialog.setMinimumWidth(400);

    QVBoxLayout *layout = new QVBoxLayout(&dialog);

    QLabel *infoLabel = new QLabel("未找到您专业/年级的培养方案，请手动录入各类别应修学分：", &dialog);
    infoLabel->setWordWrap(true);
    layout->addWidget(infoLabel);

    QMap<QString, QDoubleSpinBox*> spinBoxes;

    for (const QString &cat : categories) {
        QHBoxLayout *hLayout = new QHBoxLayout();
        QLabel *label = new QLabel(cat + ":", &dialog);
        QDoubleSpinBox *spinBox = new QDoubleSpinBox(&dialog);
        spinBox->setRange(0, 100);
        spinBox->setDecimals(1);
        spinBox->setValue(0.0);
        spinBoxes[cat] = spinBox;

        hLayout->addWidget(label);
        hLayout->addWidget(spinBox);
        layout->addLayout(hLayout);
    }

    QDialogButtonBox *buttonBox = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);
    connect(buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
    layout->addWidget(buttonBox);

    if (dialog.exec() == QDialog::Accepted) {
        requiredCredits.clear();
        for (const QString &cat : categories) {
            requiredCredits[cat] = spinBoxes[cat]->value();
        }
    } else {
        requiredCredits.clear();
        for (const QString &cat : categories) {
            requiredCredits[cat] = 0.0;
        }
    }
}

QMap<QString, double> ProgressWidget::calculateEarnedCredits()
{
    QMap<QString, double> earnedCredits;

    for (const QString &cat : categories) {
        earnedCredits[cat] = 0.0;
    }

    QList<Course> courses = DataManager::instance().getAllCourses();

    for (const Course &course : courses) {
        if (course.status != "已修") {
            continue;
        }

        if (categoryMapping.contains(course.courseType)) {
            QStringList targetCategories = categoryMapping[course.courseType];
            for (const QString &targetCat : targetCategories) {
                if (earnedCredits.contains(targetCat)) {
                    earnedCredits[targetCat] += course.credit;
                }
            }
        } else {
            if (earnedCredits.contains(course.courseType)) {
                earnedCredits[course.courseType] += course.credit;
            }
        }
    }

    return earnedCredits;
}

void ProgressWidget::refreshData()
{
    // 1. 先检查用户是否填写了个人资料
    UserProfile profile = DataManager::instance().getUserProfile();

    if (profile.major.isEmpty() || profile.grade.isEmpty()) {
        // 用户未填写专业或年级，显示提示信息
        for (const QString &category : categories) {
            if (progressBars.contains(category)) {
                progressBars[category]->setValue(0);
                progressBars[category]->setStyleSheet(
                    "QProgressBar { background-color: #f5f5f5; border: 1px solid #ddd; border-radius: 3px; }"
                );
            }
            if (creditLabels.contains(category)) {
                creditLabels[category]->setText("请先填写个人资料");
            }
            if (gapLabels.contains(category)) {
                gapLabels[category]->setText("");
            }
        }

        static bool hasShownHint = false;
        if (!hasShownHint) {
            QMessageBox::information(this, "提示",
                "检测到您尚未填写个人资料（专业、年级）。\n\n"
                "请先前往【个人资料】标签页填写您的专业和年级信息，\n"
                "系统将自动加载对应的培养方案。");
            hasShownHint = true;
        }
        return;
    }

    // 2. 用户已填写资料，正常加载培养方案
    loadGraduationReqsWithFallback();

    // 3. 重新加载映射
    categoryMapping = loadCategoryMapping();

    // 4. 计算已修学分
    QMap<QString, double> earnedCredits = calculateEarnedCredits();

    // 5. 更新进度显示
    updateProgressDisplay();
}

void ProgressWidget::updateProgressDisplay()
{
    QMap<QString, double> earnedCredits = calculateEarnedCredits();

    for (const QString &category : categories) {
        double required = requiredCredits.value(category, 0.0);
        double earned = earnedCredits.value(category, 0.0);
        double gap = qMax(0.0, required - earned);
        double completionRate = (required > 0) ? qMin(100.0, (earned / required) * 100.0) : 0.0;

        QProgressBar *progressBar = progressBars[category];
        progressBar->setValue(static_cast<int>(completionRate));

        QString styleSheet;
        if (gap > 0) {
            styleSheet = "QProgressBar { background-color: #ffebee; border: 1px solid #e57373; border-radius: 3px; } "
                        "QProgressBar::chunk { background-color: #e57373; }";
            gapLabels[category]->setText(QString("缺口: %1").arg(gap, 0, 'f', 1));
            gapLabels[category]->setStyleSheet("color: #e57373; font-weight: bold;");
        } else {
            styleSheet = "QProgressBar { background-color: #e8f5e9; border: 1px solid #66bb6a; border-radius: 3px; } "
                        "QProgressBar::chunk { background-color: #66bb6a; }";
            gapLabels[category]->setText("✓ 完成");
            gapLabels[category]->setStyleSheet("color: #66bb6a; font-weight: bold;");
        }
        progressBar->setStyleSheet(styleSheet);

        creditLabels[category]->setText(QString("%1 / %2")
            .arg(earned, 0, 'f', 1)
            .arg(required, 0, 'f', 1));

        ProgressSummary summary;
        summary.category = category;
        summary.required = required;
        summary.earned = earned;
        summary.gap = gap;
        summary.completionRate = completionRate;
        progressSummary[category] = summary;
    }
}

void ProgressWidget::generateHTMLReport()
{
    UserProfile profile = DataManager::instance().getUserProfile();

    QString htmlContent = R"(
<!DOCTYPE html>
<html lang="zh-CN">
<head>
    <meta charset="UTF-8">
    <title>培养进度报告</title>
    <style>
        body { font-family: 'Microsoft YaHei', Arial, sans-serif; margin: 40px; background-color: #f5f5f5; }
        .container { max-width: 900px; margin: 0 auto; background: white; padding: 30px; border-radius: 8px; box-shadow: 0 2px 10px rgba(0,0,0,0.1); }
        h1 { text-align: center; color: #333; border-bottom: 3px solid #4CAF50; padding-bottom: 15px; }
        .info { background: #f9f9f9; padding: 15px; border-radius: 5px; margin-bottom: 25px; }
        .info p { margin: 5px 0; }
        table { width: 100%; border-collapse: collapse; margin-top: 20px; }
        th, td { padding: 12px; text-align: left; border-bottom: 1px solid #ddd; }
        th { background-color: #4CAF50; color: white; }
        tr:hover { background-color: #f5f5f5; }
        .progress-bar { width: 100%; height: 25px; background-color: #e0e0e0; border-radius: 3px; overflow: hidden; }
        .progress-fill { height: 100%; background-color: #4CAF50; transition: width 0.3s; }
        .warning { background-color: #ffebee !important; }
        .success { color: #4CAF50; font-weight: bold; }
        .gap { color: #f44336; font-weight: bold; }
        .summary { margin-top: 30px; padding: 20px; background: #e8f5e9; border-radius: 5px; }
        @media print { body { margin: 20px; } .container { box-shadow: none; } }
    </style>
</head>
<body>
    <div class="container">
        <h1>📊 个人培养进度报告</h1>
        <div class="info">
            <p><strong>姓名：</strong>)" + profile.name + R"(</p>
            <p><strong>专业：</strong>)" + profile.major + R"(</p>
            <p><strong>年级：</strong>)" + profile.grade + R"(</p>
            <p><strong>生成时间：</strong>)" + QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") + R"(</p>
        </div>

        <h2>学分完成情况</h2>
        <table>
            <tr>
                <th>课程类别</th>
                <th>应修学分</th>
                <th>已修学分</th>
                <th>缺口</th>
                <th>完成率</th>
                <th>进度条</th>
            </tr>
    )";

    double totalRequired = 0, totalEarned = 0;
    int completedCount = 0;

    for (const QString &category : categories) {
        ProgressSummary summary = progressSummary[category];
        totalRequired += summary.required;
        totalEarned += summary.earned;
        if (summary.gap == 0) completedCount++;

        QString rowClass = (summary.gap > 0) ? "warning" : "";
        QString gapText = (summary.gap > 0) ?
            QString("<span class='gap'>%1</span>").arg(summary.gap, 0, 'f', 1) :
            "<span class='success'>✓</span>";

        htmlContent += QString(R"(
            <tr class="%1">
                <td><strong>%2</strong></td>
                <td>%3</td>
                <td>%4</td>
                <td>%5</td>
                <td>%6%</td>
                <td>
                    <div class="progress-bar">
                        <div class="progress-fill" style="width: %7%; background-color: %8;"></div>
                    </div>
                </td>
            </tr>
        )").arg(rowClass)
         .arg(category)
         .arg(summary.required, 0, 'f', 1)
         .arg(summary.earned, 0, 'f', 1)
         .arg(gapText)
         .arg(QString::number(summary.completionRate, 'f', 1))
         .arg(QString::number(summary.completionRate, 'f', 1))
         .arg(summary.gap > 0 ? "#f44336" : "#4CAF50");
    }

    double overallRate = (totalRequired > 0) ? (totalEarned / totalRequired) * 100.0 : 0.0;
    htmlContent += QString(R"(
            <tr style="background-color: #e3f2fd; font-weight: bold;">
                <td>总计</td>
                <td>%1</td>
                <td>%2</td>
                <td>%3</td>
                <td>%4%</td>
                <td>
                    <div class="progress-bar">
                        <div class="progress-fill" style="width: %5%;"></div>
                    </div>
                </td>
            </tr>
        </table>

        <div class="summary">
            <h3>📈 总体进度</h3>
            <p><strong>已完成类别：</strong>%1 / %2</p>
            <p><strong>总完成率：</strong>%3%</p>
            <p><strong>总学分进度：</strong>%4 / %5</p>
        </div>

        <div style="margin-top: 30px; padding: 15px; background: #fff3cd; border-left: 4px solid #ffc107; border-radius: 3px;">
            <h4>💡 建议</h4>
            <ul>
    )").arg(totalEarned, 0, 'f', 1)
     .arg(totalRequired, 0, 'f', 1)
     .arg(qMax(0.0, totalRequired - totalEarned), 0, 'f', 1)
     .arg(QString::number(overallRate, 'f', 1))
     .arg(completedCount)
     .arg(categories.size())
     .arg(QString::number(overallRate, 'f', 1))
     .arg(totalEarned, 0, 'f', 1)
     .arg(totalRequired, 0, 'f', 1);

    bool hasGap = false;
    for (const QString &category : categories) {
        ProgressSummary summary = progressSummary[category];
        if (summary.gap > 0) {
            hasGap = true;
            htmlContent += QString("<li><strong>%1</strong>：还需修读 %2 学分</li>")
                .arg(category)
                .arg(summary.gap, 0, 'f', 1);
        }
    }

    if (!hasGap) {
        htmlContent += "<li>🎉 恭喜！所有类别学分要求均已满足！</li>";
    }

    htmlContent += R"(
            </ul>
        </div>

        <div style="margin-top: 30px; text-align: center; color: #999; font-size: 12px;">
            <p>提示：使用 Ctrl+P 可打印或保存为PDF</p>
        </div>
    </div>
</body>
</html>
    )";

    // 保存HTML文件 - 使用QStandardPaths替代QDesktopServices
    QString fileName = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) +
                       "/培养进度报告_" +
                       QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss") +
                       ".html";

    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        file.write(htmlContent.toUtf8());
        file.close();

        QMessageBox::information(this, "导出成功",
            QString("进度报告已生成！\n\n文件位置：%1\n\n系统将自动打开报告，您可以使用Ctrl+P打印或保存为PDF。")
            .arg(fileName));

        QDesktopServices::openUrl(QUrl::fromLocalFile(fileName));
    } else {
        QMessageBox::warning(this, "导出失败", "无法保存报告文件，请检查磁盘空间或权限。");
    }
}
