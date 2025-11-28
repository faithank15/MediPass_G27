#ifndef PATIENT_HPP
#define PATIENT_HPP

#include <string>
#include "DossierMedical.h"
#include "utilisateur.h"
#include "DossierMedical.h"

class Patient : public User

{
public: 

    Patient(); 
    Patient(MediPass* mp,sqlite3* db,
            int id,
            const std::string& firstname,
            const std::string& lastname,
            bool active,
            int telephone,
            const std::string created_by,
            const std::string created_at,
            const std::string autorisation,
            const std::string statut, 
            const std::string& dateNaissance
            ); 

    int getId() const { return id; }
    std::string getNomComplet() const { return firstname + " " + lastname; }
    std::string getDateNaissance() const { return dateNaissance; }

    // Surcharge d'opérateur == (surtout ne pas supprimer)
    bool operator==(const Patient& other) const {
        return this->getNomComplet() == other.getNomComplet();
    }

    const DossierMedical& getDossierMedical() const { return dossier; }
    DossierMedical& getDossierMedical() { return dossier; }

private:
    int id;
    std::string firstname;
    std::string lastname;
    std::string dateNaissance;
    DossierMedical dossier;
    MediPass* mp;
    sqlite3* db;
};

#endif
