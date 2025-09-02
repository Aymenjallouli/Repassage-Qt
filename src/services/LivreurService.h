#ifndef LIVREURSERVICE_H
#define LIVREURSERVICE_H

#include <QObject>
#include <QList>
#include <QMap>
#include <QString>
#include "entities/Livreur.h"

class QSqlQuery;

class LivreurService : public QObject
{
    Q_OBJECT

public:
    explicit LivreurService(QObject *parent = nullptr);

    // CRUD de base
    bool ajouterLivreur(const Livreur& livreur);
    Livreur obtenirLivreur(int id);
    QList<Livreur> obtenirTousLivreurs();
    bool modifierLivreur(const Livreur& livreur);
    bool supprimerLivreur(int id);

    // Recherche et filtrage
    QList<Livreur> rechercherLivreurs(const QString& nom = "", 
                                     const QString& zone = "", 
                                     bool disponibiliteSeule = false);
    QList<Livreur> trierLivreurs(const QString& critere, bool croissant = true);

    // Fonctionnalités métier
    bool mettreAJourDisponibilite(int idLivreur, bool disponible);
    QList<Livreur> obtenirLivreursDisponibles();
    QList<Livreur> obtenirLivreursSurcharges();
    Livreur obtenirMeilleurLivreur(const QString& zone);

    // Statistiques
    QMap<QString, int> obtenirStatistiquesParZone();
    QMap<bool, int> obtenirStatistiquesDisponibilite();
    QMap<int, int> obtenirStatistiquesChargesDeTravail();

    // Génération de rapports
    bool genererRapportLivreurs(const QString& cheminFichier);
    bool genererListeLivreurs(const QString& cheminFichier);
    
    // Méthodes utilitaires publiques
    int compterCommandesActives(int idLivreur);
    int compterToutesCommandes(int idLivreur); // Nouvelle méthode pour toutes les commandes
    
private:
    Livreur mapFromQuery(const QSqlQuery& query);
    int obtenirProchainId();
};

#endif // LIVREURSERVICE_H
