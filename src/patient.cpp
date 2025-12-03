#include "patient.h"
#include "MediPass.h"
#include <iostream>

//Patient::Patient()
//    : firstname(""), last_name(""), dateNaissance("01/01/1900"), dossier(0, 0) {}


//Constructeur
Patient::Patient(MediPass* mp,sqlite3* db, int id)
{
    string sql = "SELECT firstname, last_name, date_of_birth, is_active, telephone, created_by, created_at FROM users WHERE id = " + std::to_string(id) + ";";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "[ERROR] Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        return;
    }

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        this->id = getPatientId(id);
        this->firstname = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        this->last_name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        this->dateNaissance = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        bool active = sqlite3_column_int(stmt, 3) != 0;
        int telephone = sqlite3_column_int(stmt, 4);
        std::string created_by = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
        std::string created_at = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6));

        // Initialiser le User de base
        this->mp = mp;
        this->db = db;
        this->role = "patient";
        this->active = active;
        this->telephone = telephone;
        this->created_by = created_by;
        this->created_at = created_at;

        int idDossier = mp->getDossierId(db, id);

        if (idDossier != -1) {
            this->dossier = new DossierMedical(mp, db, id, idDossier);
        } else {
            this->dossier = nullptr; // pas de dossier encore
        }
    } else {
        std::cerr << "[ERROR] No patient found with ID: " << id << std::endl;
    }
}


Patient::Patient(MediPass* mp,sqlite3* db,
            int id,
            const std::string& firstname,
            const std::string& lastname,
            const std::string& dateNaissance,
            bool active,
            int telephone,
            const std::string created_by,
            const std::string created_at)
    : User(mp, db, firstname,lastname,dateNaissance,"","patient",active,telephone,created_by,created_at,"",""),id(getPatientId(id))
{


    int idDossier = mp->getDossierId(db, id);

    if (idDossier != -1) {
        this->dossier = new DossierMedical(mp, db, id, idDossier);
    } else {
        this->dossier = nullptr; // pas de dossier encore
    }
}

// Accesseurs

int Patient::getPatientId(int id) {
    const char* sql = "SELECT id FROM PATIENTS WHERE patient_user_id = ?;";
    sqlite3_stmt* stmt = nullptr;
    int patientId = -1; // -1 si pas trouvé
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, id);
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            patientId = sqlite3_column_int(stmt, 0);
        }
    }
    sqlite3_finalize(stmt);
    return patientId;
}



void Patient::menu(){}


// Surcharge opérateur ==

// Dossier médical (accès en lecture seule)
//const DossierMedical& Patient::getDossierMedical() const {
//    return dossier;
//}

// Dossier médical (accès modifiable)

