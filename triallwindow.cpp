#include "triallwindow.h"
#include "ui_triallwindow.h"
#include "welcomwindow.h"

#include "loginwindow.h"
#include "aboutwindow.h"
#include "mainwindow.h"
#include <QProcess>
#include <QSqlQuery>
#include <QPixmap>

#include <QFileDialog>
#include <QMessageBox>
#include <QPixmap>


#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>


#include <QDragEnterEvent>
#include <QMimeData>


#include <QtConcurrent>

#include <QClipboard>
#include <QGuiApplication>

#include <QDialog>

#include<QGraphicsDropShadowEffect>

TriallWindow::TriallWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::TriallWindow)
{
    ui->setupUi(this);

    this->setFocusPolicy(Qt::StrongFocus);
    this->setFocus();


    ui->labelImage->setStyleSheet(
        "color: white;"
        "font-size: 20px;"
        "font-weight: 600;"
        );


    this->setWindowTitle("Пробная версия");
    ui->scrollAreaWidgetContents->setMinimumHeight(3000);
    ui->centralwidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    ui->centralwidget->setMinimumSize(this->size());

    this->showMaximized();

    setAcceptDrops(true);
    ui->labelImage->setAcceptDrops(true);
    ui->labelImage->installEventFilter(this);



    deleteOverlay = new QLabel(ui->labelImage->parentWidget());
    deleteOverlay->setGeometry(ui->labelImage->geometry());
    deleteOverlay->setStyleSheet(
        "background: rgba(0,0,0,140);"
        "color: white;"
        "font-size: 20px;"
        "border-radius: 10px;"
        );
    deleteOverlay->setAlignment(Qt::AlignCenter);
    deleteOverlay->setText("Удалить изображение");
    deleteOverlay->hide();
    deleteOverlay->raise();
    deleteOverlay->setAttribute(Qt::WA_TransparentForMouseEvents, false);
    deleteOverlay->installEventFilter(this);




    // Создаём слой-рамку поверх labelImage
    glowFrame = new QLabel(ui->labelImage->parentWidget());
    glowFrame->setGeometry(ui->labelImage->geometry());
    glowFrame->lower(); // чтобы рамка была под картинкой
    glowFrame->show();

    // Эффект свечения
    auto *glow = new QGraphicsDropShadowEffect(this);
    glow->setBlurRadius(40);
    glow->setColor(QColor(0, 170, 255));
    glow->setOffset(0, 0);

    glowFrame->setGraphicsEffect(glow);

    // Делаем рамку прозрачной
    glowFrame->setStyleSheet("background: transparent; border: 3px solid #00aaff; border-radius: 20px;");





    //рамка для поля распознания
    glowRecognized = new QLabel(ui->textRecognized->parentWidget());
    glowRecognized->setGeometry(ui->textRecognized->geometry());
    glowRecognized->lower();
    glowRecognized->show();

    auto *glowR = new QGraphicsDropShadowEffect(this);
    glowR->setBlurRadius(40);
    glowR->setColor(QColor(0, 170, 255));
    glowR->setOffset(0, 0);

    glowRecognized->setGraphicsEffect(glowR);

    glowRecognized->setStyleSheet(
        "background: transparent; border: 7px solid #00aaff; border-radius: 20px;"
        );



    //рамка для поля перевода
    glowTranslate = new QLabel(ui->textTranslate->parentWidget());
    glowTranslate->setGeometry(ui->textTranslate->geometry());
    glowTranslate->lower();
    glowTranslate->show();

    auto *glowT = new QGraphicsDropShadowEffect(this);
    glowT->setBlurRadius(40);
    glowT->setColor(QColor(0, 170, 255));
    glowT->setOffset(0, 0);

    glowTranslate->setGraphicsEffect(glowT);

    glowTranslate->setStyleSheet(
        "background: transparent; border: 7px solid #00aaff; border-radius: 20px;"
        );



    loadingBar = new QLabel(ui->textTranslate);
    loadingBar->setStyleSheet("background-color: #00e5ff; border-radius: 3px;");
    loadingBar->setGeometry(0, 0, 0, 4);
    loadingBar->hide();

    loadingAnim = new QPropertyAnimation(loadingBar, "geometry");
    loadingAnim->setDuration(1200);
    loadingAnim->setLoopCount(-1);




    updateAttemptsLabel();
    blockRecognitionIfNeeded();
    ui->comboLanguage->addItem("Все языки", "all");
    ui->comboLanguage->addItem("Русский", "rus");
    ui->comboLanguage->addItem("Английский", "eng");
    ui->comboLanguage->addItem("Китайский", "chi_sim");
    ui->comboLanguage->addItem("Японский", "jpn");
    ui->comboLanguage->addItem("Немецкий","deu");
    ui->comboLanguage->addItem("Испанский","spa");
    ui->comboLanguage->addItem("Французский","fra");


    ui->comboTranslate->addItem("Русский", "rus");
    ui->comboTranslate->addItem("Английский", "eng");
    ui->comboTranslate->addItem("Китайский", "chi_sim");
    ui->comboTranslate->addItem("Японский", "jpn");
    ui->comboTranslate->addItem("Немецкий","deu");
    ui->comboTranslate->addItem("Испанский","spa");
    ui->comboTranslate->addItem("Французский","fra");


    ui->textRecognized->setAlignment(Qt::AlignCenter);
    ui->textRecognized->setText(
        "Здесь вы увидите распознанный текст,\n"
        "для этого загрузите фото и нажмите кнопку \"Распознать\""
        );




    scanOverlay = new QLabel(ui->labelImage);
    scanOverlay->setStyleSheet(
        "background: qlineargradient("
        "x1:0, y1:0, x2:0, y2:1, "
        "stop:0 rgba(0,255,255,0), "
        "stop:0.5 rgba(0,255,255,80), "
        "stop:1 rgba(0,255,255,0));"
        );






    scanAnim = new QPropertyAnimation(scanOverlay, "geometry");

    scanAnim->setEasingCurve(QEasingCurve::InOutQuad);

    connect(scanAnim, &QPropertyAnimation::finished, this, [=]() {
        scanOverlay->hide();
    });


    loadingBar = new QLabel(ui->textTranslate);
    loadingBar->setStyleSheet("background-color: #00e5ff; border-radius: 3px;");
    loadingBar->setGeometry(0, 0, 0, 4); // высота 4px, ширина 0
    loadingBar->hide();

    loadingAnim = new QPropertyAnimation(loadingBar, "geometry");
    loadingAnim->setDuration(1200);
    loadingAnim->setLoopCount(-1); // бесконечно







}

