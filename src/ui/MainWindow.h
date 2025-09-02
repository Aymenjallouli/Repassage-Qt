#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTabWidget>
#include <QMenuBar>
#include <QStatusBar>
#include <QAction>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>

class CommandeWidget;
class LivreurWidget;
class StatistiquesWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void afficherAPropos();
    void quitterApplication();
    void actualiserDonnees();
    void exporterDonnees();
    void mettreAJourContraintes(); // Nouvelle méthode
    
private:
    void setupUI();
    void setupMenus();
    void setupStatusBar();
    void connecterSignaux();
    
    // Widgets principaux
    QTabWidget* tabWidget;
    CommandeWidget* commandeWidget;
    LivreurWidget* livreurWidget;
    StatistiquesWidget* statistiquesWidget;
    
    // Menus et actions
    QMenuBar* mainMenuBar;
    QStatusBar* mainStatusBar;
    QLabel* statusLabel;
    QLabel* connectionLabel;
    
    QAction* actionQuitter;
    QAction* actionAPropos;
    QAction* actionActualiser;
    QAction* actionExporter;
    QAction* actionMettreAJourContraintes; // Nouvelle action
};

#endif // MAINWINDOW_H
