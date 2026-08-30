#include "profilewindow.h"
#include "ui_profilewindow.h"
#include <QPixmap>
#include <QPainter>
#include <QtGlobal>
#include <QSqlQuery>
#include <QFile>
#include <QMessageBox>
#include <QFileDialog>
#include <QPainterPath>

#include "avatarcropdialog.h"

ProfileWindow::ProfileWindow(const QString &user, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ProfileWindow)
    , currentUser(user)
{
    ui->setupUi(this);
    this->setWindowTitle("Профиль");

    this->setFixedSize(540, 620);
    ui->labelName->setText(currentUser);

    bg = new AnimatedBackground(this);
    bg->setGeometry(this->rect());
    bg->lower();


    ui->comboLanguage->addItem("По умолчанию", "rus");
    ui->comboLanguage->addItem("Русский", "rus");
    ui->comboLanguage->addItem("Английский", "eng");
    ui->comboLanguage->addItem("Китайский", "chi_sim");
    ui->comboLanguage->addItem("Японский", "jpn");
    ui->comboLanguage->addItem("Немецкий","deu");
    ui->comboLanguage->addItem("Испанский","spa");
    ui->comboLanguage->addItem("Французский","fra");
    loadUserInfo();
}

ProfileWindow::~ProfileWindow()
{
    delete ui;
}


void ProfileWindow::on_btnClose_clicked()
{
    this->close();
}





QPixmap ProfileWindow::generateAvatar(const QString &name)
{
    int size = 150;
    QPixmap pix(size, size);
    pix.fill(Qt::transparent);

    QPainter painter(&pix);
    painter.setRenderHint(QPainter::Antialiasing);

    // Рандомный, но стабильный цвет
    srand(qHash(name));
    QColor color(rand() % 200, rand() % 200, rand() % 200);

    painter.setBrush(color);
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(0, 0, size, size);

    // Первая буква
    painter.setPen(Qt::white);
    painter.setFont(QFont("Segoe UI", 60, QFont::Bold));
    painter.drawText(pix.rect(), Qt::AlignCenter, name.left(1).toUpper());

    return pix;
}

void ProfileWindow::loadUserInfo()
{
    ui->labelName->setText(currentUser);

    QSqlQuery q;
    q.prepare("SELECT photo_path, about, translate_lang FROM users WHERE login = :login");

    q.bindValue(":login", currentUser);
    q.exec();

    if (q.next()) {
        QString photo = q.value(0).toString();
        QString about = q.value(1).toString();

        if (!photo.isEmpty() && QFile::exists(photo)) {
            QPixmap pix(photo);
            pix = makeRoundImage(pix);
            ui->labelAvatar->setPixmap(
                pix.scaled(150,150,Qt::KeepAspectRatio,Qt::SmoothTransformation)
                );
        } else {
            QPixmap pix = generateAvatar(currentUser);
            pix = makeRoundImage(pix);
            ui->labelAvatar->setPixmap(
                pix.scaled(150,150,Qt::KeepAspectRatio,Qt::SmoothTransformation)
                );
        }


        ui->textAbout->setPlainText(about);
    }


    QString lang = q.value("translate_lang").toString();
    if (lang.isEmpty()) lang = "rus";

    int index = ui->comboLanguage->findData(lang);
    if (index >= 0)
        ui->comboLanguage->setCurrentIndex(index);

}


QPixmap ProfileWindow::makeRoundImage(const QPixmap &src)
{
    int size = qMin(src.width(), src.height());
    QPixmap dst(size, size);
    dst.fill(Qt::transparent);

    QPainter painter(&dst);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);

    QPainterPath path;
    path.addEllipse(0, 0, size, size);
    painter.setClipPath(path);

    painter.drawPixmap(0, 0, src);

    return dst;
}



void ProfileWindow::on_btnChangePhoto_clicked()
{
    QString file = QFileDialog::getOpenFileName(this, "Выберите фото", "", "Images (*.png *.jpg)");
    if (file.isEmpty()) return;

    QPixmap original(file);

    // Открываем окно обрезки
    AvatarCropDialog dlg(original, this);
    if (dlg.exec() == QDialog::Accepted)
    {
        QPixmap cropped = dlg.getCroppedAvatar();

        QString tempPath = QDir::temp().filePath(currentUser + "_avatar.png");

        cropped.save(tempPath);

        QSqlQuery q;
        q.prepare("UPDATE users SET photo_path = :p WHERE login = :login");
        q.bindValue(":p", tempPath);
        q.bindValue(":login", currentUser);
        q.exec();

        ui->labelAvatar->setPixmap(
            cropped.scaled(150,150,Qt::KeepAspectRatio,Qt::SmoothTransformation)
            );
    }


}


void ProfileWindow::on_btnSave_clicked()
{
    QString about = ui->textAbout->toPlainText();

    QSqlQuery q;
    q.prepare("UPDATE users SET about = :about, translate_lang = :lang WHERE login = :login");
    q.bindValue(":about", about);
    q.bindValue(":lang", ui->comboLanguage->currentData().toString());
    q.bindValue(":login", currentUser);
    q.exec();



    QMessageBox::information(this, "Готово", "Данные сохранены");
}

void ProfileWindow::resizeEvent(QResizeEvent *event)
{
    QDialog::resizeEvent(event);
    if (bg)
        bg->setGeometry(this->rect());
}
