#include "loginwindow.h"
#include "ui_loginwindow.h"
#include "registerwindow.h"
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlError>
#include <QCryptographicHash>
#include "mainwindow.h"

#include "animatedbackground.h"


LoginWindow::LoginWindow(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::LoginWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("Вход/регистрация");

    bg = new AnimatedBackground(this);
    bg->setGeometry(this->rect());
    bg->lower();




}




QString LoginWindow::hashPassword(const QString &password)
{
    return QString(
        QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha256).toHex()
        );
}


bool LoginWindow::checkUser(const QString &login, const QString &password)
{
    QString hash = hashPassword(password);

    QSqlQuery query;
    query.prepare("SELECT id FROM users WHERE login = :login AND password_hash = :hash");
    query.bindValue(":login", login);
    query.bindValue(":hash", hash);
    query.exec();

    return query.next(); // если есть строка → логин+пароль верны
}


QString LoginWindow::getLogin() const {return ui->lineLogin->text();}

LoginWindow::~LoginWindow()
{
    delete ui;
}

void LoginWindow::on_btnLogin_clicked()
{
    QString login = ui->lineLogin->text().trimmed();
    QString password = ui->linePassword->text();

    if (login.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Введите логин и пароль.");
        return;
    }

    if (!checkUser(login, password)) {
        QMessageBox::warning(this, "Ошибка", "Неверный логин или пароль.");
        return;
    }

    // Проверяем, есть ли строка в users
    QSqlQuery q;
    q.prepare("SELECT id FROM users WHERE login = :login");
    q.bindValue(":login", login);
    q.exec();

    if (!q.next()) {
        // Создаём строку, если её нет
        QSqlQuery insert;
        insert.prepare("INSERT INTO users (login, password_hash, photo_path, about) "
                       "VALUES (:login, '', '', '')");
        insert.bindValue(":login", login);
        insert.exec();
    }


    // Успех — закрываем окно и возвращаем логин
    accept();
}


void LoginWindow::on_btnRegister_clicked()
{

    RegisterWindow reg(this);

    if (reg.exec() == QDialog::Accepted) {

        QString newUser = reg.getLogin();

        MainWindow *w = new MainWindow(newUser);
        w->show();

        this->close();
    }


}

void LoginWindow::resizeEvent(QResizeEvent *event)
{
    QDialog::resizeEvent(event);
    if (bg)
        bg->setGeometry(this->rect());
}
