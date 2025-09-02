#include "Livreur.h"

Livreur::Livreur()
    : id_livreur(0), nom(""), telephone(""), zone_livraison(""), vehicule(""), disponibilite(true)
{
}

Livreur::Livreur(int id, const QString& nom, const QString& tel, const QString& zone, const QString& vehic, bool dispo)
    : id_livreur(id), nom(nom), telephone(tel), zone_livraison(zone), vehicule(vehic), disponibilite(dispo)
{
}

QString Livreur::toString() const
{
    return QString("Livreur #%1 - %2 - Zone: %3 - %4")
           .arg(id_livreur)
           .arg(nom)
           .arg(zone_livraison)
           .arg(getDisponibiliteText());
}

bool Livreur::isValid() const
{
    return id_livreur > 0 && !nom.isEmpty() && !zone_livraison.isEmpty();
}

QString Livreur::getDisponibiliteText() const
{
    return disponibilite ? "Disponible" : "Occupé";
}

bool Livreur::operator==(const Livreur& other) const
{
    return id_livreur == other.id_livreur;
}
