#include "ResumeWidget.h"
#include "core/DataManager.h"
#include "core/models.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QFileDialog>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>
#include <QScrollArea>
#include <QSqlQuery>
#include <QVariant>
#include <QDate>
#include <QtGlobal>

ResumeWidget::ResumeWidget(QWidget *parent)
    : QWidget(parent)
{
    setupUi();
    refreshData();
    connect(&DataManager::instance(), &DataManager::dataChanged,
            this, &ResumeWidget::refreshData);
}

void ResumeWidget::setupUi()
{
    m_stack = new QStackedWidget(this);
    m_editPage = createEditPage();
    m_previewPage = createPreviewPage();

    m_stack->addWidget(m_editPage);
    m_stack->addWidget(m_previewPage);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->addWidget(m_stack);

    setStyleSheet(QString::fromUtf8(R"(
        QWidget {
            background-color: #f6f3ff;
            color: #1f2937;
            font-family: "Microsoft YaHei", "PingFang SC", Arial;
            font-size: 14px;
        }
        QLabel { background: transparent; }
        QLabel#pageTitle {
            font-size: 28px;
            font-weight: 800;
            color: #2e1065;
            background: transparent;
        }
        QLabel#pageSubTitle {
            color: #6b5f85;
            font-size: 14px;
            background: transparent;
        }
        QLabel#cardTitle {
            font-size: 18px;
            font-weight: 800;
            color: #6d28d9;
            background: #f3e8ff;
            border-radius: 10px;
            padding: 9px 13px;
        }
        QFrame#card {
            background-color: white;
            border: 1px solid #e9d5ff;
            border-radius: 20px;
        }
        QLineEdit, QTextEdit, QComboBox {
            background-color: #fbfaff;
            border: 1px solid #ddd6fe;
            border-radius: 11px;
            padding: 9px 11px;
            selection-background-color: #c4b5fd;
        }
        QLineEdit:focus, QTextEdit:focus, QComboBox:focus {
            background-color: white;
            border: 1px solid #7c3aed;
        }
        QTextEdit#summaryEditor {
            min-height: 78px;
            line-height: 1.6;
        }
        QTextEdit#resumeEditor {
            font-size: 15px;
            line-height: 1.6;
        }
        QTextBrowser#previewBrowser {
            background-color: #f6f3ff;
            border: 1px solid #e9d5ff;
            border-radius: 18px;
            padding: 6px;
        }
        QPushButton {
            background-color: #7c3aed;
            color: white;
            border: none;
            border-radius: 13px;
            padding: 11px 20px;
            font-weight: 800;
            min-height: 24px;
        }
        QPushButton:hover { background-color: #6d28d9; }
        QPushButton:pressed { background-color: #5b21b6; }
        QPushButton#secondaryButton {
            background-color: #ede9fe;
            color: #6d28d9;
        }
        QPushButton#secondaryButton:hover {
            background-color: #ddd6fe;
        }
        QScrollArea {
            border: none;
            background: transparent;
        }
        QScrollBar:vertical {
            background: #f3e8ff;
            width: 10px;
            margin: 0px;
            border-radius: 5px;
        }
        QScrollBar::handle:vertical {
            background: #c4b5fd;
            border-radius: 5px;
            min-height: 30px;
        }
    )"));

    connect(m_refreshButton, &QPushButton::clicked, this, &ResumeWidget::refreshData);
    connect(m_templateButton, &QPushButton::clicked, this, &ResumeWidget::applyTemplate);
    connect(m_previewButton, &QPushButton::clicked, this, &ResumeWidget::showPreview);
    connect(m_backButton, &QPushButton::clicked, this, &ResumeWidget::backToEdit);
    connect(m_exportButton, &QPushButton::clicked, this, &ResumeWidget::exportHtml);
}

