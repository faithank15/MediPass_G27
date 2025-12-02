#include <algorithm>
#include <iostream>
#include <limits>
#include <sqlite3.h>
#include "medecin.h"
#include "patient.h"
#include "consultation.h"
#include "DossierMedical.h"

using namespace std;


static const vector<string> specialites_medicales = {
    "Médecine générale / Médecine de famille",
    "Cardiologie", "Dermatologie et vénéréologie",
    "Endocrinologie, diabétologie et métabolisme",
    "Gastro-entérologie et hépatologie",
    "Gériatrie", "Maladies infectieuses et tropicales",
    "Médecine interne", "Neurologie", "Oncologie médicale",
    "Pédiatrie", "Pneumologie", "Psychiatrie", "Rhumatologie",
    "Chirurgie générale / viscérale et digestive",
    "Chirurgie orthopédique et traumatologique",
    "Chirurgie plastique, reconstructrice et esthétique",
    "Gynécologie obstétrique", "Ophtalmologie",
    "Oto-rhino-laryngologie (ORL)", "Urologie",
    "Anesthésiologie et réanimation",
    "Anatomie et cytologie pathologiques",
    "Radiologie et imagerie médicale"
};


bool specialite_est_valide(const string& specialite)
{
    return find(specialites_medicales.begin(),
                specialites_medicales.end(),
                specialite) != specialites_medicales.end();
}



Medecin::Medecin(MediPass* mp,sqlite3* db,std::string firstname,
            std::string last_name,
            std::string dateNaissance,
            std::string password,
            bool active,
            int telephone,
            std::string created_by,
            std::string created_at,
            std::string autorisation,
            std::string specialite)
     : Pro_sante(mp, db, firstname,last_name,dateNaissance,password,active,telephone,created_by,created_at,"A2","medecin",specialite)
{
    if(!specialite_est_valide(specialite))
        throw Invalid{};
}


void Medecin::lire_dossier_medical(sqlite3* db, MediPass* mp, const string& firstname, const string& last_name) const
{
    vector<string> creds = mp->getUserCreds(db, firstname, last_name);
    if (creds.empty()) { std::cerr << "Patient introuvable ou inactif.\n"; return; }

    Patient* patient = mp->load_patient(db, creds);

    auto it = find(patients_id.begin(), patients_id.end(), patient->getId());

    if(it != patients_id.end()) {
        std::cout << "Accès autorisé. Affichage du dossier médical...\n";
        DossierMedical* dossier = patient->getDossierMedical();
        dossier->afficher(autorisation);

    } else {
        std::cerr << "Accès refusé : vous ne prenez pas en charge ce patient.\n";
    }
    delete patient;
}

bool Medecin::lire_dossier_medical_interactive()
{
    cout << "\n--- Lire le dossier medical d'un patient---\n";

    string fname, last_name;

    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    cout << "Prenom : ";
    getline(cin, fname);

    cout << "Nom : ";
    getline(cin, last_name);

    vector<string> creds = mp->getUserCreds(db, fname, last_name);
    if (creds.empty()) {
        std::cerr << "Patient introuvable ou inactif.\n";
        return false;
    }

    Patient* patient = nullptr;
    try {
        patient = mp->load_patient(db, creds);
    }
    catch (const exception& e) {
        cout << "Erreur : impossible de charger le patient.\n";
        return false;
    }

    int id_patient = patient->getId();
    for(int x: patients_id){
        if(id_patient == x){
            patient->getDossierMedical()->afficher(autorisation);
            delete patient;
            return true;
        }
    }

    cout << "Vous n'avez pas accès à ce dossier médical car vous ne suivez pas ce patient.\n";
    delete patient;
    return false;
}



