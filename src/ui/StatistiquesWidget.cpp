#include "StatistiquesWidget.h"
#include <QHBoxLayout>
#include <QMessageBox>
#include <QFileDialog>
#include <QStandardPaths>

StatistiquesWidget::StatistiquesWidget(QWidget *parent)
    : QWidget(parent)
    , totalCommandes(0)
    , commandesEnAttente(0)
    , commandesEnCours(0)
    , commandesLivrees(0)
    , commandesAnnulees(0)
    , totalLivreurs(0)
    , livreursDisponibles(0)
    , livreursOccupes(0)
{
    // Initialiser les services
    commandeService = new CommandeService();
    livreurService = new LivreurService();
    
    setupUI();
    connecterSignaux();
    appliquerStyle();
    actualiserStatistiques();
}

void StatistiquesWidget::setupUI()
{
    mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(15);
    mainLayout->setContentsMargins(15, 15, 15, 15);
    
    // Titre principal
    QLabel* titre = new QLabel("📊 Tableau de Bord - Statistiques Logistiques");
    titre->setStyleSheet("font-size: 24px; font-weight: bold; color: #2c3e50; margin: 10px; text-align: center;");
    titre->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(titre);
    
    // ScrollArea pour le contenu
    scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    
    QWidget* contentWidget = new QWidget();
    QVBoxLayout* contentLayout = new QVBoxLayout(contentWidget);
    contentLayout->setSpacing(20);
    
    // Panneau de résumé avec cartes
    setupPanneauResume();
    contentLayout->addWidget(groupResume);
    
    // Graphiques
    setupGraphiques();
    contentLayout->addWidget(groupGraphiques);
    
    // Boutons d'action
    QHBoxLayout* btnLayout = new QHBoxLayout();
    btnActualiser = new QPushButton("🔄 Actualiser les données");
    btnGenererRapport = new QPushButton("� Générer rapport PDF");
    btnExporterExcel = new QPushButton("📊 Exporter vers Excel");
    
    btnLayout->addWidget(btnActualiser);
    btnLayout->addWidget(btnGenererRapport);
    btnLayout->addWidget(btnExporterExcel);
    btnLayout->addStretch();
    
    contentLayout->addLayout(btnLayout);
    contentLayout->addStretch();
    
    scrollArea->setWidget(contentWidget);
    mainLayout->addWidget(scrollArea);
}

void StatistiquesWidget::setupPanneauResume()
{
    groupResume = new QGroupBox("📈 Résumé Exécutif");
    resumeLayout = new QGridLayout(groupResume);
    resumeLayout->setSpacing(15);
    
    // Carte des commandes totales
    carteCommandes = new QLabel();
    carteCommandes->setAlignment(Qt::AlignCenter);
    carteCommandes->setMinimumHeight(120);
    resumeLayout->addWidget(carteCommandes, 0, 0);
    
    // Carte des livreurs
    carteLivreurs = new QLabel();
    carteLivreurs->setAlignment(Qt::AlignCenter);
    carteLivreurs->setMinimumHeight(120);
    resumeLayout->addWidget(carteLivreurs, 0, 1);
    
    // Carte des livreurs disponibles
    carteDisponibles = new QLabel();
    carteDisponibles->setAlignment(Qt::AlignCenter);
    carteDisponibles->setMinimumHeight(120);
    resumeLayout->addWidget(carteDisponibles, 0, 2);
    
    // Carte des commandes en cours
    carteEnCours = new QLabel();
    carteEnCours->setAlignment(Qt::AlignCenter);
    carteEnCours->setMinimumHeight(120);
    resumeLayout->addWidget(carteEnCours, 0, 3);
}

