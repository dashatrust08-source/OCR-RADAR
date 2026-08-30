#include "aboutwindow.h"
#include <QLabel>
#include <QPushButton>
#include <QFrame>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScrollArea>
#include <QScrollBar>

AboutWindow::AboutWindow(QWidget *parent)
    : QDialog(parent)
{
    setupModernUI();

    this->setWindowTitle("Справка · Гид по программе");
    this->setFixedSize(1000, 750);

    // Стиль диалога в стеклянном стиле
    this->setStyleSheet(
        "QDialog {"
        "   background: rgba(255, 255, 255, 0.85);"
        "   border-radius: 22px;"
        "   border: 1px solid rgba(255,255,255,0.45);"
        "   box-shadow: 0 12px 40px rgba(0,0,0,0.35);"
        "}"
        );

    if (parent) {
        this->move(parent->geometry().center() - this->rect().center());
    }
}

AboutWindow::~AboutWindow() {}

void AboutWindow::setupModernUI()
{
    // Главный layout
    QVBoxLayout *dialogLayout = new QVBoxLayout(this);
    dialogLayout->setContentsMargins(0, 0, 0, 0);
    dialogLayout->setSpacing(0);

    // ========== ШАПКА (стеклянный синий градиент) ==========
    QWidget *header = new QWidget();
    header->setFixedHeight(130);
    header->setStyleSheet(
        "QWidget {"
        "   background: qlineargradient(x1:0, y1:0, x2:1, y2:1,"
        "       stop:0 rgba(10, 25, 80, 0.65),"
        "       stop:1 rgba(30, 60, 110, 0.55));"
        "   border-radius: 22px 22px 0 0;"
        "   backdrop-filter: blur(12px);"
        "}"
        );

    QVBoxLayout *headerLayout = new QVBoxLayout(header);
    headerLayout->setContentsMargins(30, 25, 30, 20);

    QLabel *titleLabel = new QLabel("📘 Справка · Гид по программе");
    titleLabel->setStyleSheet(
        "color: white;"
        "font-size: 28px;"
        "font-weight: 800;"
        "font-family: 'Segoe UI';"
        "background: transparent;"
        );
    titleLabel->setAlignment(Qt::AlignCenter);

    QLabel *subtitleLabel = new QLabel("Как пользоваться приложением «OCR RADAR» — от загрузки до перевода и профиля");
    subtitleLabel->setStyleSheet(
        "color: #c9e9ff;"
        "font-size: 14px;"
        "font-family: 'Segoe UI';"
        "padding-top: 8px;"
        "background: transparent;"
        );
    subtitleLabel->setAlignment(Qt::AlignCenter);

    headerLayout->addWidget(titleLabel);
    headerLayout->addWidget(subtitleLabel);
    dialogLayout->addWidget(header);

    // ========== КНОПКА ЗАКРЫТИЯ ==========
    closeBtn = new QPushButton("✕", header);
    closeBtn->setFixedSize(40, 40);
    closeBtn->move(header->width() - 55, 15);
    closeBtn->setStyleSheet(
        "QPushButton {"
        "   background-color: rgba(255,255,255,0.2);"
        "   color: white;"
        "   border: 1px solid rgba(255,255,255,0.3);"
        "   border-radius: 20px;"
        "   font-size: 20px;"
        "   font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "   background-color: rgba(255,255,255,0.4);"
        "}"
        );
    connect(closeBtn, &QPushButton::clicked, this, &AboutWindow::close);

    // ========== SCROLL AREA (прозрачный) ==========
    scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);
    scrollArea->setStyleSheet(
        "QScrollArea {"
        "   background: transparent;"
        "   border: none;"
        "}"
        "QScrollBar:vertical {"
        "   background: rgba(200, 220, 250, 0.5);"
        "   width: 10px;"
        "   border-radius: 5px;"
        "}"
        "QScrollBar::handle:vertical {"
        "   background: rgba(100, 150, 200, 0.7);"
        "   border-radius: 5px;"
        "   min-height: 30px;"
        "}"
        "QScrollBar::handle:vertical:hover {"
        "   background: rgba(70, 120, 180, 0.9);"
        "}"
        "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {"
        "   border: none;"
        "   background: none;"
        "}"
        );

    contentWidget = new QWidget();
    contentWidget->setStyleSheet("background: transparent;");

    QVBoxLayout *mainLayout = new QVBoxLayout(contentWidget);
    mainLayout->setContentsMargins(40, 30, 40, 40);
    mainLayout->setSpacing(28);

    cardsLayout = new QVBoxLayout();
    cardsLayout->setSpacing(28);
    mainLayout->addLayout(cardsLayout);

    // ========== БЛОК 1: Начало работы ==========
    QFrame *card1 = createHelpCard("🚀", "Начало работы · Вход и регистрация",
                                   "При запуске вас встречает окно приветствия с двумя вариантами:");
    addFeatureList(card1, {
                              "🔓 Полная версия — нажмите «Войти / зарегистрироваться», создайте аккаунт. После регистрации вы получаете неограниченные попытки, историю, профиль и расширенные настройки.",
                              "✨ Пробная версия — доступна без регистрации, но с ограничением на 15 распознаваний."
                          });
    addTwoColumns(card1,
                  "✅ Полная версия",
                  "✔️ Безлимитное распознавание\n✔️ История всех действий\n✔️ Сохранение переводов в БД\n✔️ Профиль с аватаркой",
                  "⚡ Пробная версия",
                  "🔹 15 попыток распознавания\n🔹 Перевод без ограничений\n🔹 Нет истории и профиля");
    cardsLayout->addWidget(card1);

    // ========== БЛОК 2: Загрузка изображения ==========
    QFrame *card2 = createHelpCard("📂", "Загрузка изображения или PDF",
                                   "Чтобы распознать текст, сперва загрузите файл. Поддерживаются форматы:");
    addFeatureList(card2, {
                              "🖼️ Изображения — PNG, JPG, JPEG",
                              "📄 PDF-документы — первые 10 страниц"
                          });
    addTipLabel(card2, "✨ Два способа: кнопка «Выбрать файл» или перетащите файл в область изображения");
    cardsLayout->addWidget(card2);

    // ========== БЛОК 3: Распознавание ==========
    QFrame *card3 = createHelpCard("🔍", "Распознавание текста",
                                   "После загрузки выберите язык распознавания:");
    addFeatureList(card3, {
                              "Русский, English, 中文, 日本語, Deutsch, Español, Français",
                              "🌍 «Все языки» — автоматическое распознавание"
                          });
    addTipLabel(card3, "Нажмите «Распознать» — текст появится в поле результата");
    cardsLayout->addWidget(card3);

    // ========== БЛОК 4: Перевод ==========
    QFrame *card4 = createHelpCard("🌎", "Мгновенный перевод (AI)",
                                   "Нажмите «Перевести» — нейросеть переведёт текст.");
    addTwoColumns(card4,
                  "🎯 Выбор языка перевода",
                  "Выпадающий список «Язык перевода»\n«По умолчанию» — язык из профиля",
                  "⚙️ Настройка в профиле",
                  "Профиль → выберите язык → сохраните");
    addTipLabel(card4, "💬 Перевод сохраняется в историю. Пока идёт запрос — анимация загрузки");
    cardsLayout->addWidget(card4);

    // ========== БЛОК 5: История ==========
    QFrame *card5 = createHelpCard("📜", "История операций",
                                   "После распознавания всё сохраняется в базе данных.");
    addFeatureList(card5, {
                              "Таблица с миниатюрой, текстом и датой",
                              "Двойной клик — просмотр оригинала и перевода",
                              "Перевод заново — выберите другой язык",
                              "Очистить историю — удалить все записи"
                          });
    cardsLayout->addWidget(card5);

    // ========== БЛОК 6: Профиль ==========
    QFrame *card6 = createHelpCard("👤", "Профиль и настройки",
                                   "Кнопка «Профиль» на главной панели:");
    addFeatureList(card6, {
                              "Сменить аватар — загрузите фото (обрежется в круг)",
                              "Добавить информацию о себе",
                              "Установить язык перевода по умолчанию"
                          });
    cardsLayout->addWidget(card6);

    // ========== БЛОК 7: Сравнение версий ==========
    QFrame *card7 = createHelpCard("⚖️", "Пробная vs Полная версия", "");
    addCompareBlock(card7,
                    "🎲 Пробная версия",
                    "• 15 попыток распознавания\n• Перевод без лимитов\n• Нет истории\n• Нет профиля",
                    "⭐ Полная версия",
                    "• Неограниченные попытки\n• История и переводы\n• Профиль с фото\n• Настройки языка");
    cardsLayout->addWidget(card7);

    // ========== БЛОК 8: Частые вопросы ==========
    QFrame *card8 = createHelpCard("❓", "Частые вопросы", "");
    addFeatureList(card8, {
                              "Ошибка распознавания? Проверьте Tesseract OCR: C:/Program Files/Tesseract-OCR/",
                              "Не переводит? Проверьте интернет-соединение",
                              "Как удалить историю? История → Очистить историю",
                              "PDF не показывает превью? Распознавание работает корректно"
                          });
    cardsLayout->addWidget(card8);

    // ========== ФУТЕР ==========
    QLabel *footer = new QLabel("💙 «OCR RADAR» — интеллектуальное распознавание и перевод");
    footer->setStyleSheet("color: #3A4A5A; font-size: 11px; padding: 20px 0 10px 0; background: transparent;");
    footer->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(footer);

    scrollArea->setWidget(contentWidget);
    dialogLayout->addWidget(scrollArea);
}