bool Medecin::editer_dossier_medical(sqlite3* db, MediPass* mp, const string& firstname, const string& last_name)
{
    vector<string> creds = mp->getUserCreds(db, firstname, last_name);
    if (creds.empty()) { std::cerr << "Patient introuvable ou inactif.\n"; return false; }

    Patient* patient = mp->load_patient(db, creds);

    auto it = find(patients_id.begin(), patients_id.end(), patient->getId());

    if(it == patients_id.end()){
        std::cout << "Accès refusé : ce patient n'est pas sous votre responsabilité.\n";
        return false;
    }

    DossierMedical* dossier = patient->getDossierMedical(); // accès modifiable

    int choix = -1;

    do {
        std::cout << "\n=== Edition du dossier médical ===\n";
        std::cout << "1. Ajouter un antécédent\n";
        std::cout << "2. Ajouter une consultation\n";
        std::cout << "0. Quitter\n";
        std::cout << "Votre choix : ";
        std::cin >> choix;

        switch (choix)
        {
            case 1: {
                std::string desc, date, type;
                int id;
                // L'attribution de l'ID devrait être automatique pour antecedant. Nous garantissons un code fiable en le faisant - vous savez
                std::cout << "ID : ";
                std::cin.ignore();
                cin >> id;

                std::cout << "Description de l'antécédent : ";
                std::cin.ignore();
                std::getline(std::cin, desc);

                std::cout << "Date : ";
                std::getline(std::cin, date);

                std::cout << "Type : ";
                std::getline(std::cin, type);

                dossier->ajouterAntecedent(Antecedant(id, desc, date, type));
                break;
            }

            case 2: {
                std::string motif, obs;
                std::cout << "Souhaitez vous lire le dossier médical d'abord ? Oui(o) ou Non(n) \n";
                char r = ' ';
                cin >> r;
                if(r == 'o' || r == 'O'){
                    dossier->afficher(autorisation);
                }
                std::cout << "Motif de consultation : ";
                std::cin.ignore();
                std::getline(std::cin, motif);

                std::cout << "Observations : ";
                std::getline(std::cin, obs);

                // Une consultation devrait inclure médecin + examens + BD
                Consultation c;
                c.setMotif(motif);
                c.setObservations(obs);
                c.setPatientId(patient->getId());
                c.setProId(this->getId());


                // --- Ajouter des examens à la consultation ---
                char choixExamen = 'n';
                std::cout << "Ajouter un examen ? (o/n) : ";
                std::cin >> choixExamen;
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

                while (choixExamen == 'o' || choixExamen == 'O') {
                    std::string dateExamen, typeExamen, resultat;

                    std::cout << "Date de l'examen (JJ/MM/AAAA) : ";
                    std::getline(std::cin, dateExamen);

                    std::cout << "Type d'examen : ";
                    std::getline(std::cin, typeExamen);

                    std::cout << "Résultat : ";
                    std::getline(std::cin, resultat);
                    Examen examen(dateExamen, typeExamen, resultat);

                    c.ajouter_examen(examen);                           // à nécessité une méthode ajouterExamen dans Consultation

                    std::cout << "Ajouter un autre examen ? (o/n) : ";
                    std::cin >> choixExamen;
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                }

                dossier->ajouterConsultation(c);
                break;
            }
            case 0:
                std::cout << "Fin de l'édition du dossier.\n";
                break;

            default:
                std::cout << "Choix invalide.\n";
        }

    } while (choix != 0);
    delete patient;
    return true;
}


// Cette méthode a été achevé
bool Medecin::creer_consultation(
                        sqlite3* db,
                        MediPass* mp,
                        const string& firstname,
                        const string& last_name,
                        const string& observations,
                        const string& motif)

{

    vector<string> creds = mp->getUserCreds(db, firstname, last_name);
    if (creds.empty()) { std::cerr << "Patient introuvable ou inactif.\n"; return false; }

    Patient* patient = mp->load_patient(db, creds);

    // 1. Vérifier si son ID est dans la liste ids du médecin
    auto it = find(patients_id.begin(), patients_id.end(), patient->getId());

    // 2. Créer la consultation
    Consultation c(mp, db, this,
                   patient->getId(),
                   motif,
                   observations);

    // --- Ajouter des examens à la consultation ---
        char choixExamen = 'n';
        std::cout << "Ajouter un examen ? (o/n) : ";
        std::cin >> choixExamen;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        while (choixExamen == 'o' || choixExamen == 'O') {
            std::string dateExamen, typeExamen, resultat;

            std::cout << "Date de l'examen (JJ/MM/AAAA) : ";
            std::getline(std::cin, dateExamen);

            std::cout << "Type d'examen : ";
            std::getline(std::cin, typeExamen);

            std::cout << "Résultat : ";
            std::getline(std::cin, resultat);
            Examen examen(dateExamen, typeExamen, resultat);

            c.ajouter_examen(examen);                           // à nécessité une méthode ajouterExamen dans Consultation

                    std::cout << "Ajouter un autre examen ? (o/n) : ";
                    std::cin >> choixExamen;
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }

    // 3. Ajouter la consultation au dossier médical du patient
    patient->getDossierMedical()->ajouterConsultation(c);
    delete patient;
    return true;
}

