#include "Validator.h"

bool Validator::isNameNotEmpty(const QString& name)
{
    return !name.trimmed().isEmpty();
}

bool Validator::isCreditValid(double credit)
{
    return credit > 0 && credit <= 10.0;
}

bool Validator::isScoreValid(double score)
{
    return score >= 0.0 && score <= 100.0;
}

bool Validator::isDateRangeValid(const QDate& start, const QDate& end)
{
    return start.isValid() && end.isValid() && start <= end;
}
