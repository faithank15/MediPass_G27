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

    while (choix == 'o' || choix == 'O') {

        std::string date, typeExamen, resultat;

        std::cout << "\n--- Nouvel examen ---\n";

        std::cout << "Date (jj/mm/aaaa) : ";
        std::getline(std::cin, date);

        std::cout << "Type d'examen : ";
        std::getline(std::cin, typeExamen);

        std::cout << "Résultat : ";
        std::getline(std::cin, resultat);

        examens.emplace_back(date, typeExamen, resultat);

        std::cout << "Ajouter un autre examen ? (o/n) : ";
        std::cin >> choix;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
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

    std::cout << "\nConsultation ajoutée avec succès pour le patient "
              << patient->getNomComplet() << ".\n";
}


void Medecin::exportDossiersCSV() {

    std::string path = "./exported";  // Dossier d'exportation
    // Créer le dossier s'il n'existe pas
    if (!fs::exists(path)) {
        fs::create_directory(path);
    }

    std::cout << "Veuillez renseigner l'ID du patient dont vous souhaitez exporter le dossier medical : ";
    int patientId;
    std::cin >> patientId;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    std::vector<string> patientNames;
    string sql = "SELECT firstname, last_name FROM USERS WHERE id = " + std::to_string(patientId) + " AND is_active = 1;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Erreur SQL : " << sqlite3_errmsg(db) << std::endl;
        return;
    }

    patientNames.push_back(""); // firstname
    patientNames.push_back(""); // last_name
    sqlite3_step(stmt);
    patientNames[0] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
    patientNames[1] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
    sqlite3_finalize(stmt);
    sqlite3_reset(stmt);

    std::vector<string> creds = mp->getUserCreds(db, patientNames[0], patientNames[1]);
    if (creds.empty()) {
        std::cerr << "Patient introuvable ou inactif.\n";
        return;
    }

    std::string filename = path + "/dossiers_export_" + std::to_string(patientId) + ".csv";
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "[!] Impossible d'ouvrir le fichier pour l'écriture.\n";
        return;
    }

    file << "DossierID,CreatedAt,Antecedants,Consultations,Examens,Soins\n";

    sql = "SELECT id, patient_id, created_at FROM DOSSIERS_MEDICAUX"
          " WHERE patient_id = " + std::to_string(patientId) + ";";


    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Erreur SQL : " << sqlite3_errmsg(db) << std::endl;
        return;
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int dossierId = sqlite3_column_int(stmt, 0);
        std::string created_at = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));

        auto concatChamp = [&](const char* table, const char* champ) {
            sqlite3_stmt* s;
            std::string result;
            std::string q = "SELECT " + std::string(champ) + " FROM " + std::string(table) +
                            " WHERE dossier_id=" + std::to_string(dossierId) + ";";
            if (sqlite3_prepare_v2(db, q.c_str(), -1, &s, nullptr) == SQLITE_OK) {
                while (sqlite3_step(s) == SQLITE_ROW) {
                    const unsigned char* txt = sqlite3_column_text(s, 0);
                    if (txt) result += std::string(reinterpret_cast<const char*>(txt)) + "|";
                }
            }
            sqlite3_finalize(s);
            if (!result.empty()) result.pop_back();
            return result;
        };

        std::string antecedants   = concatChamp("ANTECEDANTS", "type");
        std::string consultations = concatChamp("CONSULTATIONS", "notes");
        std::string examens       = concatChamp("EXAMENS", "type");
        std::string soins         = concatChamp("SOINS", "description");

        file << dossierId << "," << created_at << ","
             << "\"" << antecedants << "\","
             << "\"" << consultations << "\","
             << "\"" << examens << "\","
             << "\"" << soins << "\"\n";
    }

    sqlite3_finalize(stmt);
    file.close();
    std::cout << "[+] Export terminé : " << filename << std::endl;
}



