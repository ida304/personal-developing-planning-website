#ifndef MODELS_H
#define MODELS_H

#include <QString>
#include <QDate>
#include <QMetaType>

struct Course {
    int id = -1;
    QString courseCode;
    QString name;
    QString semester;
    double credit = 0.0;
    double score = 0.0;
    QString courseType;
    QString tags;
    QString status;
};

struct UserProfile {
    QString name;
    QString school;
    QString college;
    QString major;
    QString grade;
    QString education;
    QString photoPath;
};

struct Achievement {
    int id = -1;
    QString name;
    QString level;          // 国家级/省级/校级等
    QDate obtainDate;       // 获得时间
    QStringList tags;       // 标签列表
    QString materialPath;   // 材料路径
};

// 毕业要求项
struct Requirement {
    QString category;      // 如 "学科基础必修"
    double requiredCredits; // 要求总学分
    double earnedCredits;   // 已修学分
};

#endif // MODELS_H
