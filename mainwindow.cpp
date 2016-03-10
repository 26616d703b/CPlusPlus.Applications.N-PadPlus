#include "mainwindow.h"
#include "ui_mainwindow.h"

                                            // CONSTRUCTOR

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent),
                                          ui(new Ui::MainWindow),
                                          index(0)
{
    ui->setupUi(this);

    loadSettings();
}

                                            // DESTRUCTOR

MainWindow::~MainWindow() {

    delete ui;
}

                                            // OTHER METHODS

unsigned short int MainWindow::currentIndex() const {

    QList<QMdiSubWindow*>::iterator it(ui->central_area->subWindowList().begin());

    unsigned short int i = 0;

    while (it != ui->central_area->subWindowList().end()) {

        if (ui->central_area->currentSubWindow() == *it) {

            return i;
        }

        ++i;
        ++it;
    }

    return errno;
}

void MainWindow::indexTheSubWindow(TextArea *textArea) {

    textAreaIndex.append(textArea);

    ui->central_area->addSubWindow(textArea);
}

                                                // EVENTS

void MainWindow::closeEvent(QCloseEvent *event) {

    if (textAreaIndex.count() == 0) {

        writeSettings();
        event->accept();
    }
    else {

        for (unsigned short int i(0); i < textAreaIndex.count(); i++) {

            if (!textAreaIndex.at(i)->isRecorded()) {

                int answer = QMessageBox::warning(this, tr("Attention!"), tr("Si vous quittez maintenant, vous devez être conscient que tout le contenu non sauvegard? sera perdu. Voulez-vous quand même continuer ?"), QMessageBox::Yes | QMessageBox::No);

                if (answer == QMessageBox::Yes) {

                    writeSettings();
                    event->accept();
                }
                else {

                    event->ignore();
                }

                break;
            }
        }
    }
}

                                                    // IO

void MainWindow::loadSettings() {

    QSettings settings("Ampersand");

    // Load the size and position of the window
    QPoint pos = settings.value("pos", QPoint(250, 120)).toPoint();
    move(pos);

    QSize size = settings.value("size", QSize(800, 500)).toSize();
    resize(size);

    // Load the sub window font
    QString family = settings.value("family", "Courier").toString();
    int pointSize = settings.value("pointSize", 10).toInt();

    m_currentFont = QFont(family, pointSize);

    // Load the content of the toolbar
    ui->main_toolbar->addAction(ui->new_action);
    ui->main_toolbar->addAction(ui->open_action);
    ui->main_toolbar->addAction(ui->save_action);
    ui->main_toolbar->addAction(ui->save_as_action);
    ui->main_toolbar->addSeparator();
    ui->main_toolbar->addAction(ui->undo_action);
    ui->main_toolbar->addAction(ui->redo_action);
    ui->main_toolbar->addAction(ui->find_action);
    ui->main_toolbar->addAction(ui->replace_action);
    ui->main_toolbar->addSeparator();
    ui->main_toolbar->addAction(ui->font_action);
    ui->main_toolbar->addSeparator();
    ui->main_toolbar->addAction(ui->parameters_action);
    ui->main_toolbar->addAction(ui->exit_action);

    on_new_action_triggered();

    statusBar()->showMessage(tr("Pr?t!"));
}

void MainWindow::writeSettings() {

    QSettings settings("Ampersand");

    settings.setValue("pos", pos());
    settings.setValue("size", size());

    settings.setValue("family", m_currentFont.family());
    settings.setValue("pointSize", m_currentFont.pointSize());
}

                                                    // SLOTS

// SubW
void MainWindow::on_central_area_subWindowActivated(QMdiSubWindow *) {

    index = currentIndex();
}

void MainWindow::change_subwindow_settings() {

    textAreaIndex.at(index)->setRecorded(false);

    if (!tabBar) {

        return;
    }

    tabBar->setTabIcon(index, QIcon("res/icon/tab/unsaved_note-32.png"));
}

void MainWindow::close_subwindow(int i) {

    if (!textAreaIndex.at(i)->isRecorded()) {

        int answer = QMessageBox::warning(this, tr("Attention!"), tr("Ce fichier a ?t? modifi?, voulez vous sauvegarder les changement effectu?s ?"), QMessageBox::Save | QMessageBox::No);

        if (answer == QMessageBox::Yes) {

            on_save_action_triggered();
        }
    }

    textAreaIndex.at(i)->deleteLater();
    textAreaIndex.removeAt(i);
}

