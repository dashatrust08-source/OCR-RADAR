#include "registerwindow.h"
#include "ui_registerwindow.h"
#include "mainwindow.h"
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlError>
#include <QCryptographicHash>
#include "animatedbackground.h"

RegisterWindow::RegisterWindow(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::RegisterWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("Регистрация");

    bg = new AnimatedBackground(this);
    bg->setGeometry(this->rect());
    bg->lower();


}

bool RegisterWindow::loginExists(const QString &login)
{
    QSqlQuery query;
    query.prepare("SELECT id FROM users WHERE login = :login");
    query.bindValue(":login", login);
    query.exec();


    return query.next(); // если есть строка → логин существует
}

QString RegisterWindow::hashPassword(const QString &password)
{
    return QString(
        QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha256).toHex()
        );
}


RegisterWindow::~RegisterWindow()
{
    delete ui;
}



void RegisterWindow::on_btnCreateAccount_clicked()
{
    QString login = ui->lineLogin->text().trimmed();
    QString pass1 = ui->linePassword->text();
    QString pass2 = ui->linePassword2->text();

    if (login.isEmpty() || pass1.isEmpty() || pass2.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Заполните все поля.");
        return;
    }

    if (pass1 != pass2) {
        QMessageBox::warning(this, "Ошибка", "Пароли не совпадают.");
        return;
    }

    if (loginExists(login)) {
        QMessageBox::warning(this, "Ошибка", "Такой логин уже существует.");
        return;
    }

    QString hash = hashPassword(pass1);


    QSqlQuery query;
    query.prepare("INSERT INTO users (login, password_hash, photo_path, about) "
                  "VALUES (:login, :hash, '', '')");
    query.bindValue(":login", login);
    query.bindValue(":hash", hash);

    if (!query.exec()) {
        QMessageBox::critical(this, "Ошибка", "Не удалось создать аккаунт.");
        return;
    }


    QMessageBox::information(this, "Успех", "Аккаунт создан!");
    createdLogin = login;
    accept(); // закрыть окно
}

void RegisterWindow::resizeEvent(QResizeEvent *event)
{
    QDialog::resizeEvent(event);
    if (bg)
        bg->setGeometry(this->rect());
}
