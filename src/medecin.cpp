#include <algorithm>
#include <iostream>
#include <limits>
#include <fstream>
#include <sstream>
#include <sqlite3.h>
#include "medecin.h"
#include "patient.h"
#include "consultation.h"
#include "DossierMedical.h"
#include <filesystem>
#include <unordered_map>

using namespace std;
namespace fs = std::filesystem;


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



Medecin::Medecin(MediPass* mp,sqlite3* db,int id,std::string firstname,
            std::string last_name,
            std::string dateNaissance,
            std::string password,
            bool active,
            int telephone,
            std::string created_by,
            std::string created_at,
            std::string autorisation,
            std::string specialite)
     : Pro_sante(mp, db, firstname,last_name,dateNaissance,password,active,telephone,created_by,created_at,"A2","medecin",specialite) ,id(id)
{

    if(!specialite_est_valide(specialite))
        throw Invalid{};

    cout << "Medecin ID " << id << " cree avec succes.\n";

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

    int id_patient = patient->id;
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

                dossier->ajouterAntecedent(Antecedant(id,type,date,desc));
                break;
            }

            case 2: {
                std::string motif, obs,prec;
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

                std::cout << "Prescription : ";
                std::getline(std::cin, prec);

                // Une consultation devrait inclure médecin + examens + BD
                Consultation c(mp, db, this,
                                patient->getId(),
                                motif,
                                obs,
                                prec);


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
                    Examen examen(db, c.id, mp->getDossierId(db,patient->getId()), dateExamen, typeExamen, resultat);

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

bool Medecin::patientValide(int patient_id)
{
    string sql = "SELECT COUNT(*) FROM PATIENTS WHERE id = " + std::to_string(patient_id) + ";";
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Erreur lors de la préparation de la requête : " << sqlite3_errmsg(db) << std::endl;
        return false;
    }
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_ROW) {
        std::cerr << "Erreur lors de l'exécution de la requête : " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt);
        return false;
    }
    int count = sqlite3_column_int(stmt, 0);
    sqlite3_finalize(stmt);
    return count > 0;
}

// Cette méthode a été achevé

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

    // --- Sélection du patient ---
    std::string fname, last_name;
    std::cin.ignore();
    std::cout << "\nVeuillez entrer le prénom du patient : ";
    std::getline(std::cin, fname);

    std::cout << "Veuillez entrer le nom du patient : ";
    std::getline(std::cin, last_name);

    // Récupération des infos du patient
    vector<string> creds = mp->getUserCreds(db, fname, last_name);
    if (creds.empty()) {
        std::cerr << "Patient introuvable ou inactif.\n";
        return;
    }

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

    // Demander au médecin s'il souhaite consulter le dossier medical
    std::cout << "Souhaitez-vous consulter le dossier médical du patient ? (o/n) : ";
    char choix;
    std::cin >> choix;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    if (choix == 'o' || choix == 'O') {
        DossierMedical* dossier = patient->getDossierMedical();
        if (dossier)
            dossier->afficher(autorisation);
        else
            std::cout << "Aucun dossier médical trouvé.\n";
    }

    // --- Saisie des infos de la consultation ---
    std::cout << "\n--- Ajouter une consultation ---\n";

    std::string motif, observations;

    std::cout << "Motif : ";
    std::getline(std::cin, motif);

    std::cout << "Observations : ";
    std::getline(std::cin, observations);

    // --- Ajout éventuel d'examens ---
    std::vector<Examen> examens;
    std::cout << "\nAjouter un examen ? (o/n) : ";
    std::cin >> choix;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');


        std::string date, typeExamen, resultat;

        std::cout << "\n--- Nouvel examen ---\n";

        std::cout << "Date (jj/mm/aaaa) : ";
        std::getline(std::cin, date);

        std::cout << "Type d'examen : ";
        std::getline(std::cin, typeExamen);

        std::cout << "Résultat : ";
        std::getline(std::cin, resultat);

        cout << "\n----------------\n";

    std::string prescription;
    std::cout << "Prescription : ";
    std::getline(std::cin, prescription);

    // --- Création de la consultation ---
    Consultation nouvelleConsultation(
        mp,
        db,
        this,
        patient->getId(),
        motif,
        observations,
        prescription,
        examens
    );

    examens.emplace_back(db, nouvelleConsultation.id, mp->getDossierId(db,patient->getId()), date, typeExamen, resultat);

    std::cout << "\nConsultation ajoutée avec succès pour le patient "
              << patient->getNomComplet() << ".\n";
}


