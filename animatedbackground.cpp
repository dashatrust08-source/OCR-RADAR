#include "animatedbackground.h"
#include <QPainter>

AnimatedBackground::AnimatedBackground(QWidget *parent)
    : QWidget(parent)
{
    setAttribute(Qt::WA_TransparentForMouseEvents);
    setAttribute(Qt::WA_NoSystemBackground);
    setAttribute(Qt::WA_TranslucentBackground);

    anim = new QPropertyAnimation(this, "pulse");
    anim->setDuration(2500);
    anim->setStartValue(0.0);
    anim->setEndValue(1.0);
    anim->setLoopCount(-1);
    anim->setEasingCurve(QEasingCurve::InOutQuad);
    anim->start();
}

void AnimatedBackground::setPulse(float p)
{
    m_pulse = p;
    update();
}

void AnimatedBackground::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // ⭐ Более светлый и мягкий градиент
    QLinearGradient grad(0, 0, 0, height());
    grad.setColorAt(0, QColor(70, 100, 150));   // светлее, чем было
    grad.setColorAt(1, QColor(120, 160, 210));  // ещё светлее

    painter.fillRect(rect(), grad);

    // ⭐ Еле заметные "огоньки"
    painter.setPen(Qt::NoPen);

    for (int i = 0; i < 25; i++)
    {
        // Позиция огонька
        float x = (width()  * ((i * 37) % 100) / 100.0f);
        float y = (height() * ((i * 53) % 100) / 100.0f);

        // Лёгкое движение вверх-вниз
        y += sin((m_pulse * 6.28f) + i) * 6;

        // Цвет огонька
        QColor glow(200, 230, 255, 80 + (m_pulse * 50));

        painter.setBrush(glow);

        // Маленький круг
        painter.drawEllipse(QPointF(x, y), 2.5, 2.5);
    }
}
