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

    // 初始化数据库（首次运行时创建表）
    bool initDatabase();

    // 课程操作
    bool addCourse(const Course& course);
    bool updateCourse(int id, const Course& course);
    bool deleteCourse(int id);
    QList<Course> getAllCourses() const;
    QList<Course> searchCourses(const QString& keyword, const QString& field) const;
    QList<Course> getCoursesByFilter(const QString& keyword = "",
                                     const QString& semester = "",
                                     const QString& courseType = "",
                                     const QString& tagKeyword = "") const;
    // ... 其他课程接口可后续扩展


    // 用户资料操作
    UserProfile getUserProfile() const;
    bool saveUserProfile(const UserProfile& profile);

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
