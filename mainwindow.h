#ifndef N_PAD_PLUS_MAINWINDOW_H_
#define N_PAD_PLUS_MAINWINDOW_H_

#include <QMainWindow>
#include <QMdiSubWindow>
#include <QTranslator>
#include <QLocale>
#include <QLibraryInfo>
#include <QSettings>
#include <QTextStream>
#include <QClipboard>
#include <QList>
#include <QFile>
#include <QFileDialog>
#include <QPrinter>
#include <QPrintDialog>
#include <QInputDialog>
#include <QFontDialog>
#include <QMessageBox>
#include <QTabBar>

#include "TextArea.h"


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {

    Q_OBJECT
    
    public:
        explicit MainWindow(QWidget *parent = 0);
        ~MainWindow();

        // Accessors
        unsigned short int currentIndex() const;
        void indexTheSubWindow(TextArea *textArea);

        // Events
        void closeEvent(QCloseEvent *event);

        // IO
        void loadSettings();
        void writeSettings();

                     // Slots
    private slots:
        void on_central_area_subWindowActivated(QMdiSubWindow*);
        void change_subwindow_settings();
        void close_subwindow(int i);

        void on_new_action_triggered();
        void on_open_action_triggered();
        void on_save_action_triggered();
        void on_save_as_action_triggered();
        void on_print_action_triggered();
        void on_exit_action_triggered();

        void on_undo_action_triggered();
        void on_redo_action_triggered();
        void on_cut_action_triggered();
        void on_copy_action_triggered();
        void on_paste_action_triggered();
        void on_remove_action_triggered();
        void on_find_action_triggered();
        void on_replace_action_triggered();
        void on_reach_action_triggered();
        void on_select_all_action_triggered();
        void on_date_action_triggered();

        void on_automatic_return_to_the_line_action_triggered();
        void on_font_action_triggered();

        void on_toolbar_action_triggered();
        void on_statusbar_action_triggered();

        void on_about_action_triggered();
    
        // Attributes
    private:
        Ui::MainWindow *ui;

        unsigned short int index;
        QFont m_currentFont;
        QTabBar *tabBar;
        QList<TextArea*> textAreaIndex;

};

#endif  // !N_PAD_PLUS_MAINWINDOW_H_
