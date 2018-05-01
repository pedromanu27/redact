#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "finddialog.h"

#include <QMessageBox>
#include <QFileDialog>
#include <QCloseEvent>
#include <QSettings>
#include <QTextStream>
#include <QtPrintSupport/QPrinter>
#include <QPrintDialog>
#include <QFontDialog>
#include <QColorDialog>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setAttribute(Qt::WA_DeleteOnClose);
    setCentralWidget(ui->textEdit);
    connect(ui->textEdit, SIGNAL(textChanged()), this,
            SLOT(foiModificado()));

    criarArquivosRecentActions();

    findDialog = 0;
    carregarConfiguracoes();
    definirArquivoAtual("");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (continuar()) {
        gravarConfiguracoes();
        event->accept();
    } else {
        event->ignore();
    }
}

void MainWindow::criarArquivosRecentActions()
{
    for (int i = 0; i < MaximoDeArquivosRecente; ++i) {
        arquivosRecenteActions[i] = new QAction(this);
        arquivosRecenteActions[i]->setVisible(false);
        connect(arquivosRecenteActions[i], SIGNAL(triggered(bool)),
                this, SLOT(abrirAquivoRecente()));
    }
    menuArquivosRecente = ui->menuArquivosRecente;
    for (int i = 0; i < MaximoDeArquivosRecente; ++i)
        menuArquivosRecente->addAction(arquivosRecenteActions[i]);
}

bool MainWindow::continuar()
{
    if (isWindowModified()) {
        int resposta = QMessageBox::warning(this, tr("Redact"),
                                            tr("O documento foi modificado.\n"
                                               "Você quer salvar suas mudanças?"),
                                            QMessageBox::Yes | QMessageBox::No
                                            | QMessageBox::Cancel);
        if (resposta == QMessageBox::Yes) {
            return salvar();
        } else if (resposta == QMessageBox::Cancel){
            return false;
        }
    }
    return true;
}

bool MainWindow::salvar()
{
    if (arquivoAtual.isEmpty()) {
        return salvarComo();
    } else {
        return salvarArquivo(arquivoAtual);
    }
}

bool MainWindow::salvarArquivo(const QString &nome)
{
    QFile file(nome);
    file.open(QIODevice::WriteOnly | QIODevice::Text);
    if (!file.isOpen()) {return false; }
    QTextStream str(&file);
    QString texto = ui->textEdit->toPlainText();
    str << texto;
    file.flush();
    file.close();

    definirArquivoAtual(nome);
    statusBar()->showMessage(tr("Arquivo salvado"), 2000);
    return true;
}

bool MainWindow::salvarComo()
{
    QString nome = QFileDialog::getSaveFileName(this,
                                                tr("Salvar documento"), tr("Sem título.txt"),
                                                tr("Documento de texto (*.txt)"
                                                   ";;Todos os arquivos (*.*)"));
    if (nome.isEmpty())
        return false;
    return salvarArquivo(nome);
}

void MainWindow::carregarArquivo(const QString &nome)
{
    QFile file(nome);
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    if (!file.isOpen()) {return; }
    QTextStream str(&file);
    QString texto = str.readAll();
    file.flush();
    file.close();
    ui->textEdit->setText(texto);

    definirArquivoAtual(nome);
    statusBar()->showMessage(tr("Arquivo carregado"), 2000);
}

void MainWindow::definirArquivoAtual(const QString &nome)
{
    arquivoAtual = nome;
    setWindowModified(false);
    QString nomeMostrado = tr("Sem título");
    if (!arquivoAtual.isEmpty()) {
        nomeMostrado = enxugarNome(arquivoAtual);
        arquivosRecente.removeAll(arquivoAtual);
        arquivosRecente.prepend(arquivoAtual);

        //atualizarArquivosRecenteActions();
        foreach (QWidget *win, QApplication::topLevelWidgets()) {
            if (MainWindow *mainWin = qobject_cast<MainWindow *>(win))
                mainWin->atualizarArquivosRecenteActions();
        }
    }
    setWindowTitle(tr("%1[*] - %2").arg(nomeMostrado)
                   .arg(tr("Redact")));
}

QString MainWindow::enxugarNome(const QString &nomeCompleto) const
{
    return QFileInfo(nomeCompleto).fileName();
}

void MainWindow::gravarConfiguracoes()
{
    QSettings settings("Pedro Santana", "Redact");

    settings.setValue("geometria", saveGeometry());
    settings.setValue("arquivosRecente", arquivosRecente);
}

void MainWindow::carregarConfiguracoes()
{
    QSettings settings("Pedro Santana", "Redact");

    restoreGeometry(settings.value("geometria").toByteArray());

    arquivosRecente = settings.value("arquivosRecente").toStringList();
    atualizarArquivosRecenteActions();
}

