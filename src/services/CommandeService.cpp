#include "CommandeService.h"
#include "db/DatabaseManager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QMap>
#include <QPdfWriter>
#include <QPainter>
#include <QStandardPaths>

CommandeService::CommandeService()
{
}

bool CommandeService::ajouterCommande(const Commande& commande)
{
    if (!commande.isValid()) {
        return false;
    }
    
    DatabaseManager* db = DatabaseManager::getInstance();
    QString query = "INSERT INTO COMMANDES (date_commande, statut, ville_livraison, id_client, id_livreur) "
                   "VALUES (?, ?, ?, ?, ?)";
    
    QVariantList values;
    values << commande.getDateCommande()
           << commande.getStatut()
           << commande.getVilleLivraison()
           << commande.getIdClient()
           << (commande.getIdLivreur() > 0 ? commande.getIdLivreur() : QVariant());
    
    QSqlQuery result = db->executePreparedQuery(query, values);
    return !result.lastError().isValid();
}

Commande CommandeService::obtenirCommande(int id)
{
    DatabaseManager* db = DatabaseManager::getInstance();
    QString query = "SELECT * FROM COMMANDES WHERE id_commande = ?";
    
    QSqlQuery result = db->executePreparedQuery(query, {id});
    
    if (result.next()) {
        return mapFromQuery(result);
    }
    
    return Commande();
}

QList<Commande> CommandeService::obtenirToutesCommandes()
{
    QList<Commande> commandes;
    DatabaseManager* db = DatabaseManager::getInstance();
    QString query = "SELECT * FROM COMMANDES ORDER BY date_commande DESC";
    
    QSqlQuery result = db->executeQuery(query);
    
    while (result.next()) {
        commandes.append(mapFromQuery(result));
    }
    
    return commandes;
}

bool CommandeService::modifierCommande(const Commande& commande)
{
    if (!commande.isValid()) {
        return false;
    }
    
    DatabaseManager* db = DatabaseManager::getInstance();
    QString query = "UPDATE COMMANDES SET date_commande = ?, statut = ?, "
                   "ville_livraison = ?, id_client = ?, id_livreur = ? "
                   "WHERE id_commande = ?";
    
    QVariantList values;
    values << commande.getDateCommande()
           << commande.getStatut()
           << commande.getVilleLivraison()
           << commande.getIdClient()
           << (commande.getIdLivreur() > 0 ? commande.getIdLivreur() : QVariant())
           << commande.getIdCommande();
    
    QSqlQuery result = db->executePreparedQuery(query, values);
    return !result.lastError().isValid();
}

bool CommandeService::supprimerCommande(int id)
{
    DatabaseManager* db = DatabaseManager::getInstance();
    QString query = "DELETE FROM COMMANDES WHERE id_commande = ?";
    
    QSqlQuery result = db->executePreparedQuery(query, {id});
    return !result.lastError().isValid();
}

QList<Commande> CommandeService::rechercherCommandes(const QString& statut, 
                                                     const QString& ville,
                                                     const QDate& dateDebut, 
                                                     const QDate& dateFin)
{
    QList<Commande> commandes;
    DatabaseManager* db = DatabaseManager::getInstance();
    
    QString query = "SELECT * FROM COMMANDES WHERE 1=1";
    QVariantList values;
    
    if (!statut.isEmpty()) {
        query += " AND statut = ?";
        values << statut;
    }
    
    if (!ville.isEmpty()) {
        query += " AND ville_livraison LIKE ?";
        values << QString("%" + ville + "%");
    }
    
    if (dateDebut.isValid()) {
        query += " AND date_commande >= ?";
        values << dateDebut;
    }
    
    if (dateFin.isValid()) {
        query += " AND date_commande <= ?";
        values << dateFin;
    }
    
    query += " ORDER BY date_commande DESC";
    
    QSqlQuery result = db->executePreparedQuery(query, values);
    
    while (result.next()) {
        commandes.append(mapFromQuery(result));
    }
    
    return commandes;
}