QWidget *ResumeWidget::createEditPage()
{
    QWidget *page = new QWidget(this);
    QVBoxLayout *pageLayout = new QVBoxLayout(page);
    pageLayout->setContentsMargins(34, 28, 34, 28);
    pageLayout->setSpacing(18);

    QHBoxLayout *headerLayout = new QHBoxLayout;
    QVBoxLayout *titleLayout = new QVBoxLayout;
    QLabel *title = new QLabel("简历生成 Resume Builder", page);
    title->setObjectName("pageTitle");
    QLabel *subTitle = new QLabel("先完善基本信息，再编辑正文内容；确认后可预览并生成 HTML 简历", page);
    subTitle->setObjectName("pageSubTitle");
    titleLayout->addWidget(title);
    titleLayout->addWidget(subTitle);
    headerLayout->addLayout(titleLayout);
    headerLayout->addStretch();

    m_previewButton = new QPushButton("生成预览", page);
    m_previewButton->setFixedWidth(150);
    headerLayout->addWidget(m_previewButton, 0, Qt::AlignTop);
    pageLayout->addLayout(headerLayout);

    QWidget *content = new QWidget(page);
    QVBoxLayout *contentLayout = new QVBoxLayout(content);
    contentLayout->setContentsMargins(0, 0, 0, 0);
    contentLayout->setSpacing(18);

    m_templateCombo = new QComboBox(content);
    m_templateCombo->addItem("简洁模板", "simple");
    m_templateCombo->addItem("详细模板", "detailed");

    m_schoolEdit = new QLineEdit(content);
    m_schoolEdit->setPlaceholderText("例如：对外经济贸易大学");
    m_majorEdit = new QLineEdit(content);
    m_majorEdit->setPlaceholderText("例如：计算机科学与技术");
    m_nameEdit = new QLineEdit(content);
    m_genderEdit = new QLineEdit(content);
    m_genderEdit->setPlaceholderText("例如：女 / 男");
    m_phoneEdit = new QLineEdit(content);
    m_emailEdit = new QLineEdit(content);
    m_targetEdit = new QLineEdit(content);
    m_targetEdit->setPlaceholderText("例如：软件开发 / 数据分析 / 产品运营");

    m_summaryEdit = new QTextEdit(content);
    m_summaryEdit->setObjectName("summaryEditor");
    m_summaryEdit->setPlaceholderText("写几句个人优势、职业目标或自我评价。生成预览后会放在“个人简介”板块中。 ");

    QWidget *infoContent = new QWidget(content);
    QGridLayout *infoLayout = new QGridLayout(infoContent);
    infoLayout->setContentsMargins(0, 0, 0, 0);
    infoLayout->setHorizontalSpacing(18);
    infoLayout->setVerticalSpacing(14);

    infoLayout->addWidget(new QLabel("模板"), 0, 0);
    infoLayout->addWidget(m_templateCombo, 0, 1);
    infoLayout->addWidget(new QLabel("学校"), 0, 2);
    infoLayout->addWidget(m_schoolEdit, 0, 3);

    infoLayout->addWidget(new QLabel("专业"), 1, 0);
    infoLayout->addWidget(m_majorEdit, 1, 1);
    infoLayout->addWidget(new QLabel("姓名"), 1, 2);
    infoLayout->addWidget(m_nameEdit, 1, 3);

    infoLayout->addWidget(new QLabel("性别"), 2, 0);
    infoLayout->addWidget(m_genderEdit, 2, 1);
    infoLayout->addWidget(new QLabel("电话"), 2, 2);
    infoLayout->addWidget(m_phoneEdit, 2, 3);

    infoLayout->addWidget(new QLabel("邮箱"), 3, 0);
    infoLayout->addWidget(m_emailEdit, 3, 1);
    infoLayout->addWidget(new QLabel("求职方向"), 3, 2);
    infoLayout->addWidget(m_targetEdit, 3, 3);

    infoLayout->addWidget(new QLabel("个人简介"), 4, 0, Qt::AlignTop);
    infoLayout->addWidget(m_summaryEdit, 4, 1, 1, 3);
    infoLayout->setColumnStretch(1, 1);
    infoLayout->setColumnStretch(3, 1);

    contentLayout->addWidget(createCard("基本信息", infoContent));

    m_resumeEdit = new QTextEdit(content);
    m_resumeEdit->setObjectName("resumeEditor");
    m_resumeEdit->setMinimumHeight(360);
    m_resumeEdit->setPlaceholderText("这里会自动生成教育背景、课程信息、实践经历、成果荣誉等内容。你可以在这里直接修改。 ");
    contentLayout->addWidget(createCard("正文编辑：教育背景 / 课程信息 / 实践经历 / 成果荣誉", m_resumeEdit));

    QHBoxLayout *buttonLayout = new QHBoxLayout;
    m_refreshButton = new QPushButton("刷新前面数据", content);
    m_templateButton = new QPushButton("套用模板", content);
    m_refreshButton->setObjectName("secondaryButton");
    m_templateButton->setObjectName("secondaryButton");
    buttonLayout->addWidget(m_refreshButton);
    buttonLayout->addWidget(m_templateButton);
    buttonLayout->addStretch();
    buttonLayout->addWidget(m_previewButton);
    contentLayout->addLayout(buttonLayout);

    QScrollArea *scrollArea = new QScrollArea(page);
    scrollArea->setWidgetResizable(true);
    scrollArea->setWidget(content);
    pageLayout->addWidget(scrollArea, 1);

    return page;
}

