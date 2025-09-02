#include "MainWindow.h"
#include "CommandeWidget.h"
#include "LivreurWidget.h"
#include "StatistiquesWidget.h"
#include "db/DatabaseManager.h"
#include <QApplication>
#include <QMessageBox>
#include <QFileDialog>
#include <QStandardPaths>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , tabWidget(nullptr)
    , commandeWidget(nullptr)
    , livreurWidget(nullptr)
    , statistiquesWidget(nullptr)
    , statusLabel(nullptr)
    , connectionLabel(nullptr)
{
    setupUI();
    setupMenus();
    setupStatusBar();
    connecterSignaux();
    
    setWindowTitle("Système de Gestion Logistique - v1.0");
    setMinimumSize(1200, 800);
    resize(1400, 900);
}

MainWindow::~MainWindow()
{
    // Les widgets seront automatiquement détruits par Qt
}

void MainWindow::setupUI()
{
    // Widget central avec onglets
    tabWidget = new QTabWidget(this);
    setCentralWidget(tabWidget);
    
    // Création des widgets pour chaque module
    commandeWidget = new CommandeWidget(this);
    livreurWidget = new LivreurWidget(this);
    statistiquesWidget = new StatistiquesWidget(this);
    
    // Ajout des onglets
    tabWidget->addTab(commandeWidget, "📦 Gestion des Commandes");
    tabWidget->addTab(livreurWidget, "🚚 Gestion des Livreurs");
    tabWidget->addTab(statistiquesWidget, "📊 Statistiques");
    
    // Style des onglets
    tabWidget->setTabPosition(QTabWidget::North);
    tabWidget->setMovable(true);
    tabWidget->setDocumentMode(true);
}

void MainWindow::setupMenus()
{
    // Menu Fichier
    mainMenuBar = menuBar(); // Récupérer la barre de menu
    QMenu* menuFichier = mainMenuBar->addMenu("&Fichier");
    
    actionActualiser = new QAction("&Actualiser", this);
    actionActualiser->setShortcut(QKeySequence::Refresh);
    // Utiliser une icône standard plus commune
    actionActualiser->setIcon(QIcon::fromTheme("view-refresh"));
    
    actionExporter = new QAction("&Exporter...", this);
    actionExporter->setShortcut(QKeySequence("Ctrl+E"));
    // Utiliser une icône standard plus commune
    actionExporter->setIcon(QIcon::fromTheme("document-save"));
    
    actionQuitter = new QAction("&Quitter", this);
    actionQuitter->setShortcut(QKeySequence::Quit);
    // Utiliser une icône standard plus commune
    actionQuitter->setIcon(QIcon::fromTheme("application-exit"));
    
    menuFichier->addAction(actionActualiser);
    menuFichier->addSeparator();
    menuFichier->addAction(actionExporter);
    menuFichier->addSeparator();
    menuFichier->addAction(actionQuitter);
    
    // Menu Base de données
    QMenu* menuBaseDonnees = mainMenuBar->addMenu("&Base de données");
    
    actionMettreAJourContraintes = new QAction("Mettre à jour les &contraintes CASCADE", this);
    actionMettreAJourContraintes->setToolTip("Force la mise à jour des contraintes de clé étrangère pour permettre la suppression en cascade");
    actionMettreAJourContraintes->setIcon(QIcon::fromTheme("database"));
    
    menuBaseDonnees->addAction(actionMettreAJourContraintes);
    
    // Menu Aide
    QMenu* menuAide = mainMenuBar->addMenu("&Aide");
    
    actionAPropos = new QAction("À &propos", this);
    // Utiliser une icône standard plus commune
    actionAPropos->setIcon(QIcon::fromTheme("help-about"));
    
    menuAide->addAction(actionAPropos);
}

void MainWindow::setupStatusBar()
{
    statusLabel = new QLabel("Prêt");
    connectionLabel = new QLabel();
    
    mainStatusBar = statusBar(); // Récupérer la barre d'état
    mainStatusBar->addWidget(statusLabel, 1);
    mainStatusBar->addPermanentWidget(connectionLabel);
    
    // Vérifier l'état de la connexion
    DatabaseManager* db = DatabaseManager::getInstance();
    if (db->isConnected()) {
        connectionLabel->setText("🟢 Connecté à Oracle");
        connectionLabel->setStyleSheet("color: green;");
    } else {
        connectionLabel->setText("🔴 Déconnecté");
        connectionLabel->setStyleSheet("color: red;");
    }
}