QList<Commande> CommandeService::trierCommandes(const QString& critere, bool croissant)
{
    QList<Commande> commandes;
    DatabaseManager* db = DatabaseManager::getInstance();
    
    QString ordre = croissant ? "ASC" : "DESC";
    QString query = QString("SELECT * FROM COMMANDES ORDER BY %1 %2").arg(critere, ordre);
    
    QSqlQuery result = db->executeQuery(query);
    
    while (result.next()) {
        commandes.append(mapFromQuery(result));
    }
    
    return commandes;
}

double CommandeService::calculerDelaiMoyenLivraison()
{
    DatabaseManager* db = DatabaseManager::getInstance();
    QString query = "SELECT AVG(EXTRACT(DAY FROM (SYSDATE - date_commande))) as delai_moyen "
                   "FROM COMMANDES WHERE statut = 'Livree'";
    
    QSqlQuery result = db->executeQuery(query);
    
    if (result.next()) {
        return result.value("delai_moyen").toDouble();
    }
    
    return 0.0;
}

bool CommandeService::affecterLivreur(int idCommande, int idLivreur)
{
    DatabaseManager* db = DatabaseManager::getInstance();
    QString query = "UPDATE COMMANDES SET id_livreur = ?, statut = 'En cours' "
                   "WHERE id_commande = ?";
    
    QSqlQuery result = db->executePreparedQuery(query, {idLivreur, idCommande});
    return !result.lastError().isValid();
}

QMap<QString, int> CommandeService::obtenirStatistiquesParStatut()
{
    QMap<QString, int> statistiques;
    DatabaseManager* db = DatabaseManager::getInstance();
    QString query = "SELECT statut, COUNT(*) as nombre FROM COMMANDES GROUP BY statut";
    
    QSqlQuery result = db->executeQuery(query);
    
    while (result.next()) {
        statistiques[result.value("statut").toString()] = result.value("nombre").toInt();
    }
    
    return statistiques;
}

QMap<QString, int> CommandeService::obtenirStatistiquesParVille()
{
    QMap<QString, int> statistiques;
    DatabaseManager* db = DatabaseManager::getInstance();
    QString query = "SELECT ville_livraison, COUNT(*) as nombre FROM COMMANDES GROUP BY ville_livraison";
    
    QSqlQuery result = db->executeQuery(query);
    
    while (result.next()) {
        statistiques[result.value("ville_livraison").toString()] = result.value("nombre").toInt();
    }
    
    return statistiques;
}

bool CommandeService::genererPDFCommande(int idCommande, const QString& cheminFichier)
{
    Commande commande = obtenirCommande(idCommande);
    if (!commande.isValid()) {
        return false;
    }
    
    QPdfWriter writer(cheminFichier);
    writer.setPageSize(QPageSize::A4);
    writer.setResolution(300); // 300 DPI pour une meilleure qualité
    
    QPainter painter(&writer);
    painter.setFont(QFont("Arial", 16, QFont::Bold));
    
    // Configuration des dimensions (en unités de périphérique)
    int pageWidth = writer.width();
    int margin = pageWidth / 20; // Marge de 5%
    int lineHeight = writer.height() / 50; // Hauteur de ligne adaptée
    int y = margin;
    
    // Titre principal
    painter.drawText(margin, y, "BON DE COMMANDE");
    y += lineHeight * 2;
    
    // Informations de la commande
    painter.setFont(QFont("Arial", 12));
    painter.drawText(margin, y, QString("Commande #%1").arg(commande.getIdCommande()));
    y += lineHeight;
    painter.drawText(margin, y, QString("Date: %1").arg(commande.getDateCommande().toString("dd/MM/yyyy")));
    y += lineHeight;
    painter.drawText(margin, y, QString("Statut: %1").arg(commande.getStatut()));
    y += lineHeight;
    painter.drawText(margin, y, QString("Ville de livraison: %1").arg(commande.getVilleLivraison()));
    y += lineHeight;
    painter.drawText(margin, y, QString("Client ID: %1").arg(commande.getIdClient()));
    
    if (commande.getIdLivreur() > 0) {
        y += lineHeight;
        painter.drawText(margin, y, QString("Livreur ID: %1").arg(commande.getIdLivreur()));
    }
    
    return true;
}

