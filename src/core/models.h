#ifndef MODELS_H
#define MODELS_H

#include <QString>
#include <QDate>
#include <QMetaType>

struct Course {
    int id = -1;
    QString courseCode;   // 课序号
    QString name;
    QString semester;     // 如 "2024-2025-1"
    double credit = 0.0;
    double score = 0.0;
    QString courseType;   // 学科基础必修, 学科基础选修, ...
    QString tags;         // 自定义标签，逗号分隔
    QString status;
};

struct UserProfile {
    QString name;
    QString school;
    QString college;
    QString major;
    QString grade;        // 如 "2025级"
    QString education;    // 本科/硕士/博士
    QString photoPath;    // 相对路径
};

// 注册自定义类型到 Qt 元对象系统（可选，用于信号槽）
Q_DECLARE_METATYPE(Course)
Q_DECLARE_METATYPE(UserProfile)

#endif // MODELS_H
