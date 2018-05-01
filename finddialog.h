#ifndef FINDDIALOG_H
#define FINDDIALOG_H

#include <QDialog>
#include <QTextDocument>

class QCheckBox;
class QLabel;
class QLineEdit;
class QPushButton;

class FindDialog : public QDialog
{
    Q_OBJECT

public:
    FindDialog(QWidget *parent = 0);

signals:
    void findNext(const QString &str, QTextDocument::FindFlags flags);
    //void findPrevious(const QString &str, QTextDocument::FindFlag flags);

private slots:
    void findClicked();
    void enableFindButton(const QString &text);

private:
    QLabel *label;
    QLineEdit *lineEdit;
    QCheckBox *caseCheckBox;
    QCheckBox *backwardCheckBox;
    QPushButton *findButton;
    QPushButton *closeButton;
};

#endif // FINDDIALOG_H
