#include "LivreurService.h"
#include "db/DatabaseManager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QDebug>
#include <QDate>

LivreurService::LivreurService(QObject *parent)
    : QObject(parent)
{
}

bool LivreurService::ajouterLivreur(const Livreur& livreur)
{
    DatabaseManager* db = DatabaseManager::getInstance();
    
    QString query = "INSERT INTO LIVREURS (nom, telephone, zone_livraison, vehicule, disponibilite) "
                   "VALUES (?, ?, ?, ?, ?)";
    
    QVariantList values;
    values << livreur.getNom() 
           << livreur.getTelephone()
           << livreur.getZoneLivraison()
           << livreur.getVehicule()
           << (livreur.getDisponibilite() ? 1 : 0);
    
    QSqlQuery result = db->executePreparedQuery(query, values);
    return !result.lastError().isValid();
}

Livreur LivreurService::obtenirLivreur(int id)
{
    DatabaseManager* db = DatabaseManager::getInstance();
    
    QString query = "SELECT * FROM LIVREURS WHERE id_livreur = ?";
    QVariantList values;
    values << id;
    
    QSqlQuery result = db->executePreparedQuery(query, values);
    
    if (result.next()) {
        return mapFromQuery(result);
    }
    
    return Livreur(); // Retourne un livreur vide si non trouvé
}

QList<Livreur> LivreurService::obtenirTousLivreurs()
{
    QList<Livreur> livreurs;
    DatabaseManager* db = DatabaseManager::getInstance();
    
    QString query = "SELECT * FROM LIVREURS ORDER BY nom";
    QSqlQuery result = db->executePreparedQuery(query, {});
    
    while (result.next()) {
        livreurs.append(mapFromQuery(result));
    }
    
    return livreurs;
}

bool LivreurService::modifierLivreur(const Livreur& livreur)
{
    DatabaseManager* db = DatabaseManager::getInstance();
    
    QString query = "UPDATE LIVREURS SET nom = ?, telephone = ?, zone_livraison = ?, "
                   "vehicule = ?, disponibilite = ? WHERE id_livreur = ?";
    
    QVariantList values;
    values << livreur.getNom()
           << livreur.getTelephone()
           << livreur.getZoneLivraison()
           << livreur.getVehicule()
           << (livreur.getDisponibilite() ? 1 : 0)
           << livreur.getIdLivreur();
    
    QSqlQuery result = db->executePreparedQuery(query, values);
    return !result.lastError().isValid();
}

bool LivreurService::supprimerLivreur(int id)
{
    DatabaseManager* db = DatabaseManager::getInstance();
    
    // D'abord, compter les commandes qui seront supprimées en cascade
    int commandesASupprimer = compterToutesCommandes(id);
    
    // Informer l'utilisateur si des commandes seront supprimées
    if (commandesASupprimer > 0) {
        qDebug() << "ATTENTION: La suppression du livreur ID" << id 
                 << "supprimera également" << commandesASupprimer 
                 << "commande(s) associée(s) (CASCADE)";
    }
    
    // Procéder à la suppression (CASCADE automatique grâce à la contrainte)
    QString query = "DELETE FROM LIVREURS WHERE id_livreur = ?";
    QVariantList values;
    values << id;
    
    QSqlQuery result = db->executePreparedQuery(query, values);
    
    if (!result.lastError().isValid()) {
        qDebug() << "Livreur supprimé avec succès (ID:" << id << ")";
        if (commandesASupprimer > 0) {
            qDebug() << "Commandes associées supprimées automatiquement:" << commandesASupprimer;
        }
        return true;
    } else {
        qDebug() << "Erreur suppression livreur:" << result.lastError().text();
        return false;
    }
}

QList<Livreur> LivreurService::rechercherLivreurs(const QString& nom, const QString& zone, bool disponibiliteSeule)
{
    QList<Livreur> livreurs;
    DatabaseManager* db = DatabaseManager::getInstance();
    
    QString query = "SELECT * FROM LIVREURS WHERE 1=1";
    QVariantList values;
    
    if (!nom.isEmpty()) {
        query += " AND UPPER(nom) LIKE UPPER(?)";
        values << "%" + nom + "%";
    }
    
    if (!zone.isEmpty()) {
        query += " AND UPPER(zone_livraison) LIKE UPPER(?)";
        values << "%" + zone + "%";
    }
    
    if (disponibiliteSeule) {
        query += " AND disponibilite = 1";
    }
    
    query += " ORDER BY nom";
    
    QSqlQuery result = db->executePreparedQuery(query, values);
    
    while (result.next()) {
        livreurs.append(mapFromQuery(result));
    }
    
    return livreurs;
}

