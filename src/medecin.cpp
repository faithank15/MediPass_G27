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

    auto csv = [&](const std::string& s) {
        if (s.find(';') != std::string::npos ||
            s.find('"') != std::string::npos)
            return "\"" + s + "\"";
        return s;
    };

    // -------------------------
    // ENTÊTE PATIENT
    // -------------------------
    out << patientId << ";" << csv(lastname) << ";" << csv(firstname) << "\n\n";


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
        std::string type   = (const char*)sqlite3_column_text(stmtAnte, 0);
        std::string obs    = (const char*)sqlite3_column_text(stmtAnte, 1);
        out << csv(type) << ";" << csv(obs) << "\n";
    }
    sqlite3_finalize(stmtAnte);

    out << "FIN_ANTECEDENTS\n\n";


    // =======================================================================================
    //  SECTION 2 — CONSULTATIONS
    // =======================================================================================
    out << "# CONSULTATIONS\n";
    out << "notes\n";

    sqlite3_stmt* stmtCons = nullptr;
    const char* sqlCons =
        "SELECT id, notes FROM CONSULTATIONS "
        "WHERE dossier_id IN (SELECT id FROM DOSSIERS_MEDICAUX WHERE patient_id = ?);";

    sqlite3_prepare_v2(db, sqlCons, -1, &stmtCons, nullptr);
    sqlite3_bind_int(stmtCons, 1, patientId);

    std::vector<int> consultationIds;

    while (sqlite3_step(stmtCons) == SQLITE_ROW) {
        int cId = sqlite3_column_int(stmtCons, 0);
        consultationIds.push_back(cId);

        std::string notes = (const char*)sqlite3_column_text(stmtCons, 1);
        out << csv(notes) << "\n";
    }
    sqlite3_finalize(stmtCons);

    out << "FIN_CONSULTATIONS\n\n";


    // =======================================================================================
    //  SECTION 3 — EXAMENS
    // =======================================================================================
    out << "# EXAMENS\n";
    out << "type;results;consultation_index\n";

    sqlite3_stmt* stmtExam = nullptr;
    const char* sqlExam =
        "SELECT type, results, consultation_id "
        "FROM EXAMENS "
        "WHERE dossier_id IN (SELECT id FROM DOSSIERS_MEDICAUX WHERE patient_id = ?);";

    sqlite3_prepare_v2(db, sqlExam, -1, &stmtExam, nullptr);
    sqlite3_bind_int(stmtExam, 1, patientId);

    while (sqlite3_step(stmtExam) == SQLITE_ROW) {

        std::string type   = (const char*)sqlite3_column_text(stmtExam, 0);
        std::string res    = (const char*)sqlite3_column_text(stmtExam, 1);
        int cID            = sqlite3_column_int(stmtExam, 2);

        // calcul de l’index de la consultation
        int index = 0;
        for (size_t i = 0; i < consultationIds.size(); ++i)
            if (consultationIds[i] == cID) index = i + 1;

        out << csv(type) << ";" << csv(res) << ";" << index << "\n";
    }
    sqlite3_finalize(stmtExam);

    out << "FIN_EXAMENS\n\n";


    // =======================================================================================
    //  SECTION 4 — SOINS
    // =======================================================================================
    out << "# SOINS\n";
    out << "description;date;infirmier_id\n";

    sqlite3_stmt* stmtSoins = nullptr;
    const char* sqlSoins =
        "SELECT description, date, infirmier_id "
        "FROM SOINS "
        "WHERE dossier_id IN (SELECT id FROM DOSSIERS_MEDICAUX WHERE patient_id = ?);";

    sqlite3_prepare_v2(db, sqlSoins, -1, &stmtSoins, nullptr);
    sqlite3_bind_int(stmtSoins, 1, patientId);

    while (sqlite3_step(stmtSoins) == SQLITE_ROW) {

        std::string desc = (const char*)sqlite3_column_text(stmtSoins, 0);
        std::string date = (const char*)sqlite3_column_text(stmtSoins, 1);

        int infID;
        if (sqlite3_column_type(stmtSoins, 2) == SQLITE_NULL)
            infID = -1;
        else
            infID = sqlite3_column_int(stmtSoins, 2);

        out << csv(desc) << ";" << csv(date) << ";";
        if (infID == -1) out << "";
        else out << infID;
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

    // ----------------------------------------------------
    // 1. Sélection du fichier CSV
    // ----------------------------------------------------
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


    // ----------------------------------------------------
    // 2. Parseur CSV simple avec gestion des guillemets
    // ----------------------------------------------------
    auto parseCSV = [&](const std::string& line) {
        std::vector<std::string> fields;
        std::string cur;
        bool inQuotes = false;

        for (char c : line) {
            if (c == '"') {
                inQuotes = !inQuotes;
            }
            else if (c == ';' && !inQuotes) {
                fields.push_back(cur);
                cur.clear();
            }
            else {
                cur.push_back(c);
            }
        }
        fields.push_back(cur);

        // enlever les guillemets autour
        for (auto &f : fields)
            if (f.size() >= 2 && f.front() == '"' && f.back() == '"')
                f = f.substr(1, f.size() - 2);

        return fields;
    };


    // ----------------------------------------------------
    // 3. Lire l’en-tête du fichier
    // ----------------------------------------------------
    std::string line;

    if (!std::getline(file, line)) {
        std::cerr << "Fichier vide.\n";
        return;
    }

    auto header = parseCSV(line);

    if (header.size() < 2) {
        std::cerr << "En-tête CSV invalide.\n";
        return;
    }

    std::getline(file, line);
    auto patientRow = parseCSV(line);

    //int csvPatientId = std::stoi(patientRow[0]);
    std::string csvLastname = patientRow[1];
    std::string csvFirstname = patientRow[2];


    // ----------------------------------------------------
    // 4. Vérifier la correspondance patient
    // ----------------------------------------------------
    sqlite3_stmt* stmt = nullptr;

    const char* sql =
        "SELECT firstname, last_name "
        "FROM USERS WHERE id = ?";

    sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
    sqlite3_bind_int(stmt, 1, mp->getUserId(db, patientId));

    if (sqlite3_step(stmt) != SQLITE_ROW) {
        std::cerr << "Patient inexistant.\n";
        sqlite3_finalize(stmt);
        return;
    }

    std::string dbFirstname = (const char*)sqlite3_column_text(stmt, 0);
    std::string dbLastname  = (const char*)sqlite3_column_text(stmt, 1);
    sqlite3_finalize(stmt);

    cout << "\n\nS'agit -il bien du patient "
         << dbFirstname << " " << dbLastname << " ? (o/n) : ";
    char confirm; std::cin >> confirm;
    if (confirm != 'o' && confirm != 'O') {
        std::cout << "Importation annulée par l'utilisateur.\n";
        return;
    }

    if (dbFirstname != csvFirstname || dbLastname != csvLastname) {
        std::cout << "[!] Le fichier ne correspond pas au patient réel.\n"
                  << "Fichier: " << csvFirstname << " " << csvLastname << "\n"
                  << "Base:   " << dbFirstname  << " " << dbLastname  << "\n"
                  << "Continuer l'import ? (o/n) : ";
        char c; std::cin >> c;
        if (c != 'o' && c != 'O') return;
    }


    // ----------------------------------------------------
    // 5. Créer Dossier Médical
    // ----------------------------------------------------
    int dossierId = mp->getDossierId(db,patientId);

    // Pour associer examens → consultations
    std::vector<int> createdConsultIds;


    // ----------------------------------------------------
    // 6. Parcours du reste du fichier section par section
    // ----------------------------------------------------
    enum Section { NONE, ANTE, CONS, EXAM, SOIN };
    Section current = NONE;

    bool firstLine = true;

    while (std::getline(file, line)) {

        if (line.empty()) continue;

        if (line == "# ANTECEDENTS") { current = ANTE; std::getline(file, line); if(firstLine){std::getline(file, line); firstLine = false;} continue; }
        if (line == "FIN_ANTECEDENTS") { current = NONE; firstLine = true;continue; }

        if (line == "# CONSULTATIONS") { current = CONS; std::getline(file, line);if(firstLine){std::getline(file, line); firstLine = false;} continue; }
        if (line == "FIN_CONSULTATIONS") { current = NONE; firstLine = true; continue; }

        if (line == "# EXAMENS") { current = EXAM; std::getline(file, line); if(firstLine){std::getline(file, line); firstLine = false;} continue; }
        if (line == "FIN_EXAMENS") { current = NONE; firstLine = true; continue; }
        if (line == "# SOINS") { current = SOIN; std::getline(file, line); if(firstLine){std::getline(file, line); firstLine = false;} continue; }
        if (line == "FIN_SOINS") { current = NONE; firstLine = true; continue; }


        // -----------------------
        // SECTION ANTECEDENTS
        // -----------------------
        if (current == ANTE) {
            auto f = parseCSV(line);
            if (f.size() < 2) continue;

            const char* sqlA =
                "INSERT INTO ANTECEDANTS (dossier_id, type, observations) "
                "VALUES (?,?,?);";

            sqlite3_stmt* st;
            sqlite3_prepare_v2(db, sqlA, -1, &st, nullptr);
            sqlite3_bind_int(st, 1, dossierId);
            sqlite3_bind_text(st, 2, f[0].c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(st, 3, f[1].c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_step(st);
            sqlite3_finalize(st);
        }


        // -----------------------
        // SECTION CONSULTATIONS
        // -----------------------
        if (current == CONS) {

            auto f = parseCSV(line);
            if (f.size() < 7) continue;

            int f_dossier_id      = std::stoi(f[0]);
            int f_prof_id         = std::stoi(f[1]);
            int f_patient_id      = std::stoi(f[2]);
            std::string f_date    = f[3];
            std::string f_motif   = f[4];
            std::string f_obs     = f[5];
            std::string f_presc   = f[6];

            const char* sqlC =
                "INSERT INTO CONSULTATIONS "
                "(dossier_id, professionnel_id, patient_id, date_et_heure, motif, observations, prescription) "
                "VALUES (?,?,?,?,?,?,?);";

            sqlite3_stmt* st;
            sqlite3_prepare_v2(db, sqlC, -1, &st, nullptr);

            sqlite3_bind_int(st, 1, dossierId);               // remplace le dossier_id du CSV
            sqlite3_bind_int(st, 2, f_prof_id);
            sqlite3_bind_int(st, 3, patientId);
            sqlite3_bind_text(st, 4, f_date.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(st, 5, f_motif.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(st, 6, f_obs.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(st, 7, f_presc.c_str(), -1, SQLITE_TRANSIENT);

            sqlite3_step(st);
            sqlite3_finalize(st);

            // pour EXAMENS plus tard
            int consId = sqlite3_last_insert_rowid(db);
            createdConsultIds.push_back(consId);
        }


        // -----------------------
        // SECTION EXAMENS
        // -----------------------
        if (current == EXAM) {

            auto f = parseCSV(line);
            if (f.size() < 5) continue;

            sqlite3_stmt* st = nullptr;

            const char* sqlE =
                "INSERT INTO EXAMENS "
                "(consultation_id, dossier_id, type, results, date) "
                "VALUES (?,?,?,?,?);";

            sqlite3_prepare_v2(db, sqlE, -1, &st, nullptr);

            sqlite3_bind_int(st, 1, std::stoi(f[0]));   // consultation_id
            sqlite3_bind_int(st, 2, std::stoi(f[1]));   // dossier_id
            sqlite3_bind_text(st, 3, f[2].c_str(), -1, SQLITE_TRANSIENT); // type
            sqlite3_bind_text(st, 4, f[3].c_str(), -1, SQLITE_TRANSIENT); // results
            sqlite3_bind_text(st, 5, f[4].c_str(), -1, SQLITE_TRANSIENT); // date

            sqlite3_step(st);
            sqlite3_finalize(st);
        }


        // -----------------------
        // SECTION SOINS
        // -----------------------
        if (current == SOIN) {

            auto f = parseCSV(line);
            if (f.size() < 4) continue;

            int f_dossier_csv   = std::stoi(f[0]);
            std::string f_type  = f[1];
            std::string f_date  = f[2];
            std::string f_infid = f[3];

            const char* sqlS =
                "INSERT INTO SOINS (dossier_id, type, description, date, infirmier_id) "
                "VALUES (?,?,?,?,?);";

            sqlite3_stmt* st;
            sqlite3_prepare_v2(db, sqlS, -1, &st, nullptr);

            sqlite3_bind_int(st, 1, dossierId);           // ignore dossier CSV
            sqlite3_bind_text(st, 2, f_type.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(st, 3, f_type.c_str(), -1, SQLITE_TRANSIENT); // description = type
            sqlite3_bind_text(st, 4, f_date.c_str(), -1, SQLITE_TRANSIENT);

            if (f_infid.empty())
                sqlite3_bind_null(st, 5);
            else
                sqlite3_bind_int(st, 5, std::stoi(f_infid));

            sqlite3_step(st);
            sqlite3_finalize(st);
        }

    }

    file.close();
    fs::remove(filename);

    std::cout << "[+] Import terminé avec succès.\n";
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

    const char* sql =
        "INSERT INTO PRIS_EN_CHARGE (professionnel_id, patient_id) "
        "VALUES (?, ?);";

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
        std::cerr << "Erreur insert: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt);
        return;
    }

    sqlite3_finalize(stmt);

    patients_id.emplace_back(mp, db, patient_id);
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
    int day;
    std::string heure_start, heure_end;

    std::cout << "\n--- Enregistrement de disponibilités ---\n";
    std::cout << "Entrez le jour de la semaine (0=Dimanche, 1=Lundi, ..., 6=Samedi) : ";
    std::cin >> day;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    std::cout << "Heure de début (HH:MM) : ";
    std::getline(std::cin, heure_start);

    std::cout << "Heure de fin (HH:MM) : ";
    std::getline(std::cin, heure_end);

    if (ajouter_disponibilite(db, this->getId(), day, heure_start, heure_end)) {
        std::cout << "Disponibilité enregistrée avec succès.\n";
    } else {
        std::cout << "Erreur lors de l'enregistrement de la disponibilité.\n";
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
        cout << "1. Voir la liste de mes patients\n";
        cout << "2. Enregistrer mes disponibilites\n";
        cout << "3. Voir mes disponibilites\n";
        cout << "4. Créer une consultation\n";
        cout << "5. Voir mes informations professionnelles\n";
        cout << "6. Prendre en charge un patient\n";
        cout << "7. Libérer un patient\n";
        cout << "8. Lire le dossier medical d'un patient\n";
        cout << "9. Export CSV des dossiers médicaux\n";
        cout << "10. Import CSV des dossiers médicaux\n";
        cout << "0. Se deconnecter / Quitter\n";
        cout << "------------------------------\n";
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
                afficher_patients();
                break;

            case 2:
                enregistrerDisponibilites();
                break;

            case 3:
                afficher_disponibilites();
                break;

            case 4:
                ajouter_consultation_interactive();
                break;

            case 5:
                afficher_infos_professionnelles();
                break;

            case 6:
                prendreEnChargePatient();
                break;

            case 7:
                libererPatient();
                break;

            case 8:
                lire_dossier_medical_interactive();
                break;

            case 9:

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

            case 10:
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


