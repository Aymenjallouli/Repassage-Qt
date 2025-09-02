#include "CommandeWidget.h"
#include "services/LivreurService.h"
#include <QHeaderView>
#include <QMessageBox>
#include <QFileDialog>
#include <QStandardPaths>
#include <QSplitter>
#include <QGridLayout>
#include <QFormLayout>
#include <QSpinBox>
#include <QDialogButtonBox>
#include <QInputDialog>
#include <QApplication>
#include <QRegularExpression>
#include <QRegularExpressionValidator>

// Implémentation de CommandeDialog
CommandeDialog::CommandeDialog(QWidget* parent, const Commande& commande)
    : QDialog(parent)
    , modeModification(commande.getIdCommande() > 0)
    , idCommande(commande.getIdCommande())
{
    setupUI();
    if (modeModification) {
        setWindowTitle("Modifier une commande");
        remplirFormulaire(commande);
    } else {
        setWindowTitle("Ajouter une commande");
    }
    resize(400, 300);
}

void CommandeDialog::setupUI()
{
    setModal(true);
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    
    // Formulaire
    QGroupBox* formGroup = new QGroupBox("Informations de la commande");
    formLayout = new QFormLayout(formGroup);
    
    editDateCommande = new QDateEdit(QDate::currentDate());
    editDateCommande->setCalendarPopup(true);
    editDateCommande->setDisplayFormat("dd/MM/yyyy");
    formLayout->addRow("Date de commande:", editDateCommande);
    
    comboStatut = new QComboBox();
    comboStatut->addItems({"En attente", "En cours", "Livree", "Annulee"});
    formLayout->addRow("Statut:", comboStatut);
    
    editVilleLivraison = new QLineEdit();
    editVilleLivraison->setPlaceholderText("Entrez la ville de livraison");
    // Validation: Seulement lettres, espaces, tirets et apostrophes
    QRegularExpression villeRegex("^[A-Za-zÀ-ÿ\\s\\-\\']{2,50}$");
    editVilleLivraison->setValidator(new QRegularExpressionValidator(villeRegex, this));
    editVilleLivraison->setMaxLength(50);
    formLayout->addRow("Ville de livraison:", editVilleLivraison);
    
    spinIdClient = new QSpinBox();
    spinIdClient->setRange(1, 999999);
    spinIdClient->setValue(1);
    spinIdClient->setToolTip("ID client doit être entre 1 et 999999");
    formLayout->addRow("ID Client:", spinIdClient);
    
    comboLivreur = new QComboBox();
    comboLivreur->addItem("Aucun livreur assigné", 0);
    
    // Charger les livreurs disponibles
    LivreurService livreurService;
    QList<Livreur> livreurs = livreurService.obtenirLivreursDisponibles();
    for (const Livreur& livreur : livreurs) {
        comboLivreur->addItem(QString("%1 - %2").arg(livreur.getNom()).arg(livreur.getZoneLivraison()), 
                              livreur.getIdLivreur());
    }
    formLayout->addRow("Livreur:", comboLivreur);
    
    mainLayout->addWidget(formGroup);
    
    // Boutons
    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &CommandeDialog::validerSaisie);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    mainLayout->addWidget(buttonBox);
}

void CommandeDialog::validerSaisie()
{
    if (validerChamps()) {
        accept();
    }
}

bool CommandeDialog::validerChamps()
{
    // Validation de la ville de livraison
    if (editVilleLivraison->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Erreur de saisie", "La ville de livraison est obligatoire.");
        editVilleLivraison->setFocus();
        return false;
    }
    
    // Validation de la longueur minimale de la ville
    if (editVilleLivraison->text().trimmed().length() < 2) {
        QMessageBox::warning(this, "Erreur de saisie", "La ville doit contenir au moins 2 caractères.");
        editVilleLivraison->setFocus();
        return false;
    }
    
    // Validation de l'ID client
    if (spinIdClient->value() <= 0) {
        QMessageBox::warning(this, "Erreur de saisie", "L'ID client doit être supérieur à 0.");
        spinIdClient->setFocus();
        return false;
    }
    
    // Validation du livreur sélectionné
    if (comboLivreur->currentData().toInt() <= 0) {
        QMessageBox::warning(this, "Erreur de saisie", "Veuillez sélectionner un livreur valide.");
        comboLivreur->setFocus();
        return false;
    }
    
    return true;
}

