#include "DossierMedical.h"
#include <iostream>
#include <ctime>
#include <chrono>
#include "consultation.h"
#include "MediPass.h"

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
    this->db=db;
    heureCreation = getCurrentTimeAsString();

    int idD = 0;

}

DossierMedical::DossierMedical(int idPatient, const std::string& heureCreation,std::vector<Antecedant> antecedants,std::vector<Consultation> consultations,std::vector<Examen> examens,std::vector<Soin> soins)
    : idPatient(idPatient), heureCreation(heureCreation), antecedants(antecedants), consultations(consultations), examens(examens), soins(soins) {
    // Constructeur avec initialisation de tous les attributs
}

DossierMedical::DossierMedical(MediPass* mp,sqlite3* db, int patientId, int DossierId){
    this->mp = mp;
    this->db = db;
    this->idPatient = patientId;
    this->idDossier = DossierId;

    // Charger la date de création du dossier
    chargerInfosDossier();

    // Charger les antécédants
    chargerAntecedants();

    // Charger les consultations + examens liés
    chargerConsultations();

    // Charger les soins
    chargerSoins();
}

void DossierMedical::chargerInfosDossier()
{
    const char* sql = "SELECT created_at FROM DOSSIERS_MEDICAUX WHERE id = ?;";
    sqlite3_stmt* stmt = nullptr;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, idDossier);

        if (sqlite3_step(stmt) == SQLITE_ROW) {
            heureCreation = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        }
    }

    sqlite3_finalize(stmt);
}

void DossierMedical::chargerAntecedants()
{
    const char* sql =
        "SELECT id, type, observations FROM ANTECEDANTS WHERE dossier_id = ?;";

    sqlite3_stmt* stmt = nullptr;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, idDossier);

        while (sqlite3_step(stmt) == SQLITE_ROW) {
            Antecedant a;
            a.id = sqlite3_column_int(stmt, 0);
            a.type = (const char*)sqlite3_column_text(stmt, 1);
            a.description = (const char*)sqlite3_column_text(stmt, 2);

            antecedants.push_back(a);
        }
    }

    sqlite3_finalize(stmt);
}

void DossierMedical::chargerConsultations()
{
    const char* sql =
        "SELECT id, notes, date FROM CONSULTATIONS WHERE dossier_id = ?;";

    sqlite3_stmt* stmt = nullptr;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, idDossier);

        while (sqlite3_step(stmt) == SQLITE_ROW) {
            Consultation c;
            c.id = sqlite3_column_int(stmt, 0);
            c.observations = (const char*)sqlite3_column_text(stmt, 1);
            c.date_et_heure = (const char*)sqlite3_column_text(stmt, 2);

            // Charger les examens de cette consultation
            chargerExamensPourConsultation(c);

            consultations.push_back(c);
        }
    }

    sqlite3_finalize(stmt);
}

void DossierMedical::chargerExamensPourConsultation(Consultation& c)
{
    const char* sql =
        "SELECT id, type, results, date FROM EXAMENS WHERE consultation_id = ?;";

    sqlite3_stmt* stmt = nullptr;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, c.id);

        while (sqlite3_step(stmt) == SQLITE_ROW) {
            Examen e;
            e.id = sqlite3_column_int(stmt, 0);
            e.typeExamen = (const char*)sqlite3_column_text(stmt, 1);
            e.resultat = (const char*)sqlite3_column_text(stmt, 2);
            e.date = (const char*)sqlite3_column_text(stmt, 3);

            c.ajouter_examen(e);
        }
    }

    sqlite3_finalize(stmt);
}


void DossierMedical::chargerSoins()
{
    const char* sql =
        "SELECT id, description, date, infirmier_id FROM SOINS WHERE dossier_id = ?;";

    sqlite3_stmt* stmt = nullptr;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, idDossier);

        while (sqlite3_step(stmt) == SQLITE_ROW) {
            Soin s;
            s.id = sqlite3_column_int(stmt, 0);
            s.description = (const char*)sqlite3_column_text(stmt, 1);
            s.date = (const char*)sqlite3_column_text(stmt, 2);
            s.infirmier_id = sqlite3_column_int(stmt, 3);

            soins.push_back(s);
        }
    }

    sqlite3_finalize(stmt);
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
    // A1 → N'as pas accès au dossier Médical
    // -----------------------------------------------------------

    std::cout << "Vous n'avez pas accès au dossier médical.\n";

    return 0;
}


DossierMedical::~DossierMedical()
{
    //dtor
}
