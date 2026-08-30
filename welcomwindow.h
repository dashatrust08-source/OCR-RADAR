#ifndef WELCOMWINDOW_H
#define WELCOMWINDOW_H

#include <QMainWindow>

namespace Ui {
class WelcomWindow;
}

class WelcomWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit WelcomWindow(QWidget *parent = nullptr);
    ~WelcomWindow();

private slots:
    void on_btnLogin_2_clicked();
    void on_btnTrial_2_clicked();


private:
    Ui::WelcomWindow *ui;
};

#endif // WELCOMWINDOW_H