QFrame* AboutWindow::createHelpCard(const QString &icon, const QString &title, const QString &description)
{
    QFrame *card = new QFrame();
    card->setObjectName("helpCard");
    card->setStyleSheet(
        "QFrame#helpCard {"
        "   background: rgba(255, 255, 255, 0.45);"
        "   border-radius: 26px;"
        "   border: 1px solid rgba(255, 255, 255, 0.55);"
        "   backdrop-filter: blur(8px);"
        "}"
        "QFrame#helpCard:hover {"
        "   background: rgba(255, 255, 255, 0.55);"
        "   border: 1px solid rgba(255, 255, 255, 0.7);"
        "}"
        );

    QVBoxLayout *cardLayout = new QVBoxLayout(card);
    cardLayout->setContentsMargins(28, 24, 28, 28);
    cardLayout->setSpacing(16);

    // Заголовок
    QHBoxLayout *titleLayout = new QHBoxLayout();
    titleLayout->setSpacing(12);

    QLabel *iconLabel = new QLabel(icon);
    iconLabel->setStyleSheet(
        "font-size: 32px;"
        "background: rgba(255,255,255,0.3);"
        "border-radius: 24px;"
        "padding: 8px;"
        "min-width: 48px;"
        "min-height: 48px;"
        );
    iconLabel->setAlignment(Qt::AlignCenter);

    QLabel *titleLabel = new QLabel(title);
    titleLabel->setStyleSheet(
        "font-size: 20px;"
        "font-weight: 600;"
        "color: #1E2A36;"
        "background: transparent;"
        );

    titleLayout->addWidget(iconLabel);
    titleLayout->addWidget(titleLabel);
    titleLayout->addStretch();
    cardLayout->addLayout(titleLayout);

    // Описание
    if (!description.isEmpty()) {
        QLabel *descLabel = new QLabel(description);
        descLabel->setStyleSheet("color: #2A3A48; font-size: 14px; background: transparent; margin-left: 8px;");
        descLabel->setWordWrap(true);
        cardLayout->addWidget(descLabel);
    }

    // Разделитель
    QFrame *line = new QFrame();
    line->setFrameShape(QFrame::HLine);
    line->setStyleSheet("background: rgba(100, 130, 160, 0.3); max-height: 1px;");
    cardLayout->addWidget(line);

    // Контентная область
    QWidget *contentArea = new QWidget();
    contentArea->setStyleSheet("background: transparent;");
    QVBoxLayout *contentLayout = new QVBoxLayout(contentArea);
    contentLayout->setContentsMargins(8, 0, 0, 0);
    contentLayout->setSpacing(12);
    cardLayout->addWidget(contentArea);

    return card;
}