TriallWindow::~TriallWindow()
{
    delete ui;
}



// попыточки

void TriallWindow::updateAttemptsLabel()
{
    ui->labelAttempts->setText("Осталось попыток: " + QString::number(attemptsLeft));
}

void TriallWindow::blockRecognitionIfNeeded()
{
    if (attemptsLeft <= 0) {
        ui->btnRecognize_4->setEnabled(false);
        ui->labelAttempts->setText("Попытки закончились");
    }
}



void TriallWindow::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasUrls())
        event->acceptProposedAction();
}


void TriallWindow::on_btnLoadImage_clicked()
{
    QString file = QFileDialog::getOpenFileName(
        this,
        "Выберите изображение",
        "",
        "Images (*.png *.jpg *.jpeg)"
        );

    if (file.isEmpty())
        return;

    currentImagePath = file;

    QPixmap pix(file);
    ui->labelImage->setPixmap(
        pix.scaled(ui->labelImage->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation)
        );
}

QString TriallWindow::recognizeImage(const QString &imagePath, const QString &lang)
{
    QString tesseractPath = "C:/Program Files/Tesseract-OCR/tesseract.exe";

    QProcess process;
    QStringList args;
    args << imagePath << "stdout" << "-l" << lang;

    process.start(tesseractPath, args);
    process.waitForFinished();

    QString output = process.readAllStandardOutput();
    QString error = process.readAllStandardError();

    if (process.exitStatus() != QProcess::NormalExit || process.exitCode() != 0) {
        return "Ошибка: " + error;
    }

    return output;
}




//чтоб нормально картинка тянулась при изменениях
void TriallWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);

    if (!currentImagePath.isEmpty()) {
        QPixmap pix(currentImagePath);
        ui->labelImage->setPixmap(
            pix.scaled(ui->labelImage->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation)
            );
    }
}


void TriallWindow::convertPdfToImages(const QString &pdfPath)
{
    QString outputPattern = QDir::temp().filePath("page_%d.png");

    QString mutool = "D:/Libraries/mutool.exe";

    QStringList args;
    args << "convert"
         << "-o" << outputPattern
         << pdfPath
         << "1-10"; // первые 10 страниц

    QProcess::execute(mutool, args);
}

