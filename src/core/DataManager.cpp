#include "DataManager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDir>
#include <QCoreApplication>
#include <QDebug>

DataManager::DataManager(QObject *parent) : QObject(parent)
{
    openDatabase();
}

DataManager::~DataManager()
{
    if (m_db.isOpen()) m_db.close();
}

DataManager& DataManager::instance()
{
    static DataManager instance;
    return instance;
}

bool DataManager::openDatabase()
{
    QString dataDir = QCoreApplication::applicationDirPath() + "/data";
    QDir dir;
    if (!dir.exists(dataDir)) dir.mkpath(dataDir);
    QString dbPath = dataDir + "/app.db";
    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName(dbPath);
    if (!m_db.open()) {
        qWarning() << "Failed to open database:" << m_db.lastError().text();
        return false;
    }
    return initDatabase();
}

bool DataManager::initDatabase()
{
    QSqlQuery query;

    // 课程表
    QString createCourses = R"(
        CREATE TABLE IF NOT EXISTS courses (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            course_code TEXT,
            name TEXT NOT NULL,
            semester TEXT NOT NULL,
            credit REAL NOT NULL,
            score REAL NOT NULL,
            course_type TEXT NOT NULL,
            tags TEXT,
            status TEXT DEFAULT '已修'
        )
    )";
    if (!query.exec(createCourses)) {
        qWarning() << "Failed to create courses:" << query.lastError().text();
        return false;
    }

    // 用户资料表
    QString createProfile = R"(
        CREATE TABLE IF NOT EXISTS user_profile (
            id INTEGER PRIMARY KEY CHECK (id = 1),
            name TEXT,
            school TEXT,
            college TEXT,
            major TEXT,
            grade TEXT,
            education TEXT,
            photo_path TEXT
        )
    )";
    if (!query.exec(createProfile)) {
        qWarning() << "Failed to create user_profile:" << query.lastError().text();
        return false;
    }
    // 插入默认行
    query.exec("SELECT COUNT(*) FROM user_profile");
    if (query.next() && query.value(0).toInt() == 0) {
        query.exec("INSERT INTO user_profile (id) VALUES (1)");
    }

    return true;
}

// ========== 课程操作 ==========
bool DataManager::updateCourse(int id, const Course& course)
{
    QSqlQuery query;
    query.prepare(R"(
        UPDATE courses SET
            course_code = ?,
            name = ?,
            semester = ?,
            credit = ?,
            score = ?,
            course_type = ?,
            tags = ?,
            status = ?
        WHERE id = ?
    )");
    query.addBindValue(course.courseCode);
    query.addBindValue(course.name);
    query.addBindValue(course.semester);
    query.addBindValue(course.credit);
    query.addBindValue(course.score);
    query.addBindValue(course.courseType);
    query.addBindValue(course.tags);
    query.addBindValue(course.status);
    query.addBindValue(id);

    if (!query.exec()) {
        qWarning() << "updateCourse failed:" << query.lastError().text();
        return false;
    }
    emit dataChanged();
    return true;
}

bool DataManager::addCourse(const Course& course)
{
    QSqlQuery query;
    query.prepare(R"(
        INSERT INTO courses (course_code, name, semester, credit, score, course_type, tags, status)
        VALUES (?, ?, ?, ?, ?, ?, ?, ?)
    )");
    query.addBindValue(course.courseCode);
    query.addBindValue(course.name);
    query.addBindValue(course.semester);
    query.addBindValue(course.credit);
    query.addBindValue(course.score);
    query.addBindValue(course.courseType);
    query.addBindValue(course.tags);
    query.addBindValue(course.status);

    if (!query.exec()) {
        qWarning() << "addCourse failed:" << query.lastError().text();
        return false;
    }
    emit dataChanged();
    return true;
}

bool DataManager::deleteCourse(int id)
{
    QSqlQuery query;
    query.prepare("DELETE FROM courses WHERE id=?");
    query.addBindValue(id);
    if (!query.exec()) {
        qWarning() << "deleteCourse failed:" << query.lastError().text();
        return false;
    }
    emit dataChanged();
    return true;
}

QList<Course> DataManager::getAllCourses() const
{
    return getCoursesByFilter();
}

QList<Course> DataManager::searchCourses(const QString& keyword, const QString& field) const
{
    QList<Course> result;
    QString sql = "SELECT id, course_code, name, semester, credit, score, course_type, tags FROM courses WHERE ";
    if (field == "name") {
        sql += "name LIKE '%" + keyword + "%'";
    } else if (field == "code") {
        sql += "course_code LIKE '%" + keyword + "%'";
    } else {
        return result;
    }
    QSqlQuery query(sql);
    while (query.next()) {
        Course c;
        c.id = query.value(0).toInt();
        c.courseCode = query.value(1).toString();
        c.name = query.value(2).toString();
        c.semester = query.value(3).toString();
        c.credit = query.value(4).toDouble();
        c.score = query.value(5).toDouble();
        c.courseType = query.value(6).toString();
        c.tags = query.value(7).toString();
        result.append(c);
    }
    return result;
}

QList<Course> DataManager::getCoursesByFilter(const QString& keyword,
                                              const QString& semester,
                                              const QString& courseType,
                                              const QString& tagKeyword) const
{
    QList<Course> courses;
    QString sql = "SELECT id, course_code, name, semester, credit, score, course_type, tags FROM courses WHERE 1=1";
    if (!keyword.isEmpty()) {
        sql += " AND (name LIKE '%" + keyword + "%' OR course_code LIKE '%" + keyword + "%')";
    }
    if (!semester.isEmpty()) {
        sql += " AND semester = '" + semester + "'";
    }
    if (!courseType.isEmpty()) {
        sql += " AND course_type = '" + courseType + "'";
    }
    if (!tagKeyword.isEmpty()) {
        sql += " AND tags LIKE '%" + tagKeyword + "%'";
    }
    QSqlQuery query(sql);
    while (query.next()) {
        Course c;
        c.id = query.value(0).toInt();
        c.courseCode = query.value(1).toString();
        c.name = query.value(2).toString();
        c.semester = query.value(3).toString();
        c.credit = query.value(4).toDouble();
        c.score = query.value(5).toDouble();
        c.courseType = query.value(6).toString();
        c.tags = query.value(7).toString();
        courses.append(c);
    }
    return courses;
}

// ========== 用户资料操作 ==========
UserProfile DataManager::getUserProfile() const
{
    UserProfile profile;
    QSqlQuery query("SELECT name, school, college, major, grade, education, photo_path FROM user_profile WHERE id=1");
    if (query.next()) {
        profile.name = query.value(0).toString();
        profile.school = query.value(1).toString();
        profile.college = query.value(2).toString();
        profile.major = query.value(3).toString();
        profile.grade = query.value(4).toString();
        profile.education = query.value(5).toString();
        profile.photoPath = query.value(6).toString();
    }
    return profile;
}

bool DataManager::saveUserProfile(const UserProfile& profile)
{
    QSqlQuery query;
    query.prepare(R"(
        UPDATE user_profile SET name=?, school=?, college=?, major=?, grade=?, education=?, photo_path=?
        WHERE id=1
    )");
    query.addBindValue(profile.name);
    query.addBindValue(profile.school);
    query.addBindValue(profile.college);
    query.addBindValue(profile.major);
    query.addBindValue(profile.grade);
    query.addBindValue(profile.education);
    query.addBindValue(profile.photoPath);
    if (!query.exec()) {
        qWarning() << "saveUserProfile failed:" << query.lastError().text();
        return false;
    }
    emit dataChanged();
    return true;
}
