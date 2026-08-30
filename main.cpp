#include "mainwindow.h"
#include "loginwindow.h"
#include "mainwindow.h"
#include "database.h"
#include "welcomwindow.h"
#include <QApplication>
#include <QFile>
#include <QFontDatabase>
#include <QDir>
#include "GlassSplashScreen.h"
#include <QTimer>

int main(int argc, char *argv[])
{

    QApplication a(argc, argv);
    a.setWindowIcon(QIcon("icon.png"));


    // Создаём splash
    GlassSplashScreen splash;
    splash.start();

    // Создаём главное окно (но пока не показываем)
    // Показываем WelcomeWindow первым
    WelcomWindow welcome;


    // Показываем окно после загрузки
    QTimer::singleShot(2000, [&]() {
        splash.finish();
        welcome.show();
    });

    QFile style(QCoreApplication::applicationDirPath() + "/style.qss");

    if (style.open(QFile::ReadOnly)) {
        QString qss = style.readAll();
        a.setStyleSheet(qss);
        qDebug() << "Style loaded:" << !qss.isEmpty();
    } else {
        qDebug() << "Style NOT loaded!";
    }

    int id = QFontDatabase::addApplicationFont(":/fonts/ofont.ru_Oprah.ttf");
    if (id != -1) {
        QString family = QFontDatabase::applicationFontFamilies(id).at(0);
        QFont font(family, 16);
        a.setFont(font);
    }

    qDebug() << "Font ID =" << id;
    qDebug() << "Root:" << QDir(":/").entryList();
    qDebug() << "Fonts:" << QDir(":/fonts").entryList();


    initDatabase();



    return a.exec();
}