QList<Livreur> LivreurService::trierLivreurs(const QString& critere, bool croissant)
{
    QList<Livreur> livreurs;
    DatabaseManager* db = DatabaseManager::getInstance();
    
    QString query = "SELECT * FROM LIVREURS ORDER BY ";
    
    if (critere == "nom") {
        query += "nom";
    } else if (critere == "zone") {
        query += "zone_livraison";
    } else if (critere == "vehicule") {
        query += "vehicule";
    } else if (critere == "disponibilite") {
        query += "disponibilite";
    } else {
        query += "nom"; // Par défaut
    }
    
    query += croissant ? " ASC" : " DESC";
    
    QSqlQuery result = db->executePreparedQuery(query, {});
    
    while (result.next()) {
        livreurs.append(mapFromQuery(result));
    }
    
    return livreurs;
}

bool LivreurService::mettreAJourDisponibilite(int idLivreur, bool disponible)
{
    DatabaseManager* db = DatabaseManager::getInstance();
    
    QString query = "UPDATE livreurs SET disponibilite = ? WHERE id_livreur = ?";
    QVariantList values;
    values << (disponible ? 1 : 0) << idLivreur;
    
    QSqlQuery result = db->executePreparedQuery(query, values);
    return !result.lastError().isValid();
}

QList<Livreur> LivreurService::obtenirLivreursDisponibles()
{
    QList<Livreur> livreurs;
    DatabaseManager* db = DatabaseManager::getInstance();
    
    QString query = "SELECT * FROM LIVREURS WHERE disponibilite = 1 ORDER BY nom";
    QSqlQuery result = db->executePreparedQuery(query, {});
    
    while (result.next()) {
        livreurs.append(mapFromQuery(result));
    }
    
    return livreurs;
}

QList<Livreur> LivreurService::obtenirLivreursSurcharges()
{
    QList<Livreur> livreursSurcharges;
    DatabaseManager* db = DatabaseManager::getInstance();
    
    QString query = "SELECT l.id_livreur, l.nom, l.telephone, l.zone_livraison, l.vehicule, l.disponibilite, COUNT(c.id_commande) as nb_commandes "
                   "FROM LIVREURS l "
                   "LEFT JOIN COMMANDES c ON l.id_livreur = c.id_livreur "
                   "AND c.statut IN ('En attente', 'En cours') "
                   "GROUP BY l.id_livreur, l.nom, l.telephone, l.zone_livraison, l.vehicule, l.disponibilite "
                   "HAVING COUNT(c.id_commande) > 5 "
                   "ORDER BY nb_commandes DESC";
    
    QSqlQuery result = db->executePreparedQuery(query, {});
    
    while (result.next()) {
        livreursSurcharges.append(mapFromQuery(result));
    }
    
    return livreursSurcharges;
}

Livreur LivreurService::obtenirMeilleurLivreur(const QString& zone)
{
    DatabaseManager* db = DatabaseManager::getInstance();
    
    QString query = "SELECT l.id_livreur, l.nom, l.telephone, l.zone_livraison, l.vehicule, l.disponibilite, COUNT(c.id_commande) as nb_commandes "
                   "FROM LIVREURS l "
                   "LEFT JOIN COMMANDES c ON l.id_livreur = c.id_livreur "
                   "WHERE l.disponibilite = 1";
    
    QVariantList values;
    if (!zone.isEmpty()) {
        query += " AND UPPER(l.zone_livraison) = UPPER(?)";
        values << zone;
    }
    
    query += " GROUP BY l.id_livreur, l.nom, l.telephone, l.zone_livraison, l.vehicule, l.disponibilite "
             "ORDER BY COUNT(c.id_commande) ASC, l.nom ASC";
    
    QSqlQuery result = db->executePreparedQuery(query, values);
    
    if (result.next()) {
        return mapFromQuery(result);
    }
    
    return Livreur(); // Aucun livreur disponible
}