QString TriallWindow::recognizePdf(const QString &pdfPath, const QString &lang)
{
    convertPdfToImages(pdfPath);

    QString fullText;

    for (int i = 1; i <= 10; ++i) {
        QString imgPath = QDir::temp().filePath(QString("page_%1.png").arg(i));

        if (!QFile::exists(imgPath))
            break;

        QString text = recognizeImage(imgPath, lang);
        fullText += QString("=== Страница %1 ===\n%2\n\n").arg(i).arg(text);
    }

    return fullText;
}


bool TriallWindow::eventFilter(QObject *obj, QEvent *event)
{
    // -----------------------------
    // 1. Наведение и клики по labelImage
    // -----------------------------
    if (obj == ui->labelImage)
    {
        if (event->type() == QEvent::Enter)
        {
            if (!currentImagePath.isEmpty())
                deleteOverlay->show();
            return true;
        }

        if (event->type() == QEvent::Leave)
        {
            deleteOverlay->hide();
            return true;
        }

        if (event->type() == QEvent::MouseButtonPress)
        {
            ui->labelImage->setFocus();
            return false;
        }

        if (event->type() == QEvent::DragEnter)
        {
            auto *dragEvent = static_cast<QDragEnterEvent*>(event);
            if (dragEvent->mimeData()->hasUrls())
                dragEvent->acceptProposedAction();
            return true;
        }

        if (event->type() == QEvent::Drop)
        {
            auto *dropEvent = static_cast<QDropEvent*>(event);
            QList<QUrl> urls = dropEvent->mimeData()->urls();

            if (!urls.isEmpty())
            {
                QString filePath = urls.first().toLocalFile();

                if (filePath.endsWith(".png", Qt::CaseInsensitive) ||
                    filePath.endsWith(".jpg", Qt::CaseInsensitive) ||
                    filePath.endsWith(".jpeg", Qt::CaseInsensitive))
                {
                    currentImagePath = filePath;

                    QPixmap pix(filePath);
                    ui->labelImage->setPixmap(
                        pix.scaled(ui->labelImage->size(),
                                   Qt::KeepAspectRatio,
                                   Qt::SmoothTransformation)
                        );

                    deleteOverlay->hide();
                }
            }
            return true;
        }
    }

    // -----------------------------
    // 2. Клик по overlay → удалить изображение
    // -----------------------------
    if (obj == deleteOverlay)
    {
        if (event->type() == QEvent::MouseButtonPress)
        {
            currentImagePath.clear();
            ui->labelImage->clear();
            ui->labelImage->setText("Загрузите изображение или перетащите файл📷");

            deleteOverlay->hide();
            return true;
        }
    }

    return QMainWindow::eventFilter(obj, event);


}


void TriallWindow::on_btnRecognize_4_clicked()
{

    if (attemptsLeft <= 0) {
        QMessageBox::warning(this, "Ошибка", "Попытки закончились.");
        return;
    }

    attemptsLeft--;
    updateAttemptsLabel();
    blockRecognitionIfNeeded();



    if (currentImagePath.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Сначала загрузите изображение.");
        return;
    }

    // Показываем анимацию сканера
    scanOverlay->show();

    // ОБНОВЛЯЕМ РАЗМЕР СЛОЯ
    scanOverlay->setFixedSize(ui->labelImage->size());
    scanOverlay->raise();

    // Полный сброс анимации (самое важное!)
    scanAnim->stop();

    // Сбрасываем позицию перед каждым запуском
    scanOverlay->setGeometry(
        0,
        -ui->labelImage->height(),
        ui->labelImage->width(),
        ui->labelImage->height()
        );


    int h = scanOverlay->height();

    scanOverlay->setGeometry(
        0,
        -h,
        scanOverlay->width(),
        h
        );

    scanAnim->setStartValue(QRect(0, -h,
                                  scanOverlay->width(),
                                  h));

    scanAnim->setEndValue(QRect(0, h,
                                scanOverlay->width(),
                                h));


    scanAnim->setDuration(5000);
    scanAnim->start();

    ui->textRecognized->setAlignment(Qt::AlignCenter);
    ui->textRecognized->setText("Распознавание...");


    QString lang = ui->comboLanguage->currentData().toString();
    if (lang == "all") {
        lang = "rus+eng+chi_sim+jpn+deu+spa+fra";
    }
    QString imagePath = currentImagePath;

    // ⭐ Вот здесь определяем PDF или изображение
    bool isPdf = imagePath.endsWith(".pdf", Qt::CaseInsensitive);

    // ⭐ Запускаем нужную функцию в отдельном потоке
    auto future = QtConcurrent::run([=]() {
        if (isPdf)
            return recognizePdf(imagePath, lang);
        else
            return recognizeImage(imagePath, lang);
    });

    //Когда поток завершится — обновляем UI
    auto watcher = new QFutureWatcher<QString>(this);
    connect(watcher, &QFutureWatcher<QString>::finished, this, [=]() {

        QString result = watcher->future().result();

        ui->textRecognized->setAlignment(Qt::AlignLeft);
        ui->textRecognized->setPlainText(result);

        ui->textRecognized->setPlainText(result);


        watcher->deleteLater();
    });

    watcher->setFuture(future);
}


