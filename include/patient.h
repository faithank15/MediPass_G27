#ifndef PATIENT_HPP
#define PATIENT_HPP

#include <string>
#include "DossierMedical.h"
#include "utilisateur.h"

class DossierMedical;

class Patient : public User

{
public:

    Patient(MediPass* mp,sqlite3* db, int id);
    Patient(MediPass* mp,sqlite3* db,
            int id,
            const std::string& firstname,
            const std::string& lastname,
            const std::string& dateNaissance,
            bool active,
            int telephone,
            const std::string created_by,
            const std::string created_at);

    int getId() const { return id; }
    int getPatientId(int id);
    std::string getNomComplet() const { return this->firstname + " " + this->last_name; }
    std::string getDateNaissance() const { return dateNaissance; }

    // Surcharge d'opérateur == (surtout ne pas supprimer)
    bool operator==(const Patient& other) const {
        return this->getNomComplet() == other.getNomComplet();
    }

    DossierMedical* getDossierMedical() { return dossier; }

    void menu();
    int id;

private:
    DossierMedical* dossier = nullptr;
    MediPass* mp;
    sqlite3* db;

};

#endif