QWidget *ResumeWidget::createPreviewPage()
{
    QWidget *page = new QWidget(this);
    QVBoxLayout *pageLayout = new QVBoxLayout(page);
    pageLayout->setContentsMargins(34, 28, 34, 28);
    pageLayout->setSpacing(18);

    QHBoxLayout *headerLayout = new QHBoxLayout;
    QVBoxLayout *titleLayout = new QVBoxLayout;
    QLabel *title = new QLabel("简历预览", page);
    title->setObjectName("pageTitle");
    QLabel *subTitle = new QLabel("基本信息采用左右两栏排版，教育背景和课程信息在正文中分区展示；需要修改可返回编辑", page);
    subTitle->setObjectName("pageSubTitle");
    titleLayout->addWidget(title);
    titleLayout->addWidget(subTitle);
    headerLayout->addLayout(titleLayout);
    headerLayout->addStretch();

    m_backButton = new QPushButton("返回修改", page);
    m_backButton->setObjectName("secondaryButton");
    m_exportButton = new QPushButton("生成简历", page);
    headerLayout->addWidget(m_backButton);
    headerLayout->addWidget(m_exportButton);
    pageLayout->addLayout(headerLayout);

    m_previewBrowser = new QTextBrowser(page);
    m_previewBrowser->setObjectName("previewBrowser");
    pageLayout->addWidget(m_previewBrowser, 1);

    return page;
}

QFrame *ResumeWidget::createCard(const QString& title, QWidget *contentWidget)
{
    QFrame *card = new QFrame(this);
    card->setObjectName("card");
    QVBoxLayout *layout = new QVBoxLayout(card);
    layout->setContentsMargins(22, 18, 22, 22);
    layout->setSpacing(14);

    QLabel *titleLabel = new QLabel(title, card);
    titleLabel->setObjectName("cardTitle");
    layout->addWidget(titleLabel);
    layout->addWidget(contentWidget);
    return card;
}

void ResumeWidget::refreshData()
{
    DataManager& dm = DataManager::instance();
    UserProfile profile = dm.getUserProfile();

    // 同步个人资料到基本信息编辑框（实现联动）
    if (!profile.name.isEmpty()) m_nameEdit->setText(profile.name);
    if (!profile.school.isEmpty()) m_schoolEdit->setText(profile.school);
    if (!profile.major.isEmpty()) m_majorEdit->setText(profile.major);

    // 刷新正文内容（课程、成就等）
    m_resumeEdit->setPlainText(buildResumePlainText());
}

QString ResumeWidget::buildResumePlainText() const
{
    DataManager& dm = DataManager::instance();
    UserProfile profile = dm.getUserProfile();
    QList<Course> courses = dm.getAllCourses();
    QList<Achievement> achievements = dm.getAllAchievements();

    QString text;
    text += "【教育背景】\n";
    text += QString("学校：%1\n").arg(m_schoolEdit->text().trimmed().isEmpty() ? "待填写" : m_schoolEdit->text().trimmed());
    text += QString("专业：%1\n").arg(m_majorEdit->text().trimmed().isEmpty() ? "待填写" : m_majorEdit->text().trimmed());
    if (!profile.education.isEmpty()) text += QString("学历：%1\n").arg(profile.education);
    if (!profile.grade.isEmpty()) text += QString("年级：%1\n").arg(profile.grade);

    text += "\n【课程信息】\n";
    if (courses.isEmpty()) {
        text += "- 暂无课程数据，可在这里手动补充。\n";
    } else {
        for (const Course& c : courses) {
            if (c.status != "已修") continue;
            text += QString("- %1（%2学分，成绩%3）\n")
                    .arg(c.name.isEmpty() ? "课程名称待填写" : c.name)
                    .arg(c.credit)
                    .arg(c.score);
        }
    }

    text += "\n【实践经历】\n";
    text += readExperiencesText();

    text += "\n【成果与荣誉】\n";
    if (achievements.isEmpty()) {
        text += "- 暂无成果荣誉数据，可在这里手动补充。\n";
    } else {
        for (const Achievement& a : achievements) {
            QString line = QString("- %1").arg(a.name.isEmpty() ? "成果名称待填写" : a.name);
            if (!a.level.isEmpty()) line += QString("（%1）").arg(a.level);
            if (a.obtainDate.isValid()) line += QString("  %1").arg(a.obtainDate.toString("yyyy-MM-dd"));
            text += line + "\n";
        }
    }

    text += "\n【技能特长】\n";
    text += "- 可在这里补充编程语言、工具软件、语言能力、证书等。\n";

    return text;
}