void StatistiquesWidget::setupGraphiques()
{
    groupGraphiques = new QGroupBox("📊 Analyses Graphiques");
    graphiquesLayout = new QGridLayout(groupGraphiques);
    graphiquesLayout->setSpacing(15);
    
    // Graphique des statuts des commandes (Pie Chart)
    chartStatutsCommandes = new QChartView();
    chartStatutsCommandes->setMinimumHeight(300);
    chartStatutsCommandes->setRenderHint(QPainter::Antialiasing);
    graphiquesLayout->addWidget(chartStatutsCommandes, 0, 0);
    
    // Graphique des zones de livraison (Bar Chart)
    chartZonesLivraison = new QChartView();
    chartZonesLivraison->setMinimumHeight(300);
    chartZonesLivraison->setRenderHint(QPainter::Antialiasing);
    graphiquesLayout->addWidget(chartZonesLivraison, 0, 1);
    
    // Graphique de disponibilité des livreurs (Pie Chart)
    chartDisponibiliteLivreurs = new QChartView();
    chartDisponibiliteLivreurs->setMinimumHeight(300);
    chartDisponibiliteLivreurs->setRenderHint(QPainter::Antialiasing);
    graphiquesLayout->addWidget(chartDisponibiliteLivreurs, 1, 0, 1, 2);
}

void StatistiquesWidget::connecterSignaux()
{
    connect(btnActualiser, &QPushButton::clicked, this, &StatistiquesWidget::actualiserStatistiques);
    connect(btnGenererRapport, &QPushButton::clicked, this, &StatistiquesWidget::genererRapport);
    connect(btnExporterExcel, &QPushButton::clicked, [this]() {
        QMessageBox::information(this, "Export Excel", 
            "Fonctionnalité d'export Excel en cours de développement.\n"
            "Les données seront exportées au format XLSX avec:\n"
            "• Tableaux détaillés\n"
            "• Graphiques intégrés\n"
            "• Analyses par période");
    });
}

void StatistiquesWidget::appliquerStyle()
{
    // Style moderne pour les boutons
    QString buttonStyle = R"(
        QPushButton {
            background-color: #3498db;
            color: white;
            border: none;
            padding: 12px 20px;
            border-radius: 6px;
            font-weight: bold;
            font-size: 14px;
            min-width: 150px;
        }
        QPushButton:hover {
            background-color: #2980b9;
        }
        QPushButton:pressed {
            background-color: #21618c;
        }
    )";
    
    btnActualiser->setStyleSheet(buttonStyle);
    btnGenererRapport->setStyleSheet(buttonStyle.replace("#3498db", "#27ae60").replace("#2980b9", "#229954").replace("#21618c", "#196f3d"));
    btnExporterExcel->setStyleSheet(buttonStyle.replace("#3498db", "#e67e22").replace("#2980b9", "#d35400").replace("#21618c", "#ba4a00"));
    
    // Style pour les groupes
    QString groupStyle = R"(
        QGroupBox {
            font-weight: bold;
            font-size: 16px;
            color: #2c3e50;
            border: 2px solid #bdc3c7;
            border-radius: 8px;
            margin: 10px 0px;
            padding-top: 10px;
        }
        QGroupBox::title {
            subcontrol-origin: margin;
            left: 10px;
            padding: 0 5px 0 5px;
        }
    )";
    
    groupResume->setStyleSheet(groupStyle);
    groupGraphiques->setStyleSheet(groupStyle);
}

void StatistiquesWidget::actualiserStatistiques()
{
    // Charger les données réelles depuis la base de données
    chargerStatistiquesCommandes();
    chargerStatistiquesLivreurs();
    
    // Mettre à jour l'interface
    mettreAJourCartes();
    creerGraphiqueStatutsCommandes();
    creerGraphiqueZonesLivraison();
    creerGraphiqueDisponibiliteLivreurs();
}

