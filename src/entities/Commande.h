#ifndef COMMANDE_H
#define COMMANDE_H

#include <QString>
#include <QDate>
#include <QList>

class Commande
{
private:
    int id_commande;
    QDate date_commande;
    QString statut;
    QString ville_livraison;
    int id_client;
    int id_livreur;
    
public:
    // Constructeurs
    Commande();
    Commande(int id, const QDate& date, const QString& statut, 
             const QString& ville, int clientId, int livreurId = 0);
    
    // Getters
    int getIdCommande() const { return id_commande; }
    QDate getDateCommande() const { return date_commande; }
    QString getStatut() const { return statut; }
    QString getVilleLivraison() const { return ville_livraison; }
    int getIdClient() const { return id_client; }
    int getIdLivreur() const { return id_livreur; }
    
    // Setters
    void setIdCommande(int id) { id_commande = id; }
    void setDateCommande(const QDate& date) { date_commande = date; }
    void setStatut(const QString& s) { statut = s; }
    void setVilleLivraison(const QString& ville) { ville_livraison = ville; }
    void setIdClient(int id) { id_client = id; }
    void setIdLivreur(int id) { id_livreur = id; }
    
    // Méthodes utilitaires
    QString toString() const;
    bool isValid() const;
    
    // Opérateurs
    bool operator==(const Commande& other) const;
};

#endif // COMMANDE_H