bool Medecin::ajouter_disponibilite(sqlite3* db,
                                    int id_medecin,
                                    int day,
                                    const std::string& heure_start,
                                    const std::string& heure_end)
{
    const char* sql =
        "INSERT INTO DISPONIBILITES (professionnel_id, day, heure_start, heure_end) "
        "VALUES (?, ?, ?, ?);";

    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);

    if(rc != SQLITE_OK){
        std::cerr << "Erreur prepare: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    
    sqlite3_bind_int(stmt, 1, id_medecin);
    sqlite3_bind_int(stmt, 2, day);
    sqlite3_bind_text(stmt, 3, heure_start.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 4, heure_end.c_str(), -1, SQLITE_TRANSIENT);

    rc = sqlite3_step(stmt);

    if(rc != SQLITE_DONE){
        std::cerr << "Erreur insert: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt);
        return false;
    }

    sqlite3_finalize(stmt);
    return true;
}

std::vector<Medecin::Disponibilite> Medecin::charger_disponibilites(sqlite3* db, int id_medecin)
{
    std::vector<Disponibilite> result;

    const char* sql =
        "SELECT id, day, heure_start, heure_end "
        "FROM DISPONIBILITES "
        "WHERE professionnel_id = ? "
        "ORDER BY day, heure_start;";

    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);

    if(rc != SQLITE_OK){
        std::cerr << "Erreur prepare select: " << sqlite3_errmsg(db) << std::endl;
        return result;
    }

    sqlite3_bind_int(stmt, 1, id_medecin);

    while(sqlite3_step(stmt) == SQLITE_ROW){
        Disponibilite d;

        d.id          = sqlite3_column_int(stmt, 0);
        d.day         = sqlite3_column_int(stmt, 1);
        d.heure_start = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        d.heure_end   = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));

        result.push_back(d);
    }

    sqlite3_finalize(stmt);
    return result;
}

void Medecin::afficher_disponibilites()
{
    auto dispos = charger_disponibilites(this->db, this->getId());

    if(dispos.empty()){
        std::cout << "Aucune disponibilité enregistrée.\n";
        return;
    }

    std::cout << "\n--- Disponibilités de " 
              << firstname << " " << last_name << " ---\n";

    for(const auto& d : dispos){
        std::cout << "ID: " << d.id
                  << " | Jour: " << d.day
                  << " | De: " << d.heure_start
                  << " à " << d.heure_end
                  << "\n";
    }
}

void Medecin::afficher_patients() {
    cout << "\n--- Liste des patients suivis ---\n";

    if (patients_id.empty()) {
        cout << "Aucun patient actuellement.\n";
        return;
    }

    for (int i = 0; i < (int)patients_id.size(); ++i) {
        int patientId = patients_id[i];

        // Construire un patient à partir de son ID
        Patient p(this->mp, this->db, patientId);

        // Affichage lisible 
        cout << i + 1 << ". "
             << p.getNomComplet() << "\n";
    }
}

void Medecin::ajouter_consultation_interactive() {
    cout << "\n--- Ajouter une consultation ---\n";

    string motif, observations;

    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    cout << "Motif : ";
    getline(cin, motif);

    cout << "Observations : ";
    getline(cin, observations);

    // --- Sélection du patient ---
    string fname, last_name;
    cout << "\n Veuillez entrer le nom puis le prenom du patient concerné ";
    cin >> fname >> last_name;

    vector<string> creds = mp->getUserCreds(db, firstname, last_name);
    if (creds.empty()) { std::cerr << "Patient introuvable ou inactif.\n"; return; }

    Patient* patient = nullptr;
    try {
        patient = mp->load_patient(db, creds);
    }
    catch (const exception& e) {
        cout << "Erreur : impossible de charger le patient.\n";
    }

    cout << "\n--- Patient sélectionné : "
         << patient->getNomComplet() << " ---\n";

    // --- Ajout éventuel d'examens ---
    vector<Examen> examens;

    char choix;
    cout << "\nAjouter un examen ? (o/n) : ";
    cin >> choix;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    while (choix == 'o' || choix == 'O') {
        string date, typeExamen, resultat;

        cout << "\n--- Nouvel examen ---\n";

        std::cout << "ID : ";
        std::cin.ignore();
        cin >> id;

        cout << "Date (jj/mm/aaaa) : ";
        getline(cin, date);

        cout << "Type d'examen : ";
        getline(cin, typeExamen);

        cout << "Résultat : ";
        getline(cin, resultat);

        examens.emplace_back(date, typeExamen, resultat);

        cout << "Ajouter un autre examen ? (o/n) : ";
        cin >> choix;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }

    // --- Création de la consultation ---
    Consultation nouvelleConsultation(
        mp,
        db,
        this,
        patient->getId(),
        observations,
        motif,
        examens
    );

    cout << "\nConsultation ajoutée avec succès pour le patient "

         << patient->getNomComplet() << ".\n";

}

