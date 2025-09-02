#ifndef COMMANDEWIDGET_H
#define COMMANDEWIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QPushButton>
#include <QLineEdit>
#include <QComboBox>
#include <QDateEdit>
#include <QLabel>
#include <QGroupBox>
#include <QSplitter>
#include <QSpinBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QFileDialog>
#include "entities/Commande.h"
#include "services/CommandeService.h"

class CommandeDialog;

class CommandeWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CommandeWidget(QWidget *parent = nullptr);
    
public slots:
    void actualiserListe();

private slots:
    void ajouterCommande();
    void modifierCommande();
    void supprimerCommande();
    void rechercherCommandes();
    void viderRecherche();
    void selectionChangee();
    void affecterLivreur();
    void changerStatut();
    void genererPDF();
    void genererRapport();
    void afficherCommandesEnRetard();

private:
    void setupUI();
    void setupTableau();
    void setupPanneauRecherche();
    void setupPanneauActions();
    void connecterSignaux();
    void chargerCommandes();
    void chargerCommandes(const QList<Commande>& commandes);
    void mettreAJourDetails();
    void appliquerStyle();
    
    // Interface principale
    QVBoxLayout* mainLayout;
    QSplitter* splitter;
    
    // Tableau des commandes
    QTableWidget* tableCommandes;
    
    // Panneau de recherche
    QGroupBox* groupRecherche;
    QGridLayout* rechercheLayout;
    QComboBox* comboStatutRecherche;
    QLineEdit* editVilleRecherche;
    QDateEdit* editDateDebutRecherche;
    QDateEdit* editDateFinRecherche;
    QSpinBox* spinClientRecherche;
    QPushButton* btnRechercher;
    QPushButton* btnViderRecherche;
    
    // Panneau d'actions
    QGroupBox* groupActions;
    QVBoxLayout* actionsLayout;
    QPushButton* btnAjouter;
    QPushButton* btnModifier;
    QPushButton* btnSupprimer;
    QPushButton* btnActualiser;
    QPushButton* btnAffecterLivreur;
    QPushButton* btnChangerStatut;
    QPushButton* btnGenererPDF;
    QPushButton* btnGenererRapport;
    QPushButton* btnCommandesEnRetard;
    
    // Panneau de détails
    QGroupBox* groupDetails;
    QLabel* labelDetails;
    
    // Services
    CommandeService* commandeService;
    int commandeSelectionnee;
};

// Dialogue pour ajouter/modifier une commande
class CommandeDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CommandeDialog(QWidget* parent = nullptr, const Commande& commande = Commande());
    Commande getCommande() const;

private:
    void setupUI();
    void remplirFormulaire(const Commande& commande);
    
    QFormLayout* formLayout;
    QDateEdit* editDateCommande;
    QComboBox* comboStatut;
    QLineEdit* editVilleLivraison;
    QSpinBox* spinIdClient;
    QComboBox* comboLivreur;
    QDialogButtonBox* buttonBox;
    
    bool modeModification;
    int idCommande;
};

#endif // COMMANDEWIDGET_H
