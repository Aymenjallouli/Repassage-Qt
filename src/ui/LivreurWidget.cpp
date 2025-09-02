#include "LivreurWidget.h"
#include "services/CommandeService.h"
#include <QHeaderView>
#include <QMessageBox>
#include <QFileDialog>
#include <QStandardPaths>
#include <QSplitter>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QCheckBox>
#include <QInputDialog>
#include <QChartView>
#include <QBarSeries>
#include <QBarSet>
#include <QBarCategoryAxis>
#include <QValueAxis>

// Classe LivreurDialog
LivreurDialog::LivreurDialog(QWidget *parent, const Livreur& livreur)
    : QDialog(parent), modeModification(!livreur.getNom().isEmpty()), idLivreur(livreur.getIdLivreur())
{
    setupUI();
    if (modeModification) {
        remplirFormulaire(livreur);
    }
}

void LivreurDialog::setupUI()
{
    setWindowTitle(modeModification ? "Modifier Livreur" : "Nouveau Livreur");
    setModal(true);
    setMinimumSize(400, 300);
    
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    
    // Formulaire
    QFormLayout* formLayout = new QFormLayout();
    
    editNom = new QLineEdit(this);
    editTelephone = new QLineEdit(this);
    editZone = new QLineEdit(this);
    editVehicule = new QLineEdit(this);
    checkDisponibilite = new QCheckBox(this);
    
    formLayout->addRow("Nom:", editNom);
    formLayout->addRow("Téléphone:", editTelephone);
    formLayout->addRow("Zone de livraison:", editZone);
    formLayout->addRow("Véhicule:", editVehicule);
    formLayout->addRow("Disponible:", checkDisponibilite);
    
    mainLayout->addLayout(formLayout);
    
    // Boutons
    QDialogButtonBox* buttonBox = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    
    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    
    mainLayout->addWidget(buttonBox);
}

void LivreurDialog::remplirFormulaire(const Livreur& livreur)
{
    editNom->setText(livreur.getNom());
    editTelephone->setText(livreur.getTelephone());
    editZone->setText(livreur.getZoneLivraison());
    editVehicule->setText(livreur.getVehicule());
    checkDisponibilite->setChecked(livreur.getDisponibilite());
}

Livreur LivreurDialog::getLivreur() const
{
    Livreur livreur;
    if (modeModification) {
        livreur.setIdLivreur(idLivreur);
    }
    livreur.setNom(editNom->text());
    livreur.setTelephone(editTelephone->text());
    livreur.setZoneLivraison(editZone->text());
    livreur.setVehicule(editVehicule->text());
    livreur.setDisponibilite(checkDisponibilite->isChecked());
    return livreur;
}

// Classe LivreurWidget
LivreurWidget::LivreurWidget(QWidget *parent)
    : QWidget(parent)
    , livreurSelectionne(-1)
{
    livreurService = new LivreurService(this);
    setupUI();
    connecterSignaux();
    appliquerStyle();
    actualiserListe();
}

void LivreurWidget::setupUI()
{
    mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(10);
    mainLayout->setContentsMargins(10, 10, 10, 10);
    
    splitter = new QSplitter(Qt::Horizontal, this);
    mainLayout->addWidget(splitter);
    
    // Panneau gauche
    QWidget* leftPanel = new QWidget();
    QVBoxLayout* leftLayout = new QVBoxLayout(leftPanel);
    leftLayout->setSpacing(10);
    
    setupPanneauRecherche();
    leftLayout->addWidget(groupRecherche);
    
    setupTableau();
    leftLayout->addWidget(tableLivreurs);
    
    splitter->addWidget(leftPanel);
    
    // Panneau droit
    QWidget* rightPanel = new QWidget();
    QVBoxLayout* rightLayout = new QVBoxLayout(rightPanel);
    rightLayout->setSpacing(10);
    
    setupPanneauActions();
    rightLayout->addWidget(groupActions);
    
    // Détails du livreur
    groupDetails = new QGroupBox("Détails du livreur sélectionné");
    labelDetails = new QLabel("Aucun livreur sélectionné");
    labelDetails->setWordWrap(true);
    labelDetails->setAlignment(Qt::AlignTop);
    labelDetails->setStyleSheet("padding: 10px; background-color: #f8f9fa; border-radius: 5px; color: black;");
    
    QVBoxLayout* detailsLayout = new QVBoxLayout(groupDetails);
    detailsLayout->addWidget(labelDetails);
    
    rightLayout->addWidget(groupDetails);
    rightLayout->addStretch();
    
    splitter->addWidget(rightPanel);
    splitter->setSizes({700, 300});
}

