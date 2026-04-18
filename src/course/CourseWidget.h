#ifndef COURSEWIDGET_H
#define COURSEWIDGET_H

#include <QWidget>
#include "../core/models.h"

class QTableWidget;
class QLineEdit;
class QComboBox;
class QPushButton;
class QLabel;

class CourseWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CourseWidget(QWidget *parent = nullptr);

private slots:
    void onAddClicked();
    void onEditClicked();
    void onDeleteClicked();
    void onSearchClicked();
    void refreshTable();

private:
    void setupUI();
    void loadCoursesToTable(const QList<Course>& courses);
    void showCourseDialog(Course* course = nullptr);
    void updateStatistics(const QList<Course>& courses);

    QLineEdit* _searchEdit;
    QComboBox* _semesterCombo;
    QComboBox* _typeCombo;
    QComboBox* _tagCombo;
    QPushButton* _searchBtn;
    QPushButton* _resetBtn;
    QTableWidget* _table;
    QLabel* _statLabel;
    QPushButton* _addBtn;
    QPushButton* _editBtn;
    QPushButton* _delBtn;
};

#endif // COURSEWIDGET_H
