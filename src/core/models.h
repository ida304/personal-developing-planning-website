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

#endif // MODELS_H