void AboutWindow::addFeatureList(QFrame *card, const QStringList &features)
{
    QWidget *contentArea = qobject_cast<QWidget*>(card->layout()->itemAt(card->layout()->count() - 1)->widget());
    if (!contentArea) return;

    QVBoxLayout *contentLayout = qobject_cast<QVBoxLayout*>(contentArea->layout());
    if (!contentLayout) return;

    for (const QString &feature : features) {
        QHBoxLayout *itemLayout = new QHBoxLayout();
        itemLayout->setSpacing(12);

        QLabel *bullet = new QLabel("▹");
        bullet->setStyleSheet("color: #4A7A9E; font-size: 14px; font-weight: bold; background: transparent;");

        QLabel *text = new QLabel(feature);
        text->setStyleSheet("color: #2A3A48; font-size: 13px; background: transparent;");
        text->setWordWrap(true);

        itemLayout->addWidget(bullet);
        itemLayout->addWidget(text, 1);
        contentLayout->addLayout(itemLayout);
    }
}

void AboutWindow::addCompareBlock(QFrame *card, const QString &trialTitle, const QString &trialDesc,
                                  const QString &fullTitle, const QString &fullDesc)
{
    QWidget *contentArea = qobject_cast<QWidget*>(card->layout()->itemAt(card->layout()->count() - 1)->widget());
    if (!contentArea) return;

    QVBoxLayout *contentLayout = qobject_cast<QVBoxLayout*>(contentArea->layout());
    if (!contentLayout) return;

    QHBoxLayout *compareLayout = new QHBoxLayout();
    compareLayout->setSpacing(20);

    // Левая колонка
    QFrame *trialFrame = new QFrame();
    trialFrame->setStyleSheet(
        "QFrame {"
        "   background: rgba(255, 255, 255, 0.3);"
        "   border-radius: 20px;"
        "   border-left: 4px solid #E6A017;"
        "   padding: 12px;"
        "}"
        );
    QVBoxLayout *trialLayout = new QVBoxLayout(trialFrame);

    QLabel *trialTitleLabel = new QLabel(trialTitle);
    trialTitleLabel->setStyleSheet("font-weight: 600; font-size: 14px; color: #1E2A36; background: transparent;");

    QLabel *trialDescLabel = new QLabel(trialDesc);
    trialDescLabel->setStyleSheet("color: #2A3A48; font-size: 11px; background: transparent;");
    trialDescLabel->setWordWrap(true);

    trialLayout->addWidget(trialTitleLabel);
    trialLayout->addWidget(trialDescLabel);

    // Правая колонка
    QFrame *fullFrame = new QFrame();
    fullFrame->setStyleSheet(
        "QFrame {"
        "   background: rgba(255, 255, 255, 0.3);"
        "   border-radius: 20px;"
        "   border-left: 4px solid #4A7A9E;"
        "   padding: 12px;"
        "}"
        );
    QVBoxLayout *fullLayout = new QVBoxLayout(fullFrame);

    QLabel *fullTitleLabel = new QLabel(fullTitle);
    fullTitleLabel->setStyleSheet("font-weight: 600; font-size: 14px; color: #1E2A36; background: transparent;");

    QLabel *fullDescLabel = new QLabel(fullDesc);
    fullDescLabel->setStyleSheet("color: #2A3A48; font-size: 11px; background: transparent;");
    fullDescLabel->setWordWrap(true);

    fullLayout->addWidget(fullTitleLabel);
    fullLayout->addWidget(fullDescLabel);

    compareLayout->addWidget(trialFrame);
    compareLayout->addWidget(fullFrame);
    contentLayout->addLayout(compareLayout);
}

