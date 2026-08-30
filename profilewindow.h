#ifndef PROFILEWINDOW_H
#define PROFILEWINDOW_H

#include <QDialog>
#include <QPixmap>

#include "animatedbackground.h"

namespace Ui {
class ProfileWindow;
}

class ProfileWindow : public QDialog
{
    Q_OBJECT

public:
    explicit ProfileWindow(const QString &user, QWidget *parent = nullptr);
    ~ProfileWindow();

private slots:
    void on_btnClose_clicked();


    void on_btnChangePhoto_clicked();

    void on_btnSave_clicked();

private:
    Ui::ProfileWindow *ui;
    QString currentUser;

    QPixmap generateAvatar(const QString &name);
    QPixmap makeRoundImage(const QPixmap &src);
    void loadUserInfo();

    AnimatedBackground *bg;

    void resizeEvent(QResizeEvent *event) override;

};

#endif // PROFILEWINDOW_H