// fileMenu
void MainWindow::on_new_action_triggered() {

    TextArea *text_area = new TextArea;
    text_area->setFont(m_currentFont);
    text_area->setWindowTitle(QString(tr("Sans titre ") + QString::number(ui->central_area->subWindowList().size()+1)));

    indexTheSubWindow(text_area);

    text_area->show();

    tabBar = ui->central_area->findChild<QTabBar*>();

    if (!tabBar) {

        return;
    }

    tabBar->setTabsClosable(true);
    tabBar->setTabIcon(index, QIcon("res/icon/tab/saved_note-32.png"));

    connect(textAreaIndex.at(index), SIGNAL(textChanged()), this, SLOT(change_subwindow_settings()));
    connect(tabBar, SIGNAL(tabCloseRequested(int)), this, SLOT(close_subwindow(int)));
}

void MainWindow::on_open_action_triggered() {

    QString path = QFileDialog::getOpenFileName(this, tr("Ouvrir"), QString(), "*.txt");
    QFile file(path);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {

        return;
    }

    TextArea *text_area = new TextArea;
    text_area->setFilePath(path);
    text_area->setRecorded();
    text_area->setFont(m_currentFont);
    text_area->setWindowTitle(QFileInfo(path).fileName());

    QTextStream in(&file);
#ifndef QT_NO_CURSOR
    QApplication::setOverrideCursor(Qt::WaitCursor);
#endif
    text_area->setPlainText(in.readAll());
#ifndef QT_NO_CURSOR
    QApplication::restoreOverrideCursor();
#endif

    indexTheSubWindow(text_area);

    text_area->show();
    statusBar()->showMessage(tr("Fichier ouvert!"), 3000);
}

void MainWindow::on_save_action_triggered() {

    if (textAreaIndex.at(index)->isRecorded()) {

        return;
    }

    if (!textAreaIndex.at(index)->filePath().isEmpty()) {

        QFile file(textAreaIndex.at(index)->filePath());

        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {

            return;
        }

        QTextStream out(&file);

#ifndef QT_NO_CURSOR
        QApplication::setOverrideCursor(Qt::WaitCursor);
#endif //QT_NO_CURSOR

        out << textAreaIndex.at(index)->toPlainText();

#ifndef QT_NO_CURSOR
        QApplication::restoreOverrideCursor();
#endif //QT_NO_CURSOR

        textAreaIndex.at(index)->setRecorded();

        if (tabBar) {

            tabBar->setTabIcon(index, QIcon("res/icon/tab/saved_note-32.png"));
        }

        statusBar()->showMessage(tr("Sauvegarde effectu?e!"), 3000);
    }
    else {

        on_save_as_action_triggered();
    }
}

void MainWindow::on_save_as_action_triggered() {

    QString path = QFileDialog::getSaveFileName(this, tr("Enregistrer sous..."), QString(), "*.txt");
    QFile file(path);

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {

        return;
    }

    QTextStream out(&file);
#ifndef QT_NO_CURSOR
    QApplication::setOverrideCursor(Qt::WaitCursor);
#endif //QT_NO_CURSOR
    out << textAreaIndex.at(index)->toPlainText();
#ifndef QT_NO_CURSOR
    QApplication::restoreOverrideCursor();
#endif //QT_NO_CURSOR

    textAreaIndex.at(index)->setFilePath(path);
    textAreaIndex.at(index)->setRecorded();
    textAreaIndex.at(index)->setWindowTitle(QFileInfo(path).fileName());

    if (tabBar) {

        tabBar->setTabIcon(index, QIcon("res/tab/icon/saved_note-32.png"));
    }

    statusBar()->showMessage("Sauvegarde effectu?e!", 3000);
}

void MainWindow::on_print_action_triggered() {

    #ifndef QT_NO_PRINTDIALOG
        QTextDocument *document = textAreaIndex.at(index)->document();
        QPrinter printer;
        QPrintDialog *dlg = new QPrintDialog(&printer, this);

        if (dlg->exec() != QDialog::Accepted) {

           return;
        }

        document->print(&printer);

        statusBar()->showMessage(tr("Pr?t"), 3000);
    #endif //QT_NO_PRINTDIALOG
}

void MainWindow::on_exit_action_triggered() {

    if (textAreaIndex.count() == 0) {

        writeSettings();
        qApp->quit();
    }
    else {

        for (unsigned short int i(0); i < textAreaIndex.count(); i++) {

            if (!textAreaIndex.at(i)->isRecorded()) {

                int answer = QMessageBox::warning(this, tr("Attention!"), tr("Si vous quittez maintenant, vous devez être conscient que tout le contenu non sauvegard? sera perdu. Voulez-vous quand même continuer ?"), QMessageBox::Yes | QMessageBox::No);

                if(answer == QMessageBox::No) {

                    return;
                }

                writeSettings();
                break;
             }
        }

        qApp->quit();
    }
}

// editionMenu
void MainWindow::on_undo_action_triggered() {

    textAreaIndex.at(index)->undo();
}

void MainWindow::on_redo_action_triggered() {

    textAreaIndex.at(index)->redo();
}

void MainWindow::on_cut_action_triggered() {

    textAreaIndex.at(index)->cut();
}

