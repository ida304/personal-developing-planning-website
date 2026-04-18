QT += core widgets sql

CONFIG += c++17
CONFIG += console

TARGET = PersonalDevPlan
TEMPLATE = app

# 源文件
SOURCES += \
    src/main.cpp \
    src/MainWindow.cpp \
    src/core/DataManager.cpp \
    src/core/Validator.cpp \
    src/profile/ProfileWidget.cpp \
    src/course/CourseWidget.cpp \
    src/experience_achievement/ExpAchieveWidget.cpp \
    src/resume/ResumeWidget.cpp \
    src/progress/ProgressWidget.cpp

# 头文件
HEADERS += \
    src/MainWindow.h \
    src/core/DataManager.h \
    src/core/models.h \
    src/core/Validator.h \
    src/profile/ProfileWidget.h \
    src/course/CourseWidget.h \
    src/experience_achievement/ExpAchieveWidget.h \
    src/resume/ResumeWidget.h \
    src/progress/ProgressWidget.h

# 资源文件（包含样式、模板、JSON配置）
RESOURCES += resources/resources.qrc

# 默认规则
DEFAULT_RULES
