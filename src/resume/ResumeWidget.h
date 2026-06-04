#ifndef RESUMEWIDGET_H
#define RESUMEWIDGET_H

#include <QWidget>
<<<<<<< HEAD
#include <QStackedWidget>
#include <QTextEdit>
#include <QTextBrowser>
#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>
#include <QFrame>
#include <QLabel>
#include <QSqlDatabase>
=======
#include <QVBoxLayout>
#include <QTextEdit>
#include <QComboBox>

// 前向声明
class DataManager;
>>>>>>> origin/main

class ResumeWidget : public QWidget
{
    Q_OBJECT
public:
<<<<<<< HEAD
    explicit ResumeWidget(QWidget *parent = nullptr);
    ~ResumeWidget() override = default;

    void setDataManager(void *manager) { Q_UNUSED(manager); }
    void refreshData();

private slots:
    void applyTemplate();
    void showPreview();
    void backToEdit();
    void exportHtml();

private:
    void setupUi();
    QWidget *createEditPage();
    QWidget *createPreviewPage();
    QFrame *createCard(const QString& title, QWidget *contentWidget);
    QString buildResumePlainText() const;
    QString buildResumeHtml() const;
    QString buildBodyHtmlFromEditor() const;
    QString readExperiencesText() const;
    QString safeText(const QString& value, const QString& placeholder) const;

private:
    QStackedWidget *m_stack = nullptr;
    QWidget *m_editPage = nullptr;
    QWidget *m_previewPage = nullptr;

    QComboBox *m_templateCombo = nullptr;
    QLineEdit *m_schoolEdit = nullptr;
    QLineEdit *m_majorEdit = nullptr;
    QLineEdit *m_nameEdit = nullptr;
    QLineEdit *m_genderEdit = nullptr;
    QLineEdit *m_phoneEdit = nullptr;
    QLineEdit *m_emailEdit = nullptr;
    QLineEdit *m_targetEdit = nullptr;
    QTextEdit *m_summaryEdit = nullptr;
    QTextEdit *m_resumeEdit = nullptr;

    QTextBrowser *m_previewBrowser = nullptr;

    QPushButton *m_refreshButton = nullptr;
    QPushButton *m_templateButton = nullptr;
    QPushButton *m_previewButton = nullptr;
    QPushButton *m_backButton = nullptr;
    QPushButton *m_exportButton = nullptr;
=======
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
>>>>>>> origin/main
};

#endif // RESUMEWIDGET_H
