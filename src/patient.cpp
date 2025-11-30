#include "patient.h"
#include <iostream>

//Patient::Patient()
//    : firstname(""), last_name(""), dateNaissance("01/01/1900"), dossier(0, 0) {}


//Constructeur
Patient::Patient(MediPass* mp,sqlite3* db,
            int id,
            const std::string& firstname,
            const std::string& lastname,
            const std::string& dateNaissance,
            bool active,
            int telephone,
            const std::string created_by,
            const std::string created_at)
    : User(mp, db, firstname,lastname,dateNaissance,"","patient",active,telephone,created_by,created_at,"","")
{
    dossier = new DossierMedical(db,id); // L'ID du dossier est le même que l'ID du patient pour simplifier
    this->id = id;
}

// Accesseurs



void Patient::menu(){}


// Surcharge opérateur ==

// Dossier médical (accès en lecture seule)
//const DossierMedical& Patient::getDossierMedical() const {
//    return dossier;
//}

// Dossier médical (accès modifiable)

