#ifndef TRIALLWINDOW_H
#define TRIALLWINDOW_H

#include <QMainWindow>

#include <QLabel>
#include <QPropertyAnimation>
#include <QMovie>


namespace Ui {
class TriallWindow;
}

class TriallWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit TriallWindow(QWidget *parent = nullptr);
    ~TriallWindow();

private slots:
    void on_btnLoadImage_clicked();
    void on_btnRecognize_4_clicked();
    void on_btnLogin_clicked();
    void on_btnAbout_clicked();

    void on_btnBackToMain_clicked();

    void on_btnTranslate_clicked();

protected:
    void resizeEvent(QResizeEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;


private:
    Ui::TriallWindow *ui;


    int attemptsLeft = 15;
    QString currentImagePath;


    QString recognizeImage(const QString &imagePath, const QString &lang);  // функция распознавания

    void updateAttemptsLabel();
    void blockRecognitionIfNeeded();

    QLabel *scanOverlay;
    QPropertyAnimation *scanAnim;

    QLabel *loadingBar;
    QPropertyAnimation *loadingAnim;

    void translateWithAI(const QString &text, const QString &targetLang);
    QString recognizePdf(const QString &pdfPath, const QString &lang);
    void convertPdfToImages(const QString &pdfPath);

    void dragEnterEvent(QDragEnterEvent *event) override;

    bool eventFilter(QObject *obj, QEvent *event) override;

    QLabel *deleteOverlay;


    QLabel *glowFrame = nullptr;
    QLabel *glowRecognized = nullptr;     // для textRecognized
    QLabel *glowTranslate = nullptr;





};

#endif // TRIALLWINDOW_H
