#ifndef HISTORYWINDOW_H
#define HISTORYWINDOW_H

#include <QDialog>
#include <QTableWidgetItem>

#include <QPropertyAnimation>
#include <QLabel>

#include "animatedbackground.h"

namespace Ui {
class HistoryWindow;
}

class HistoryWindow : public QDialog
{
    Q_OBJECT

public:
    explicit HistoryWindow(const QString &user, QWidget *parent = nullptr);
    ~HistoryWindow();

private slots:
    void onImageOpenRequested(int row, int column);
    void on_btnOpenImage_clicked();
    void onTextOpenRequested(int row, int column);

    void on_btnCleanHistory_clicked();

private:
    Ui::HistoryWindow *ui;
    QString currentUser;

    void loadHistory();

    QLabel *spinner = nullptr;
    QMovie *spinnerMovie = nullptr;


    QWidget *overlay = nullptr;

    AnimatedBackground *bg;

    void resizeEvent(QResizeEvent *event) override;



};

#endif // HISTORYWINDOW_H