QMap<QString, int> LivreurService::obtenirStatistiquesParZone()
{
    QMap<QString, int> statistiques;
    DatabaseManager* db = DatabaseManager::getInstance();
    
    QString query = "SELECT zone_livraison, COUNT(*) as nombre "
                   "FROM LIVREURS "
                   "GROUP BY zone_livraison "
                   "ORDER BY nombre DESC";
    
    QSqlQuery result = db->executePreparedQuery(query, {});
    
    while (result.next()) {
        QString zone = result.value("zone_livraison").toString();
        int nombre = result.value("nombre").toInt();
        statistiques[zone] = nombre;
    }
    
    return statistiques;
}

QMap<bool, int> LivreurService::obtenirStatistiquesDisponibilite()
{
    QMap<bool, int> statistiques;
    DatabaseManager* db = DatabaseManager::getInstance();
    
    QString query = "SELECT disponibilite, COUNT(*) as nombre "
                   "FROM livreurs "
                   "GROUP BY disponibilite";
    
    QSqlQuery result = db->executePreparedQuery(query, {});
    
    while (result.next()) {
        bool disponible = result.value("disponibilite").toInt() == 1;
        int nombre = result.value("nombre").toInt();
        statistiques[disponible] = nombre;
    }
    
    return statistiques;
}

QMap<int, int> LivreurService::obtenirStatistiquesChargesDeTravail()
{
    QMap<int, int> statistiques;
    DatabaseManager* db = DatabaseManager::getInstance();
    
    QString query = "SELECT l.id_livreur, COUNT(c.id_commande) as charge "
                   "FROM livreurs l "
                   "LEFT JOIN commandes c ON l.id_livreur = c.id_livreur "
                   "AND c.statut IN ('En attente', 'En cours') "
                   "GROUP BY l.id_livreur "
                   "ORDER BY charge DESC";
    
    QSqlQuery result = db->executePreparedQuery(query, {});
    
    while (result.next()) {
        int idLivreur = result.value("id_livreur").toInt();
        int charge = result.value("charge").toInt();
        statistiques[idLivreur] = charge;
    }
    
    return statistiques;
}

bool LivreurService::genererRapportLivreurs(const QString& cheminFichier)
{
    // TODO: Implémenter la génération de rapport PDF
    Q_UNUSED(cheminFichier)
    qDebug() << "Génération de rapport PDF pour les livreurs - À implémenter";
    return true;
}

bool LivreurService::genererListeLivreurs(const QString& cheminFichier)
{
    // TODO: Implémenter la génération de liste Excel
    Q_UNUSED(cheminFichier)
    qDebug() << "Génération de liste Excel pour les livreurs - À implémenter";
    return true;
}

Livreur LivreurService::mapFromQuery(const QSqlQuery& query)
{
    Livreur livreur;
    livreur.setIdLivreur(query.value("id_livreur").toInt());
    livreur.setNom(query.value("nom").toString());
    livreur.setTelephone(query.value("telephone").toString());
    livreur.setZoneLivraison(query.value("zone_livraison").toString());
    livreur.setVehicule(query.value("vehicule").toString());
    livreur.setDisponibilite(query.value("disponibilite").toInt() == 1);
    return livreur;
}

int LivreurService::obtenirProchainId()
{
    DatabaseManager* db = DatabaseManager::getInstance();
    
    QString query = "SELECT COALESCE(MAX(id_livreur), 0) + 1 as next_id FROM livreurs";
    QSqlQuery result = db->executePreparedQuery(query, {});
    
    if (result.next()) {
        return result.value("next_id").toInt();
    }
    
    return 1;
}

int LivreurService::compterCommandesActives(int idLivreur)
{
    DatabaseManager* db = DatabaseManager::getInstance();
    
    QString query = "SELECT COUNT(*) as nombre FROM commandes "
                   "WHERE id_livreur = ? AND statut IN ('En attente', 'En cours')";
    
    QVariantList values;
    values << idLivreur;
    
    QSqlQuery result = db->executePreparedQuery(query, values);
    
    if (result.next()) {
        return result.value("nombre").toInt();
    }
    
    return 0;
}

int LivreurService::compterToutesCommandes(int idLivreur)
{
    DatabaseManager* db = DatabaseManager::getInstance();
    
    QString query = "SELECT COUNT(*) as nombre FROM COMMANDES WHERE id_livreur = ?";
    
    QVariantList values;
    values << idLivreur;
    
    QSqlQuery result = db->executePreparedQuery(query, values);
    
    if (result.next()) {
        return result.value("nombre").toInt();
    }
    
    return 0;
}