void CommandeDialog::remplirFormulaire(const Commande& commande)
{
    editDateCommande->setDate(commande.getDateCommande());
    comboStatut->setCurrentText(commande.getStatut());
    editVilleLivraison->setText(commande.getVilleLivraison());
    spinIdClient->setValue(commande.getIdClient());
    
    // Sélectionner le livreur
    int index = comboLivreur->findData(commande.getIdLivreur());
    if (index >= 0) {
        comboLivreur->setCurrentIndex(index);
    }
}

Commande CommandeDialog::getCommande() const
{
    Commande commande;
    if (modeModification) {
        commande.setIdCommande(idCommande);
    }
    commande.setDateCommande(editDateCommande->date());
    commande.setStatut(comboStatut->currentText());
    commande.setVilleLivraison(editVilleLivraison->text());
    commande.setIdClient(spinIdClient->value());
    commande.setIdLivreur(comboLivreur->currentData().toInt());
    
    return commande;
}

// Implémentation de CommandeWidget
CommandeWidget::CommandeWidget(QWidget *parent)
    : QWidget(parent)
    , commandeService(new CommandeService())
    , commandeSelectionnee(-1)
{
    setupUI();
    connecterSignaux();
    appliquerStyle();
    chargerCommandes();
}

void CommandeWidget::setupUI()
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
    leftLayout->addWidget(tableCommandes);
    
    splitter->addWidget(leftPanel);
    
    // Panneau droit
    QWidget* rightPanel = new QWidget();
    QVBoxLayout* rightLayout = new QVBoxLayout(rightPanel);
    rightLayout->setSpacing(10);
    
    setupPanneauActions();
    rightLayout->addWidget(groupActions);
    
    // Détails de la commande
    groupDetails = new QGroupBox("Détails de la commande sélectionnée");
    labelDetails = new QLabel("Aucune commande sélectionnée");
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

void CommandeWidget::setupTableau()
{
    tableCommandes = new QTableWidget();
    tableCommandes->setColumnCount(6);
    QStringList headers = {"ID", "Date", "Statut", "Ville", "Client", "Livreur"};
    tableCommandes->setHorizontalHeaderLabels(headers);
    
    // Configuration du tableau
    tableCommandes->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableCommandes->setSelectionMode(QAbstractItemView::SingleSelection);
    tableCommandes->setAlternatingRowColors(true);
    tableCommandes->setSortingEnabled(true);
    tableCommandes->verticalHeader()->setVisible(false);
    
    QHeaderView* header = tableCommandes->horizontalHeader();
    header->setStretchLastSection(true);
    header->resizeSection(0, 60);   // ID
    header->resizeSection(1, 100);  // Date
    header->resizeSection(2, 100);  // Statut
    header->resizeSection(3, 150);  // Ville
    header->resizeSection(4, 80);   // Client
}

