#include "GlassSplashScreen.h"
#include <QVBoxLayout>
#include <QPainter>
#include <QScreen>
#include <QApplication>

GlassSplashScreen::GlassSplashScreen(QWidget *parent)
    : QWidget(parent)
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::SplashScreen);
    setAttribute(Qt::WA_TranslucentBackground);

    // Стеклянный blur
    auto *blur = new QGraphicsBlurEffect(this);
    blur->setBlurRadius(25);
    setGraphicsEffect(blur);

    // Логотип из ресурсов
    logoLabel = new QLabel(this);
    logoLabel->setPixmap(QPixmap(":/logo/icon.png")
                             .scaled(120, 120, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    logoLabel->setAlignment(Qt::AlignCenter);

    // Текст "Загрузка…"
    textLabel = new QLabel("Загрузка…", this);
    textLabel->setStyleSheet("color: white; font-size: 22px; font-weight: 500;");
    textLabel->setAlignment(Qt::AlignCenter);

    // Анимация точек
    dotsLabel = new QLabel("", this);
    dotsLabel->setStyleSheet("color: white; font-size: 26px;");
    dotsLabel->setAlignment(Qt::AlignCenter);

    dotsTimer = new QTimer(this);
    connect(dotsTimer, &QTimer::timeout, [&]() {
        dotCount = (dotCount + 1) % 4;
        dotsLabel->setText(QString(".").repeated(dotCount));
    });

    // Верстка
    auto *layout = new QVBoxLayout(this);
    layout->addStretch();
    layout->addWidget(logoLabel);
    layout->addSpacing(20);
    layout->addWidget(textLabel);
    layout->addWidget(dotsLabel);
    layout->addStretch();
    setLayout(layout);

    // Анимации появления/исчезновения
    fadeInAnim = new QPropertyAnimation(this, "windowOpacity");
    fadeInAnim->setDuration(600);
    fadeInAnim->setStartValue(0.0);
    fadeInAnim->setEndValue(1.0);

    fadeOutAnim = new QPropertyAnimation(this, "windowOpacity");
    fadeOutAnim->setDuration(600);
    fadeOutAnim->setStartValue(1.0);
    fadeOutAnim->setEndValue(0.0);
}

void GlassSplashScreen::start()
{
    resize(500, 350);
    move(QApplication::primaryScreen()->geometry().center() - rect().center());

    show();
    fadeInAnim->start();
    dotsTimer->start(350);
}

void GlassSplashScreen::finish()
{
    dotsTimer->stop();
    connect(fadeOutAnim, &QPropertyAnimation::finished, this, &QWidget::close);
    fadeOutAnim->start();
}

void GlassSplashScreen::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    // Полупрозрачное стекло
    QColor bg(255, 255, 255, 40); // белый с прозрачностью
    p.setBrush(bg);
    p.setPen(Qt::NoPen);

    p.drawRoundedRect(rect(), 20, 20);
}