void MainWindow::on_copy_action_triggered() {

    textAreaIndex.at(index)->copy();
}

void MainWindow::on_paste_action_triggered() {

    textAreaIndex.at(index)->paste();
}

void MainWindow::on_remove_action_triggered() {

    QTextCursor cursor(textAreaIndex.at(index)->textCursor());
    cursor.removeSelectedText();
}

void MainWindow::on_find_action_triggered() {

    QString word = QInputDialog::getText(this, tr("Rechercher"), QString());

    if (textAreaIndex.at(index)->find(word)) {

        statusBar()->showMessage(tr("Lettre(s), mot ou ensemble de mots trouv?(s)!"), 3000);
        return;
    }
    else if (textAreaIndex.at(index)->find(word, QTextDocument::FindBackward)) {

        statusBar()->showMessage(tr("Lettre(s), mot ou ensemble de mots trouv?(s)!"), 3000);
    }
    else {

        textAreaIndex.at(index)->moveCursor(QTextCursor::StartOfWord);

        statusBar()->showMessage(tr("Lettre(s), mot ou ensemble de mots non trouv?(s)!!"), 3000);
    }
}

void MainWindow::on_replace_action_triggered()
{
    QString word1 = QInputDialog::getText(this, tr("Mots a remplacer"), QString());

    if (word1.isEmpty()) {

        return;
    }

    if (textAreaIndex.at(index)->find(word1) || textAreaIndex.at(index)->find(word1, QTextDocument::FindBackward)) {

        QString word2 = QInputDialog::getText(this, tr("Remplacer par"), QString());

        if (word2.isEmpty())
            return;

        textAreaIndex.at(index)->toPlainText().replace(word1, word2);

        statusBar()->showMessage(tr("Mots remplac?s!"), 3000);
    }
}

void MainWindow::on_reach_action_triggered() {

    int number = QInputDialog::getInt(this, tr("Se d?placer à la ligne") , QString());

    if (textAreaIndex.at(index)->toPlainText().count() < number) {

        QMessageBox::warning(this, tr("D?placement impossible"), tr("Le nombre de lignes dans ce document est inf?rieur a la valeur indiqu?e."));
    }
    else
    {
        QTextCursor cursor = textAreaIndex.at(index)->textCursor();
        cursor.movePosition(QTextCursor::Start);
        cursor.movePosition(QTextCursor::Down, QTextCursor::MoveAnchor, number-1);

        textAreaIndex.at(index)->setTextCursor(cursor);

        statusBar()->showMessage(tr("D?placement effectu?e!"), 3000);
    }
}

void MainWindow::on_select_all_action_triggered() {

    textAreaIndex.at(index)->selectAll();
}

void MainWindow::on_date_action_triggered() {

    textAreaIndex.at(index)->insertPlainText(QDateTime::currentDateTime().toString());
}

// formatMenu
void MainWindow::on_automatic_return_to_the_line_action_triggered() {

    if (ui->automatic_return_to_the_line_action->isChecked()) {

        for (unsigned short int i(0); i < textAreaIndex.count(); i++) {

            textAreaIndex.at(i)->setLineWrapMode(QPlainTextEdit::WidgetWidth);

            statusBar()->showMessage(tr("Retour automatique à la ligne activ?!"), 3000);
        }
    }
    else {

        for (unsigned short int i(0); i < textAreaIndex.count(); i++) {

            textAreaIndex.at(i)->setLineWrapMode(QPlainTextEdit::NoWrap);

            statusBar()->showMessage(tr("Retour automatique à la ligne d?sactiv?!"), 3000);
        }
    }
}

void MainWindow::on_font_action_triggered() {

    bool ok;
    m_currentFont = QFontDialog::getFont(&ok, m_currentFont, this);

    if (!ok) {

        return;
    }

    for (unsigned short int i(0); i < textAreaIndex.count(); i++)
        textAreaIndex.at(i)->setFont(m_currentFont);

    statusBar()->showMessage(tr("Propri?t?es de la police modifi?es!"), 3000);
}

// displayMenu
void MainWindow::on_toolbar_action_triggered() {

    if (ui->toolbar_action->isChecked()) {

        ui->main_toolbar->setVisible(true);

        statusBar()->showMessage(tr("Barre d'outils activ?e!"), 3000);
    }
    else {
        ui->main_toolbar->setVisible(false);

        statusBar()->showMessage(tr("Barre d'outils d?sactiv?e!"), 3000);
    }
}

void MainWindow::on_statusbar_action_triggered() {

    if (ui->statusbar_action->isChecked()) {

        ui->statusbar->setVisible(true);
    }
    else {

        ui->statusbar->setVisible(false);
    }
}

// helMenu
void MainWindow::on_about_action_triggered() {

    QMessageBox::about(this, tr("à propos..."), tr("<strong>N-Pad+</strong> est un projet r?alis? pour le fun! ;)"));
}
