<!-- Use this file to provide workspace-specific custom instructions to Copilot. For more details, visit https://code.visualstudio.com/docs/copilot/copilot-customization#_use-a-githubcopilotinstructionsmd-file -->

# Instructions Copilot pour le Système de Gestion Logistique

## Contexte du Projet
Ce projet est un système de gestion logistique développé en Qt C++ avec base de données Oracle. Il s'agit d'un projet de repassage universitaire comprenant deux modules principaux:

1. **Gestion des Commandes** - Module de CRUD pour les commandes de livraison
2. **Gestion des Livreurs** - Module de CRUD pour les livreurs et véhicules

## Architecture du Projet
- **Entities** (`src/entities/`) : Classes métier (Commande, Livreur)
- **Services** (`src/services/`) : Logique métier avec requêtes préparées Oracle
- **UI** (`src/ui/`) : Interface utilisateur Qt avec TabWidget/StackedWidget
- **Database** (`src/db/`) : Gestionnaire de connexion Oracle unique
- **Utils** (`src/utils/`) : Utilitaires (PDF, Excel, graphiques)

## Contraintes Techniques
- **Base de données** : Oracle avec requêtes préparées uniquement
- **Interface** : Qt Widgets avec containers ergonomiques (TabWidget/StackedWidget)
- **Aucune requête SQL** directement dans les boutons - utiliser les services
- **Connexion unique** à la base de données via singleton DatabaseManager
- **Respect des contraintes d'intégrité** (clés primaires, étrangères)

## Fonctionnalités Requises
### Pour chaque module:
1. **CRUD complet** (Create, Read, Update, Delete)
2. **Recherche multicritères** (minimum 3 critères)
3. **Statistiques avec graphiques** (utiliser QtCharts)
4. **Génération de documents** (PDF ou Excel)
5. **Deux fonctionnalités métier** spécifiques au domaine

### Fonctionnalités métier spécifiques:
- **Commandes**: Calcul délai moyen, affectation automatique livreur
- **Livreurs**: Mise à jour disponibilité, alertes surcharge

## Conventions de Code
- Noms de classes en PascalCase (ex: `CommandeService`)
- Noms de méthodes en camelCase (ex: `obtenirCommande`)
- Commentaires en français
- Gestion d'erreurs avec QMessageBox
- Utilisation de `const` et références pour optimisation

## Extensions VS Code Recommandées
- C/C++ Extension Pack
- CMake Tools
- Qt for Python (pour coloration Qt)

## Build System
- CMake avec support Qt6
- Auto MOC/UIC/RCC activé
- Liaison avec Qt6::Core, Qt6::Widgets, Qt6::Sql, Qt6::Charts, Qt6::PrintSupport, Qt6::Pdf

Toujours respecter ces contraintes lors de la génération de code et suggérer des améliorations conformes à l'architecture existante.
