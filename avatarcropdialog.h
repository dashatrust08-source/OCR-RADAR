#ifndef AVATARCROPDIALOG_H
#define AVATARCROPDIALOG_H

#include <QDialog>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QResizeEvent>
#include <QPainterPath>

class CropView : public QGraphicsView
{
    Q_OBJECT
public:
    explicit CropView(QWidget *parent = nullptr);

    void setPixmap(const QPixmap &pix);
    QPixmap getCroppedPixmap() const;
    void reset();

protected:
    void wheelEvent(QWheelEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;   // ⭐ ЭТОГО НЕ ХВАТАЛО
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    void clampImage();
    void updateMask();

    QGraphicsPixmapItem *imageItem = nullptr;
    QPointF lastMousePos;
    double scaleFactor = 1.0;
    QRectF cropRect;
};

class AvatarCropDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AvatarCropDialog(const QPixmap &pix, QWidget *parent = nullptr);
    QPixmap getCroppedAvatar() const;

private:
    CropView *view;
};

#endif // AVATARCROPDIALOG_H
