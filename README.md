# Système de Gestion Logistique

## Description
Système de gestion logistique développé en Qt C++ avec base de données Oracle. Application complète pour la gestion des commandes et des livreurs avec interface moderne et fonctionnalités avancées.

## Fonctionnalités

### Gestion des Commandes
- ✅ CRUD complet (Créer, Lire, Modifier, Supprimer)
- ✅ Recherche multicritères (ville, statut, dates)
- ✅ Affectation automatique de livreur
- ✅ Calcul du délai moyen de livraison
- ✅ Génération de rapports PDF
- ✅ Statistiques avec graphiques

### Gestion des Livreurs
- ✅ CRUD complet
- ✅ Recherche par nom, zone, disponibilité
- ✅ Mise à jour de disponibilité
- ✅ Alertes de surcharge de travail
- ✅ Statistiques par zone et disponibilité
- ✅ Génération de rapports PDF

### Interface Utilisateur
- ✅ Design moderne et professionnel
- ✅ Interface à onglets intuitive
- ✅ Palette de couleurs cohérente
- ✅ Boutons avec icônes et styles appropriés
- ✅ Tableaux avec tri et filtrage

## Architecture

```
src/
├── main.cpp              # Point d'entrée de l'application
├── db/                   # Gestionnaire de base de données
│   ├── DatabaseManager.h
│   └── DatabaseManager.cpp
├── entities/             # Classes métier
│   ├── Commande.h/.cpp
│   └── Livreur.h/.cpp
├── services/             # Logique métier
│   ├── CommandeService.h/.cpp
│   └── LivreurService.h/.cpp
└── ui/                   # Interface utilisateur
    ├── MainWindow.h/.cpp
    ├── CommandeWidget.h/.cpp
    ├── LivreurWidget.h/.cpp
    ├── StatistiquesWidget.h/.cpp
    └── AppStyleSheet.h/.cpp
```

## Technologies
- **Qt 6.9.1** (Widgets, Sql, Charts, PrintSupport, Pdf)
- **Oracle Database** (avec pilote QOCI)
- **CMake** (système de build)
- **C++17**

## Configuration requise
- Qt 6.9.1 ou supérieur
- Oracle Database (avec pilote QOCI configuré)
- CMake 3.16 ou supérieur
- Compilateur C++17 (MinGW ou MSVC)

## Compilation

1. Cloner le projet
2. Configurer Qt6 dans CMakeLists.txt si nécessaire
3. Exécuter :
```bash
mkdir build
cd build
cmake .. -G "MinGW Makefiles"
cmake --build .
```

## Utilisation
1. Configurer la connexion Oracle dans DatabaseManager
2. Lancer l'application
3. Les tables seront créées automatiquement au premier démarrage
4. Utiliser les onglets pour naviguer entre les modules

## Auteur
Projet de repassage universitaire - Système de Gestion Logistique

## Licence
Projet éducatif
