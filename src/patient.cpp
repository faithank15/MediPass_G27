#include "patient.h"
#include "MediPass.h"
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
    : User(mp, db, firstname,lastname,dateNaissance,"","patient",active,telephone,created_by,created_at,"",""),id(id)
{


    int idDossier = mp->getDossierId(db, id);

    if (idDossier != -1) {
        this->dossier = new DossierMedical(mp, db, id, idDossier);
    } else {
        this->dossier = nullptr; // pas de dossier encore
    }
}

// Accesseurs



void Patient::menu(){}


// Surcharge opérateur ==

// Dossier médical (accès en lecture seule)
//const DossierMedical& Patient::getDossierMedical() const {
//    return dossier;
//}

// Dossier médical (accès modifiable)

