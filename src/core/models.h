#ifndef MODELS_H
#define MODELS_H

#include <QString>
#include <QDate>

struct Course {
    int id = -1;
    QString courseCode;   // 课序号
    QString name;
    QString semester;     // 如 "2024-2025-1"
    double credit = 0.0;
    double score = 0.0;
    QString courseType;   // 学科基础必修, 学科基础选修, ...
    QString tags;         // 自定义标签，逗号分隔
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

#endif // MODELS_H
