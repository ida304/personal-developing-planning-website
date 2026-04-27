#include <QFile>
#include <QApplication>
#include "MainWindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

//    加载样式表（可选，如果 resources/style.qss 存在）
    QFile styleFile(":/style.qss");
    if (styleFile.open(QFile::ReadOnly)) {
        app.setStyleSheet(styleFile.readAll());
    }

    MainWindow w;
    w.show();
    return app.exec();
}
