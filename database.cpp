#include "database.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QCoreApplication>

bool initDatabase()
{
    QString dbPath = QCoreApplication::applicationDirPath() + "/users.db";

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(dbPath);

    if (!db.open()) {
        qDebug() << "Не удалось открыть БД:" << db.lastError().text();
        return false;
    }

    QSqlQuery query;

    // USERS
    QString createUsers =
        "CREATE TABLE IF NOT EXISTS users ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "login TEXT UNIQUE,"
        "password_hash TEXT,"
        "photo_path TEXT,"
        "about TEXT,"
        "translate_lang TEXT DEFAULT 'ru'"
        ");";

    if (!query.exec(createUsers)) {
        qDebug() << "Ошибка создания users:" << query.lastError().text();
        return false;
    }

    // HISTORY
    QString createHistory =
        "CREATE TABLE IF NOT EXISTS history ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "user TEXT,"
        "image_path TEXT,"
        "recognized_text TEXT,"
        "translated_text TEXT,"
        "timestamp DATETIME DEFAULT CURRENT_TIMESTAMP"
        ");";

    if (!query.exec(createHistory)) {
        qDebug() << "Ошибка создания history:" << query.lastError().text();
        return false;
    }

    return true;
}
