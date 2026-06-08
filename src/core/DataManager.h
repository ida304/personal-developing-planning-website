#ifndef DATAMANAGER_H
#define DATAMANAGER_H

#include <QObject>
#include <QSqlDatabase>
#include <QList>
#include "models.h"

class DataManager : public QObject
{
    Q_OBJECT
public:
    static DataManager& instance();

    bool initDatabase();

    bool addCourse(const Course& course);
    bool updateCourse(int id, const Course& course);
    bool deleteCourse(int id);
    QList<Course> getAllCourses() const;
    QList<Course> searchCourses(const QString& keyword, const QString& field) const;
    QList<Course> getCoursesByFilter(const QString& keyword = "",
                                     const QString& semester = "",
                                     const QString& courseType = "",
                                     const QString& tagKeyword = "") const;

    UserProfile getUserProfile() const;
    bool saveUserProfile(const UserProfile& profile);
    // 通知数据变化（供外部调用以触发刷新）
    void notifyDataChanged() { emit dataChanged(); }
    // 成就管理
    bool addAchievement(const Achievement& ach);
    bool updateAchievement(int id, const Achievement& ach);
    bool deleteAchievement(int id);
    QList<Achievement> getAllAchievements() const;
    // DataManager.h 中
    QList<Requirement> getRequirements() const;
    // 在 public 区域，成就管理函数后面添加
    double calculateGPA() const;
    double getTotalEarnedCredits() const;
    double getTotalRequiredCredits() const;

signals:
    void dataChanged();

private:
    explicit DataManager(QObject *parent = nullptr);
    ~DataManager();
    DataManager(const DataManager&) = delete;
    DataManager& operator=(const DataManager&) = delete;

    QSqlDatabase m_db;
    bool openDatabase();
};

#endif // DATAMANAGER_H