void Medecin::exportDossiersCSV(int patientId) {

    // -------------------------
    // Vérifier l'existence du patient
    // -------------------------
    sqlite3_stmt* stmtUser = nullptr;
    const char* sqlUser =
        "SELECT u.firstname, u.last_name "
        "FROM PATIENTS p "
        "JOIN USERS u ON p.patient_user_id = u.id "
        "WHERE p.id = ? AND u.is_active = 1;";

    if (sqlite3_prepare_v2(db, sqlUser, -1, &stmtUser, nullptr) != SQLITE_OK) {
        std::cerr << "Erreur SQL (patient lookup): " << sqlite3_errmsg(db) << std::endl;
        return;
    }

    sqlite3_bind_int(stmtUser, 1, patientId);

    if (sqlite3_step(stmtUser) != SQLITE_ROW) {
        std::cerr << "Patient introuvable ou inactif." << std::endl;
        sqlite3_finalize(stmtUser);
        return;
    }

    std::string firstname = (const char*)sqlite3_column_text(stmtUser, 0);
    std::string lastname  = (const char*)sqlite3_column_text(stmtUser, 1);

    sqlite3_finalize(stmtUser);

    // -------------------------
    // Créer fichier CSV
    // -------------------------
    std::string filename =
        "./export/dossier_" + std::to_string(patientId) + ".csv";

    std::filesystem::create_directories("./export");
    std::ofstream out(filename);

    if (!out.is_open()) {
        std::cerr << "Impossible d'écrire dans " << filename << "\n";
        return;
    }

    // helper pour échapper/quoter les champs CSV
    auto csv = [&](const std::string& s) {
        if (s.find(';') != std::string::npos ||
            s.find('"') != std::string::npos ||
            s.find('\n') != std::string::npos) {
            std::string escaped = "\"";
            for (char c : s) {
                if (c == '"') escaped += "\"\"";  // échapper les guillemets
                else escaped += c;
            }
            escaped += "\"";
            return escaped;
        }
        return s;
    };

    // -------------------------
    // ENTÊTE PATIENT (format: nom;prenom)
    // -------------------------
    out << csv(lastname) << ";" << csv(firstname) << "\n\n";

    // =======================================================================================
    //  SECTION 1 — ANTECEDENTS
    // =======================================================================================
    out << "# ANTECEDENTS\n";
    out << "type;observations\n";

    sqlite3_stmt* stmtAnte = nullptr;
    const char* sqlAnte =
        "SELECT type, observations "
        "FROM ANTECEDANTS "
        "WHERE dossier_id IN (SELECT id FROM DOSSIERS_MEDICAUX WHERE patient_id = ?);";

    sqlite3_prepare_v2(db, sqlAnte, -1, &stmtAnte, nullptr);
    sqlite3_bind_int(stmtAnte, 1, patientId);

    while (sqlite3_step(stmtAnte) == SQLITE_ROW) {
        std::string type = (const char*)sqlite3_column_text(stmtAnte, 0);
        std::string obs  = (const char*)sqlite3_column_text(stmtAnte, 1);
        out << csv(type) << ";" << csv(obs) << "\n";
    }
    sqlite3_finalize(stmtAnte);

    out << "FIN_ANTECEDENTS\n\n";


    // =======================================================================================
    //  SECTION 2 — CONSULTATIONS
    // =======================================================================================
    out << "# CONSULTATIONS\n";
    out << "dossier_id;professionnel_id;patient_id;date_et_heure;motif;observations;prescription\n";

    sqlite3_stmt* stmtCons = nullptr;
    const char* sqlCons =
        "SELECT id, dossier_id, professionnel_id, patient_id, "
        "       date_et_heure, motif, observations, prescription "
        "FROM CONSULTATIONS "
        "WHERE dossier_id IN (SELECT id FROM DOSSIERS_MEDICAUX WHERE patient_id = ?);";

    sqlite3_prepare_v2(db, sqlCons, -1, &stmtCons, nullptr);
    sqlite3_bind_int(stmtCons, 1, patientId);

    std::vector<int> consultationIds;

    while (sqlite3_step(stmtCons) == SQLITE_ROW) {
        int cId        = sqlite3_column_int(stmtCons, 0);
        int dosId      = sqlite3_column_int(stmtCons, 1);
        int profId     = sqlite3_column_int(stmtCons, 2);
        int patId      = sqlite3_column_int(stmtCons, 3);
        std::string dateHeure = (const char*)sqlite3_column_text(stmtCons, 4);
        std::string motif     = (const char*)sqlite3_column_text(stmtCons, 5);
        std::string obs       = (const char*)sqlite3_column_text(stmtCons, 6);
        std::string presc     = (const char*)sqlite3_column_text(stmtCons, 7);

        consultationIds.push_back(cId);

        out << dosId << ";"
            << profId << ";"
            << patId << ";"
            << csv(dateHeure) << ";"
            << csv(motif) << ";"
            << csv(obs) << ";"
            << csv(presc) << "\n";
    }
    sqlite3_finalize(stmtCons);

    out << "FIN_CONSULTATIONS\n\n";


    // =======================================================================================
    //  SECTION 3 — EXAMENS
    // =======================================================================================
    out << "# EXAMENS\n";
    out << "consultation_id;dossier_id;type;results;date\n";

    sqlite3_stmt* stmtExam = nullptr;
    const char* sqlExam =
        "SELECT consultation_id, dossier_id, type, results, date "
        "FROM EXAMENS "
        "WHERE dossier_id IN (SELECT id FROM DOSSIERS_MEDICAUX WHERE patient_id = ?);";

    sqlite3_prepare_v2(db, sqlExam, -1, &stmtExam, nullptr);
    sqlite3_bind_int(stmtExam, 1, patientId);

    while (sqlite3_step(stmtExam) == SQLITE_ROW) {
        int cID   = sqlite3_column_int(stmtExam, 0);
        int dID   = sqlite3_column_int(stmtExam, 1);
        std::string type    = (const char*)sqlite3_column_text(stmtExam, 2);
        std::string results = (const char*)sqlite3_column_text(stmtExam, 3);
        std::string date    = (const char*)sqlite3_column_text(stmtExam, 4);

        out << cID << ";"
            << dID << ";"
            << csv(type) << ";"
            << csv(results) << ";"
            << csv(date) << "\n";
    }
    sqlite3_finalize(stmtExam);

    out << "FIN_EXAMENS\n\n";


    // =======================================================================================
    //  SECTION 4 — SOINS
    // =======================================================================================
    out << "# SOINS\n";
    out << "dossier_id;type;description;date;infirmier_id\n";

    sqlite3_stmt* stmtSoins = nullptr;
    const char* sqlSoins =
        "SELECT dossier_id, type, description, date, infirmier_id "
        "FROM SOINS "
        "WHERE dossier_id IN (SELECT id FROM DOSSIERS_MEDICAUX WHERE patient_id = ?);";

    sqlite3_prepare_v2(db, sqlSoins, -1, &stmtSoins, nullptr);
    sqlite3_bind_int(stmtSoins, 1, patientId);

    while (sqlite3_step(stmtSoins) == SQLITE_ROW) {
        int dosId = sqlite3_column_int(stmtSoins, 0);
        std::string type = (const char*)sqlite3_column_text(stmtSoins, 1);
        std::string desc = (const char*)sqlite3_column_text(stmtSoins, 2);
        std::string date = (const char*)sqlite3_column_text(stmtSoins, 3);

        int infID = 0;
        if (sqlite3_column_type(stmtSoins, 4) != SQLITE_NULL)
            infID = sqlite3_column_int(stmtSoins, 4);

        out << dosId << ";"
            << csv(type) << ";"
            << csv(desc) << ";"
            << csv(date) << ";";
        if (infID > 0) out << infID;
        out << "\n";
    }
    sqlite3_finalize(stmtSoins);

    out << "FIN_SOINS\n\n";

    out.close();

    std::cout << "[✔] Export terminé : " << filename << std::endl;
}

