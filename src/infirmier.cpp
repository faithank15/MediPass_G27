#include "infirmier.h"
#include <iostream>
#include "MediPass.h"
#include "Soin.h"
#include "DossierMedical.h"

class Soin; // forward

Infirmier::Infirmier(MediPass* mp,
                     sqlite3* db,
                     std::string firstname,
                     std::string last_name,
                     std::string dateNaissance,
                     std::string password,
                     std::string role,
                     bool active,
                     int telephone,
                     std::string created_by,
                     std::string created_at,
                     const std::string& autorisation,
                     const std::string& specialite)
    : Pro_sante(mp, db, firstname, last_name, dateNaissance, password,
                active, telephone, created_by, created_at,
                autorisation, "infirmier", specialite)
{
}

Infirmier::~Infirmier()
{
}

void Infirmier::menu()
{
    std::cout << "===== Menu Infirmier =====\n";
    std::cout << "1. Administrer un soin\n";
    std::cout << "2. Déconnexion\n";

    int choix;

    do {
        std::cout << "\nChoisissez une option: ";
        std::cin >> choix;

        switch (choix) {
            case 1:
                administrer_soin();
                break;
            case 2:
                std::cout << "Déconnexion...\n";
                break;
            default:
                std::cout << "Choix invalide.\n";
                break;
        }

    } while (choix != 2);
}

void Infirmier::administrer_soin()
{
    int patient_id;
    std::string type;
    std::string description;
    std::string firstname;
    std::string lastname;
    std::cin.ignore();


    std::cout << "======= ADMINISTRATION DE SOIN =======\n";

    std::cout << "Prénom du patient: ";
    std::getline(std::cin, firstname);


    std::cout << "Nom du patient: ";
    std::getline(std::cin, lastname);

    // 1) Récupération du patient
    vector<string> creds = mp->getUserCreds(this->db, firstname, lastname);
    if (creds.empty()) {
        std::cerr << "Patient introuvable ou inactif.\n";
        return;
    }

    patient_id = std::stoi(creds[0]); // <-- Récupération de l’ID

    Patient* patient = nullptr;
    try {
        patient = mp->load_patient(db, creds);
    }
    catch (...) {
        std::cout << "Erreur : impossible de charger le patient.\n";
        return;
    }

    if (!patient) {
        std::cout << "Erreur interne : patient null.\n";
        return;
    }

    std::cout << "\n--- Patient sélectionné : "
              << patient->getNomComplet() << " ---\n";

    // 2) Lecture dossier médical
    std::cout << "Souhaitez-vous consulter son dossier médical ? (o/n) : ";
    char choix;
    std::cin >> choix;
    std::cin.ignore(); // IMPORTANT

    if (choix == 'o' || choix == 'O') {
        DossierMedical* dossier = patient->getDossierMedical();
        if (dossier)
            dossier->afficher(autorisation);
        else
            std::cout << "Aucun dossier médical trouvé.\n";
    }

    // 3) Infos du soin
    std::cout << "\nType de soin: ";
    std::getline(std::cin, type);

    std::cout << "Description du soin: ";
    std::getline(std::cin, description);

    // 4) Enregistrer le soin
    Soin soin(mp, db, this, patient_id, type, description);

    std::cout << "\nSoin administré avec succès.\n";
}