void LivreurWidget::setupTableau()
{
    tableLivreurs = new QTableWidget();
    tableLivreurs->setColumnCount(6);
    QStringList headers = {"ID", "Nom", "Téléphone", "Zone", "Véhicule", "Statut"};
    tableLivreurs->setHorizontalHeaderLabels(headers);
    
    // Configuration du tableau - même style que CommandeWidget
    tableLivreurs->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableLivreurs->setSelectionMode(QAbstractItemView::SingleSelection);
    tableLivreurs->setAlternatingRowColors(true);
    tableLivreurs->setSortingEnabled(true);
    tableLivreurs->verticalHeader()->setVisible(false);
    
    // Dimensionnement des colonnes pour utiliser tout l'espace
    QHeaderView* header = tableLivreurs->horizontalHeader();
    header->setStretchLastSection(true);
    header->resizeSection(0, 60);   // ID
    header->resizeSection(1, 150);  // Nom
    header->resizeSection(2, 120);  // Téléphone
    header->resizeSection(3, 150);  // Zone
    header->resizeSection(4, 120);  // Véhicule
}

void LivreurWidget::setupPanneauRecherche()
{
    groupRecherche = new QGroupBox("🔍 Recherche et filtres");
    rechercheLayout = new QGridLayout(groupRecherche);
    
    // Nom
    rechercheLayout->addWidget(new QLabel("Nom:"), 0, 0);
    editNomRecherche = new QLineEdit();
    editNomRecherche->setPlaceholderText("Filtrer par nom...");
    rechercheLayout->addWidget(editNomRecherche, 0, 1);
    
    // Zone
    rechercheLayout->addWidget(new QLabel("Zone:"), 0, 2);
    editZoneRecherche = new QLineEdit();
    editZoneRecherche->setPlaceholderText("Filtrer par zone...");
    rechercheLayout->addWidget(editZoneRecherche, 0, 3);
    
    // Disponibilité
    rechercheLayout->addWidget(new QLabel("Disponibilité:"), 1, 0);
    comboDisponibiliteRecherche = new QComboBox();
    comboDisponibiliteRecherche->addItem("Tous les statuts", -1);
    comboDisponibiliteRecherche->addItem("Disponibles", 1);
    comboDisponibiliteRecherche->addItem("Occupés", 0);
    rechercheLayout->addWidget(comboDisponibiliteRecherche, 1, 1);
    
    // Boutons
    btnRechercher = new QPushButton("🔍 Rechercher");
    btnViderRecherche = new QPushButton("🗑️ Vider");
    rechercheLayout->addWidget(btnRechercher, 1, 2);
    rechercheLayout->addWidget(btnViderRecherche, 1, 3);
}

void LivreurWidget::setupPanneauActions()
{
    groupActions = new QGroupBox("Actions");
    actionsLayout = new QVBoxLayout(groupActions);
    
    // Actions CRUD
    btnAjouter = new QPushButton("➕ Ajouter un livreur");
    btnModifier = new QPushButton("✏️ Modifier");
    btnSupprimer = new QPushButton("🗑️ Supprimer");
    btnActualiser = new QPushButton("🔄 Actualiser");
    
    // Actions métier
    btnChangerDisponibilite = new QPushButton("⚡ Changer disponibilité");
    btnLivreursSurcharges = new QPushButton("⚠️ Livreurs en surcharge");
    btnLivreursDisponibles = new QPushButton("📊 Livreurs disponibles");
    
    // Rapports
    btnGenererRapport = new QPushButton("📄 Générer PDF");
    btnGenererListe = new QPushButton("📊 Rapport complet");
    
    // Ajout des boutons
    actionsLayout->addWidget(btnAjouter);
    actionsLayout->addWidget(btnModifier);
    actionsLayout->addWidget(btnSupprimer);
    actionsLayout->addWidget(new QFrame()); // Séparateur
    actionsLayout->addWidget(btnActualiser);
    actionsLayout->addWidget(btnChangerDisponibilite);
    actionsLayout->addWidget(btnLivreursSurcharges);
    actionsLayout->addWidget(btnLivreursDisponibles);
    actionsLayout->addWidget(new QFrame()); // Séparateur
    actionsLayout->addWidget(btnGenererRapport);
    actionsLayout->addWidget(btnGenererListe);
    actionsLayout->addStretch();
    
    // Désactiver les boutons nécessitant une sélection
    btnModifier->setEnabled(false);
    btnSupprimer->setEnabled(false);
    btnChangerDisponibilite->setEnabled(false);
    btnGenererRapport->setEnabled(false);
}