void StatistiquesWidget::chargerStatistiquesCommandes()
{
    // Obtenir toutes les commandes réelles
    QList<Commande> commandes = commandeService->obtenirToutesCommandes();
    
    // Réinitialiser les compteurs
    totalCommandes = commandes.size();
    commandesEnAttente = 0;
    commandesEnCours = 0;
    commandesLivrees = 0;
    commandesAnnulees = 0;
    commandesParZone.clear();
    
    // Analyser chaque commande
    for (const Commande& cmd : commandes) {
        // Compter par statut
        if (cmd.getStatut() == "En attente") {
            commandesEnAttente++;
        } else if (cmd.getStatut() == "En cours") {
            commandesEnCours++;
        } else if (cmd.getStatut() == "Livree") {
            commandesLivrees++;
        } else if (cmd.getStatut() == "Annulee") {
            commandesAnnulees++;
        }
        
        // Compter par zone/ville
        QString ville = cmd.getVilleLivraison();
        commandesParZone[ville]++;
    }
}

void StatistiquesWidget::chargerStatistiquesLivreurs()
{
    // Obtenir tous les livreurs réels
    QList<Livreur> livreurs = livreurService->obtenirTousLivreurs();
    
    // Réinitialiser les compteurs
    totalLivreurs = livreurs.size();
    livreursDisponibles = 0;
    livreursOccupes = 0;
    livreursParZone.clear();
    
    // Analyser chaque livreur
    for (const Livreur& livreur : livreurs) {
        // Compter par disponibilité
        if (livreur.getDisponibilite()) {
            livreursDisponibles++;
        } else {
            livreursOccupes++;
        }
        
        // Compter par zone
        QString zone = livreur.getZoneLivraison();
        livreursParZone[zone]++;
    }
}

void StatistiquesWidget::mettreAJourCartes()
{
    // Style pour les cartes
    QString carteStyle = R"(
        QLabel {
            background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,
                                        stop: 0 %1, stop: 1 %2);
            color: white;
            border-radius: 10px;
            padding: 15px;
            font-weight: bold;
            font-size: 14px;
        }
    )";
    
    // Carte des commandes totales
    carteCommandes->setText(QString("📦 COMMANDES TOTALES\n\n%1\ncommandes").arg(totalCommandes));
    carteCommandes->setStyleSheet(carteStyle.arg("#3498db", "#2980b9"));
    
    // Carte des livreurs
    carteLivreurs->setText(QString("🚚 LIVREURS TOTAUX\n\n%1\nlivreurs").arg(totalLivreurs));
    carteLivreurs->setStyleSheet(carteStyle.arg("#e74c3c", "#c0392b"));
    
    // Carte des livreurs disponibles
    carteDisponibles->setText(QString("✅ DISPONIBLES\n\n%1\nlivreurs").arg(livreursDisponibles));
    carteDisponibles->setStyleSheet(carteStyle.arg("#27ae60", "#229954"));
    
    // Carte des commandes en cours
    carteEnCours->setText(QString("🔄 EN COURS\n\n%1\ncommandes").arg(commandesEnCours));
    carteEnCours->setStyleSheet(carteStyle.arg("#f39c12", "#d68910"));
}

void StatistiquesWidget::creerGraphiqueStatutsCommandes()
{
    QPieSeries* series = new QPieSeries();
    
    if (commandesEnAttente > 0) {
        series->append("En attente", commandesEnAttente);
    }
    if (commandesEnCours > 0) {
        series->append("En cours", commandesEnCours);
    }
    if (commandesLivrees > 0) {
        series->append("Livrées", commandesLivrees);
    }
    if (commandesAnnulees > 0) {
        series->append("Annulées", commandesAnnulees);
    }
    
    // Si aucune donnée, afficher un message
    if (series->count() == 0) {
        series->append("Aucune donnée", 1);
    }
    
    // Personnaliser les couleurs
    if (series->count() > 0) {
        QStringList colors = {"#FFA726", "#42A5F5", "#66BB6A", "#EF5350"};
        for (int i = 0; i < series->count() && i < colors.size(); ++i) {
            series->slices().at(i)->setBrush(QColor(colors[i]));
            series->slices().at(i)->setLabelVisible(true);
        }
    }
    
    QChart* chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("Répartition des Commandes par Statut");
    chart->setTitleFont(QFont("Arial", 14, QFont::Bold));
    chart->legend()->setAlignment(Qt::AlignBottom);
    chart->setAnimationOptions(QChart::SeriesAnimations);
    
    chartStatutsCommandes->setChart(chart);
}

