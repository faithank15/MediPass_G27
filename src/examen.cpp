#include "examen.h"
#include <iostream>
#include <sqlite3.h>
#include "MediPass.h"

Examen::Examen() {}

Examen::Examen(sqlite3* db,
    int consultation_id,
               int dossier_id,
               const std::string& date,
               const std::string& typeExamen,
               const std::string& resultat)
    : consultation_id(consultation_id), dossier_id(dossier_id), date(date), typeExamen(typeExamen), resultat(resultat) {

        //Enregistrer l'examen dans la base de données
        const char* sql =
            "INSERT INTO EXAMENS (consultation_id, dossier_id, date, type, results) "
            "VALUES (%d, %d, '%q', '%q', '%q');";
        char* errMsg = nullptr;
        std::string formatted_sql = sqlite3_mprintf(
            sql,
            consultation_id,
            dossier_id,
            date.c_str(),
            typeExamen.c_str(),
            resultat.c_str()
        );
        if (sqlite3_exec(db, formatted_sql.c_str(), nullptr, nullptr, &errMsg) != SQLITE_OK) {
            std::cerr << "Erreur lors de l'insertion de l'examen : " << errMsg << std::endl;
            sqlite3_free(errMsg);
        } else {
            id = sqlite3_last_insert_rowid(db);
        }
        sqlite3_free((void*)formatted_sql.c_str());

    }
std::string Examen::getDate() const { return date; }
std::string Examen::getTypeExamen() const { return typeExamen; }
std::string Examen::getResultat() const { return resultat; }

void Examen::setDate(const std::string& d) { date = d; }
void Examen::setTypeExamen(const std::string& t) { typeExamen = t; }
void Examen::setResultat(const std::string& r) { resultat = r; }

void Examen::afficher() const {
    std::cout << "Examen du " << date
              << "\nType : " << typeExamen
              << "\nRésultat : " << resultat
              << "\n-----------------------------\n";
}
