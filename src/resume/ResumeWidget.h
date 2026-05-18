#ifndef RESUMEWIDGET_H
#define RESUMEWIDGET_H

#include <QWidget>

class DataManager;
class QVBoxLayout;
class QComboBox;
class QTextEdit;

class ResumeWidget : public QWidget
{
    Q_OBJECT

public:

    explicit ResumeWidget(DataManager *dataManager, QWidget *parent = nullptr);
    ~ResumeWidget() override;

private:
    // 初始化UI
    void initUI();
    // 模板切换函数
    void onTemplateChanged(int index);

private:
    DataManager *m_dataManager = nullptr;
    QVBoxLayout *m_mainLayout = nullptr;
    QComboBox *m_templateSelector = nullptr;
    QTextEdit *m_editor = nullptr;
};

#endif // RESUMEWIDGET_H
