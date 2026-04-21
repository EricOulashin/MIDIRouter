#include "MainWindow.h"

#include <QApplication>

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    QApplication::setApplicationName(QStringLiteral("MIDI Router"));
    QApplication::setOrganizationName(QStringLiteral("EricOulashin"));

    MainWindow w;
    w.show();
    return app.exec();
}
