#include "patient.h"
#include <iostream>

Patient::Patient()
    : id(0), firstname(""), lastname(""), dateNaissance("01/01/1900"), dossier(0, 0) {}


//Constructeur
Patient::Patient(MediPass* mp,sqlite3* db,
            int id,
            const std::string& firstname,
            const std::string& lastname,
            bool active,
            int telephone,
            const std::string created_by,
            const std::string created_at,
            const std::string autorisation,
            const std::string statut, 
            const std::string& dateNaissance)
    : User(mp, db, firstname,last_name,password,role), dateNaissance(dateNaissance), dossier(id, id)
{
}

// Accesseurs
int Patient::getId() const {
    return id;
}

std::string Patient::getNomComplet() const {
    return firstname + " " + lastname;
}

std::string Patient::getDateNaissance() const {
    return dateNaissance;
}

// Surcharge opérateur ==
bool Patient::operator==(const Patient& other) const {
    return this->getNomComplet() == other.getNomComplet();
}

// Dossier médical (accès en lecture seule)
const DossierMedical& Patient::getDossierMedical() const {
    return dossier;
}

// Dossier médical (accès modifiable)
DossierMedical& Patient::getDossierMedical() {
    return dossier;
}
