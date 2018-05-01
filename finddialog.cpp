#include <QCheckBox>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>

#include "finddialog.h"

FindDialog::FindDialog(QWidget *parent)
    :QDialog(parent)
{
    label = new QLabel(tr("Encontrar o &que?"));
    lineEdit = new QLineEdit;
    label->setBuddy(lineEdit);

    caseCheckBox = new QCheckBox(tr("Diferenciar &maiúsculas de minúsculas"));
    backwardCheckBox = new QCheckBox(tr("Pesquisar para &trás"));

    findButton = new QPushButton(tr("&Encontrar"));
    findButton->setDefault(true);
    findButton->setEnabled(false);

    closeButton = new QPushButton(tr("Fechar"));

    connect(lineEdit, SIGNAL(textChanged(QString)),
            this, SLOT(enableFindButton(QString)));
    connect(findButton, SIGNAL(clicked(bool)),
            this, SLOT(findClicked()));
    connect(closeButton, SIGNAL(clicked(bool)),
            this, SLOT(close()));

    QHBoxLayout *topLeftLayout = new QHBoxLayout;
    topLeftLayout->addWidget(label);
    topLeftLayout->addWidget(lineEdit);

    QVBoxLayout *leftLayout = new QVBoxLayout;
    leftLayout->addLayout(topLeftLayout);
    leftLayout->addWidget(caseCheckBox);
    leftLayout->addWidget(backwardCheckBox);

    QVBoxLayout *rightLayout = new QVBoxLayout;
    rightLayout->addWidget(findButton);
    rightLayout->addWidget(closeButton);
    rightLayout->addStretch();

    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->addLayout(leftLayout);
    mainLayout->addLayout(rightLayout);
    setLayout(mainLayout);

    setWindowTitle(tr("Localizar"));
    setFixedHeight(sizeHint().height());
}

void FindDialog::findClicked()
{
    QString texto = lineEdit->text();

    QTextDocument::FindFlags flags;
    if (caseCheckBox->isChecked())
        flags = QTextDocument::FindCaseSensitively;
    if (backwardCheckBox->isChecked())
        flags |= QTextDocument::FindBackward;
    //flags = cs | bw;
    emit findNext(texto, flags);
}

void FindDialog::enableFindButton(const QString &text)
{
    findButton->setEnabled(!text.isEmpty());
}
