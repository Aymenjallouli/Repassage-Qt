#include "Commande.h"

Commande::Commande()
    : id_commande(0), date_commande(QDate::currentDate()), 
      statut("En attente"), ville_livraison(""), 
      id_client(0), id_livreur(0)
{
}

Commande::Commande(int id, const QDate& date, const QString& statut, 
                   const QString& ville, int clientId, int livreurId)
    : id_commande(id), date_commande(date), statut(statut), 
      ville_livraison(ville), id_client(clientId), id_livreur(livreurId)
{
}

QString Commande::toString() const
{
    return QString("Commande #%1 - %2 - %3 - Client: %4")
           .arg(id_commande)
           .arg(date_commande.toString("dd/MM/yyyy"))
           .arg(statut)
           .arg(id_client);
}

bool Commande::isValid() const
{
    return id_client > 0 && 
           !ville_livraison.isEmpty() && 
           !statut.isEmpty() &&
           date_commande.isValid();
}

bool Commande::operator==(const Commande& other) const
{
    return id_commande == other.id_commande;
}
