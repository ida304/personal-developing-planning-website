#ifndef RESUMEWIDGET_H
#define RESUMEWIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QTextEdit>
#include <QComboBox>

// 前向声明
class DataManager;

class ResumeWidget : public QWidget
{
    Q_OBJECT
public:
    // 单参数构造函数
    explicit ResumeWidget(QWidget *parent = nullptr);
    ~ResumeWidget() override;

    void setDataManager(DataManager *manager);

private slots:
    void onTemplateChanged(int index);

private:
    void initUI();

    QVBoxLayout *m_mainLayout = nullptr;
    QComboBox *m_templateSelector = nullptr;
    QTextEdit *m_editor = nullptr;
    DataManager *m_dataManager = nullptr;
};

#endif // RESUMEWIDGET_H
