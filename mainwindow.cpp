#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "welcomwindow.h"
#include "profilewindow.h"
#include "aboutwindow.h"
#include "historywindow.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QPixmap>
#include <QProcess>
#include <QSqlQuery>

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

#include <QGraphicsDropShadowEffect>



MainWindow::MainWindow(const QString &user,QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->labelImage->setFocusPolicy(Qt::StrongFocus);
    ui->labelImage->setAttribute(Qt::WA_InputMethodEnabled, true);

    this->setFocusPolicy(Qt::StrongFocus);
    this->setFocus();


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


    // Создаём overlay поверх labelImage
    deleteOverlay = new QLabel(ui->labelImage->parentWidget());
    deleteOverlay->setGeometry(ui->labelImage->geometry());
    deleteOverlay->setStyleSheet(
        "background: rgba(0,0,0,120);"
        "color: white;"
        "font-size: 20px;"
        "border-radius: 10px;"
        "border: 2px solid #00aaff;"
        );
    deleteOverlay->setAlignment(Qt::AlignCenter);
    deleteOverlay->setText("Удалить картинку");
    deleteOverlay->hide(); // скрыт по умолчанию

    // Чтобы overlay был поверх картинки
    deleteOverlay->raise();

    // Разрешаем клики
    deleteOverlay->setAttribute(Qt::WA_TransparentForMouseEvents, false);



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


    //для наведения на картинку для ее удаления
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
    deleteOverlay->hide();      // скрыто по умолчанию
    deleteOverlay->raise();     // поверх labelImage

    // overlay должен принимать клики
    deleteOverlay->setAttribute(Qt::WA_TransparentForMouseEvents, false);

    // регистрируем overlay в фильтре событий
    deleteOverlay->installEventFilter(this);

    // регистрируем labelImage в фильтре событий
    ui->labelImage->installEventFilter(this);



    this->setWindowTitle("Приложение для распознавания текста");
    ui->scrollAreaWidgetContents->setMinimumHeight(2000);
    ui->frameMainCard->setMinimumHeight(1600);

    setAcceptDrops(true);


    ui->labelImage->setAcceptDrops(true);
    ui->labelImage->installEventFilter(this);


    currentUser = user;
    profileLang = loadUserProfileLang(currentUser);
    qDebug() << "Profile language =" << profileLang;

    qDebug() << "MainWindow: currentUser =" << currentUser;

    this->showMaximized();
    qDebug() << "MAIN WINDOW SHOWN";
    ui->comboLanguage->addItem("Все языки", "all");
    ui->comboLanguage->addItem("Русский", "rus");
    ui->comboLanguage->addItem("Английский", "eng");
    ui->comboLanguage->addItem("Китайский", "chi_sim");
    ui->comboLanguage->addItem("Японский", "jpn");
    ui->comboLanguage->addItem("Немецкий","deu");
    ui->comboLanguage->addItem("Испанский","spa");
    ui->comboLanguage->addItem("Французский","fra");


    ui->comboTranslate->addItem("По умолчанию", profileLang);
    ui->comboTranslate->addItem("Русский", "rus");
    ui->comboTranslate->addItem("Английский", "eng");
    ui->comboTranslate->addItem("Китайский", "chi_sim");
    ui->comboTranslate->addItem("Японский", "jpn");
    ui->comboTranslate->addItem("Немецкий","deu");
    ui->comboTranslate->addItem("Испанский","spa");
    ui->comboTranslate->addItem("Французский","fra");


}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_btnProfile_clicked()
{
    ProfileWindow *p = new ProfileWindow(currentUser, this);
    p->setModal(true);
    p->exec();
}

void MainWindow::on_btnAbout_clicked()
{
    AboutWindow *a = new AboutWindow(this);
    a->setModal(true);
    a->exec();
}

void MainWindow::on_btnHistory_clicked()
{
    HistoryWindow *h = new HistoryWindow(currentUser, this);
    h->setModal(true);
    h->exec();
}


QString MainWindow::recognizeImage(const QString &imagePath, const QString &lang)
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

void MainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);
    if (deleteOverlay)
        deleteOverlay->setGeometry(ui->labelImage->geometry());

    if (glowFrame)
        glowFrame->setGeometry(ui->labelImage->geometry());

    if (glowRecognized)
        glowRecognized->setGeometry(ui->textRecognized->geometry());

    if (glowTranslate)
        glowTranslate->setGeometry(ui->textTranslate->geometry());


    if (!currentImagePath.isEmpty()) {
        QPixmap pix(currentImagePath);
        ui->labelImage->setPixmap(
            pix.scaled(ui->labelImage->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation)
            );
    }
}


void MainWindow::on_btnLoadImage_clicked()
{
    QString file = QFileDialog::getOpenFileName(this, "Выберите файл", "", "Images (*.png *.jpg *.jpeg *.pdf)");
    if (file.isEmpty()) return;

    currentImagePath = file;

    ui->textRecognized->setPlainText("Загружен файл: " + QFileInfo(file).fileName());

    // Если PDF — не показываем картинку
    if (file.endsWith(".pdf", Qt::CaseInsensitive)) {
        ui->labelImage->setText("PDF файл загружен");
        return;
    }

    QPixmap pix(file);

    ui->labelImage->setPixmap(
        pix.scaled(ui->labelImage->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation)
        );

}

