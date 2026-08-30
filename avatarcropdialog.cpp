#include "avatarcropdialog.h"
#include <QPainter>
#include <QVBoxLayout>
#include <QPushButton>
#include <QHBoxLayout>

CropView::CropView(QWidget *parent)
    : QGraphicsView(parent)
{
    setRenderHint(QPainter::Antialiasing);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setDragMode(QGraphicsView::NoDrag);
    setMouseTracking(true);
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    setResizeAnchor(QGraphicsView::AnchorUnderMouse);

    auto *sc = new QGraphicsScene(this);
    setScene(sc);

    imageItem = new QGraphicsPixmapItem();
    scene()->addItem(imageItem);

    // начальная сцена и круг
    scene()->setSceneRect(0, 0, 420, 420);
    cropRect = QRectF(60, 60, 300, 300);
}

void CropView::setPixmap(const QPixmap &pix)
{
    imageItem->setPixmap(pix);
    imageItem->setTransformationMode(Qt::SmoothTransformation);

    QRectF bounds = imageItem->boundingRect();
    qreal scaleX = cropRect.width() / bounds.width();
    qreal scaleY = cropRect.height() / bounds.height();
    scaleFactor = qMin(scaleX, scaleY);

    imageItem->setScale(scaleFactor);

    QRectF scaledBounds = imageItem->sceneBoundingRect();
    qreal dx = cropRect.center().x() - scaledBounds.center().x();
    qreal dy = cropRect.center().y() - scaledBounds.center().y();
    imageItem->moveBy(dx, dy);

    clampImage();
    updateMask();
}

void CropView::reset()
{
    if (imageItem && !imageItem->pixmap().isNull())
        setPixmap(imageItem->pixmap());
}

void CropView::wheelEvent(QWheelEvent *event)
{
    if (!imageItem || imageItem->pixmap().isNull())
        return;

    double delta = event->angleDelta().y() > 0 ? 1.1 : 0.9;
    qreal newScale = scaleFactor * delta;

    if (newScale < 0.5) newScale = 0.5;
    if (newScale > 5.0) newScale = 5.0;

    if (newScale != scaleFactor) {
        scaleFactor = newScale;
        imageItem->setScale(scaleFactor);
        clampImage();
        updateMask();
    }
}

void CropView::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && imageItem && !imageItem->pixmap().isNull()) {
        lastMousePos = event->pos();   // event->pos() — QPoint
        setCursor(Qt::ClosedHandCursor);
    }
    QGraphicsView::mousePressEvent(event);
}

void CropView::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton && imageItem && !imageItem->pixmap().isNull()) {

        QPointF delta =
            mapToScene(event->pos()) -
            mapToScene(lastMousePos.toPoint());   // ⭐ исправлено

        imageItem->moveBy(delta.x(), delta.y());
        lastMousePos = event->pos();
        clampImage();
        updateMask();
    }

    QGraphicsView::mouseMoveEvent(event);
}

void CropView::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
        setCursor(Qt::ArrowCursor);

    QGraphicsView::mouseReleaseEvent(event);
}

void CropView::clampImage()
{
    if (!imageItem || imageItem->pixmap().isNull())
        return;

    QRectF imgRect = imageItem->sceneBoundingRect();

    qreal dx = 0, dy = 0;

    if (imgRect.left() > cropRect.left())
        dx = cropRect.left() - imgRect.left();
    if (imgRect.top() > cropRect.top())
        dy = cropRect.top() - imgRect.top();
    if (imgRect.right() < cropRect.right())
        dx = cropRect.right() - imgRect.right();
    if (imgRect.bottom() < cropRect.bottom())
        dy = cropRect.bottom() - imgRect.bottom();

    if (dx != 0 || dy != 0)
        imageItem->moveBy(dx, dy);
}

void CropView::updateMask()
{
    viewport()->update();
}

void CropView::paintEvent(QPaintEvent *event)
{
    QGraphicsView::paintEvent(event);

    QPainter painter(viewport());
    painter.setRenderHint(QPainter::Antialiasing);

    QPainterPath fullPath;
    fullPath.addRect(viewport()->rect());

    QPolygonF poly = mapFromScene(cropRect);
    QRectF viewCropRect = poly.boundingRect();

    QPainterPath circlePath;
    circlePath.addEllipse(viewCropRect);

    fullPath -= circlePath;

    painter.fillPath(fullPath, QColor(0, 0, 0, 150));

    painter.setPen(QPen(Qt::white, 3));
    painter.setBrush(Qt::NoBrush);
    painter.drawEllipse(viewCropRect);
}

void CropView::resizeEvent(QResizeEvent *event)
{
    QGraphicsView::resizeEvent(event);

    scene()->setSceneRect(0, 0, viewport()->width(), viewport()->height());

    qreal size = qMin(viewport()->width(), viewport()->height()) - 40;
    cropRect = QRectF(0, 0, size, size);
    cropRect.moveCenter(scene()->sceneRect().center());

    if (imageItem && !imageItem->pixmap().isNull())
        setPixmap(imageItem->pixmap());
}

QPixmap CropView::getCroppedPixmap() const
{
    if (!imageItem || imageItem->pixmap().isNull())
        return QPixmap();

    QImage result((int)cropRect.width(), (int)cropRect.height(), QImage::Format_ARGB32);
    result.fill(Qt::transparent);

    QPainter painter(&result);
    painter.setRenderHint(QPainter::Antialiasing);

    QPainterPath circlePath;
    circlePath.addEllipse(QRectF(0, 0, cropRect.width(), cropRect.height()));
    painter.setClipPath(circlePath);

    QPixmap scenePixmap(scene()->sceneRect().size().toSize());
    scenePixmap.fill(Qt::transparent);

    QPainter scenePainter(&scenePixmap);
    scene()->render(&scenePainter);
    scenePainter.end();

    QRectF sourceRect = cropRect;
    QRectF targetRect(0, 0, cropRect.width(), cropRect.height());

    painter.drawPixmap(targetRect, scenePixmap, sourceRect);

    return QPixmap::fromImage(result);
}

// ---------------- AvatarCropDialog ----------------

AvatarCropDialog::AvatarCropDialog(const QPixmap &pix, QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("Обрезка аватара");
    setFixedSize(450, 520);

    view = new CropView(this);
    view->setFixedSize(420, 420);
    view->setPixmap(pix);

    QPushButton *btnOk = new QPushButton("Обрезать");
    QPushButton *btnCancel = new QPushButton("Отмена");
    QPushButton *btnReset = new QPushButton("Сброс");

    // ⭐ Делаем кнопки нормальной ширины
    btnOk->setMinimumWidth(120);
    btnCancel->setMinimumWidth(120);
    btnReset->setMinimumWidth(120);

    // ⭐ Делаем кнопки растягиваемыми (красиво и удобно)
    btnOk->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    btnCancel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    btnReset->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);




    connect(btnOk, &QPushButton::clicked, this, &QDialog::accept);
    connect(btnCancel, &QPushButton::clicked, this, &QDialog::reject);
    connect(btnReset, &QPushButton::clicked, view, &CropView::reset);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(btnReset);
    buttonLayout->addStretch();
    buttonLayout->addWidget(btnOk);
    buttonLayout->addWidget(btnCancel);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(view, 0, Qt::AlignCenter);
    layout->addLayout(buttonLayout);
}

QPixmap AvatarCropDialog::getCroppedAvatar() const
{
    return view->getCroppedPixmap();
}
