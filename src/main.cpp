#include <QApplication>
#include <QFile>
#include <QDebug>
#include "MainWindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QFile styleFile(":/styles/style.qss");
    if (styleFile.open(QFile::ReadOnly)) {
        QString style = QLatin1String(styleFile.readAll());
        a.setStyleSheet(style);
        qDebug() << "蓝紫色样式加载成功";
    } else {
        qDebug() << "样式文件加载失败，请检查资源路径";
        // 可选：后备样式
        a.setStyleSheet("QWidget { background-color: #f3f4f8; }");
    }

    MainWindow w;
    w.setWindowIcon(QIcon(":/images/logo.png"));
    w.show();
    return a.exec();
}