void LivreurWidget::connecterSignaux()
{
    // Tableau
    connect(tableLivreurs, &QTableWidget::itemSelectionChanged, 
            this, &LivreurWidget::selectionChangee);
    
    // Recherche
    connect(btnRechercher, &QPushButton::clicked, this, &LivreurWidget::rechercherLivreurs);
    connect(btnViderRecherche, &QPushButton::clicked, this, &LivreurWidget::viderRecherche);
    connect(editNomRecherche, &QLineEdit::returnPressed, this, &LivreurWidget::rechercherLivreurs);
    
    // Actions
    connect(btnAjouter, &QPushButton::clicked, this, &LivreurWidget::ajouterLivreur);
    connect(btnModifier, &QPushButton::clicked, this, &LivreurWidget::modifierLivreur);
    connect(btnSupprimer, &QPushButton::clicked, this, &LivreurWidget::supprimerLivreur);
    connect(btnActualiser, &QPushButton::clicked, this, &LivreurWidget::actualiserListe);
    connect(btnChangerDisponibilite, &QPushButton::clicked, this, &LivreurWidget::changerDisponibilite);
    connect(btnLivreursSurcharges, &QPushButton::clicked, this, &LivreurWidget::afficherLivreursSurcharges);
    connect(btnLivreursDisponibles, &QPushButton::clicked, this, &LivreurWidget::afficherLivreursDisponibles);
    connect(btnGenererRapport, &QPushButton::clicked, this, &LivreurWidget::genererRapportLivreurs);
    connect(btnGenererListe, &QPushButton::clicked, this, &LivreurWidget::genererListeLivreurs);
}

void LivreurWidget::appliquerStyle()
{
    // Style moderne pour les boutons - exactement comme CommandeWidget
    QString buttonStyle = R"(
        QPushButton {
            background-color: #2196F3;
            color: white;
            border: none;
            padding: 8px 16px;
            border-radius: 4px;
            font-weight: bold;
            min-width: 120px;
        }
        QPushButton:hover {
            background-color: #1976D2;
        }
        QPushButton:pressed {
            background-color: #0D47A1;
        }
        QPushButton:disabled {
            background-color: #BDBDBD;
            color: #757575;
        }
    )";
    
    // Appliquer le style aux boutons d'action
    btnAjouter->setStyleSheet(buttonStyle);
    btnModifier->setStyleSheet(buttonStyle);
    btnSupprimer->setStyleSheet(R"(
        QPushButton {
            background-color: #f44336;
            color: white;
            border: none;
            padding: 8px 16px;
            border-radius: 4px;
            font-weight: bold;
            min-width: 120px;
        }
        QPushButton:hover {
            background-color: #d32f2f;
        }
        QPushButton:pressed {
            background-color: #B71C1C;
        }
    )");
    btnActualiser->setStyleSheet(buttonStyle);
    btnChangerDisponibilite->setStyleSheet(buttonStyle);
    btnLivreursSurcharges->setStyleSheet(R"(
        QPushButton {
            background-color: #FF5722;
            color: white;
            border: none;
            padding: 8px 16px;
            border-radius: 4px;
            font-weight: bold;
            min-width: 120px;
        }
        QPushButton:hover {
            background-color: #E64A19;
        }
        QPushButton:pressed {
            background-color: #BF360C;
        }
    )");
    btnLivreursDisponibles->setStyleSheet(buttonStyle);
    btnGenererRapport->setStyleSheet(buttonStyle);
    btnGenererListe->setStyleSheet(buttonStyle);
    btnRechercher->setStyleSheet(buttonStyle);
    btnViderRecherche->setStyleSheet(R"(
        QPushButton {
            background-color: #FF9800;
            color: white;
            border: none;
            padding: 8px 16px;
            border-radius: 4px;
            font-weight: bold;
            min-width: 120px;
        }
        QPushButton:hover {
            background-color: #F57C00;
        }
        QPushButton:pressed {
            background-color: #E65100;
        }
    )");
    
    // Style pour le tableau - exactement comme CommandeWidget
    tableLivreurs->setStyleSheet(R"(
        QTableWidget {
            background-color: white;
            color: black;
            alternate-background-color: #f5f5f5;
            selection-background-color: #2196F3;
            selection-color: white;
            gridline-color: #e0e0e0;
        }
        QTableWidget::item {
            color: black;
            padding: 4px;
        }
        QTableWidget::item:selected {
            background-color: #2196F3;
            color: white;
        }
        QHeaderView::section {
            background-color: #37474F;
            color: white;
            padding: 8px;
            border: none;
            font-weight: bold;
        }
    )");
}

