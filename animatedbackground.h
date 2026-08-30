#ifndef ANIMATEDBACKGROUND_H
#define ANIMATEDBACKGROUND_H

#include <QWidget>
#include <QPropertyAnimation>

class AnimatedBackground : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(float pulse READ pulse WRITE setPulse)

public:
    explicit AnimatedBackground(QWidget *parent = nullptr);

    float pulse() const { return m_pulse; }
    void setPulse(float p);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    float m_pulse = 0.0f;
    QPropertyAnimation *anim;
};

#endif // ANIMATEDBACKGROUND_H