void MainWindow::atualizarArquivosRecenteActions()
{
    QMutableStringListIterator i(arquivosRecente);
    while (i.hasNext()) {
        if (!QFile::exists(i.next()))
            i.remove();
    }

    for (int j = 0; j < MaximoDeArquivosRecente; ++j) {
        if (j < arquivosRecente.count()) {
            QString texto = tr("&%1 %2")
                    .arg(j + 1)
                    .arg(enxugarNome(arquivosRecente[j]));
            arquivosRecenteActions[j]->setText(texto);
            arquivosRecenteActions[j]->setData(arquivosRecente[j]);
            arquivosRecenteActions[j]->setVisible(true);
        } else {
            arquivosRecenteActions[j]->setVisible(false);
        }
    }
}

void MainWindow::abrirAquivoRecente()
{
    if (continuar()) {
        QAction *action = qobject_cast<QAction *>(sender());
        if (action)
            carregarArquivo(action->data().toString());
    }
}

void MainWindow::on_actionLocalizar_triggered()
{
    if (!findDialog) {
        findDialog = new FindDialog(this);
        connect(findDialog, SIGNAL(findNext(QString,QTextDocument::FindFlags)),
                this, SLOT(localizar(QString, QTextDocument::FindFlags)));
    }

    if (findDialog->isHidden()) {
        findDialog->show();
    } else {
        findDialog->raise();
        findDialog->activateWindow();
    }
}

void MainWindow::on_actionNovo_triggered()
{
    MainWindow *mainWin = new MainWindow;
    mainWin->show();
}

void MainWindow::on_actionAbrir_triggered()
{
    if (continuar()) {
        QString nome = QFileDialog::getOpenFileName(this,
                                                    tr("Abrir Documento"), ".",
                                                    tr("Documento de texto (*.txt)\n"
                                                       "Todos os arquivos (*.*)"));
        if (!nome.isEmpty()){
            carregarArquivo(nome);
        }
    }
}

void MainWindow::on_actionSobre_triggered()
{
    QMessageBox::about(this, tr("Sobre o Redact"),
                       tr("<h2>Redact 1.0</h2>"
                          "<p>Direito autoral &copy; 2018 Pedro Santana."
                          "<p>Redact é um pequeno aplicativo de "
                          "edição de texto"));
}

void MainWindow::on_actionSair_triggered()
{
    close();
}


void MainWindow::on_actionSalvar_triggered()
{
    salvar();
}

void MainWindow::on_actionSalvarComo_triggered()
{
    salvarComo();
}

void MainWindow::foiModificado()
{
    setWindowModified(true);
}

void MainWindow::on_actionFonte_triggered()
{
    bool ok = true;
    QFont fonte = QFontDialog::getFont(&ok, this);
    if (!ok) {return; }
    ui->textEdit->setCurrentFont(fonte);
}

void MainWindow::on_actionCorDaFonte_triggered()
{
    QColor cor = QColorDialog::getColor(Qt::white, this);
    ui->textEdit->setTextColor(cor);
}

void MainWindow::on_actionCorDoFundo_triggered()
{
    QPalette paleta;
    QColor cor = QColorDialog::getColor(Qt::white, this);
    paleta.setColor(QPalette::Base, cor);
    ui->textEdit->setPalette(paleta);
}

void MainWindow::on_actionCopiar_triggered()
{
    ui->textEdit->copy();
}

void MainWindow::on_actionColar_triggered()
{
    ui->textEdit->paste();
}

void MainWindow::on_actionRecortar_triggered()
{
    ui->textEdit->cut();
}

void MainWindow::on_actionImprimir_triggered()
{
    QPrinter impressora;
    QTextDocument *documento = ui->textEdit->document();
    QPrintDialog dialogo(&impressora, this);
    if (dialogo.exec() == QPrintDialog::Rejected) { return; }
    documento->print(&impressora);
}

void MainWindow::on_actionSelecionarTudo_triggered()
{
    ui->textEdit->selectAll();
}

void MainWindow::on_actionDesfazer_triggered()
{
    ui->textEdit->undo();
}

void MainWindow::localizar(const QString &str, QTextDocument::FindFlags flags)
{
    QTextDocument *documento = ui->textEdit->document();

    bool encontrado = false;

    if (primeiraVezPesquisado == false)
        documento->undo();

    if (str.isEmpty()) {
        QMessageBox::information(this, tr("Campo de pesquisa vazio"),
                                 tr("O campo de pesquisa está vazio. Por favor, digite uma palavra e clique em Localizar."));
    } else {
        QTextCursor realcarCursor(documento);
        QTextCursor cursor(documento);

        cursor.beginEditBlock();

        QTextCharFormat formatoSimples(realcarCursor.charFormat());
        QTextCharFormat formatoColorido = formatoSimples;
        formatoColorido.setForeground(Qt::red);

        while (!realcarCursor.isNull() && !realcarCursor.atEnd()) {
            realcarCursor = documento->find(str, realcarCursor, flags);

            if (!realcarCursor.isNull()) {
                encontrado = true;
                realcarCursor.movePosition(QTextCursor::WordRight, QTextCursor::KeepAnchor);
                realcarCursor.mergeCharFormat(formatoColorido);
            }
        }
        cursor.endEditBlock();

        primeiraVezPesquisado = false;

        if (encontrado == false) {
            QMessageBox::information(this, tr("Palavra não encontrada"),
                                     tr("Desculpe, a palavra não pode ser encontrada."));
        }
    }
}