void LivreurWidget::actualiserListe()
{
    QList<Livreur> livreurs = livreurService->obtenirTousLivreurs();
    chargerLivreurs(livreurs);
}

void LivreurWidget::ajouterLivreur()
{
    Livreur livreurVide;
    LivreurDialog dialog(this, livreurVide);
    if (dialog.exec() == QDialog::Accepted) {
        Livreur livreur = dialog.getLivreur();
        
        if (livreurService->ajouterLivreur(livreur)) {
            QMessageBox::information(this, "Succès", "Livreur ajouté avec succès!");
            actualiserListe();
        } else {
            QMessageBox::warning(this, "Erreur", "Impossible d'ajouter le livreur!");
        }
    }
}

void LivreurWidget::modifierLivreur()
{
    int row = tableLivreurs->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Attention", "Sélectionnez un livreur à modifier!");
        return;
    }
    
    int idLivreur = tableLivreurs->item(row, 0)->text().toInt();
    Livreur livreur = livreurService->obtenirLivreur(idLivreur);
    
    if (!livreur.isValid()) {
        QMessageBox::warning(this, "Erreur", "Livreur introuvable!");
        return;
    }
    
    LivreurDialog dialog(this, livreur);
    if (dialog.exec() == QDialog::Accepted) {
        Livreur livreurModifie = dialog.getLivreur();
        
        if (livreurService->modifierLivreur(livreurModifie)) {
            QMessageBox::information(this, "Succès", "Livreur modifié avec succès!");
            actualiserListe();
        } else {
            QMessageBox::warning(this, "Erreur", "Impossible de modifier le livreur!");
        }
    }
}

void LivreurWidget::supprimerLivreur()
{
    int row = tableLivreurs->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Attention", "Sélectionnez un livreur à supprimer!");
        return;
    }
    
    int idLivreur = tableLivreurs->item(row, 0)->text().toInt();
    QString nomLivreur = tableLivreurs->item(row, 1)->text();
    
    int reponse = QMessageBox::question(this, "Confirmation",
        QString("Êtes-vous sûr de vouloir supprimer le livreur '%1'?").arg(nomLivreur),
        QMessageBox::Yes | QMessageBox::No);
    
    if (reponse == QMessageBox::Yes) {
        if (livreurService->supprimerLivreur(idLivreur)) {
            QMessageBox::information(this, "Succès", "Livreur supprimé avec succès!");
            actualiserListe();
        } else {
            QMessageBox::warning(this, "Erreur", "Impossible de supprimer le livreur!");
        }
    }
}

void LivreurWidget::rechercherLivreurs()
{
    QString nom = editNomRecherche->text();
    QString zone = editZoneRecherche->text();
    int disponibilite = comboDisponibiliteRecherche->currentData().toInt();
    bool seulementDisponibles = (disponibilite == 1);
    
    QList<Livreur> livreurs = livreurService->rechercherLivreurs(nom, zone, seulementDisponibles);
    chargerLivreurs(livreurs);
}