QString ResumeWidget::readExperiencesText() const
{
    QSqlDatabase db = QSqlDatabase::database();
    if (!db.isOpen()) {
        return "- 暂无实践经历数据，可在这里手动补充。\n";
    }

    QStringList sqlList;
    sqlList << "SELECT type, name, startDate, endDate, role, description FROM experience ORDER BY startDate DESC";
    sqlList << "SELECT type, name, start_date, end_date, role, description FROM experiences ORDER BY start_date DESC";

    for (const QString& sql : sqlList) {
        QSqlQuery query(db);
        if (!query.exec(sql)) continue;

        QString text;
        while (query.next()) {
            QString type = query.value(0).toString();
            QString name = query.value(1).toString();
            QString start = query.value(2).toString();
            QString end = query.value(3).toString();
            QString role = query.value(4).toString();
            QString desc = query.value(5).toString();

            if (name.isEmpty()) continue;
            text += QString("- %1%2").arg(name).arg(type.isEmpty() ? "" : QString("（%1）").arg(type));
            if (!start.isEmpty() || !end.isEmpty()) text += QString("  %1-%2").arg(start).arg(end);
            text += "\n";
            if (!role.isEmpty()) text += QString("  角色：%1\n").arg(role);
            if (!desc.isEmpty()) text += QString("  %1\n").arg(desc);
        }

        if (!text.isEmpty()) return text;
    }

    return "- 暂无实践经历数据，可在这里手动补充。\n";
}

void ResumeWidget::applyTemplate()
{
    QString text = buildResumePlainText();

    if (m_templateCombo->currentData().toString() == "detailed") {
        text += "\n【项目亮点】\n";
        text += "- 可补充项目背景、个人职责、使用技术和最终成果。\n";
        text += "\n【自我评价】\n";
        text += "- 可补充学习能力、沟通协作能力、责任心等。\n";
    }

    m_resumeEdit->setPlainText(text);
}

QString ResumeWidget::safeText(const QString& value, const QString& placeholder) const
{
    QString trimmed = value.trimmed();
    return trimmed.isEmpty() ? placeholder.toHtmlEscaped() : trimmed.toHtmlEscaped();
}

