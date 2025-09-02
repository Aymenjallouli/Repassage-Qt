#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QString>
#include <QDebug>
#include <QMessageBox>

class DatabaseManager : public QObject
{
    Q_OBJECT
    
private:
    static DatabaseManager* instance;
    QSqlDatabase database;
    
    DatabaseManager(QObject *parent = nullptr);
    
public:
    static DatabaseManager* getInstance();
    ~DatabaseManager();
    
    bool connectToDatabase();
    void disconnectFromDatabase();
    bool isConnected();
    
    QSqlQuery executeQuery(const QString& queryString);
    QSqlQuery executePreparedQuery(const QString& queryString, const QVariantList& values = QVariantList());
    
    bool beginTransaction();
    bool commitTransaction();
    bool rollbackTransaction();
    
    QString getLastError();
    
    // Database initialization
    bool createTables();
    bool insertSampleData();
    
private:
    void showDatabaseError(const QString& error);
};

#endif // DATABASEMANAGER_H
