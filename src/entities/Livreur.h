#ifndef LIVREUR_H
#define LIVREUR_H

#include <QString>

class Livreur
{
private:
    int id_livreur;
    QString nom;
    QString telephone;
    QString zone_livraison;
    QString vehicule;
    bool disponibilite;
    
public:
    // Constructeurs
    Livreur();
    Livreur(int id, const QString& nom, const QString& tel, const QString& zone, const QString& vehic, bool dispo = true);
    
    // Getters
    int getIdLivreur() const { return id_livreur; }
    QString getNom() const { return nom; }
    QString getTelephone() const { return telephone; }
    QString getZoneLivraison() const { return zone_livraison; }
    QString getVehicule() const { return vehicule; }
    bool getDisponibilite() const { return disponibilite; }
    
    // Setters
    void setIdLivreur(int id) { id_livreur = id; }
    void setNom(const QString& n) { nom = n; }
    void setTelephone(const QString& tel) { telephone = tel; }
    void setZoneLivraison(const QString& zone) { zone_livraison = zone; }
    void setVehicule(const QString& vehic) { vehicule = vehic; }
    void setDisponibilite(bool dispo) { disponibilite = dispo; }
    
    // Méthodes utilitaires
    QString toString() const;
    bool isValid() const;
    QString getDisponibiliteText() const;
    
    // Opérateurs
    bool operator==(const Livreur& other) const;
};

#endif // LIVREUR_H