QString ResumeWidget::buildResumeHtml() const
{
    // 基本信息
    QString school = safeText(m_schoolEdit->text(), "学校待填写");
    QString major = safeText(m_majorEdit->text(), "专业待填写");
    QString name = safeText(m_nameEdit->text(), "姓名待填写");
    QString gender = safeText(m_genderEdit->text(), "性别待填写");
    QString phone = safeText(m_phoneEdit->text(), "电话待填写");
    QString email = safeText(m_emailEdit->text(), "邮箱待填写");
    QString target = safeText(m_targetEdit->text(), "求职方向待填写");
    QString summary = m_summaryEdit->toPlainText().trimmed().isEmpty()
            ? "可在左侧填写个人简介。"
            : m_summaryEdit->toPlainText().trimmed().toHtmlEscaped().replace("\n", "<br>");

    // 从纯文本构建正文
    QString bodyHtml;
    QStringList lines = m_resumeEdit->toPlainText().split('\n');
    bool inSection = false;
    for (const QString& line : lines) {
        QString trimmed = line.trimmed();
        if (trimmed.isEmpty()) continue;
        if (trimmed.startsWith("【") && trimmed.endsWith("】")) {
            if (inSection) bodyHtml += "</div>";
            QString title = trimmed.mid(1, trimmed.length() - 2).toHtmlEscaped();
            bodyHtml += QString("<div class='section-title'>%1</div><div class='section-content'>").arg(title);
            inSection = true;
        } else if (trimmed.startsWith("-") || trimmed.startsWith("•")) {
            QString item = trimmed.mid(1).trimmed().toHtmlEscaped();
            bodyHtml += QString("<div class='item-block'>%1</div>").arg(item);
        } else {
            bodyHtml += QString("<div class='item-block'>%1</div>").arg(trimmed.toHtmlEscaped());
        }
    }
    if (inSection) bodyHtml += "</div>";

    QString html;
    html += "<!DOCTYPE html><html><head><meta charset='UTF-8'><style>";
    html += "* { margin:0; padding:0; box-sizing:border-box; }";
    html += "body { background:#f7f2ff; font-family:'Microsoft YaHei','PingFang SC',Arial; padding:20px; }";
    // 白色大框无任何内边距
    html += ".paper { max-width:1100px; margin:0 auto; background:#fff; border-radius:16px; box-shadow:0 5px 20px rgba(0,0,0,0.1); overflow:auto; }";
    html += ".resume-title { text-align:center; font-size:28px; font-weight:900; color:#2e1065; border-bottom:3px solid #d8b4fe; padding:20px 30px; }";

    // 普通 section 样式（用于个人简介和正文）
    html += ".section-title { background:#f1e4ff; color:#7e22ce; font-weight:900; font-size:20px; padding:12px 20px; margin:20px 0 0 0; border-left:6px solid #9333ea; }";
    html += ".section-content { padding:0 20px 20px 20px; }";

    // ★ 基本信息专用样式：标题无左右内边距，表格直接贴边 ★
    html += ".info-title { background:#f1e4ff; color:#7e22ce; font-weight:900; font-size:20px; padding:12px 0; margin:20px 0 0 0; border-left:6px solid #9333ea; }";
    // 表格强制 100% 宽度，固定布局，无任何边距
    html += ".info-table { width:100%; table-layout:fixed; border-collapse:collapse; margin:0; padding:0; }";
    html += ".info-table td { border:1px solid #e9d5ff; padding:12px; word-break:break-word; white-space:normal; }";
    html += ".info-table td:nth-child(1) { width:20%; background:#f3e8ff; color:#4c1d95; font-weight:900; text-align:center; }";
    html += ".info-table td:nth-child(2) { width:30%; background:#ffffff; }";
    html += ".info-table td:nth-child(3) { width:20%; background:#f3e8ff; color:#4c1d95; font-weight:900; text-align:center; }";
    html += ".info-table td:nth-child(4) { width:30%; background:#ffffff; }";

    html += ".item-block { background:#f3e8ff; border-radius:12px; padding:12px 16px; margin:12px 0; }";
    html += ".summary { background:#fff; border:1px solid #eadcff; padding:16px; margin:0; }";
    html += "@media print { body { background:white; } .item-block { background:white; border:1px solid #e9d5ff; } }";
    html += "</style></head><body><div class='paper'>";

    html += "<h1 class='resume-title'>个人简历</h1>";
        html += "<div class='section-title'>基本信息</div>";
        html += "<table class='info-table' width='100%' cellspacing='0' cellpadding='0'>";
        html += "<tr>"
                "<td class='info-label' width='12%'>学校</td><td class='info-value' width='38%'>" + school + "</td>"
                "<td class='info-label' width='12%'>专业</td><td class='info-value' width='38%'>" + major + "</td></tr>";
        html += "<tr>"
                "<td class='info-label' width='12%'>姓名</td><td class='info-value' width='38%'>" + name + "</td>"
                "<td class='info-label' width='12%'>性别</td><td class='info-value' width='38%'>" + gender + "</td></tr>";
        html += "<tr>"
                "<td class='info-label' width='12%'>电话</td><td class='info-value' width='38%'>" + phone + "</td>"
                "<td class='info-label' width='12%'>邮箱</td><td class='info-value' width='38%'>" + email + "</td></tr>";
        html += "<tr><td class='info-label' width='12%'>求职方向</td><td class='info-value' colspan='3'>" + target + "</td></tr>";
        html += "</table>";

    // 个人简介（保持原有缩进）
    html += "<div class='section-title'>个人简介</div>";
    html += "<div class='section-content'><div class='summary'>" + summary + "</div></div>";

    // 正文
    html += bodyHtml;

    html += "</div></body></html>";
    return html;
}

void ResumeWidget::showPreview()
{
    m_previewBrowser->setHtml(buildResumeHtml());
    m_stack->setCurrentWidget(m_previewPage);
}

void ResumeWidget::backToEdit()
{
    m_stack->setCurrentWidget(m_editPage);
}

void ResumeWidget::exportHtml()
{
    QString fileName = QFileDialog::getSaveFileName(this, "生成简历", "我的简历.html", "HTML 文件 (*.html)");
    if (fileName.isEmpty()) return;

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "生成失败", "无法创建文件，请检查保存路径。 ");
        return;
    }

    QTextStream out(&file);
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    out.setEncoding(QStringConverter::Utf8);
#else
    out.setCodec("UTF-8");
#endif
    out << buildResumeHtml();
    file.close();

    QMessageBox::information(this, "生成成功", "简历 HTML 文件已生成，可用浏览器打开后打印为 PDF。 ");
}
