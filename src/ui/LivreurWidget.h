#ifndef LIVREURWIDGET_H
#define LIVREURWIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QPushButton>
#include <QLineEdit>
#include <QComboBox>
#include <QLabel>
#include <QGroupBox>
#include <QSplitter>
#include <QCheckBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QFileDialog>
#include "entities/Livreur.h"
#include "services/LivreurService.h"

class LivreurDialog;

class LivreurWidget : public QWidget
{
    Q_OBJECT

public:
    explicit LivreurWidget(QWidget *parent = nullptr);
    
public slots:
    void actualiserListe();

private slots:
    void ajouterLivreur();
    void modifierLivreur();
    void supprimerLivreur();
    void rechercherLivreurs();
    void viderRecherche();
    void selectionChangee();
    void changerDisponibilite();
    void afficherLivreursSurcharges();
    void afficherLivreursDisponibles();
    void genererRapportLivreurs();
    void genererListeLivreurs();

private:
    void setupUI();
    void setupTableau();
    void setupPanneauRecherche();
    void setupPanneauActions();
    void connecterSignaux();
    void chargerLivreurs();
    void chargerLivreurs(const QList<Livreur>& livreurs);
    void mettreAJourDetails();
    void appliquerStyle();
    
    // Interface principale
    QVBoxLayout* mainLayout;
    QSplitter* splitter;
    
    // Tableau des livreurs
    QTableWidget* tableLivreurs;
    
    // Panneau de recherche
    QGroupBox* groupRecherche;
    QGridLayout* rechercheLayout;
    QLineEdit* editNomRecherche;
    QLineEdit* editZoneRecherche;
    QComboBox* comboDisponibiliteRecherche;
    QPushButton* btnRechercher;
    QPushButton* btnViderRecherche;
    
    // Panneau d'actions
    QGroupBox* groupActions;
    QVBoxLayout* actionsLayout;
    QPushButton* btnAjouter;
    QPushButton* btnModifier;
    QPushButton* btnSupprimer;
    QPushButton* btnActualiser;
    QPushButton* btnChangerDisponibilite;
    QPushButton* btnLivreursSurcharges;
    QPushButton* btnLivreursDisponibles;
    QPushButton* btnGenererRapport;
    QPushButton* btnGenererListe;
    
    // Panneau de détails
    QGroupBox* groupDetails;
    QLabel* labelDetails;
    
    // Services
    LivreurService* livreurService;
    int livreurSelectionne;
};

// Dialogue pour ajouter/modifier un livreur
class LivreurDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LivreurDialog(QWidget* parent = nullptr, const Livreur& livreur = Livreur());
    Livreur getLivreur() const;

private:
    void setupUI();
    void remplirFormulaire(const Livreur& livreur);
    
    QFormLayout* formLayout;
    QLineEdit* editNom;
    QLineEdit* editTelephone;
    QLineEdit* editZone;
    QLineEdit* editVehicule;
    QCheckBox* checkDisponibilite;
    QDialogButtonBox* buttonBox;
    
    bool modeModification;
    int idLivreur;
};

#endif // LIVREURWIDGET_H
