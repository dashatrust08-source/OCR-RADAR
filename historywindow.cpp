#include "historywindow.h"
#include "ui_historywindow.h"

#include <QFile>
#include <QMessageBox>
#include <QLabel>
#include <QPixmap>
#include <QDesktopServices>
#include <QUrl>
#include <QSqlQuery>
#include <QSqlError>
#include <QHeaderView>
#include <QTextEdit>


#include <QComboBox>
#include <QNetworkReply>

#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonValue>

#include <QMovie>


HistoryWindow::HistoryWindow(const QString &user, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::HistoryWindow)
    , currentUser(user)
{

    ui->setupUi(this);
    this->setWindowTitle("История");

    this->setFixedSize(850, 600);


    bg = new AnimatedBackground(this);
    bg->setGeometry(this->rect());
    bg->lower();


    // Растягиваем таблицу на всё окно
    ui->tableHistory->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    // Высота строк
    ui->tableHistory->verticalHeader()->setDefaultSectionSize(80);

    // Перенос строк
    ui->tableHistory->setWordWrap(true);

    // Запрет редактирования
    ui->tableHistory->setEditTriggers(QAbstractItemView::NoEditTriggers);

    // Выделение всей строки
    ui->tableHistory->setSelectionBehavior(QAbstractItemView::SelectRows);

    // Красивый стиль
    ui->tableHistory->setStyleSheet(
        "QTableWidget {"
        "   font-size: 14px;"
        "   gridline-color: #cccccc;"
        "}"
        "QHeaderView::section {"
        "   background-color: #f0f0f0;"
        "   padding: 6px;"
        "   font-weight: bold;"
        "   border: 1px solid #d0d0d0;"
        "}"
        );

    // Двойной клик — открыть изображение
    connect(ui->tableHistory, &QTableWidget::cellDoubleClicked,
            this, [=](int row, int column){
                if (column == 1)  // колонка текста
                    onTextOpenRequested(row, column);
                else
                    onImageOpenRequested(row, column);
            });


    loadHistory();
}

HistoryWindow::~HistoryWindow()
{
    delete ui;
}

void HistoryWindow::loadHistory()
{
    ui->tableHistory->setRowCount(0);
    ui->tableHistory->setColumnCount(3);
    ui->tableHistory->setHorizontalHeaderLabels({"Изображение", "Текст", "Дата"});

    QSqlQuery query;
    query.prepare("SELECT id, image_path, recognized_text, translated_text, timestamp "
                  "FROM history WHERE user = :user ORDER BY id DESC");
    query.bindValue(":user", currentUser);
    query.exec();

    int row = 0;

    while (query.next()) {
        int id = query.value(0).toInt();
        QString imagePath = query.value(1).toString();
        QString text = query.value(2).toString();
        QString translated = query.value(3).toString();
        QString date = query.value(4).toString();

        ui->tableHistory->insertRow(row);

        // 0 — изображение (иконка + путь + перевод в data)
        QPixmap pix(imagePath);
        QPixmap thumb = pix.scaled(70, 70, Qt::KeepAspectRatio, Qt::SmoothTransformation);

        QTableWidgetItem *imgItem = new QTableWidgetItem();
        imgItem->setIcon(QIcon(thumb));
        imgItem->setData(Qt::UserRole, imagePath);      // путь к картинке
        imgItem->setData(Qt::UserRole + 1, translated); // перевод
        imgItem->setData(Qt::UserRole + 2, id);         // id записи
        ui->tableHistory->setItem(row, 0, imgItem);

        // 1 — текст
        QTableWidgetItem *textItem = new QTableWidgetItem(text);
        textItem->setTextAlignment(Qt::AlignLeft | Qt::AlignTop);
        ui->tableHistory->setItem(row, 1, textItem);

        // 2 — дата
        ui->tableHistory->setItem(row, 2, new QTableWidgetItem(date));

        row++;
    }
}