void Medecin::afficher_infos_professionnelles() {
    cout << "\n--- Vos informations professionnelles ---\n";
    cout << "Nom d'utilisateur : " << Medecin::getLast_name() << "\n";
    cout << "Rôle : " << Medecin::getRole() << "\n";
    cout << "Autorisation : " << obtenir_autorisation() << "\n";
    cout << "Statut : " << obtenir_statut() << "\n";
    cout << "Actif : " << (isActive() ? "Oui" : "Non") << "\n";
}

void Medecin::menu() {
    int choix = -1;

    while (choix != 0) {
        cout << "\n==============================\n";
        cout << "     MENU DU MÉDECIN\n";
        cout << "==============================\n";
        cout << "Connecté en tant que : Dr " << Medecin::getLast_name() << " (" << Medecin::getRole() << ")\n";
        cout << "Statut : " << obtenir_statut()
             << " | Autorisation : " << obtenir_autorisation() << "\n";
        cout << "------------------------------\n";
        cout << "1. Voir la liste de mes patients\n";
        cout << "2. Voir mes disponibilités\n";
        cout << "3. Créer une consultation\n";
        cout << "4. Voir mes informations professionnelles\n";
        cout << "5. Lire le dossier medical d'un patient\n";
        cout << "0. Se déconnecter / Quitter\n";
        cout << "------------------------------\n";
        cout << "Votre choix : ";

        cin >> choix;

        // Ici on gère les entrées invalides
        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Entrée invalide. Veuillez réessayer.\n";
            continue;
        }

        switch (choix) {
            case 1:
                afficher_patients();
                break;

            case 2:
                afficher_disponibilites();
                break;

            case 3:
                ajouter_consultation_interactive();
                break;

            case 4:
                afficher_infos_professionnelles();
                break;

            case 5:
                lire_dossier_medical_interactive();
                break;

            case 0:
                cout << "Déconnexion...\n";
                break;

            default:
                cout << "Choix invalide.\n";
                break;
        }
    }
}



/*int main() {

    try {
        cout << "\n===== TEST : Création d'un médecin =====\n";

        Medecin med(
            1,                      // id
            "drfaith",              // username
            "pass123",              // password
            "Directeur Médical",    // role
            true,                   // active
            229998877,              // telephone
            "admin",                // created_by
            "2024-12-01",           // created_at
            "A1",                   // autorisation
            "Actif",                // statut
            "Cardiologie"           // spécialité
        );

        cout << "Médecin créé avec succès.\n";
        cout << "Nom d'utilisateur : " << med.getUsername() << "\n";
        cout << "Spécialité : " << med.obtenir_specialite() << "\n\n";

        // ------------------------------
        cout << "===== TEST : Création de patients =====\n";

        Patient p1(1, "Aimane", "Diallo", "12/03/2004");
        Patient p2(2, "Madi", "Soglo", "21/08/2003");

        cout << "Patient 1 : " << p1.getNomComplet() << "\n";
        cout << "Patient 2 : " << p2.getNomComplet() << "\n\n";

        // ------------------------------
        cout << "===== TEST : Ajout de disponibilités =====\n";

        vector<chrono::system_clock::time_point> dispo;
        dispo.push_back(chrono::system_clock::now());

        med.mettre_disponibilite(dispo);

        cout << "Disponibilité ajoutée.\n\n";

        // ------------------------------
        cout << "===== TEST : Création d'examens =====\n";

        vector<Examen> examens{
            Examen("01/01/2025", "ECG", "Normal"),
            Examen("02/01/2025", "Scanner Thoracique", "RAS")
        };

        cout << "2 examens créés.\n\n";

        // ------------------------------
        cout << "===== TEST : Création consultation =====\n";

        auto now = chrono::system_clock::now();

        bool ok = med.creer_consultation(
            now,
            p1,
            "Douleur thoracique",
            "Suspicion angine de poitrine",
            examens
        );

        if(ok)
            cout << "Consultation créée avec succès.\n";
        else
            cout << "Échec : le médecin n'était pas disponible.\n";

        cout << "\n===== DISPONIBILITÉS =====\n";
        med.afficher_disponibilites();

        cout << "\n===== PATIENTS SUIVIS =====\n";
        med.afficher_patients();

        cout << "\n===== INFORMATIONS PROFESSIONNELLES =====\n";
        med.afficher_infos_professionnelles();

    }
    catch (const Pro_sante::Invalid&) {
        cout << "\nERREUR : Paramètres invalides pour Pro_sante.\n";
    }
    catch (const Medecin::Invalid&) {
        cout << "\nERREUR : Spécialité invalide.\n";
    }
    catch (...) {
        cout << "\nERREUR INCONNUE.\n";
    }

    return 0;
}*/

//<>


