#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTextDocument>

namespace Ui {
class MainWindow;
}

class FindDialog;
static QStringList arquivosRecente;

class MainWindow : public QMainWindow
{
    Q_OBJECT    

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    virtual void closeEvent(QCloseEvent *event);

private:
    Ui::MainWindow *ui;
    QString arquivoAtual;
    FindDialog *findDialog;

    enum { MaximoDeArquivosRecente = 5 };
    QAction *arquivosRecenteActions[MaximoDeArquivosRecente];
    QMenu *menuArquivosRecente;

    bool primeiraVezPesquisado;

    void criarArquivosRecentActions();
    void atualizarArquivosRecenteActions();

    bool continuar();
    bool salvar();
    bool salvarArquivo(const QString &nome);
    bool salvarComo();
    void carregarArquivo(const QString &nome);

    void definirArquivoAtual(const QString &nome);
    QString enxugarNome(const QString &nomeCompleto) const;
    void gravarConfiguracoes();
    void carregarConfiguracoes();

private slots:
    void on_actionNovo_triggered();
    void on_actionAbrir_triggered();
    void abrirAquivoRecente();
    void on_actionSobre_triggered();
    void on_actionSair_triggered();
    void on_actionLocalizar_triggered();
    void on_actionSalvar_triggered();
    void on_actionSalvarComo_triggered();
    void foiModificado();
    void on_actionFonte_triggered();
    void on_actionCorDaFonte_triggered();
    void on_actionCorDoFundo_triggered();
    void on_actionCopiar_triggered();
    void on_actionColar_triggered();
    void on_actionRecortar_triggered();
    void on_actionImprimir_triggered();
    void on_actionSelecionarTudo_triggered();
    void on_actionDesfazer_triggered();

    void localizar(const QString &str, QTextDocument::FindFlags flags);
};

#endif // MAINWINDOW_H