void CommandeWidget::setupPanneauRecherche()
{
    groupRecherche = new QGroupBox("🔍 Recherche et filtres");
    rechercheLayout = new QGridLayout(groupRecherche);
    
    // Statut
    rechercheLayout->addWidget(new QLabel("Statut:"), 0, 0);
    comboStatutRecherche = new QComboBox();
    comboStatutRecherche->addItem("Tous les statuts", "");
    comboStatutRecherche->addItem("En attente", "En attente");
    comboStatutRecherche->addItem("En cours", "En cours");
    comboStatutRecherche->addItem("Livrée", "Livree");
    comboStatutRecherche->addItem("Annulée", "Annulee");
    rechercheLayout->addWidget(comboStatutRecherche, 0, 1);
    
    // Ville
    rechercheLayout->addWidget(new QLabel("Ville:"), 0, 2);
    editVilleRecherche = new QLineEdit();
    editVilleRecherche->setPlaceholderText("Filtrer par ville...");
    rechercheLayout->addWidget(editVilleRecherche, 0, 3);
    
    // Date début
    rechercheLayout->addWidget(new QLabel("Date début:"), 1, 0);
    editDateDebutRecherche = new QDateEdit();
    editDateDebutRecherche->setCalendarPopup(true);
    editDateDebutRecherche->setDate(QDate::currentDate().addDays(-30));
    rechercheLayout->addWidget(editDateDebutRecherche, 1, 1);
    
    // Date fin
    rechercheLayout->addWidget(new QLabel("Date fin:"), 1, 2);
    editDateFinRecherche = new QDateEdit();
    editDateFinRecherche->setCalendarPopup(true);
    editDateFinRecherche->setDate(QDate::currentDate());
    rechercheLayout->addWidget(editDateFinRecherche, 1, 3);
    
    // Client ID
    rechercheLayout->addWidget(new QLabel("ID Client:"), 2, 0);
    spinClientRecherche = new QSpinBox();
    spinClientRecherche->setRange(0, 999999);
    spinClientRecherche->setValue(0);
    spinClientRecherche->setSpecialValueText("Tous les clients");
    rechercheLayout->addWidget(spinClientRecherche, 2, 1);
    
    // Boutons
    btnRechercher = new QPushButton("🔍 Rechercher");
    btnViderRecherche = new QPushButton("🗑️ Vider");
    rechercheLayout->addWidget(btnRechercher, 2, 2);
    rechercheLayout->addWidget(btnViderRecherche, 2, 3);
}

void CommandeWidget::setupPanneauActions()
{
    groupActions = new QGroupBox("Actions");
    actionsLayout = new QVBoxLayout(groupActions);
    
    // Actions CRUD
    btnAjouter = new QPushButton("➕ Ajouter une commande");
    btnModifier = new QPushButton("✏️ Modifier");
    btnSupprimer = new QPushButton("🗑️ Supprimer");
    btnActualiser = new QPushButton("🔄 Actualiser");
    
    // Actions métier
    btnAffecterLivreur = new QPushButton("🚚 Affecter un livreur");
    btnChangerStatut = new QPushButton("📝 Changer le statut");
    btnCommandesEnRetard = new QPushButton("⚠️ Commandes en retard");
    
    // Rapports
    btnGenererPDF = new QPushButton("📄 Générer PDF");
    btnGenererRapport = new QPushButton("📊 Rapport complet");
    
    // Ajout des boutons
    actionsLayout->addWidget(btnAjouter);
    actionsLayout->addWidget(btnModifier);
    actionsLayout->addWidget(btnSupprimer);
    actionsLayout->addWidget(new QFrame()); // Séparateur
    actionsLayout->addWidget(btnActualiser);
    actionsLayout->addWidget(btnAffecterLivreur);
    actionsLayout->addWidget(btnChangerStatut);
    actionsLayout->addWidget(btnCommandesEnRetard);
    actionsLayout->addWidget(new QFrame()); // Séparateur
    actionsLayout->addWidget(btnGenererPDF);
    actionsLayout->addWidget(btnGenererRapport);
    actionsLayout->addStretch();
    
    // Désactiver les boutons nécessitant une sélection
    btnModifier->setEnabled(false);
    btnSupprimer->setEnabled(false);
    btnAffecterLivreur->setEnabled(false);
    btnChangerStatut->setEnabled(false);
    btnGenererPDF->setEnabled(false);
}

void CommandeWidget::connecterSignaux()
{
    // Tableau
    connect(tableCommandes, &QTableWidget::itemSelectionChanged, 
            this, &CommandeWidget::selectionChangee);
    
    // Recherche
    connect(btnRechercher, &QPushButton::clicked, this, &CommandeWidget::rechercherCommandes);
    connect(btnViderRecherche, &QPushButton::clicked, this, &CommandeWidget::viderRecherche);
    connect(editVilleRecherche, &QLineEdit::returnPressed, this, &CommandeWidget::rechercherCommandes);
    
    // Actions
    connect(btnAjouter, &QPushButton::clicked, this, &CommandeWidget::ajouterCommande);
    connect(btnModifier, &QPushButton::clicked, this, &CommandeWidget::modifierCommande);
    connect(btnSupprimer, &QPushButton::clicked, this, &CommandeWidget::supprimerCommande);
    connect(btnActualiser, &QPushButton::clicked, this, &CommandeWidget::actualiserListe);
    connect(btnAffecterLivreur, &QPushButton::clicked, this, &CommandeWidget::affecterLivreur);
    connect(btnChangerStatut, &QPushButton::clicked, this, &CommandeWidget::changerStatut);
    connect(btnGenererPDF, &QPushButton::clicked, this, &CommandeWidget::genererPDF);
    connect(btnGenererRapport, &QPushButton::clicked, this, &CommandeWidget::genererRapport);
    connect(btnCommandesEnRetard, &QPushButton::clicked, this, &CommandeWidget::afficherCommandesEnRetard);
}