void Medecin::importDossiersCSV() {

    std::string path = "./import";  // Chemin du dossier à lire

    // Vérifier si le dossier existe
    if (!fs::exists(path) || !fs::is_directory(path)) {
        std::cout << "Dossier introuvable !" << std::endl;
        return;
    }
    // Lister les fichiers CSV dans le dossier
    std::vector<fs::path> csvFiles;
    for (const auto& entry : fs::directory_iterator(path)) {
        if (entry.path().extension() == ".csv") {
            csvFiles.push_back(entry.path());
        }
    }

    if (csvFiles.empty()) {
        std::cout << "Aucun fichier CSV trouvé dans le dossier." << std::endl;
        return;
    }

    std::cout << "Fichiers CSV disponibles pour l'importation :\n";
    for (size_t i = 0; i < csvFiles.size(); ++i) {
        std::cout << i + 1 << ". " << csvFiles[i].filename().string() << "\n";
    }

    int choice;
    std::cout << "Choisissez un fichier à importer (entrez le numéro) : ";
    std::cin >> choice;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');


    if (choice < 1 || choice > static_cast<int>(csvFiles.size())) {
        std::cerr << "Choix invalide. Import annulé." << std::endl;
        return;
    }

    std::string filename = csvFiles[choice - 1].string();

    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "[!] Impossible d'ouvrir le fichier pour lecture.\n";
        return;
    }

    std::string line;
    std::getline(file, line); // ignorer l'entête

    // Demander le patient
    int patientId;
    std::cout << "Entrez le matricule (ID du patient) pour lequel importer les dossiers : ";
    std::cin >> patientId;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    std::vector<string> patientNames;
    string sql = "SELECT firstname, last_name FROM USERS WHERE id = " + std::to_string(patientId) + " AND is_active = 1;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Erreur SQL : " << sqlite3_errmsg(db) << std::endl;
        return;
    }

    patientNames.push_back(""); // firstname
    patientNames.push_back(""); // last_name
    sqlite3_step(stmt);
    patientNames[0] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
    patientNames[1] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
    sqlite3_finalize(stmt);


    // Vérifier qu'il s'agit du bon patient
    std::vector<string> creds = mp->getUserCreds(db, patientNames[0], patientNames[1]);
    if (creds.empty()) {
        std::cerr << "Patient introuvable ou inactif.\n";
        return;
    }

    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string dummyPatientIdStr, dossierIdStr, created_at, antecedants, consultations, examens, soins;

        std::getline(ss, dummyPatientIdStr, ','); // Ignorer l'ID du patient du CSV
        std::getline(ss, dossierIdStr, ',');
        std::getline(ss, created_at, ',');
        std::getline(ss, antecedants, ',');
        std::getline(ss, consultations, ',');
        std::getline(ss, examens, ',');
        std::getline(ss, soins, ',');



        // Créer un nouveau dossier (toujours auto-increment pour éviter les conflits)
        std::string sql = sqlite3_mprintf("INSERT INTO DOSSIERS_MEDICAUX (patient_id, created_at) VALUES (%d, '%q');",
                          patientId, created_at.c_str());
        char* errMsg = nullptr;
        if (sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errMsg) != SQLITE_OK) {
            std::cerr << "Erreur SQL : " << errMsg << std::endl;
            sqlite3_free(errMsg);
            continue;
        }

        int dossierId = (int)sqlite3_last_insert_rowid(db);

        auto insererLignes = [&](const std::string& table, const std::string& champ, const std::string& valeurs) {
            std::stringstream ss_val(valeurs);
            std::string item;
            while (std::getline(ss_val, item, '|')) {
                if (!item.empty()) {
                    std::string sql2 = "INSERT INTO " + table + " (dossier_id, " + champ + ") VALUES (" +
                        std::to_string(dossierId) + ", '" + item + "');";
                    sqlite3_exec(db, sql2.c_str(), nullptr, nullptr, nullptr);
                }
            }
        };

        insererLignes("ANTECEDANTS", "type", antecedants);
        insererLignes("CONSULTATIONS", "notes", consultations);
        insererLignes("EXAMENS", "type", examens);
        insererLignes("SOINS", "description", soins);
    }

    fs::remove(filename); // Supprimer le fichier après importation

    file.close();
    std::cout << "[+] Import terminé pour le patient ID " << patientId << std::endl;
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
        cout << "6. Export CSV des dossiers médicaux\n";
        cout << "7. Import CSV des dossiers médicaux\n";
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
            case 6:
                exportDossiersCSV();
                break;

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