void HistoryWindow::onTextOpenRequested(int row, int column)
{
    QString original = ui->tableHistory->item(row, 1)->text();
    QString translated =
        ui->tableHistory->item(row, 0)->data(Qt::UserRole + 1).toString();
    int historyId =
        ui->tableHistory->item(row, 0)->data(Qt::UserRole + 2).toInt();

    QDialog *dlg = new QDialog(this);
    dlg->setWindowTitle("Текст");
    dlg->resize(750, 650);

    QVBoxLayout *layout = new QVBoxLayout(dlg);

    // Оригинал
    layout->addWidget(new QLabel("Оригинал:"));
    QTextEdit *origEdit = new QTextEdit();
    origEdit->setPlainText(original);
    origEdit->setReadOnly(true);
    layout->addWidget(origEdit);

    // Перевод
    layout->addWidget(new QLabel("Перевод:"));
    QTextEdit *transEdit = new QTextEdit();
    transEdit->setPlainText(translated);
    transEdit->setReadOnly(true);
    layout->addWidget(transEdit);

    // === OVERLAY ДЛЯ СПИННЕРА ===
    // overlay поверх transEdit
    overlay = new QWidget(transEdit);
    overlay->setStyleSheet("background-color: transparent;");
    overlay->setAttribute(Qt::WA_TransparentForMouseEvents);
    overlay->resize(transEdit->size());
    overlay->hide();

    // Спиннер (всегда большой)
    spinner = new QLabel(overlay);
    spinner->setAlignment(Qt::AlignCenter);
    spinner->resize(120, 50);

    spinnerMovie = new QMovie(":/load/loading.gif");
    spinnerMovie->setScaledSize(QSize(120, 50));
    spinner->setMovie(spinnerMovie);

    // Центрируем спиннер
    spinner->move(
        (overlay->width() - spinner->width()) / 2,
        (overlay->height() - spinner->height()) / 2
        );






    spinnerMovie = new QMovie(":/load/loading.gif");
    spinnerMovie->setScaledSize(QSize(48, 20));

    spinner->setMovie(spinnerMovie);

    layout->addWidget(spinner, 0, Qt::AlignCenter);

    // Блок перевода
    QHBoxLayout *translateLayout = new QHBoxLayout();

    QLabel *lbl = new QLabel("Перевести на:");
    translateLayout->addWidget(lbl);

    QComboBox *combo = new QComboBox();
    combo->addItem("Русский", "rus");
    combo->addItem("Английский", "eng");
    combo->addItem("Китайский", "chi_sim");
    combo->addItem("Японский", "jpn");
    combo->addItem("Немецкий", "deu");
    combo->addItem("Испанский", "spa");
    combo->addItem("Французский", "fra");
    translateLayout->addWidget(combo);

    QPushButton *btnTranslate = new QPushButton("Перевести");
    translateLayout->addWidget(btnTranslate);

    layout->addLayout(translateLayout);

    // Кнопка закрытия
    QPushButton *closeBtn = new QPushButton("Закрыть");
    layout->addWidget(closeBtn);

    connect(closeBtn, &QPushButton::clicked, dlg, &QDialog::close);

    // ЛОГИКА ПЕРЕВОДА
    connect(btnTranslate, &QPushButton::clicked, this, [=]() {

        overlay->resize(transEdit->size());
        overlay->show();
        spinnerMovie->start();

        // ВСЕГДА большой спиннер
        spinnerMovie->setScaledSize(QSize(120, 50));
        spinner->resize(120, 50);
        spinner->move(
            (overlay->width() - spinner->width()) / 2,
            (overlay->height() - spinner->height()) / 2
            );






        QString lang = combo->currentData().toString();
        QString textToTranslate = original;

        // --- ВЫЗОВ ПЕРЕВОДА ---
        QNetworkAccessManager *mgr = new QNetworkAccessManager(dlg);

        QUrl url("https://openrouter.ai/api/v1/chat/completions");
        QNetworkRequest req(url);

        req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
        req.setRawHeader("Authorization", "Bearer YOUR-API-KEY");  // Получить ключ на openrouter.ai
        req.setRawHeader("HTTP-Referer", "https://your-app.com");
        req.setRawHeader("X-Title", "TextRecognizer");

        QString prompt = QString("Translate to %1:\n%2").arg(lang, textToTranslate);

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
            spinnerMovie->stop();
            overlay->hide();


            if (reply->error() != QNetworkReply::NoError) {
                QMessageBox::warning(dlg, "Ошибка", reply->errorString());
                return;
            }

            QByteArray data = reply->readAll();
            QJsonDocument doc = QJsonDocument::fromJson(data);
            QString newTranslated = doc["choices"][0]["message"]["content"].toString();

            // Обновляем окно
            transEdit->setPlainText(newTranslated);

            // Сохраняем в БД
            QSqlQuery q;
            q.prepare("UPDATE history SET translated_text = :t WHERE id = :id");
            q.bindValue(":t", newTranslated);
            q.bindValue(":id", historyId);
            q.exec();

            // Обновляем таблицу
            ui->tableHistory->item(row, 0)->setData(Qt::UserRole + 1, newTranslated);
        });
    });

    dlg->setAttribute(Qt::WA_DeleteOnClose);
    dlg->show();
}




void HistoryWindow::onImageOpenRequested(int row, int column)
{
    QString imagePath = ui->tableHistory->item(row, 0)->data(Qt::UserRole).toString();

    if (!QFile::exists(imagePath)) {
        QMessageBox::warning(this, "Ошибка", "Файл изображения не найден.");
        return;
    }

    QPixmap pix(imagePath);

    QLabel *label = new QLabel;
    label->setPixmap(pix.scaled(700, 700, Qt::KeepAspectRatio));
    label->setWindowTitle("Просмотр изображения");
    label->setAttribute(Qt::WA_DeleteOnClose);
    label->show();
}

void HistoryWindow::on_btnOpenImage_clicked()
{
    int row = ui->tableHistory->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Ошибка", "Выберите запись.");
        return;
    }

    QString imagePath = ui->tableHistory->item(row, 0)->data(Qt::UserRole).toString();
    QDesktopServices::openUrl(QUrl::fromLocalFile(imagePath));
}

void HistoryWindow::on_btnCleanHistory_clicked()
{
    if (QMessageBox::question(this,
                              "Очистить историю",
                              "Вы уверены, что хотите удалить всю историю?",
                              QMessageBox::Yes | QMessageBox::No)
        != QMessageBox::Yes)
        return;

    QSqlQuery query;
    query.prepare("DELETE FROM history WHERE user = :user");
    query.bindValue(":user", currentUser);

    if (!query.exec()) {
        QMessageBox::warning(this, "Ошибка",
                             "Не удалось очистить историю:\n" + query.lastError().text());
        return;
    }

    // Очистить таблицу в интерфейсе
    ui->tableHistory->setRowCount(0);
}

void HistoryWindow::resizeEvent(QResizeEvent *event)
{
    QDialog::resizeEvent(event);
    if (bg)
        bg->setGeometry(this->rect());
}

