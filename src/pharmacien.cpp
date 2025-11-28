#include "pharmacien.h"
#include "DossierMedical.h"
#include "patient.h"
#include "MediPass.h"

#include <iostream>
#include <limits>

using namespace std;


Pharmacien::Pharmacien(MediPass* mp, sqlite3* db, const std::string& firstname,
              const std::string& last_name,
              std::string password,
              bool active,
              int telephone,
              std::string created_by,
              std::string created_at,
              std::string autorisation,
              std::string statut)
     : Pro_sante(mp, db, firstname,last_name,password,active,telephone,created_by,created_at,autorisation,statut)
{
}

string Recommander::info_recommandation() {
    time_t t = chrono::system_clock::to_time_t(date_et_heure);
    return string("Recommandation faite le : ") + ctime(&t);
}


void Pharmacien::recommander() {
    cout << "\n--- Recommandation pharmaceutique ---\n";

    string recommandation;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    cout << "Votre recommandation : ";
    getline(cin, recommandation);

    Recommander r;
    r.date_et_heure = chrono::system_clock::now();

    cout << "\nRecommandation enregistrée : \n";
    cout << recommandation << "\n";
    cout << r.info_recommandation() << "\n";
}


void Pharmacien::lire_dossier_medical(int patient_id) {
    cout << "\n--- Lecture du dossier médical (Pharmacien) ---\n";


    // ---------------------------------------------------------
    // Chargement du patient (tu adapteras avec ta fonction de load)
    // ---------------------------------------------------------
    Patient p;                                                                                  // En attente de MédiPass
    cout << "\nSimulation chargement patient...\n";
    cout << "(Quand la DB sera prête, on utilisera charger_par_id)\n\n";


    cout << "\n\n--- AFFICHAGE DU DOSSIER ---\n";

    // ---------------------------------------------------------
    // On laisse DossierMedical faire le travail
    // ---------------------------------------------------------
    p.getDossierMedical().afficher(autorisation);
}

void Pharmacien::menu() {
    int choix = 0;

    do {
        cout << "\n=============================\n";
        cout << "     MENU PHARMACIEN\n";
        cout << "=============================\n";
        cout << "1. Lire un dossier médical\n";
        cout << "2. Faire une recommandation\n";
        cout << "3. Quitter\n";
        cout << "Votre choix : ";
        cin >> choix;

        // Sécurisation entrée utilisateur
        while (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Entrée invalide. Réessayez : ";
            cin >> choix;
        }

        switch (choix) {
            case 1: {
                int pid;
                cout << "\nID du patient : ";
                cin >> pid;

                // Sécurisation
                while (cin.fail()) {
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    cout << "Entrée invalide. Réessayez : ";
                    cin >> pid;
                }

                lire_dossier_medical(pid);
                break;
            }

            case 2:
                recommander();
                break;

            case 3:
                cout << "\nDéconnexion...\n";
                break;

            default:
                cout << "Option invalide.\n";
        }

    } while (choix != 3);
}
