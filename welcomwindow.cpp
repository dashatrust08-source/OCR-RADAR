#include "welcomwindow.h"
#include "ui_welcomwindow.h"
#include "mainwindow.h"
#include "loginwindow.h"
#include "triallwindow.h"


WelcomWindow::WelcomWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::WelcomWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("Главная");

    this->showMaximized();
    qDebug() << "WELCOME WINDOW SHOWN";
    QPixmap pix("logo.png");   // путь к твоему логотипу в ресурсах
    ui->labelLogo->setPixmap(
        pix.scaled(200, 200, Qt::KeepAspectRatio, Qt::SmoothTransformation)
        );


}

WelcomWindow::~WelcomWindow()
{
    delete ui;
}


void WelcomWindow::on_btnLogin_2_clicked()
{
    LoginWindow *login = new LoginWindow();
    if (login->exec() == QDialog::Accepted) {

        MainWindow *w = new MainWindow(login->getLogin());
        w->show();

        this->close();
    }
}

void WelcomWindow::on_btnTrial_2_clicked()
{
    TriallWindow *trial = new TriallWindow(nullptr);
    trial->show();
    this->hide();
}
