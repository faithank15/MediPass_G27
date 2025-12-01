#ifndef SECRETAIRE_H
#define SECRETAIRE_H

#include <sqlite3.h>
#include <string>
#include <vector>

#include "utilisateur.h"


class Secretaire : public User
{
    public:
        Secretaire();
        Secretaire(MediPass* mp, sqlite3* db, const std::string& firstname,
                   const std::string& last_name,
                   const std::string& dateNaissance,
                   const std::string& password,
                   const int telephone,
                   const std::string& created_by,
                   const std::string& created_at);

        ~Secretaire();

    protected:
    bool prendreRdv(sqlite3* db);
    bool annulerRdv(sqlite3* db);
    void consulterDisponibilites(sqlite3* db);
    std::vector<std::string> getDocSpecialites(sqlite3* db, const std::string& specialite);
    void getDisponibilites(sqlite3* db, int doc_id, const std::string& firstname, const std::string& last_name);
    int findDay(std::string date);
    bool rdvValide(sqlite3* db, int doc_id, const std::string& date, const std::string& time);
    bool timeValide(sqlite3* db, const std::string& time, int doc_id, const std::string& date);
    void consulterRdv(sqlite3* db);
    void menu();

    private:
};

#endif // SECRETAIRE_H