bool CommandeService::genererRapportCommandes(const QString& cheminFichier)
{
    QList<Commande> commandes = obtenirToutesCommandes();
    
    QPdfWriter writer(cheminFichier);
    writer.setPageSize(QPageSize::A4);
    writer.setResolution(300); // 300 DPI pour une meilleure qualité
    
    QPainter painter(&writer);
    
    // Configuration des dimensions (en unités de périphérique)
    int pageWidth = writer.width();
    int pageHeight = writer.height();
    int margin = pageWidth / 20; // Marge de 5%
    int lineHeight = pageHeight / 60; // Hauteur de ligne adaptée
    int y = margin;
    
    // Titre principal
    painter.setFont(QFont("Arial", 18, QFont::Bold));
    painter.drawText(margin, y, "RAPPORT DES COMMANDES");
    y += lineHeight * 2;
    
    // Informations générales
    painter.setFont(QFont("Arial", 12));
    painter.drawText(margin, y, QString("Date de génération: %1").arg(QDate::currentDate().toString("dd/MM/yyyy")));
    y += lineHeight;
    painter.drawText(margin, y, QString("Nombre total de commandes: %1").arg(commandes.size()));
    y += lineHeight * 2;
    
    // En-têtes du tableau
    painter.setFont(QFont("Arial", 10, QFont::Bold));
    int col1 = margin;
    int col2 = margin + pageWidth / 8;
    int col3 = margin + pageWidth / 4;
    int col4 = margin + pageWidth / 2;
    int col5 = margin + 3 * pageWidth / 4;
    
    painter.drawText(col1, y, "ID");
    painter.drawText(col2, y, "Date");
    painter.drawText(col3, y, "Statut");
    painter.drawText(col4, y, "Ville");
    painter.drawText(col5, y, "Client");
    y += lineHeight;
    
    // Ligne de séparation
    painter.drawLine(margin, y, pageWidth - margin, y);
    y += lineHeight / 2;
    
    // Données
    painter.setFont(QFont("Arial", 9));
    for (const Commande& commande : commandes) {
        if (y > pageHeight - margin * 2) { // Nouvelle page si nécessaire
            writer.newPage();
            y = margin;
        }
        
        painter.drawText(col1, y, QString::number(commande.getIdCommande()));
        painter.drawText(col2, y, commande.getDateCommande().toString("dd/MM/yyyy"));
        painter.drawText(col3, y, commande.getStatut());
        painter.drawText(col4, y, commande.getVilleLivraison());
        painter.drawText(col5, y, QString::number(commande.getIdClient()));
        y += lineHeight;
    }
    
    return true;
}

QList<Commande> CommandeService::obtenirCommandesEnRetard()
{
    DatabaseManager* db = DatabaseManager::getInstance();
    QString query = "SELECT * FROM COMMANDES WHERE statut = 'En retard' OR "
                   "(statut = 'En cours' AND date_commande < ?)";
    
    // Commandes de plus de 7 jours considérées en retard
    QDate dateRetard = QDate::currentDate().addDays(-7);
    
    QSqlQuery result = db->executePreparedQuery(query, {dateRetard});
    
    QList<Commande> commandes;
    while (result.next()) {
        commandes.append(mapFromQuery(result));
    }
    
    return commandes;
}

Commande CommandeService::mapFromQuery(const QSqlQuery& query)
{
    Commande commande;
    commande.setIdCommande(query.value("id_commande").toInt());
    commande.setDateCommande(query.value("date_commande").toDate());
    commande.setStatut(query.value("statut").toString());
    commande.setVilleLivraison(query.value("ville_livraison").toString());
    commande.setIdClient(query.value("id_client").toInt());
    commande.setIdLivreur(query.value("id_livreur").toInt());
    
    return commande;
}

int CommandeService::obtenirProchainId()
{
    DatabaseManager* db = DatabaseManager::getInstance();
    QString query = "SELECT NVL(MAX(id_commande), 0) + 1 as next_id FROM COMMANDES";
    
    QSqlQuery result = db->executeQuery(query);
    
    if (result.next()) {
        return result.value("next_id").toInt();
    }
    
    return 1;
}