void MainWindow::saveHistory(const QString &imagePath, const QString &text)
{
    QSqlQuery query;
    query.prepare("INSERT INTO history (user, image_path, recognized_text) "
                  "VALUES (:user, :image, :text)");
    query.bindValue(":user", currentUser);
    query.bindValue(":image", imagePath);
    query.bindValue(":text", text);
    query.exec();

    // ← ВАЖНО: получаем ID вставленной записи
    query.exec("SELECT last_insert_rowid()");
    if (query.next())
        lastHistoryId = query.value(0).toInt();

    QSqlQuery cleanup;
    cleanup.exec(
        "DELETE FROM history WHERE id NOT IN ("
        "SELECT id FROM history WHERE user = '" + currentUser + "' "
                        "ORDER BY id DESC LIMIT 30"
                        ");"
        );
}



void MainWindow::convertPdfToImages(const QString &pdfPath)
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

QString MainWindow::recognizePdf(const QString &pdfPath, const QString &lang)
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


void MainWindow::on_btnRecognize_clicked()
{
    if (currentImagePath.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Сначала загрузите изображение.");
        return;
    }




    ui->labelImage->setStyleSheet(
        "color: white;"
        "font-size: 20px;"
        "font-weight: 600;"
        );


    // ⭐ Очищаем поле перевода при новом распознавании
    ui->textTranslate->clear();

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

        if (!result.startsWith("Ошибка"))
            saveHistory(imagePath, result);

        watcher->deleteLater();
    });

    watcher->setFuture(future);

}


QString MainWindow::loadUserProfileLang(const QString &user)
{
    QSqlQuery q;
    q.prepare("SELECT translate_lang FROM users WHERE login = :login");
    q.bindValue(":login", user);
    q.exec();

    if (q.next()) {
        QString lang = q.value(0).toString();
        if (!lang.isEmpty())
            return lang;
    }

    return "rus"; // по умолчанию
}

void MainWindow::translateWithAI(const QString &text, const QString &targetLang)
{
    QString apiKey = ""YOUR_API_KEY_HERE"; // Получить ключ на openrouter.ai

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

        if (lastHistoryId != -1) {
            QSqlQuery q;
            q.prepare("UPDATE history SET translated_text = :t WHERE id = :id");
            q.bindValue(":t", translated);
            q.bindValue(":id", lastHistoryId);
            q.exec();
        }

    });
}



QString MainWindow::resolveTranslateLang()
{
    QString selected = ui->comboTranslate->currentData().toString();

    // 1. Если выбран НЕ "По умолчанию"
    if (selected != profileLang)
        return selected;

    // 2. Если есть язык профиля
    if (!profileLang.isEmpty())
        return profileLang;

    // 3. По умолчанию русский
    return "rus";
}


void MainWindow::on_btnTranslate_clicked()
{
    QString original = ui->textRecognized->toPlainText();
    if (original.isEmpty()) return;

    loadingBar->show();
    loadingAnim->stop();
    loadingAnim->setStartValue(QRect(0, 0, 0, 6));
    loadingAnim->setEndValue(QRect(0, 0, ui->frameMainCard->width(), 6));
    loadingAnim->start();


    QString lang = resolveTranslateLang();
    translateWithAI(original, lang);




}

void MainWindow::on_btnBackToMain_clicked()
{
    WelcomWindow *w = new WelcomWindow();
    w -> show();
    this -> close();
}




void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasUrls())
        event->acceptProposedAction();
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    // ----------------------------------------------------
    // 1. Наведение на labelImage
    // ----------------------------------------------------
    if (obj == ui->labelImage)
    {
        // Показать overlay при наведении
        if (event->type() == QEvent::Enter)
        {
            if (!currentImagePath.isEmpty())
                deleteOverlay->show();
            return true;
        }

        // Скрыть overlay при уходе
        if (event->type() == QEvent::Leave)
        {
            deleteOverlay->hide();
            return true;
        }

        // DragEnter — навели файл
        if (event->type() == QEvent::DragEnter)
        {
            auto *dragEvent = static_cast<QDragEnterEvent*>(event);
            if (dragEvent->mimeData()->hasUrls())
                dragEvent->acceptProposedAction();
            return true;
        }


        if (event->type() == QEvent::MouseButtonPress)
        {
            ui->labelImage->setFocus();
            return false;
        }



        // Drop — бросили файл
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

        return false;
    }

    // ----------------------------------------------------
    // 2. Клик по overlay → удалить картинку
    // ----------------------------------------------------
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

        return false;
    }




    return QMainWindow::eventFilter(obj, event);
}


void MainWindow::on_btnCopy_clicked()
{
    QString text = ui->textRecognized->toPlainText();
    if (text.isEmpty())
        return;

    QClipboard *clipboard = QGuiApplication::clipboard();
    clipboard->setText(text);

    ui->statusbar->showMessage("Распознанный текст скопирован", 2000);
}


void MainWindow::on_btnCopyTranslate_clicked()
{
    QString text = ui->textTranslate->toPlainText();
    if (text.isEmpty())
        return;

    QClipboard *clipboard = QGuiApplication::clipboard();
    clipboard->setText(text);

    ui->statusbar->showMessage("Перевод скопирован", 2000);
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->matches(QKeySequence::Paste)) {

        QClipboard *cb = QGuiApplication::clipboard();
        QImage img = cb->image();

        if (!img.isNull()) {

            // ⭐ 1. Сохраняем скриншот во временный файл
            QString tempPath = QDir::temp().filePath("pasted_image.png");
            img.save(tempPath, "PNG");

            // ⭐ 2. Запоминаем путь, чтобы распознавание работало
            currentImagePath = tempPath;

            // ⭐ 3. Показываем картинку
            QPixmap pix = QPixmap::fromImage(img);
            ui->labelImage->setPixmap(
                pix.scaled(ui->labelImage->size(),
                           Qt::KeepAspectRatio,
                           Qt::SmoothTransformation)
                );

            deleteOverlay->hide();
            return;
        }
    }

    QMainWindow::keyPressEvent(event);
}
