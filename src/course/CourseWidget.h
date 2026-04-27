#ifndef COURSEWIDGET_H
#define COURSEWIDGET_H

#include <QWidget>
#include <QLineEdit>
#include <QDoubleSpinBox>
#include <QComboBox>
#include <QPushButton>
#include <QTableWidget>
#include <QLabel>

class CourseWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CourseWidget(QWidget *parent = nullptr);

private slots:
    void onAddClicked();
    void onUpdateClicked();
    void onDeleteClicked(int courseId);
    void onEditClicked(int row);
    void onSearchClicked();

private:
    void refreshTable(const QString& keyword = "", const QString& field = "");
    void updateGPA();
    void clearForm();
    void setEditingMode(bool editing, int id = -1);

    // 表单控件
    QLineEdit *m_nameEdit;
    QDoubleSpinBox *m_creditSpin;
    QDoubleSpinBox *m_scoreSpin;
    QComboBox *m_courseTypeCombo;
    QLineEdit *m_tagsEdit;
    QComboBox *m_semesterCombo;
    QComboBox *m_statusCombo;
    QPushButton *m_addBtn;
    QPushButton *m_updateBtn;

    // 搜索控件
    QLineEdit *m_searchEdit;
    QComboBox *m_searchFieldCombo;
    QPushButton *m_searchBtn;

    // 表格和显示
    QTableWidget *m_tableWidget;
    QLabel *m_gpaLabel;

    int m_currentEditId;
};

#endif // COURSEWIDGET_H
