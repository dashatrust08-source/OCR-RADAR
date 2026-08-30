#include "imagelabel.h"
#include <QFileInfo>

ImageLabel::ImageLabel(QWidget *parent)
    : QLabel(parent)
{
    setAcceptDrops(true);
}

void ImageLabel::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasUrls())
        event->acceptProposedAction();
}

void ImageLabel::dropEvent(QDropEvent *event)
{
    QList<QUrl> urls = event->mimeData()->urls();
    if (urls.isEmpty())
        return;

    QString filePath = urls.first().toLocalFile();

    QFileInfo info(filePath);
    QString ext = info.suffix().toLower();

    if (ext == "png" || ext == "jpg" || ext == "jpeg") {
        emit imageDropped(filePath);
    }
}