void Medecin::importDossiersCSV() {
    std::string path = "./import";

    if (!fs::exists(path) || !fs::is_directory(path)) {
        std::cout << "Dossier d'import introuvable." << std::endl;
        return;
    }

    std::vector<fs::path> files;
    for (auto &e : fs::directory_iterator(path)) {
        if (e.path().extension() == ".csv")
            files.push_back(e.path());
    }

    if (files.empty()) {
        std::cout << "Aucun fichier CSV trouvé.\n";
        return;
    }

    std::cout << "Fichiers détectés : \n";
    for (size_t i = 0; i < files.size(); i++)
        std::cout << i + 1 << ". " << files[i].filename().string() << "\n";

    int choice;
    std::cout << "Choisissez un fichier : ";
    std::cin >> choice;
    std::cin.ignore();

    if (choice < 1 || choice > (int)files.size()) {
        std::cout << "Choix invalide." << std::endl;
        return;
    }

    std::string filename = files[choice - 1].string();

    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Impossible d'ouvrir le fichier.\n";
        return;
    }

    cout << "Veuillez préciser l'ID du patient pour l'importation : ";
    int patientId;
    cin >> patientId;
    cin.ignore();
    if (!patientValide(patientId)) {
        std::cerr << "ID patient invalide.\n";
        return;
    }

    // activer les foreign keys (important)
    char* errMsg = nullptr;
    if (sqlite3_exec(db, "PRAGMA foreign_keys = ON;", nullptr, nullptr, &errMsg) != SQLITE_OK) {
        std::cerr << "Impossible d'activer foreign_keys: " << (errMsg ? errMsg : "") << std::endl;
        if (errMsg) sqlite3_free(errMsg);
        // continuer quand même pour log
    }

    // parser CSV simple
    auto parseCSV = [&](const std::string& line) {
        std::vector<std::string> fields;
        std::string cur;
        bool inQuotes = false;
        for (char c : line) {
            if (c == '"') inQuotes = !inQuotes;
            else if (c == ';' && !inQuotes) { fields.push_back(cur); cur.clear(); }
            else cur.push_back(c);
        }
        fields.push_back(cur);
        for (auto &f : fields)
            if (f.size() >= 2 && f.front() == '"' && f.back() == '"')
                f = f.substr(1, f.size() - 2);
        return fields;
    };

    // lire entête + ligne patient
    std::string line;
    while (std::getline(file, line) && line.size() == 0) { }
    if (file.fail() || line.empty()) { std::cerr << "Fichier vide ou en-tête introuvable.\n"; return; }
    auto header = parseCSV(line);
    if (header.size() < 2) { std::cerr << "En-tête CSV invalide.\n"; return; }

    while (std::getline(file, line) && line.size() == 0) { }
    if (file.fail() || line.empty()) { std::cerr << "Ligne patient manquante après l'en-tête.\n"; return; }
    auto patientRow = parseCSV(line);
    if (patientRow.size() < 2) { std::cerr << "Format de la ligne patient invalide. Contenu lu: \"" << line << "\"\n"; return; }

    auto trim = [](std::string s)->std::string {
        size_t a = 0; while (a < s.size() && isspace((unsigned char)s[a])) ++a;
        size_t b = s.size(); while (b > a && isspace((unsigned char)s[b-1])) --b;
        return s.substr(a, b-a);
    };

    std::string csvLastname, csvFirstname;
    if (patientRow.size() == 2) {
        csvLastname  = trim(patientRow[0]);
        csvFirstname = trim(patientRow[1]);
    } else {
        bool firstIsNum = !patientRow[0].empty() &&
                          std::all_of(patientRow[0].begin(), patientRow[0].end(),
                                      [](char c){ return std::isdigit((unsigned char)c); });
        if (firstIsNum) {
            csvLastname  = trim(patientRow[1]);
            csvFirstname = trim(patientRow[2]);
        } else {
            csvLastname  = trim(patientRow[0]);
            csvFirstname = trim(patientRow[1]);
        }
    }

    // vérifier correspondance patient dans la base
    sqlite3_stmt* stmt = nullptr;
    const char* sql = "SELECT firstname, last_name FROM USERS WHERE id = ?";
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Erreur prepare select user: " << sqlite3_errmsg(db) << std::endl;
        return;
    }
    sqlite3_bind_int(stmt, 1, mp->getUserId(db, patientId));
    if (sqlite3_step(stmt) != SQLITE_ROW) {
        std::cerr << "Patient inexistant.\n";
        sqlite3_finalize(stmt);
        return;
    }
    std::string dbFirstname = (const char*)sqlite3_column_text(stmt, 0);
    std::string dbLastname  = (const char*)sqlite3_column_text(stmt, 1);
    sqlite3_finalize(stmt);

    cout << "\nS'agit -il bien du patient " << dbFirstname << " " << dbLastname << " ? (o/n) : ";
    char confirm; std::cin >> confirm;
    if (confirm != 'o' && confirm != 'O') { std::cout << "Import annulé.\n"; return; }

    if (dbFirstname != csvFirstname || dbLastname != csvLastname) {
        cout << "[!] Fichier vs Base : " << csvFirstname << " " << csvLastname << "  /  " << dbFirstname << " " << dbLastname << "\n";
        cout << "Continuer l'import ? (o/n) : ";
        char c; std::cin >> c; if (c != 'o' && c != 'O') return;
    }

    // dossier existant ou création
    int dossierId = mp->getDossierId(db, patientId);
    if (dossierId <= 0) {
        const char* sqlCreateD = "INSERT INTO DOSSIERS_MEDICAUX (patient_id, created_at) VALUES (?, datetime('now'));";
        sqlite3_stmt* stCreate = nullptr;
        if (sqlite3_prepare_v2(db, sqlCreateD, -1, &stCreate, nullptr) != SQLITE_OK) {
            std::cerr << "Erreur prepare create dossier: " << sqlite3_errmsg(db) << std::endl;
            return;
        }
        sqlite3_bind_int(stCreate, 1, patientId);
        int rc = sqlite3_step(stCreate);
        if (rc != SQLITE_DONE) {
            std::cerr << "Erreur create dossier: " << sqlite3_errmsg(db) << std::endl;
            sqlite3_finalize(stCreate);
            return;
        }
        sqlite3_finalize(stCreate);
        dossierId = (int)sqlite3_last_insert_rowid(db);
        std::cout << "[DBG] Dossier créé id=" << dossierId << "\n";
    } else {
        std::cout << "[DBG] Dossier existant id=" << dossierId << "\n";
    }

    // début transaction
    if (sqlite3_exec(db, "BEGIN TRANSACTION;", nullptr, nullptr, &errMsg) != SQLITE_OK) {
        std::cerr << "BEGIN TRANSACTION failed: " << (errMsg ? errMsg : "") << std::endl;
        if (errMsg) sqlite3_free(errMsg);
        // on continue quand même
    }

    // préparer statements
    sqlite3_stmt* stAnte = nullptr;
    sqlite3_stmt* stCons = nullptr;
    sqlite3_stmt* stExam = nullptr;
    sqlite3_stmt* stSoin = nullptr;

    const char* sqlA = "INSERT INTO ANTECEDANTS (dossier_id, type, observations) VALUES (?,?,?);";
    const char* sqlC = "INSERT INTO CONSULTATIONS (dossier_id, professionnel_id, patient_id, date_et_heure, motif, observations, prescription) VALUES (?,?,?,?,?,?,?);";
    const char* sqlE = "INSERT INTO EXAMENS (consultation_id, dossier_id, type, results, date) VALUES (?,?,?,?,?);";
    const char* sqlS = "INSERT INTO SOINS (dossier_id, type, description, date, infirmier_id) VALUES (?,?,?,?,?);";

    if (sqlite3_prepare_v2(db, sqlA, -1, &stAnte, nullptr) != SQLITE_OK) { std::cerr << "prepare ANTE failed: " << sqlite3_errmsg(db) << "\n"; }
    if (sqlite3_prepare_v2(db, sqlC, -1, &stCons, nullptr) != SQLITE_OK) { std::cerr << "prepare CONS failed: " << sqlite3_errmsg(db) << "\n"; }
    if (sqlite3_prepare_v2(db, sqlE, -1, &stExam, nullptr) != SQLITE_OK) { std::cerr << "prepare EXAM failed: " << sqlite3_errmsg(db) << "\n"; }
    if (sqlite3_prepare_v2(db, sqlS, -1, &stSoin, nullptr) != SQLITE_OK) { std::cerr << "prepare SOIN failed: " << sqlite3_errmsg(db) << "\n"; }

    // helper: check if a user id exists in USERS
    auto userExists = [&](int uid)->bool {
        if (uid <= 0) return false;
        sqlite3_stmt* ch = nullptr;
        const char* sqlu = "SELECT 1 FROM USERS WHERE id = ? LIMIT 1;";
        if (sqlite3_prepare_v2(db, sqlu, -1, &ch, nullptr) != SQLITE_OK) return false;
        sqlite3_bind_int(ch, 1, uid);
        int r = sqlite3_step(ch);
        sqlite3_finalize(ch);
        return r == SQLITE_ROW;
    };

    // helper: execute step and check result
   auto do_step_check = [&](sqlite3_stmt* s, const std::string& ctx)->bool {
       if (!s) { std::cerr << "[DBG] statement null for " << ctx << "\n"; return false; }
       int rc = sqlite3_step(s);
       if (rc != SQLITE_DONE) {
           std::cerr << "Erreur insert (" << ctx << "): " << sqlite3_errmsg(db) << "\n";
           return false;
       }
       return true;
   };

    // parcourir sections
    enum Section { NONE, ANTE, CONS, EXAM, SOIN };
    Section current = NONE;
    bool firstLine = true;
    std::vector<int> createdConsultIds;
    std::unordered_map<int,int> origToNewCons; // mapping original_cons_id -> new_cons_id

    while (std::getline(file, line)) {
        if (line.empty()) continue;

        if (line == "# ANTECEDENTS") { current = ANTE; std::getline(file, line); if(firstLine){std::getline(file, line); firstLine = false;} continue; }
        if (line == "FIN_ANTECEDENTS") { current = NONE; firstLine = true; continue; }
        if (line == "# CONSULTATIONS") { current = CONS; std::getline(file, line); if(firstLine){std::getline(file, line); firstLine = false;} continue; }
        if (line == "FIN_CONSULTATIONS") { current = NONE; firstLine = true; continue; }
        if (line == "# EXAMENS") { current = EXAM; std::getline(file, line); if(firstLine){std::getline(file, line); firstLine = false;} continue; }
        if (line == "FIN_EXAMENS") { current = NONE; firstLine = true; continue; }
        if (line == "# SOINS") { current = SOIN; std::getline(file, line); if(firstLine){std::getline(file, line); firstLine = false;} continue; }
        if (line == "FIN_SOINS") { current = NONE; firstLine = true; continue; }

        if (current == ANTE) {
            auto f = parseCSV(line);
            if (f.size() < 2) continue;
            sqlite3_reset(stAnte); sqlite3_clear_bindings(stAnte);
            sqlite3_bind_int(stAnte, 1, dossierId);
            sqlite3_bind_text(stAnte, 2, f[0].c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(stAnte, 3, f[1].c_str(), -1, SQLITE_TRANSIENT);
            do_step_check(stAnte, "ANTECEDANTS");
            sqlite3_reset(stAnte);
        }

        if (current == CONS) {
            auto f = parseCSV(line);
            if (f.size() < 7) continue;
            // Support possible optional original_cons_id as first column:
            // if f.size() == 8 → f[0] = original_cons_id, else no original id.
            int offset = (f.size() >= 8) ? 1 : 0;
            int origConsId = 0;
            if (offset == 1) {
                try { origConsId = std::stoi(f[0]); } catch(...) { origConsId = 0; }
            }
            int f_prof_id = 0;
            try { f_prof_id = std::stoi(f[offset + 1]); } catch(...) { f_prof_id = 0; }

            // si le professionnel référencé n'existe pas dans la DB, fallback vers ce médecin
            if (!userExists(f_prof_id)) {
                std::cerr << "[WARN] professionnel_id " << f_prof_id << " introuvable -> utilisation de l'id du médecin (" << this->id << ")\n";
                f_prof_id = this->id;
            }

            sqlite3_reset(stCons); sqlite3_clear_bindings(stCons);
            sqlite3_bind_int(stCons, 1, dossierId);                         // dossier_id (from CSV or overridden)
            sqlite3_bind_int(stCons, 2, f_prof_id);                         // professionnel_id
            sqlite3_bind_int(stCons, 3, patientId);                         // patient_id (we enforce chosen patient)
            sqlite3_bind_text(stCons, 4, f[offset + 3].c_str(), -1, SQLITE_TRANSIENT); // date_et_heure
            sqlite3_bind_text(stCons, 5, f[offset + 4].c_str(), -1, SQLITE_TRANSIENT); // motif
            sqlite3_bind_text(stCons, 6, f[offset + 5].c_str(), -1, SQLITE_TRANSIENT); // observations
            sqlite3_bind_text(stCons, 7, f[offset + 6].c_str(), -1, SQLITE_TRANSIENT); // prescription
            if (do_step_check(stCons, "CONSULTATIONS")) {
                int consId = (int)sqlite3_last_insert_rowid(db);
                createdConsultIds.push_back(consId);
                if (origConsId != 0) origToNewCons[origConsId] = consId;
                std::cout << "[DBG] Consultation inserted id=" << consId << " (orig=" << origConsId << ")\n";
            }
            sqlite3_reset(stCons);
        }

        if (current == EXAM) {
            auto f = parseCSV(line);
            if (f.size() < 5) continue;
            sqlite3_reset(stExam); sqlite3_clear_bindings(stExam);
            // la première colonne peut être :
            // - l'ID original de la consultation (ex: 10)
            // - ou l'index 1-based de la consultation telle qu'elle apparaît dans le fichier
            int csvCons = 0;
            try { csvCons = std::stoi(f[0]); } catch(...) { csvCons = 0; }

            int realCons = 0;
            auto it = origToNewCons.find(csvCons);
            if (it != origToNewCons.end()) {
                realCons = it->second; // mapping original -> new
            } else if (csvCons > 0 && csvCons <= (int)createdConsultIds.size()) {
                // treat as index into createdConsultIds (1-based)
                realCons = createdConsultIds[csvCons - 1];
            } else if (!createdConsultIds.empty()) {
                // fallback: associate to last created consultation
                realCons = createdConsultIds.back();
            } else {
                // no consultation available -> insert exam without consultation (NULL) if schema allows
                realCons = 0;
            }
            if (realCons > 0) sqlite3_bind_int(stExam, 1, realCons); else sqlite3_bind_null(stExam, 1);

            sqlite3_bind_int(stExam, 2, dossierId);
            sqlite3_bind_text(stExam, 3, f[2].c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(stExam, 4, f[3].c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(stExam, 5, f[4].c_str(), -1, SQLITE_TRANSIENT);
            do_step_check(stExam, "EXAMENS");
            sqlite3_reset(stExam);
        }

        if (current == SOIN) {
            auto f = parseCSV(line);
            // expected: dossier_id;type;description;date;infirmier_id
            if (f.size() < 3) continue;
            sqlite3_reset(stSoin); sqlite3_clear_bindings(stSoin);
            sqlite3_bind_int(stSoin, 1, dossierId);
            // type
            sqlite3_bind_text(stSoin, 2, (f.size() > 1 ? f[1] : "").c_str(), -1, SQLITE_TRANSIENT);
            // description
            sqlite3_bind_text(stSoin, 3, (f.size() > 2 ? f[2] : "").c_str(), -1, SQLITE_TRANSIENT);
            // date
            if (f.size() > 3 && !f[3].empty()) sqlite3_bind_text(stSoin, 4, f[3].c_str(), -1, SQLITE_TRANSIENT);
            else sqlite3_bind_null(stSoin, 4);
            // infirmier_id (optional, only bind if exists in USERS)
            if (f.size() > 4 && !f[4].empty()) {
                int infId = 0;
                try { infId = std::stoi(f[4]); } catch(...) { infId = 0; }
                if (userExists(infId)) sqlite3_bind_int(stSoin, 5, infId);
                else sqlite3_bind_null(stSoin, 5);
            } else {
                sqlite3_bind_null(stSoin, 5);
            }
            do_step_check(stSoin, "SOINS");
            sqlite3_reset(stSoin);
        }
    }

    // finaliser statements
    if (stAnte) sqlite3_finalize(stAnte);
    if (stCons) sqlite3_finalize(stCons);
    if (stExam) sqlite3_finalize(stExam);
    if (stSoin) sqlite3_finalize(stSoin);

    // commit
    if (sqlite3_exec(db, "COMMIT;", nullptr, nullptr, &errMsg) != SQLITE_OK) {
        std::cerr << "COMMIT failed: " << (errMsg ? errMsg : "") << std::endl;
        if (errMsg) sqlite3_free(errMsg);
        sqlite3_exec(db, "ROLLBACK;", nullptr, nullptr, nullptr);
    } else {
        std::cout << "[+] Import terminé avec succès. changements = " << sqlite3_changes(db) << "\n";
    }

    file.close();
    fs::remove(filename);

    // debug counts
    sqlite3_stmt* stCount = nullptr;
    const char* sqlCount = "SELECT COUNT(*) FROM ANTECEDANTS WHERE dossier_id = ?;";
    if (sqlite3_prepare_v2(db, sqlCount, -1, &stCount, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stCount, 1, dossierId);
        if (sqlite3_step(stCount) == SQLITE_ROW) {
            std::cout << "[DBG] ANTECEDANTS count for dossier " << dossierId << " = " << sqlite3_column_int(stCount,0) << "\n";
        }
        sqlite3_finalize(stCount);
    }
    // (similaire pour CONSULTATIONS, EXAMENS, SOINS si nécessaire)
}

void Medecin::prendreEnChargePatient() {

    int patient_id;
    std::cout << "Entrez le patient ID du patient à prendre en charge : ";
    std::cin >> patient_id;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    if(!patientValide(patient_id)){
        std::cerr << "Erreur : patient invalide ou non attribué à ce professionnel.\n";
        return;
    }

    // ============ NOUVELLES VÉRIFICATIONS ============
    // Vérifier que le patient existe dans PATIENTS
    sqlite3_stmt* chk1 = nullptr;
    const char* sqlChk1 = "SELECT id FROM PATIENTS WHERE id = ? LIMIT 1;";
    if (sqlite3_prepare_v2(db, sqlChk1, -1, &chk1, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(chk1, 1, patient_id);
        if (sqlite3_step(chk1) != SQLITE_ROW) {
            std::cerr << "Erreur : patient ID " << patient_id << " n'existe pas dans PATIENTS.\n";
            sqlite3_finalize(chk1);
            return;
        }
        sqlite3_finalize(chk1);
    } else {
        std::cerr << "Erreur prepare check patient: " << sqlite3_errmsg(db) << std::endl;
        return;
    }

    // Vérifier que le professionnel (you) existe dans la table parent
    // (ajuster le nom de la table selon votre schéma : PROFESSIONALS, USERS, PRO_SANTE, etc.)
    sqlite3_stmt* chk2 = nullptr;
    const char* sqlChk2 = "SELECT id FROM USERS WHERE id = ? LIMIT 1;";  // adapter si nécessaire
    if (sqlite3_prepare_v2(db, sqlChk2, -1, &chk2, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(chk2, 1, this->id);
        if (sqlite3_step(chk2) != SQLITE_ROW) {
            std::cerr << "Erreur : vous (ID " << this->getId()
                      << ") n'existe pas dans PROFESSIONALS. Vérifiez votre enregistrement.\n";
            sqlite3_finalize(chk2);
            return;
        }
        sqlite3_finalize(chk2);
    } else {
        // Si la table PROFESSIONALS n'existe pas, essayer USERS
        std::cerr << "Attention : impossible de vérifier professionnel dans PROFESSIONALS. "
                  << "Tentative avec USERS...\n";
        sqlite3_finalize(chk2);

        chk2 = nullptr;
        sqlChk2 = "SELECT id FROM USERS WHERE id = ? AND role LIKE '%professionnel%' LIMIT 1;";
        if (sqlite3_prepare_v2(db, sqlChk2, -1, &chk2, nullptr) == SQLITE_OK) {
            sqlite3_bind_int(chk2, 1, this->getId());
            if (sqlite3_step(chk2) != SQLITE_ROW) {
                std::cerr << "Erreur : vous (ID " << this->getId() << ") n'existe pas dans USERS.\n";
                sqlite3_finalize(chk2);
                return;
            }
            sqlite3_finalize(chk2);
        } else {
            std::cerr << "Erreur prepare check professionnel: " << sqlite3_errmsg(db) << std::endl;
            return;
        }
    }
    // ============ FIN VÉRIFICATIONS ============

    const char* sql =
        "INSERT INTO PRIS_EN_CHARGE (professionnel_id, patient_id) "
        "VALUES (?, ?);";

    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);

    if(rc != SQLITE_OK){
        std::cerr << "Erreur prepare: " << sqlite3_errmsg(db) << std::endl;
        return;
    }

    sqlite3_bind_int(stmt, 1, this->id);
    sqlite3_bind_int(stmt, 2, patient_id);

    rc = sqlite3_step(stmt);

    if(rc != SQLITE_DONE){
        std::cerr << "Erreur insert: " << sqlite3_errmsg(db) << " (professionnel_id="
                  << this->id << ", patient_id=" << patient_id << ")\n";
        sqlite3_finalize(stmt);
        return;
    }

    sqlite3_finalize(stmt);

    // Récupérer l'ID utilisateur correspondant au patient (users.id)
    int userId = mp->getUserId(db, patient_id); // mp->getUserId doit retourner users.id
    if (userId <= 0) {
        std::cerr << "Attention : impossible de récupérer l'ID utilisateur du patient.\n";
        return;
    }

    // Construire un Patient avec l'ID utilisateur et afficher son nom
    Patient p(this->mp, this->db, patient_id);
    std::cout << "Vous prenez désormais en charge le patient : " << p.getNomComplet() << "\n";

    // stocker l'ID utilisateur dans patients_id (cohérence avec load_patient / getId)
    if (std::find(patients_id.begin(), patients_id.end(), userId) == patients_id.end()) {
        patients_id.push_back(userId);
    }
}

void Medecin::libererPatient() {

    int patient_id;
    std::cout << "\nVoici la liste de vos patients actuels :\n";
    afficher_patients();
    std::cout << "\nEntrez le patient ID du patient à libérer de votre prise en charge : ";
    std::cin >> patient_id;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    const char* sql =
        "DELETE FROM PRIS_EN_CHARGE "
        "WHERE professionnel_id = ? AND patient_id = ?;";

    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);

    if(rc != SQLITE_OK){
        std::cerr << "Erreur prepare: " << sqlite3_errmsg(db) << std::endl;
        return;
    }

    sqlite3_bind_int(stmt, 1, this->getId());
    sqlite3_bind_int(stmt, 2, patient_id);

    rc = sqlite3_step(stmt);

    if(rc != SQLITE_DONE){
        std::cerr << "Erreur delete: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt);
        return;
    }

    sqlite3_finalize(stmt);
    patients_id.erase(std::remove(patients_id.begin(), patients_id.end(), patient_id), patients_id.end());
}

void Medecin::afficher_infos_professionnelles() {
    cout << "\n--- Vos informations professionnelles ---\n";
    cout << "Nom d'utilisateur : " << Medecin::getLast_name() << "\n";
    cout << "Rôle : " << Medecin::getRole() << "\n";
    cout << "Autorisation : " << obtenir_autorisation() << "\n";
    cout << "Statut : " << obtenir_statut() << "\n";
    cout << "Actif : " << (isActive() ? "Oui" : "Non") << "\n";
}

void Medecin::enregistrerDisponibilites() {
    vector<string> heure_start;
    vector<string> heure_end;

    vector<string> days;
    days.push_back("Lundi");
    days.push_back("Mardi");
    days.push_back("Mercredi");
    days.push_back("Jeudi");
    days.push_back("Vendredi");
    days.push_back("Samedi");
    days.push_back("Dimanche");

    int day=0;
    std::cout << "\n--- Enregistrement de disponibilités ---\n";

    while(day!=6){
        std::cout << "============ " << days[day] << " ============\n";
        std::cout << "Voulez-vous ajouter une disponibilité pour ce jour ? (1=Oui, 0=Non) : ";
        int ajouter;
        std::cin >> ajouter;
        if(ajouter == 1){
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

            std::cout << "Heure de début (HH:MM) : ";
            std::string heure_start;
            std::getline(std::cin, heure_start);

            std::cout << "Heure de fin (HH:MM) : ";
            std::string heure_end;
            std::getline(std::cin, heure_end);

            if (ajouter_disponibilite(db, this->getId(), day, heure_start, heure_end)) {
                std::cout << "Disponibilité enregistrée avec succès.\n";
            } else {
                std::cout << "Erreur lors de l'enregistrement de la disponibilité.\n";
            }
        }
        day++;
    }
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
        cout << "1. Enregistrer mes disponibilites\n";
        cout << "2. Voir mes disponibilites\n";
        cout << "3. Créer une consultation\n";
        cout << "4. Voir mes informations professionnelles\n";
        cout << "5. Editer un dossier medical\n";
        cout << "6. Export CSV des dossiers medicaux\n";
        cout << "7. Import CSV des dossiers medicaux\n";
        cout << "0. Se deconnecter / Quitter\n";
        cout << "------------------------------\n\n";
        cout << "Votre choix : ";

        cin >> choix;

        // Ici on gère les entrées invalides
        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Entree invalide. Veuillez reessayer.\n";
            continue;
        }

        switch (choix) {

    case 1:
        enregistrerDisponibilites();
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

    case 5: {
        cout << "\n--- Edition d'un dossier médical ---\n";
        cout << "Veuillez préciser le nom du patient : ";
        std::string nom_patient;
        cin.ignore();
        std::getline(cin, nom_patient);

        cout << "Veuillez préciser le prenom du patient : ";
        std::string prenom_patient;
        std::getline(cin, prenom_patient);

        editer_dossier_medical(db, mp, nom_patient, prenom_patient);
        break;
    }

    case 6: {
        cout << "Veuillez préciser l'ID du patient pour l'exportation : ";
        int patientIdExport;
        cin >> patientIdExport;
        cin.ignore();

        if (!patientValide(patientIdExport)) {
            std::cerr << "ID patient invalide.\n";
            break;
        }

        exportDossiersCSV(patientIdExport);
        break;
    }

    case 7:
        importDossiersCSV();
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

//<>


