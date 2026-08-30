#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>


#include <QLabel>
#include <QPropertyAnimation>
#include <QMovie>


QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(const QString &user,QWidget *parent = nullptr);
    QString currentUser;

    ~MainWindow() override;

private slots:
    void on_btnProfile_clicked();
    void on_btnAbout_clicked();
    void on_btnHistory_clicked();
    void on_btnLoadImage_clicked();
    void on_btnRecognize_clicked();

    void on_btnBackToMain_clicked();

    void on_btnTranslate_clicked();

    void on_btnCopy_clicked();

    void on_btnCopyTranslate_clicked();

protected:
    void resizeEvent(QResizeEvent *event) override;

    void keyPressEvent(QKeyEvent *event) override;

    void dragEnterEvent(QDragEnterEvent *event) override;



private:
    Ui::MainWindow *ui;
    QString currentImagePath;

    QString loadUserProfileLang(const QString &user);
    QString profileLang;
    QString resolveTranslateLang();


    QString recognizeImage(const QString &imagePath, const QString &lang);  // функция распознавания
    void saveHistory(const QString &imagePath, const QString &text);

    bool eventFilter(QObject *obj, QEvent *event) override;


    void translateWithAI(const QString &text, const QString &targetLang);


    QLabel *scanOverlay;
    QPropertyAnimation *scanAnim;

    QLabel *loadingBar;
    QPropertyAnimation *loadingAnim;


    void convertPdfToImages(const QString &pdfPath);
    QString recognizePdf(const QString &pdfPath, const QString &lang);



    int lastHistoryId = -1;
    QLabel *glowFrame = nullptr;
    QLabel *glowRecognized = nullptr;     // для textRecognized
    QLabel *glowTranslate = nullptr;


    QLabel *deleteOverlay = nullptr;






};
#endif // MAINWINDOW_H
