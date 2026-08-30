#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H

#include <QDialog>

#include "animatedbackground.h"



namespace Ui {
class LoginWindow;
}

class LoginWindow : public QDialog
{
    Q_OBJECT

public:
    explicit LoginWindow(QWidget *parent = nullptr);

    QString getLogin() const;


    ~LoginWindow();

private slots:
    void on_btnLogin_clicked();

    void on_btnRegister_clicked();

private:
    Ui::LoginWindow *ui;

    bool checkUser(const QString &login, const QString &password);
    QString hashPassword(const QString &password);

    AnimatedBackground *bg;

   void resizeEvent(QResizeEvent *event) override;


};

#endif // LOGINWINDOW_H
