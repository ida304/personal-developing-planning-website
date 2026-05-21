#include "ResumeWidget.h"
#include "../core/DataManager.h"
#include <QVBoxLayout>
#include <QComboBox>
#include <QTextEdit>
#include <QFileDialog>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>

ResumeWidget::ResumeWidget(QWidget *parent)
    : QWidget(parent)
{
    initUI();
    // 默认加载模板1（索引0）
    onTemplateChanged(0);
}

ResumeWidget::~ResumeWidget()
{
}

// 外部设置DataManager
void ResumeWidget::setDataManager(DataManager *manager)
{
    m_dataManager = manager;
}

void ResumeWidget::initUI()
{
    // 函数主布局
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(10, 10, 10, 10);

    // 模板选择器
    m_templateSelector = new QComboBox(this);
    m_templateSelector->addItems({"模板1：简约风", "模板2：专业风"});

    // 连接信号槽，处理模板切换
    connect(m_templateSelector, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &ResumeWidget::onTemplateChanged);

    m_mainLayout->addWidget(m_templateSelector);

    // 文本编辑框
    //【用户可直接修改】
    m_editor = new QTextEdit(this);
    m_editor->setPlaceholderText("请在这里编辑你的简历内容...");
    m_mainLayout->addWidget(m_editor);
}

void ResumeWidget::onTemplateChanged(int index)
{
    // 可以根据不同模板加载不同的预设文本
    switch (index) {
    case 0:
        m_editor->setText("===== 简约风简历模板 =====\n\n"
                          "姓名: \n"
                          "电话: \n"
                          "邮箱: \n\n"
                          "教育经历: \n\n"
                          "工作经历: \n\n"
                          "技能特长: ");
        break;
    case 1:
        m_editor->setText("===== 专业风简历模板 =====\n\n"
                          "【个人信息】\n"
                          "姓名: \n"
                          "联系方式: \n"
                          "邮箱: \n\n"
                          "【教育背景】\n"
                          "学校: \n"
                          "专业: \n"
                          "学位: \n\n"
                          "【工作/项目经历】\n"
                          "时间: \n"
                          "公司/项目: \n"
                          "职责与成果: \n\n"
                          "【专业技能】\n"
                          "编程语言: \n"
                          "工具/软件: \n"
                          "软技能: ");
        break;
    default:
        m_editor->clear();
        break;
    }
}
