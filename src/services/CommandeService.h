#ifndef COMMANDESERVICE_H
#define COMMANDESERVICE_H

#include <QList>
#include <QVariant>
#include <QSqlQuery>
#include <QDate>
#include "entities/Commande.h"

class CommandeService
{
public:
    CommandeService();
    
    // CRUD Operations
    bool ajouterCommande(const Commande& commande);
    Commande obtenirCommande(int id);
    QList<Commande> obtenirToutesCommandes();
    bool modifierCommande(const Commande& commande);
    bool supprimerCommande(int id);
    
    // Recherche et tri multicritères
    QList<Commande> rechercherCommandes(const QString& statut = "", 
                                       const QString& ville = "",
                                       const QDate& dateDebut = QDate(),
                                       const QDate& dateFin = QDate());
    
    QList<Commande> trierCommandes(const QString& critere, bool croissant = true);
    
    // Fonctionnalités métier
    double calculerDelaiMoyenLivraison();
    bool affecterLivreur(int idCommande, int idLivreur);
    QList<Commande> obtenirCommandesEnRetard();
    
    // Statistiques
    QMap<QString, int> obtenirStatistiquesParStatut();
    QMap<QString, int> obtenirStatistiquesParVille();
    QMap<QDate, int> obtenirStatistiquesParDate();
    
    // Génération de documents
    bool genererPDFCommande(int idCommande, const QString& cheminFichier);
    bool genererRapportCommandes(const QString& cheminFichier);
    
private:
    Commande mapFromQuery(const QSqlQuery& query);
    int obtenirProchainId();
};

#endif // COMMANDESERVICE_H
