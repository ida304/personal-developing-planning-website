#ifndef PROFILEWIDGET_H
#define PROFILEWIDGET_H

#include <QWidget>

class QLineEdit;
class QComboBox;
class QPushButton;
class QLabel;

class ProfileWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ProfileWidget(QWidget *parent = nullptr);

private slots:
    void onSaveClicked();
    void onSelectPhotoClicked();

private:
    void loadProfile();
    void setupUI();

    QLineEdit* _nameEdit;
    QLineEdit* _schoolEdit;
    QComboBox* _collegeCombo;
    QComboBox* _majorCombo;
    QComboBox* _gradeCombo;
    QComboBox* _educationCombo;
    QLabel* _photoLabel;
    QPushButton* _selectPhotoBtn;
    QPushButton* _saveBtn;
    QString _photoPath;
};

#endif // PROFILEWIDGET_H