void CommandeWidget::appliquerStyle()
{
    // Style moderne pour les boutons
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
    btnAffecterLivreur->setStyleSheet(buttonStyle);
    btnChangerStatut->setStyleSheet(buttonStyle);
    btnGenererPDF->setStyleSheet(buttonStyle);
    btnGenererRapport->setStyleSheet(buttonStyle);
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
    btnCommandesEnRetard->setStyleSheet(R"(
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
    
    // Style pour le tableau
    tableCommandes->setStyleSheet(R"(
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

void CommandeWidget::chargerCommandes()
{
    QList<Commande> commandes = commandeService->obtenirToutesCommandes();
    chargerCommandes(commandes);
}

void CommandeWidget::chargerCommandes(const QList<Commande>& commandes)
{
    tableCommandes->setRowCount(commandes.size());
    
    for (int i = 0; i < commandes.size(); ++i) {
        const Commande& cmd = commandes[i];
        
        // ID avec couleur explicite
        QTableWidgetItem* idItem = new QTableWidgetItem(QString::number(cmd.getIdCommande()));
        idItem->setForeground(QColor("#000000")); // Noir
        tableCommandes->setItem(i, 0, idItem);
        
        // Date avec couleur explicite
        QTableWidgetItem* dateItem = new QTableWidgetItem(cmd.getDateCommande().toString("dd/MM/yyyy"));
        dateItem->setForeground(QColor("#000000")); // Noir
        tableCommandes->setItem(i, 1, dateItem);
        
        // Statut avec couleur
        QTableWidgetItem* statutItem = new QTableWidgetItem(cmd.getStatut());
        if (cmd.getStatut() == "En attente") {
            statutItem->setBackground(QColor("#FFA726"));  // Orange vif
            statutItem->setForeground(QColor("#FFFFFF"));  // Blanc
        } else if (cmd.getStatut() == "En cours") {
            statutItem->setBackground(QColor("#42A5F5"));  // Bleu vif
            statutItem->setForeground(QColor("#FFFFFF"));  // Blanc
        } else if (cmd.getStatut() == "Livree") {
            statutItem->setBackground(QColor("#66BB6A"));  // Vert vif
            statutItem->setForeground(QColor("#FFFFFF"));  // Blanc
        } else if (cmd.getStatut() == "Annulee") {
            statutItem->setBackground(QColor("#EF5350"));  // Rouge vif
            statutItem->setForeground(QColor("#FFFFFF"));  // Blanc
        }
        tableCommandes->setItem(i, 2, statutItem);
        
        // Ville avec couleur explicite
        QTableWidgetItem* villeItem = new QTableWidgetItem(cmd.getVilleLivraison());
        villeItem->setForeground(QColor("#000000")); // Noir
        tableCommandes->setItem(i, 3, villeItem);
        
        // Client avec couleur explicite
        QTableWidgetItem* clientItem = new QTableWidgetItem(QString::number(cmd.getIdClient()));
        clientItem->setForeground(QColor("#000000")); // Noir
        tableCommandes->setItem(i, 4, clientItem);
        
        // Livreur avec couleur explicite
        QString livreur = cmd.getIdLivreur() > 0 ? QString::number(cmd.getIdLivreur()) : "Non assigné";
        QTableWidgetItem* livreurItem = new QTableWidgetItem(livreur);
        livreurItem->setForeground(QColor("#000000")); // Noir
        tableCommandes->setItem(i, 5, livreurItem);
    }
    
    tableCommandes->resizeColumnsToContents();
}

void CommandeWidget::selectionChangee()
{
    int row = tableCommandes->currentRow();
    bool hasSelection = row >= 0;
    
    btnModifier->setEnabled(hasSelection);
    btnSupprimer->setEnabled(hasSelection);
    btnAffecterLivreur->setEnabled(hasSelection);
    btnChangerStatut->setEnabled(hasSelection);
    btnGenererPDF->setEnabled(hasSelection);
    
    if (hasSelection) {
        commandeSelectionnee = tableCommandes->item(row, 0)->text().toInt();
        mettreAJourDetails();
    } else {
        commandeSelectionnee = -1;
        labelDetails->setText("Aucune commande sélectionnée");
    }
}

void CommandeWidget::mettreAJourDetails()
{
    if (commandeSelectionnee <= 0) return;
    
    Commande cmd = commandeService->obtenirCommande(commandeSelectionnee);
    if (!cmd.isValid()) return;
    
    QString details = QString(R"(
<h3>Commande #%1</h3>
<p><b>📅 Date:</b> %2</p>
<p><b>📊 Statut:</b> <span style="color: %6;">%3</span></p>
<p><b>🏙️ Ville:</b> %4</p>
<p><b>👤 Client:</b> %5</p>
<p><b>🚚 Livreur:</b> %7</p>
    )").arg(cmd.getIdCommande())
       .arg(cmd.getDateCommande().toString("dd/MM/yyyy"))
       .arg(cmd.getStatut())
       .arg(cmd.getVilleLivraison())
       .arg(cmd.getIdClient())
       .arg(cmd.getStatut() == "Livree" ? "green" : cmd.getStatut() == "Annulee" ? "red" : "orange")
       .arg(cmd.getIdLivreur() > 0 ? QString("ID %1").arg(cmd.getIdLivreur()) : "Non assigné");
    
    labelDetails->setText(details);
}

void CommandeWidget::ajouterCommande()
{
    CommandeDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        Commande nouvelleCommande = dialog.getCommande();
        if (commandeService->ajouterCommande(nouvelleCommande)) {
            QMessageBox::information(this, "Succès", "Commande ajoutée avec succès!");
            actualiserListe();
        } else {
            QMessageBox::warning(this, "Erreur", "Erreur lors de l'ajout de la commande.");
        }
    }
}

void CommandeWidget::modifierCommande()
{
    if (commandeSelectionnee <= 0) return;
    
    Commande cmd = commandeService->obtenirCommande(commandeSelectionnee);
    CommandeDialog dialog(this, cmd);
    
    if (dialog.exec() == QDialog::Accepted) {
        Commande commandeModifiee = dialog.getCommande();
        if (commandeService->modifierCommande(commandeModifiee)) {
            QMessageBox::information(this, "Succès", "Commande modifiée avec succès!");
            actualiserListe();
        } else {
            QMessageBox::warning(this, "Erreur", "Erreur lors de la modification de la commande.");
        }
    }
}

void CommandeWidget::supprimerCommande()
{
    if (commandeSelectionnee <= 0) return;
    
    Commande cmd = commandeService->obtenirCommande(commandeSelectionnee);
    int ret = QMessageBox::question(this, "Confirmation", 
        QString("Êtes-vous sûr de vouloir supprimer la commande #%1 ?").arg(cmd.getIdCommande()),
        QMessageBox::Yes | QMessageBox::No);
    
    if (ret == QMessageBox::Yes) {
        if (commandeService->supprimerCommande(commandeSelectionnee)) {
            QMessageBox::information(this, "Succès", "Commande supprimée avec succès!");
            actualiserListe();
        } else {
            QMessageBox::warning(this, "Erreur", "Erreur lors de la suppression de la commande.");
        }
    }
}

void CommandeWidget::rechercherCommandes()
{
    QString statut = comboStatutRecherche->currentData().toString();
    QString ville = editVilleRecherche->text();
    QDate dateDebut = editDateDebutRecherche->date();
    QDate dateFin = editDateFinRecherche->date();
    
    QList<Commande> resultats = commandeService->rechercherCommandes(statut, ville, dateDebut, dateFin);
    
    // Filtrer par client si spécifié
    if (spinClientRecherche->value() > 0) {
        QList<Commande> filtrees;
        for (const Commande& cmd : resultats) {
            if (cmd.getIdClient() == spinClientRecherche->value()) {
                filtrees.append(cmd);
            }
        }
        resultats = filtrees;
    }
    
    chargerCommandes(resultats);
}

void CommandeWidget::viderRecherche()
{
    comboStatutRecherche->setCurrentIndex(0);
    editVilleRecherche->clear();
    editDateDebutRecherche->setDate(QDate::currentDate().addDays(-30));
    editDateFinRecherche->setDate(QDate::currentDate());
    spinClientRecherche->setValue(0);
    chargerCommandes();
}

void CommandeWidget::actualiserListe()
{
    chargerCommandes();
    selectionChangee();
}

void CommandeWidget::affecterLivreur()
{
    if (commandeSelectionnee <= 0) return;
    
    LivreurService livreurService;
    QList<Livreur> livreursDisponibles = livreurService.obtenirLivreursDisponibles();
    
    if (livreursDisponibles.isEmpty()) {
        QMessageBox::information(this, "Information", "Aucun livreur disponible.");
        return;
    }
    
    QStringList livreurs;
    QList<int> ids;
    for (const Livreur& livreur : livreursDisponibles) {
        livreurs << QString("%1 - %2").arg(livreur.getNom()).arg(livreur.getZoneLivraison());
        ids << livreur.getIdLivreur();
    }
    
    bool ok;
    QString selection = QInputDialog::getItem(this, "Affecter un livreur", 
        "Choisissez un livreur:", livreurs, 0, false, &ok);
    
    if (ok && !selection.isEmpty()) {
        int index = livreurs.indexOf(selection);
        int idLivreur = ids[index];
        
        if (commandeService->affecterLivreur(commandeSelectionnee, idLivreur)) {
            QMessageBox::information(this, "Succès", "Livreur affecté avec succès!");
            actualiserListe();
        } else {
            QMessageBox::warning(this, "Erreur", "Erreur lors de l'affectation du livreur.");
        }
    }
}

void CommandeWidget::changerStatut()
{
    if (commandeSelectionnee <= 0) return;
    
    QStringList statuts = {"En attente", "En cours", "Livree", "Annulee"};
    bool ok;
    QString nouveauStatut = QInputDialog::getItem(this, "Changer le statut", 
        "Nouveau statut:", statuts, 0, false, &ok);
    
    if (ok && !nouveauStatut.isEmpty()) {
        Commande cmd = commandeService->obtenirCommande(commandeSelectionnee);
        cmd.setStatut(nouveauStatut);
        
        if (commandeService->modifierCommande(cmd)) {
            QMessageBox::information(this, "Succès", "Statut modifié avec succès!");
            actualiserListe();
        } else {
            QMessageBox::warning(this, "Erreur", "Erreur lors de la modification du statut.");
        }
    }
}

void CommandeWidget::genererPDF()
{
    if (commandeSelectionnee <= 0) return;
    
    QString fileName = QFileDialog::getSaveFileName(this, "Enregistrer le PDF",
        QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + 
        QString("/commande_%1.pdf").arg(commandeSelectionnee),
        "Fichiers PDF (*.pdf)");
    
    if (!fileName.isEmpty()) {
        if (commandeService->genererPDFCommande(commandeSelectionnee, fileName)) {
            QMessageBox::information(this, "Succès", "PDF généré avec succès!");
        } else {
            QMessageBox::warning(this, "Erreur", "Erreur lors de la génération du PDF.");
        }
    }
}

void CommandeWidget::genererRapport()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Enregistrer le rapport",
        QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/rapport_commandes.pdf",
        "Fichiers PDF (*.pdf)");
    
    if (!fileName.isEmpty()) {
        if (commandeService->genererRapportCommandes(fileName)) {
            QMessageBox::information(this, "Succès", "Rapport généré avec succès!");
        } else {
            QMessageBox::warning(this, "Erreur", "Erreur lors de la génération du rapport.");
        }
    }
}

void CommandeWidget::afficherCommandesEnRetard()
{
    QList<Commande> commandesEnRetard = commandeService->obtenirCommandesEnRetard();
    chargerCommandes(commandesEnRetard);
    
    QMessageBox::information(this, "Commandes en retard", 
        QString("Nombre de commandes en retard: %1").arg(commandesEnRetard.size()));
}