void AboutWindow::addTwoColumns(QFrame *card, const QString &col1Title, const QString &col1Text,
                                const QString &col2Title, const QString &col2Text)
{
    QWidget *contentArea = qobject_cast<QWidget*>(card->layout()->itemAt(card->layout()->count() - 1)->widget());
    if (!contentArea) return;

    QVBoxLayout *contentLayout = qobject_cast<QVBoxLayout*>(contentArea->layout());
    if (!contentLayout) return;

    QHBoxLayout *columnsLayout = new QHBoxLayout();
    columnsLayout->setSpacing(20);

    // Колонка 1
    QFrame *col1 = new QFrame();
    col1->setStyleSheet(
        "QFrame {"
        "   background: rgba(255, 255, 255, 0.25);"
        "   border-radius: 20px;"
        "   border: 1px solid rgba(255, 255, 255, 0.4);"
        "   padding: 12px;"
        "}"
        );
    QVBoxLayout *col1Layout = new QVBoxLayout(col1);

    QLabel *col1TitleLabel = new QLabel(col1Title);
    col1TitleLabel->setStyleSheet("font-weight: 600; color: #1E2A36; font-size: 13px; background: transparent;");

    QLabel *col1TextLabel = new QLabel(col1Text);
    col1TextLabel->setStyleSheet("color: #2A3A48; font-size: 11px; background: transparent;");
    col1TextLabel->setWordWrap(true);

    col1Layout->addWidget(col1TitleLabel);
    col1Layout->addWidget(col1TextLabel);

    // Колонка 2
    QFrame *col2 = new QFrame();
    col2->setStyleSheet(
        "QFrame {"
        "   background: rgba(255, 255, 255, 0.25);"
        "   border-radius: 20px;"
        "   border: 1px solid rgba(255, 255, 255, 0.4);"
        "   padding: 12px;"
        "}"
        );
    QVBoxLayout *col2Layout = new QVBoxLayout(col2);

    QLabel *col2TitleLabel = new QLabel(col2Title);
    col2TitleLabel->setStyleSheet("font-weight: 600; color: #1E2A36; font-size: 13px; background: transparent;");

    QLabel *col2TextLabel = new QLabel(col2Text);
    col2TextLabel->setStyleSheet("color: #2A3A48; font-size: 11px; background: transparent;");
    col2TextLabel->setWordWrap(true);

    col2Layout->addWidget(col2TitleLabel);
    col2Layout->addWidget(col2TextLabel);

    columnsLayout->addWidget(col1);
    columnsLayout->addWidget(col2);
    contentLayout->addLayout(columnsLayout);
}

void AboutWindow::addTipLabel(QFrame *card, const QString &tipText)
{
    QWidget *contentArea = qobject_cast<QWidget*>(card->layout()->itemAt(card->layout()->count() - 1)->widget());
    if (!contentArea) return;

    QVBoxLayout *contentLayout = qobject_cast<QVBoxLayout*>(contentArea->layout());
    if (!contentLayout) return;

    QLabel *tip = new QLabel(tipText);
    tip->setStyleSheet(
        "color: #2A3A48;"
        "background: rgba(100, 150, 200, 0.15);"
        "padding: 10px 14px;"
        "border-radius: 20px;"
        "font-size: 12px;"
        );
    tip->setWordWrap(true);
    contentLayout->addWidget(tip);
}

void AboutWindow::on_btnClose_clicked()
{
    this->close();
}