void LivreurWidget::viderRecherche()
{
    editNomRecherche->clear();
    editZoneRecherche->clear();
    comboDisponibiliteRecherche->setCurrentIndex(0);
    actualiserListe();
}

void LivreurWidget::selectionChangee()
{
    int row = tableLivreurs->currentRow();
    bool hasSelection = row >= 0;
    
    btnModifier->setEnabled(hasSelection);
    btnSupprimer->setEnabled(hasSelection);
    btnChangerDisponibilite->setEnabled(hasSelection);
    btnGenererRapport->setEnabled(hasSelection);
    
    if (hasSelection) {
        livreurSelectionne = tableLivreurs->item(row, 0)->text().toInt();
        mettreAJourDetails();
    } else {
        livreurSelectionne = -1;
        labelDetails->setText("Aucun livreur sélectionné");
    }
}

void LivreurWidget::mettreAJourDetails()
{
    if (livreurSelectionne <= 0) return;
    
    Livreur livreur = livreurService->obtenirLivreur(livreurSelectionne);
    if (!livreur.isValid()) return;
    
    // Compter les commandes actives (relation avec Commande)
    int commandesActives = livreurService->compterCommandesActives(livreurSelectionne);
    
    QString couleurStatut = livreur.getDisponibilite() ? "green" : "red";
    QString alerteSurcharge = (commandesActives > 5) ? 
        "<span style='color: red;'>⚠️ SURCHARGE DÉTECTÉE</span>" : 
        "<span style='color: green;'>✅ Charge normale</span>";
    
    QString details = QString(R"(
<h3>Livreur #%1</h3>
<p><b>👤 Nom:</b> %2</p>
<p><b>📱 Téléphone:</b> %3</p>
<p><b>📍 Zone:</b> %4</p>
<p><b>🚗 Véhicule:</b> %5</p>
<p><b>📊 Statut:</b> <span style="color: %6;">%7</span></p>
<p><b>📦 Commandes actives:</b> %8</p>
<p><b>⚠️ Surcharge:</b> %9</p>
    )").arg(livreur.getIdLivreur())
       .arg(livreur.getNom())
       .arg(livreur.getTelephone())
       .arg(livreur.getZoneLivraison())
       .arg(livreur.getVehicule())
       .arg(couleurStatut)
       .arg(livreur.getDisponibiliteText())
       .arg(commandesActives)
       .arg(alerteSurcharge);
    
    labelDetails->setText(details);
}

void LivreurWidget::changerDisponibilite()
{
    if (livreurSelectionne <= 0) {
        QMessageBox::warning(this, "Attention", "Sélectionnez un livreur!");
        return;
    }
    
    Livreur livreur = livreurService->obtenirLivreur(livreurSelectionne);
    
    bool nouvelleDisponibilite = !livreur.getDisponibilite();
    
    if (livreurService->mettreAJourDisponibilite(livreurSelectionne, nouvelleDisponibilite)) {
        QString message = nouvelleDisponibilite ? "Livreur marqué comme disponible" : "Livreur marqué comme occupé";
        QMessageBox::information(this, "Succès", message);
        actualiserListe();
    } else {
        QMessageBox::warning(this, "Erreur", "Impossible de changer la disponibilité!");
    }
}

void LivreurWidget::afficherLivreursSurcharges()
{
    QList<Livreur> livreursSurcharges = livreurService->obtenirLivreursSurcharges();
    
    if (livreursSurcharges.isEmpty()) {
        QMessageBox::information(this, "Surcharges", "Aucun livreur en surcharge actuellement.");
        return;
    }
    
    QString message = "Livreurs en surcharge (plus de 5 commandes actives):\n\n";
    for (const Livreur& livreur : livreursSurcharges) {
        int nbCommandes = livreurService->compterCommandesActives(livreur.getIdLivreur());
        message += QString("• %1 - %2 commandes actives\n")
                  .arg(livreur.getNom())
                  .arg(nbCommandes);
    }
    
    QMessageBox::warning(this, "Alertes Surcharge", message);
}

