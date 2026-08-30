#ifndef ABOUTWINDOW_H
#define ABOUTWINDOW_H

#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScrollArea>
#include <QWidget>
#include <QLabel>
#include <QFrame>
#include <QPushButton>

class AboutWindow : public QDialog
{
    Q_OBJECT

public:
    explicit AboutWindow(QWidget *parent = nullptr);
    ~AboutWindow();

private slots:
    void on_btnClose_clicked();

private:
    void setupModernUI();
    QFrame* createHelpCard(const QString &icon, const QString &title, const QString &description);
    void addFeatureList(QFrame *card, const QStringList &features);
    void addCompareBlock(QFrame *card, const QString &trialTitle, const QString &trialDesc,
                         const QString &fullTitle, const QString &fullDesc);
    void addTwoColumns(QFrame *card, const QString &col1Title, const QString &col1Text,
                       const QString &col2Title, const QString &col2Text);
    void addTipLabel(QFrame *card, const QString &tipText);

    QScrollArea *scrollArea;
    QWidget *contentWidget;
    QVBoxLayout *cardsLayout;
    QPushButton *closeBtn;
};

#endif // ABOUTWINDOW_H