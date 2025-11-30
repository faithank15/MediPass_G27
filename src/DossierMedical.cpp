#include "DossierMedical.h"
#include <iostream>
#include <ctime>
#include <chrono>
#include "consultation.h"

// M�thode Utilitaires (Interne)


std::string DossierMedical::getCurrentTimeAsString() const {
    // Obtenir le temps actuel en secondes depuis l'�poque
    std::time_t now = std::time(nullptr);

    // Convertir l'heure en structure locale (pour le formatage)
    std::tm* local_time = std::localtime(&now);

    char buf[100];

    // Formater l'heure dans la cha�ne de caract�res (buffer)
    if (std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", local_time)) {
        return buf;
    }
    return "Erreur lors de l'obtention de l'heure.";
}

// Constructeur


DossierMedical::DossierMedical(sqlite3* db,int idP)
    : idPatient(idP) {

    heureCreation = getCurrentTimeAsString();

    int idD = 0;

    sqlite3_exec(db,
        "INSERT INTO dossiers_medicaux (id_patient, heure_creation) VALUES (?, ?);",
        nullptr, nullptr, nullptr);

}

DossierMedical::DossierMedical(int idPatient, const std::string& heureCreation,std::vector<Antecedant> antecedants,std::vector<Consultation> consultations,std::vector<Examen> examens,std::vector<Soin> soins)
    : idPatient(idPatient), heureCreation(heureCreation), antecedants(antecedants), consultations(consultations), examens(examens), soins(soins) {
    // Constructeur avec initialisation de tous les attributs
}

// Accesseurs

int DossierMedical::getIdDossier() const {
    return idDossier;
}

int DossierMedical::getIdPatient() const {
    return idPatient;
}

std::string DossierMedical::getHeureCreation() const {
    return heureCreation;
}

// Getters pour les collections (retournent une r�f�rence constante)
const std::vector<Antecedant>& DossierMedical::getAntecedents() const {
    return antecedants;
}

const std::vector<Consultation>& DossierMedical::getConsultations() const {
    return consultations;
}

const std::vector<Examen>& DossierMedical::getExamens() const {
    return examens;
}

// Mutateurs

void DossierMedical::setIdDossier(int idD) {
    idDossier = idD;
}

void DossierMedical::setIdPatient(int idP) {
    idPatient = idP;
}

// Mutateurs d'ajout pour les collections
void DossierMedical::ajouterAntecedent(const Antecedant& a) {
    antecedants.push_back(a);
}

void DossierMedical::ajouterConsultation(const Consultation& c) {
    consultations.push_back(c);
}

void DossierMedical::ajouterExamen(const Examen& e) {
    examens.push_back(e);
}

int DossierMedical::afficher(std::string autorisation){
    using std::cout;
    using std::cin;
    using std::endl;

    cout << "\n===== Dossier Médical =====\n";
    cout << "ID Dossier : " << idDossier << endl;
    cout << "ID Patient : " << idPatient << endl;
    cout << "Créé le    : " << heureCreation << endl;
    cout << "================================\n\n";

    // -----------------------------------------------
    // A3 → accès le plus faible : uniquement les soins
    // -----------------------------------------------
    if (autorisation == "A3") {
        cout << "--- SOINS (" << soins.size() << ") ---\n";
        for (const auto& s : soins) {
            s.afficher();
            cout << "----------------------------------\n";
        }
        cout << "===== Fin du dossier médical =====\n";
        return 0;
    }

    // --------------------------------------------------------------------
    // A2 → menu interactif : soins / consultations / examens / prescr.
    // --------------------------------------------------------------------
    if (autorisation == "A2") {
        char choix = '0';

        while (choix != '5') {
            cout << "\nQue souhaitez-vous afficher ?\n";
            cout << "1. Soins\n";
            cout << "2. Consultations\n";
            cout << "3. Examens\n";
            cout << "4. Prescriptions (simulation)\n";
            cout << "5. Quitter\n";
            cout << "Votre choix : ";
            cin >> choix;

            cout << "\n";

            switch (choix) {
                case '1':
                    cout << "--- SOINS (" << soins.size() << ") ---\n";
                    for (const auto& s : soins) {
                        s.afficher();
                        cout << "----------------------------------\n";
                    }
                    break;

                case '2':
                    cout << "--- CONSULTATIONS (" << consultations.size() << ") ---\n";
                    for (const auto& c : consultations) {
                        c.afficher();
                        cout << "----------------------------------\n";
                    }
                    break;

                case '3':
                    cout << "--- EXAMENS (" << examens.size() << ") ---\n";
                    for (const auto& e : examens) {
                        e.afficher();
                        cout << "----------------------------------\n";
                    }
                    break;

                case '4':
                    cout << "--- PRESCRIPTIONS ---\n";
                    cout << "(La classe Prescription n’est pas encore définie.)\n";
                    cout << "(Simulation d’affichage.)\n";
                    cout << "----------------------------------\n";
                    break;

                case '5':
                    cout << "Fermeture du dossier.\n";
                    break;

                default:
                    cout << "Choix invalide.\n";
            }
        }

        cout << "===== Fin du dossier médical =====\n";
        return 0;
    }

    // -----------------------------------------------------------
    // A1 → accès maximal : Afficher TOUT sans poser de questions
    // -----------------------------------------------------------

    cout << "--- ANTÉCÉDENTS (" << antecedants.size() << ") ---\n";
    for (const auto& a : antecedants) {
        a.afficher();
        cout << "----------------------------------\n";
    }

    cout << "\n--- CONSULTATIONS (" << consultations.size() << ") ---\n";
    for (const auto& c : consultations) {
        c.afficher();
        cout << "----------------------------------\n";
    }

    cout << "\n--- EXAMENS (" << examens.size() << ") ---\n";
    for (const auto& e : examens) {
        e.afficher();
        cout << "----------------------------------\n";
    }

    cout << "\n--- SOINS (" << soins.size() << ") ---\n";
    for (const auto& s : soins) {
        s.afficher();
        cout << "----------------------------------\n";
    }

    cout << "\n--- PRESCRIPTIONS ---\n";
    cout << "(Simulation – classe non encore implémentée.)\n";

    cout << "\n===== Fin du dossier médical =====\n\n";

    return 0;
}


DossierMedical::~DossierMedical()
{
    //dtor
}