void StatistiquesWidget::creerGraphiqueZonesLivraison()
{
    QBarSeries* series = new QBarSeries();
    QBarSet* set = new QBarSet("Commandes");
    
    QStringList categories;
    
    // Trier les zones par nombre de commandes (décroissant)
    QList<QPair<int, QString>> zonesTriees;
    for (auto it = commandesParZone.begin(); it != commandesParZone.end(); ++it) {
        zonesTriees.append(qMakePair(it.value(), it.key()));
    }
    std::sort(zonesTriees.begin(), zonesTriees.end(), std::greater<QPair<int, QString>>());
    
    // Prendre les 5 zones les plus actives
    int maxZones = qMin(5, zonesTriees.size());
    for (int i = 0; i < maxZones; ++i) {
        categories << zonesTriees[i].second;
        *set << zonesTriees[i].first;
    }
    
    // Si aucune donnée
    if (categories.isEmpty()) {
        categories << "Aucune donnée";
        *set << 0;
    }
    
    set->setBrush(QColor("#3498db"));
    series->append(set);
    
    QChart* chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("Top 5 des Zones de Livraison");
    chart->setTitleFont(QFont("Arial", 14, QFont::Bold));
    chart->setAnimationOptions(QChart::SeriesAnimations);
    
    QBarCategoryAxis* axisX = new QBarCategoryAxis();
    axisX->append(categories);
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);
    
    QValueAxis* axisY = new QValueAxis();
    axisY->setRange(0, set->count() > 0 ? set->at(0) * 1.2 : 10);
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);
    
    chart->legend()->setVisible(false);
    
    chartZonesLivraison->setChart(chart);
}

void StatistiquesWidget::creerGraphiqueDisponibiliteLivreurs()
{
    QPieSeries* series = new QPieSeries();
    
    if (livreursDisponibles > 0) {
        series->append("Disponibles", livreursDisponibles);
    }
    if (livreursOccupes > 0) {
        series->append("Occupés", livreursOccupes);
    }
    
    // Si aucune donnée
    if (series->count() == 0) {
        series->append("Aucun livreur", 1);
    }
    
    // Personnaliser les couleurs
    if (livreursDisponibles > 0) {
        series->slices().at(0)->setBrush(QColor("#66BB6A"));
        series->slices().at(0)->setLabelVisible(true);
    }
    if (livreursOccupes > 0 && series->count() > 1) {
        series->slices().at(1)->setBrush(QColor("#EF5350"));
        series->slices().at(1)->setLabelVisible(true);
    }
    
    QChart* chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("Disponibilité des Livreurs");
    chart->setTitleFont(QFont("Arial", 14, QFont::Bold));
    chart->legend()->setAlignment(Qt::AlignBottom);
    chart->setAnimationOptions(QChart::SeriesAnimations);
    
    chartDisponibiliteLivreurs->setChart(chart);
}

void StatistiquesWidget::genererRapport()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Enregistrer le rapport statistiques",
        QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/rapport_statistiques.pdf",
        "Fichiers PDF (*.pdf)");
    
    if (!fileName.isEmpty()) {
        // TODO: Implémenter la génération de rapport PDF avec les vraies données
        QMessageBox::information(this, "Information", 
            QString("Génération du rapport statistiques:\n\n"
                    "📊 Données incluses:\n"
                    "• %1 commandes analysées\n"
                    "• %2 livreurs répertoriés\n"
                    "• %3 zones de livraison\n"
                    "• Graphiques et analyses détaillées\n\n"
                    "Le rapport sera sauvegardé vers:\n%4")
                    .arg(totalCommandes)
                    .arg(totalLivreurs) 
                    .arg(commandesParZone.size())
                    .arg(fileName));
    }
}
