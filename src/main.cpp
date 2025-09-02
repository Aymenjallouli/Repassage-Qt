#include <QApplication>
#include <QStyleFactory>
#include <QDir>
#include "ui/MainWindow.h"
#include "db/DatabaseManager.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // Fix Qt platform plugin path
    QString appDir = QApplication::applicationDirPath();
    QApplication::addLibraryPath(appDir);
    QApplication::addLibraryPath(appDir + "/platforms");
    QApplication::addLibraryPath(appDir + "/sqldrivers");
    
    // Set application properties
    app.setApplicationName("Système de Gestion Logistique");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("Aymen Jallouli");
    
    // Set modern style
    app.setStyle(QStyleFactory::create("Fusion"));
    
    // Initialize database connection
    DatabaseManager* dbManager = DatabaseManager::getInstance();
    if (!dbManager->connectToDatabase()) {
        return -1;
    }
    
    // Create and show main window
    MainWindow window;
    window.show();
    
    return app.exec();
}
