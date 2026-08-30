#ifndef GLASSSPLASHSCREEN_H
#define GLASSSPLASHSCREEN_H

#include <QWidget>
#include <QLabel>
#include <QPropertyAnimation>
#include <QGraphicsBlurEffect>
#include <QTimer>

class GlassSplashScreen : public QWidget
{
    Q_OBJECT
public:
    explicit GlassSplashScreen(QWidget *parent = nullptr);

    void start();
    void finish();

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QLabel *logoLabel;
    QLabel *textLabel;
    QLabel *dotsLabel;

    QPropertyAnimation *fadeInAnim;
    QPropertyAnimation *fadeOutAnim;
    QTimer *dotsTimer;

    int dotCount = 0;
};

#endif // GLASSSPLASHSCREEN_H
