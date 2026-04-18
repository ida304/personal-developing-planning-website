#ifndef VALIDATOR_H
#define VALIDATOR_H

#include <QString>
#include <QDate>

class Validator
{
public:
    static bool isNameNotEmpty(const QString& name);
    static bool isCreditValid(double credit);
    static bool isScoreValid(double score);
    static bool isDateRangeValid(const QDate& start, const QDate& end);
};

#endif // VALIDATOR_H