void LivreurWidget::afficherLivreursDisponibles()
{
    QList<Livreur> livreursDisponibles = livreurService->obtenirLivreursDisponibles();
    
    if (livreursDisponibles.isEmpty()) {
        QMessageBox::information(this, "Livreurs Disponibles", "Aucun livreur disponible actuellement.");
        return;
    }
    
    QString message = "Livreurs disponibles:\n\n";
    for (const Livreur& livreur : livreursDisponibles) {
        message += QString("• %1 - Zone: %2\n")
                  .arg(livreur.getNom())
                  .arg(livreur.getZoneLivraison());
    }
    
    QMessageBox::information(this, "Livreurs Disponibles", message);
}

void LivreurWidget::genererRapportLivreurs()
{
    QString fileName = QFileDialog::getSaveFileName(this, 
        "Enregistrer le rapport", 
        QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/rapport_livreurs.pdf",
        "Fichiers PDF (*.pdf)");
    
    if (!fileName.isEmpty()) {
        if (livreurService->genererRapportLivreurs(fileName)) {
            QMessageBox::information(this, "Succès", "Rapport généré avec succès!");
        } else {
            QMessageBox::warning(this, "Erreur", "Impossible de générer le rapport!");
        }
    }
}

void LivreurWidget::genererListeLivreurs()
{
    QString fileName = QFileDialog::getSaveFileName(this, 
        "Enregistrer la liste", 
        QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/liste_livreurs.xlsx",
        "Fichiers Excel (*.xlsx)");
    
    if (!fileName.isEmpty()) {
        if (livreurService->genererListeLivreurs(fileName)) {
            QMessageBox::information(this, "Succès", "Liste générée avec succès!");
        } else {
            QMessageBox::warning(this, "Erreur", "Impossible de générer la liste!");
        }
    }
}

void LivreurWidget::chargerLivreurs(const QList<Livreur>& livreurs)
{
    tableLivreurs->setRowCount(livreurs.size());
    
    for (int i = 0; i < livreurs.size(); ++i) {
        const Livreur& livreur = livreurs[i];
        
        // ID avec couleur explicite
        QTableWidgetItem* idItem = new QTableWidgetItem(QString::number(livreur.getIdLivreur()));
        idItem->setForeground(QColor("#000000")); // Noir
        tableLivreurs->setItem(i, 0, idItem);
        
        // Nom avec couleur explicite
        QTableWidgetItem* nomItem = new QTableWidgetItem(livreur.getNom());
        nomItem->setForeground(QColor("#000000")); // Noir
        tableLivreurs->setItem(i, 1, nomItem);
        
        // Téléphone avec couleur explicite
        QTableWidgetItem* telItem = new QTableWidgetItem(livreur.getTelephone());
        telItem->setForeground(QColor("#000000")); // Noir
        tableLivreurs->setItem(i, 2, telItem);
        
        // Zone avec couleur explicite
        QTableWidgetItem* zoneItem = new QTableWidgetItem(livreur.getZoneLivraison());
        zoneItem->setForeground(QColor("#000000")); // Noir
        tableLivreurs->setItem(i, 3, zoneItem);
        
        // Véhicule avec couleur explicite
        QTableWidgetItem* vehiculeItem = new QTableWidgetItem(livreur.getVehicule());
        vehiculeItem->setForeground(QColor("#000000")); // Noir
        tableLivreurs->setItem(i, 4, vehiculeItem);
        
        // Statut avec couleur
        QTableWidgetItem* statutItem = new QTableWidgetItem(livreur.getDisponibiliteText());
        if (livreur.getDisponibilite()) {
            statutItem->setBackground(QColor("#66BB6A"));  // Vert vif
            statutItem->setForeground(QColor("#FFFFFF"));  // Blanc
        } else {
            statutItem->setBackground(QColor("#EF5350"));  // Rouge vif
            statutItem->setForeground(QColor("#FFFFFF"));  // Blanc
        }
        tableLivreurs->setItem(i, 5, statutItem);
    }
    
    tableLivreurs->resizeColumnsToContents();
}
