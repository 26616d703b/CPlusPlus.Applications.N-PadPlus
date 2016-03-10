#include <QApplication>

#include "MainWindow.h"

int main(int argc, char *argv[])
{
    QApplication application(argc, argv);
    application.setOrganizationName("Ampersand");
    application.setApplicationName("N-Pad+");

    // Traduction des cha�nes pr�d�finies par Qt dans notre langue
    QString locale = QLocale::system().name();
    QTranslator translator;
    translator.load(QString("qt_") + locale, QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    application.installTranslator(&translator);

    // Ouverture de la fen�tre principale
    MainWindow window;
    window.show();

    return application.exec();
}