// входим

void TriallWindow::on_btnLogin_clicked()
{
    LoginWindow *login = new LoginWindow(nullptr);
    if (login->exec() == QDialog::Accepted) {

        MainWindow *w = new MainWindow(login->getLogin());
        w->show();

        this->close();
    }
}



void TriallWindow::on_btnAbout_clicked()
{
    AboutWindow *about = new AboutWindow(this);
    about->setModal(true);
    about->exec();
}

void TriallWindow::on_btnBackToMain_clicked()
{
    WelcomWindow *w = new WelcomWindow();
    w->show();
    this->close();
}



void TriallWindow::translateWithAI(const QString &text, const QString &targetLang)
{
    QString apiKey = "YOUR_API_KEY_HERE"; // Получить ключ на openrouter.ai

    QNetworkAccessManager *mgr = new QNetworkAccessManager(this);

    QUrl url("https://openrouter.ai/api/v1/chat/completions");
    QNetworkRequest req(url);

    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    req.setRawHeader("Authorization", ("Bearer " + apiKey).toUtf8());
    req.setRawHeader("HTTP-Referer", "https://your-app.com");
    req.setRawHeader("X-Title", "TextRecognizer");

    // Формируем запрос к ИИ
    QString prompt = QString("Translate to %1:\n%2").arg(targetLang, text);

    QJsonObject message;
    message["role"] = "user";
    message["content"] = prompt;

    QJsonArray messages;
    messages.append(message);

    QJsonObject body;
    body["model"] = "nvidia/nemotron-3-super-120b-a12b:free";
    body["messages"] = messages;

    QNetworkReply *reply = mgr->post(req, QJsonDocument(body).toJson());

    connect(reply, &QNetworkReply::finished, this, [=]() {

        loadingAnim->stop();
        loadingBar->hide();


        if (reply->error() != QNetworkReply::NoError) {

            ui->textTranslate->setPlainText("Ошибка сети: " + reply->errorString());
            return;
        }

        QByteArray data = reply->readAll();
        QJsonDocument doc = QJsonDocument::fromJson(data);

        QString translated = doc["choices"][0]["message"]["content"].toString();

        ui->textTranslate->setPlainText(translated);
    });
}







void TriallWindow::on_btnTranslate_clicked()
{
    QString original = ui->textRecognized->toPlainText();
    if (original.isEmpty()) return;

    // ⭐ Показываем анимацию
    loadingBar->show();
    loadingAnim->stop();
    loadingAnim->setStartValue(QRect(0, 0, 0, 6));
    loadingAnim->setEndValue(QRect(0, 0, ui->textTranslate->width(), 6));
    loadingAnim->start();

    QString lang = ui->comboTranslate->currentData().toString();
    translateWithAI(original, lang);

}

void TriallWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->matches(QKeySequence::Paste)) {

        QClipboard *cb = QGuiApplication::clipboard();
        QImage img = cb->image();

        if (!img.isNull()) {

            QString tempPath = QDir::temp().filePath("trial_pasted_image.png");
            img.save(tempPath, "PNG");

            currentImagePath = tempPath;

            QPixmap pix = QPixmap::fromImage(img);
            ui->labelImage->setPixmap(
                pix.scaled(ui->labelImage->size(),
                           Qt::KeepAspectRatio,
                           Qt::SmoothTransformation)
                );

            deleteOverlay->setGeometry(ui->labelImage->geometry());
            deleteOverlay->raise();

            return;
        }
    }

    QMainWindow::keyPressEvent(event);
}
