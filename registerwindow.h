#ifndef REGISTERWINDOW_H
#define REGISTERWINDOW_H

#include <QDialog>

#include "animatedbackground.h"

namespace Ui {
class RegisterWindow;
}

class RegisterWindow : public QDialog
{
    Q_OBJECT

public:
    explicit RegisterWindow(QWidget *parent = nullptr);
    ~RegisterWindow();

private slots:
    void on_btnCreateAccount_clicked();

public:
    QString getLogin() const { return createdLogin; }


private:
    Ui::RegisterWindow *ui;
    QString createdLogin;
    bool loginExists(const QString &login);
    QString hashPassword(const QString &password);

    AnimatedBackground *bg;

    void resizeEvent(QResizeEvent *event) override;

};

#endif // REGISTERWINDOW_H
