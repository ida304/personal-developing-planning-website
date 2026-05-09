#include "ResumeWidget.h"
#include "../core/DataManager.h"

#include <QVBoxLayout>
#include <QComboBox>
#include <QTextEdit>
#include <QFileDialog>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>

// 简历生成模块
ResumeWidget::ResumeWidget(DataManager *dataManager, QWidget *parent)
    : QWidget(parent),
      m_dataManager(dataManager)
{
    initUI();

    // 默认加载模板1（索引0）
    onTemplateChanged(0);
}

ResumeWidget::~ResumeWidget()
{

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

    // 文本编辑框（用户可直接修改）
    m_editor = new QTextEdit(this);
    m_editor->setPlaceholderText("请在这里编辑你的简历内容...");
    m_mainLayout->addWidget(m_editor);
}

void ResumeWidget::onTemplateChanged(int index)
{
    // 可以根据不同模板加载不同的预设文本
    switch (index) {
    case 0:
        m_editor->setText("===== 简约风简历模板 =====\n\n姓名：\n电话：\n邮箱：\n\n教育经历：\n\n工作经历：\n\n技能特长：");
        break;
    case 1:
        m_editor->setText("===== 专业风简历模板 =====\n\n【个人信息】\n姓名：\n联系方式：\n邮箱：\n\n【教育背景】\n学校：\n专业：\n时间：\n\n【工作经历】\n公司：\n职位：\n职责：\n\n【专业技能】\n");
        break;
    default:
        m_editor->clear();
        break;
    }
}