void MainWindow::connecterSignaux()
{
    connect(actionQuitter, &QAction::triggered, this, &MainWindow::quitterApplication);
    connect(actionAPropos, &QAction::triggered, this, &MainWindow::afficherAPropos);
    connect(actionActualiser, &QAction::triggered, this, &MainWindow::actualiserDonnees);
    connect(actionExporter, &QAction::triggered, this, &MainWindow::exporterDonnees);
    connect(actionMettreAJourContraintes, &QAction::triggered, this, &MainWindow::mettreAJourContraintes);
}

void MainWindow::afficherAPropos()
{
    QMessageBox::about(this, "À propos", 
        "<h3>Système de Gestion Logistique</h3>"
        "<p>Version 1.0.0</p>"
        "<p>Application développée en Qt C++ pour la gestion des commandes et des livreurs.</p>"
        "<p><b>Modules:</b></p>"
        "<ul>"
        "<li>Gestion des Commandes</li>"
        "<li>Gestion des Livreurs</li>"
        "<li>Statistiques et Rapports</li>"
        "</ul>"
        "<p><b>Fonctionnalités:</b> CRUD complet, recherche multicritères, "
        "génération de documents PDF, statistiques avec graphiques.</p>");
}

void MainWindow::quitterApplication()
{
    int reponse = QMessageBox::question(this, "Quitter", 
        "Êtes-vous sûr de vouloir quitter l'application ?",
        QMessageBox::Yes | QMessageBox::No, 
        QMessageBox::No);
    
    if (reponse == QMessageBox::Yes) {
        QApplication::quit();
    }
}

void MainWindow::actualiserDonnees()
{
    statusLabel->setText("Actualisation en cours...");
    
    // Actualiser tous les widgets
    if (commandeWidget) {
        commandeWidget->actualiserListe();
    }
    
    if (livreurWidget) {
        livreurWidget->actualiserListe();
    }
    
    if (statistiquesWidget) {
        statistiquesWidget->actualiserStatistiques();
    }
    
    statusLabel->setText("Données actualisées");
}

void MainWindow::exporterDonnees()
{
    QString fileName = QFileDialog::getSaveFileName(this,
        "Exporter les données", 
        QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/export_logistique.pdf",
        "Fichiers PDF (*.pdf);;Fichiers Excel (*.xlsx)");
    
    if (!fileName.isEmpty()) {
        statusLabel->setText("Export en cours...");
        
        // Ici vous pouvez implémenter l'export selon l'onglet actuel
        int currentTab = tabWidget->currentIndex();
        bool success = false;
        
        switch (currentTab) {
            case 0: // Commandes
                // success = commandeWidget->exporterDonnees(fileName);
                break;
            case 1: // Livreurs
                // success = livreurWidget->exporterDonnees(fileName);
                break;
            case 2: // Statistiques
                // success = statistiquesWidget->exporterDonnees(fileName);
                break;
        }
        
        if (success) {
            statusLabel->setText(QString("Export terminé: %1").arg(fileName));
            QMessageBox::information(this, "Export réussi", 
                QString("Les données ont été exportées vers:\n%1").arg(fileName));
        } else {
            statusLabel->setText("Erreur lors de l'export");
            QMessageBox::warning(this, "Erreur d'export", 
                "Une erreur s'est produite lors de l'export des données.");
        }
    }
}

void MainWindow::mettreAJourContraintes()
{
    int reponse = QMessageBox::question(this, "Mise à jour des contraintes",
        "Cette opération va mettre à jour les contraintes de la base de données "
        "pour permettre la suppression en cascade.\n\n"
        "⚠️ ATTENTION: Cette opération modifie la structure de la base de données.\n\n"
        "Voulez-vous continuer?",
        QMessageBox::Yes | QMessageBox::No,
        QMessageBox::No);
    
    if (reponse == QMessageBox::Yes) {
        DatabaseManager* db = DatabaseManager::getInstance();
        
        if (db->isConnected()) {
            statusLabel->setText("Mise à jour des contraintes en cours...");
            
            db->forceUpdateConstraints();
            
            statusLabel->setText("Contraintes mises à jour avec succès");
            QMessageBox::information(this, "Succès", 
                "Les contraintes de la base de données ont été mises à jour.\n"
                "La suppression en cascade est maintenant active.");
        } else {
            QMessageBox::warning(this, "Erreur", 
                "Impossible de mettre à jour les contraintes: base de données non connectée.");
            statusLabel->setText("Erreur: Base de données non connectée");
        }
    }
}
