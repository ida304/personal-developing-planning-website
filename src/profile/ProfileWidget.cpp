#include "ProfileWidget.h"
#include "../core/DataManager.h"
#include "../core/Validator.h"
#include <QVBoxLayout>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QFileDialog>
#include <QMessageBox>
#include <QDir>
#include <QFile>
#include <QUuid>
#include <QPixmap>

ProfileWidget::ProfileWidget(QWidget *parent) : QWidget(parent)
{
    setupUI();
    loadProfile();
}

void ProfileWidget::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(15, 15, 15, 15);
    mainLayout->setSpacing(10);

    QFormLayout *formLayout = new QFormLayout;
    formLayout->setLabelAlignment(Qt::AlignRight);

    _nameEdit = new QLineEdit;
    formLayout->addRow("姓名 *:", _nameEdit);

    _schoolEdit = new QLineEdit("对外经济贸易大学");
    formLayout->addRow("学校:", _schoolEdit);

    _collegeCombo = new QComboBox;
    _collegeCombo->setEditable(true);
    _collegeCombo->addItems({"信息学院", "国际经济贸易学院", "金融学院", "法学院", "英语学院"});
    formLayout->addRow("学院:", _collegeCombo);

    _majorCombo = new QComboBox;
    _majorCombo->setEditable(true);
    _majorCombo->addItems({"计算机科学与技术", "软件工程", "数据科学与大数据技术", "国际经济与贸易", "金融学", "法学"});
    formLayout->addRow("专业:", _majorCombo);

    _gradeCombo = new QComboBox;
    _gradeCombo->setEditable(true);
    _gradeCombo->addItems({"2025级", "2024级", "2023级", "2022级", "2021级"});
    formLayout->addRow("年级:", _gradeCombo);

    _educationCombo = new QComboBox;
    _educationCombo->addItems({"本科", "硕士", "博士"});
    formLayout->addRow("学历:", _educationCombo);

    QHBoxLayout *photoLayout = new QHBoxLayout;
    _photoLabel = new QLabel("未选择照片");
    _photoLabel->setFixedSize(80, 80);
    _photoLabel->setStyleSheet("border: 1px solid gray; background-color: #f0f0f0;");
    _photoLabel->setAlignment(Qt::AlignCenter);
    _selectPhotoBtn = new QPushButton("选择照片");
    photoLayout->addWidget(_photoLabel);
    photoLayout->addWidget(_selectPhotoBtn);
    photoLayout->addStretch();
    formLayout->addRow("个人照片:", photoLayout);

    _saveBtn = new QPushButton("保存");
    _saveBtn->setFixedWidth(100);
    QHBoxLayout *btnLayout = new QHBoxLayout;
    btnLayout->addStretch();
    btnLayout->addWidget(_saveBtn);

    mainLayout->addLayout(formLayout);
    mainLayout->addLayout(btnLayout);
    mainLayout->addStretch();

    connect(_selectPhotoBtn, &QPushButton::clicked, this, &ProfileWidget::onSelectPhotoClicked);
    connect(_saveBtn, &QPushButton::clicked, this, &ProfileWidget::onSaveClicked);
}

void ProfileWidget::loadProfile()
{
    UserProfile profile = DataManager::instance().getUserProfile();
    _nameEdit->setText(profile.name);
    _schoolEdit->setText(profile.school.isEmpty() ? "对外经济贸易大学" : profile.school);
    _collegeCombo->setCurrentText(profile.college);
    _majorCombo->setCurrentText(profile.major);
    _gradeCombo->setCurrentText(profile.grade);
    _educationCombo->setCurrentText(profile.education);
    _photoPath = profile.photoPath;
    if (!_photoPath.isEmpty()) {
        QString absPath = QCoreApplication::applicationDirPath() + "/" + _photoPath;
        if (QFile::exists(absPath)) {
            QPixmap pix(absPath);
            if (!pix.isNull())
                _photoLabel->setPixmap(pix.scaled(80, 80, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        }
    }
}

void ProfileWidget::onSelectPhotoClicked()
{
    QString filePath = QFileDialog::getOpenFileName(this, "选择照片", "",
        "图片文件 (*.png *.jpg *.jpeg);;所有文件 (*.*)");
    if (filePath.isEmpty()) return;

    QDir dir(QCoreApplication::applicationDirPath() + "/photos");
    if (!dir.exists()) dir.mkpath(".");

    QString suffix = QFileInfo(filePath).suffix();
    QString uniqueName = QUuid::createUuid().toString(QUuid::WithoutBraces) + "." + suffix;
    QString destPath = dir.absoluteFilePath(uniqueName);

    if (QFile::copy(filePath, destPath)) {
        _photoPath = "photos/" + uniqueName;
        QPixmap pix(destPath);
        _photoLabel->setPixmap(pix.scaled(80, 80, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    } else {
        QMessageBox::warning(this, "错误", "照片复制失败");
    }
}

void ProfileWidget::onSaveClicked()
{
    if (!Validator::isNameNotEmpty(_nameEdit->text())) {
        QMessageBox::warning(this, "提示", "姓名不能为空");
        return;
    }
    UserProfile profile;
    profile.name = _nameEdit->text().trimmed();
    profile.school = _schoolEdit->text().trimmed();
    profile.college = _collegeCombo->currentText().trimmed();
    profile.major = _majorCombo->currentText().trimmed();
    profile.grade = _gradeCombo->currentText().trimmed();
    profile.education = _educationCombo->currentText();
    profile.photoPath = _photoPath;

    if (DataManager::instance().saveUserProfile(profile)) {
        QMessageBox::information(this, "成功", "个人信息已保存");
    } else {
        QMessageBox::warning(this, "错误", "保存失败");
    }
}
