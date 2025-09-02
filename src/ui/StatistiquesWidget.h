#ifndef STATISTIQUESWIDGET_H
#define STATISTIQUESWIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QTextEdit>
#include <QGroupBox>
#include <QSplitter>
#include <QScrollArea>
#include <QFrame>
#include <QtCharts/QChartView>
#include <QtCharts/QPieSeries>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QValueAxis>
#include <QtCharts/QChart>
#include "services/CommandeService.h"
#include "services/LivreurService.h"

class StatistiquesWidget : public QWidget
{
    Q_OBJECT

public:
    explicit StatistiquesWidget(QWidget *parent = nullptr);
    
public slots:
    void actualiserStatistiques();

private slots:
    void genererRapport();

private:
    void setupUI();
    void setupPanneauResume();
    void setupGraphiques();
    void connecterSignaux();
    void appliquerStyle();
    void chargerStatistiquesCommandes();
    void chargerStatistiquesLivreurs();
    void creerGraphiqueStatutsCommandes();
    void creerGraphiqueZonesLivraison();
    void creerGraphiqueDisponibiliteLivreurs();
    void mettreAJourCartes();
    
    // Interface principale
    QVBoxLayout* mainLayout;
    QSplitter* splitter;
    QScrollArea* scrollArea;
    
    // Panneau de résumé (cartes)
    QGroupBox* groupResume;
    QGridLayout* resumeLayout;
    QLabel* carteCommandes;
    QLabel* carteLivreurs;
    QLabel* carteDisponibles;
    QLabel* carteEnCours;
    
    // Graphiques
    QGroupBox* groupGraphiques;
    QGridLayout* graphiquesLayout;
    QChartView* chartStatutsCommandes;
    QChartView* chartZonesLivraison;
    QChartView* chartDisponibiliteLivreurs;
    
    // Boutons et contrôles
    QPushButton* btnActualiser;
    QPushButton* btnGenererRapport;
    QPushButton* btnExporterExcel;
    
    // Services
    CommandeService* commandeService;
    LivreurService* livreurService;
    
    // Données statistiques
    int totalCommandes;
    int commandesEnAttente;
    int commandesEnCours;
    int commandesLivrees;
    int commandesAnnulees;
    int totalLivreurs;
    int livreursDisponibles;
    int livreursOccupes;
    QMap<QString, int> commandesParZone;
    QMap<QString, int> livreursParZone;
};

#endif // STATISTIQUESWIDGET_H
