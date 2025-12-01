#include "Secretaire.h"
#include <iostream>
#include <ctime>
#include <vector>
#include <map>

using namespace std;

Secretaire::Secretaire()
{
    //ctor
}

Secretaire::Secretaire(MediPass* mp, sqlite3* db, const std::string& firstname,
                   const std::string& last_name,
                   const std::string& dateNaissance,
                   const std::string& password,
                   const int telephone,
                   const std::string& created_by,
                   const std::string& created_at) : User(mp, db, firstname, last_name, dateNaissance, password, "secretaire", true,telephone,created_by,created_at," ","secretaire") {}

Secretaire::~Secretaire()
{
    //dtor
}

vector<string> Secretaire::getDocSpecialites(sqlite3* db, const std::string& specialite){
    vector<string> docNames;
    string sql = "SELECT id, firstname, last_name FROM users WHERE specialite = ? AND role = 'professionnel de sante';";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        cerr << "[ERROR] Failed to prepare statement: " << sqlite3_errmsg(db) << endl;
        return docNames;
    }

    sqlite3_bind_text(stmt, 1, specialite.c_str(), -1, SQLITE_STATIC);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        string id = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        string firstName = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        string lastName = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        docNames.push_back(id + " " + firstName + " " + lastName);
    }

    sqlite3_finalize(stmt);
    return docNames;
}

void Secretaire::getDisponibilites(sqlite3* db, int doc_id, const std::string& firstname, const std::string& last_name){
    string sql = "SELECT date, time FROM DISPONIBILITES WHERE professionnel_id = ?;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        cerr << "[ERROR] Failed to prepare statement: " << sqlite3_errmsg(db) << endl;
        return;
    }

    sqlite3_bind_int(stmt, 1, doc_id);

    cout << "[ ]: Disponibilités pour " << firstname << " " << last_name << " :" << endl;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        string date = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        string time = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        cout << "\t - " << date << " à " << time << endl;
    }

    sqlite3_finalize(stmt);
}

void Secretaire::consulterDisponibilites(sqlite3* db){

    std::string specialite;
    int doc_id;
    std::string firstname;
    std::string last_name;
    cout << "[ ]: Quelle est la spécialité recherchée ? ";
    getline(cin, specialite);

    std::vector<std::string> docs = getDocSpecialites(db, specialite);
    if(docs.empty()){
        cout << "[!]: Aucun professionnel de santé trouvé pour cette spécialité." << endl;
        return;
    }
    cout << "[ ]: Professionnels de santé disponibles en " << specialite << " :" << endl;
    for (const auto& doc : docs) {
        cout << "\t - " << doc << endl;
    }

    cout << "[ ]: Entrez l'ID du professionnel de santé pour voir ses disponibilités : ";
    cin >> doc_id;
    cin.ignore();

    cout << "[ ]: Entrez le prénom du professionnel de santé : ";
    getline(cin, firstname);

    cout << "[ ]: Entrez le nom du professionnel de santé : ";
    getline(cin, last_name);

    getDisponibilites(db, doc_id, firstname, last_name);


}

int Secretaire::findDay(std::string date){
    struct tm time_in = {};
    strptime(date.c_str(), "%Y-%m-%d", &time_in);
    mktime(&time_in);

    int wday = time_in.tm_wday;  // 0 = dimanche
    return (wday == 0 ? 6 : wday - 1); // lundi = 0, mardi = 1, ...
}

bool Secretaire::timeValide(sqlite3* db, const std::string& time, int doc_id, const std::string& date){
    // Vérifie si l'heure est au format HH:MM et entre 08:00 et 18:00
    if (time.length() != 5 || time[2] != ':') {
        cout << "[!]: Format d'heure invalide. Utilisez HH:MM." << endl;
        return false;
    }

    int hour = stoi(time.substr(0, 2));
    int minute = stoi(time.substr(3, 2));

    if (hour < 8 || hour > 18 || (hour == 18 && minute > 0) || minute < 0 || minute >= 60) {
        cout << "[!]: L'heure doit être entre 08:00 et 18:00." << endl;
        return false;
    }

    string sql = "SELECT COUNT(*) FROM RENDEZVOUS WHERE time = ?;";
    sqlite3_stmt* stmt;
    int count = 0;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        cerr << "[ERROR] Failed to prepare statement: " << sqlite3_errmsg(db) << endl;
        return false;
    }

    sqlite3_bind_text(stmt, 1, time.c_str(), -1, SQLITE_STATIC);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        count = sqlite3_column_int(stmt, 0);
    }

    sqlite3_finalize(stmt);

    if (count > 0) {
        cout << "[!]: Il y a déjà un rendez-vous à cette heure." << endl;
        return false;
    }

    sql = "SELECT heure_start, heure_end FROM DISPONIBILITES WHERE professionnel_id = ? AND day = ?;";

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        cerr << "[ERROR] Failed to prepare statement: " << sqlite3_errmsg(db) << endl;
        return false;
    }
    sqlite3_bind_int(stmt, 1, doc_id);
    sqlite3_bind_int(stmt, 2, findDay(date));
    bool valide = false;

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        string start = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        string end = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));

        if (time >= start && time <= end) {
            valide = true;
            break;
        }
    }

    sqlite3_finalize(stmt);
    if (!valide) {
        cout << "[!]: L'heure ne correspond pas aux disponibilités du professionnel de santé." << endl;
        return false;
    }



    return true;
}

bool Secretaire::rdvValide(sqlite3* db, int doc_id, const std::string& date, const std::string& time){

    if(!timeValide(db, time, doc_id, date)){
        return false;
    }

    return true;
}

bool Secretaire::prendreRdv(sqlite3* db){

    int professionnel_id;
    int patient_id;
    std::string date;
    std::string time;

    cout << "[ ]: Entrez l'ID du patient : ";
    cin >> patient_id;
    cin.ignore();
    cout << "[ ]: Entrez l'ID du professionnel de santé : ";
    cin >> professionnel_id;
    cin.ignore();
    cout << "[ ]: Entrez la date du rendez-vous (YYYY-MM-DD) : ";
    getline(cin, date);
    cout << "[ ]: Entrez l'heure du rendez-vous (HH:MM) : ";
    getline(cin, time);

    if(!rdvValide(db, professionnel_id, date, time)){
        return false;
    }

    string sql = "INSERT INTO RENDEZVOUS (patient_id, professionnel_id, date, time) VALUES (?, ?, ?, ?);";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        cerr << "[ERROR] Failed to prepare statement: " << sqlite3_errmsg(db) << endl;
        return false;
    }

    sqlite3_bind_int(stmt, 1, patient_id);
    sqlite3_bind_int(stmt, 2, professionnel_id);
    sqlite3_bind_text(stmt, 3, date.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, time.c_str(), -1, SQLITE_STATIC);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        cerr << "[ERROR] Failed to execute statement: " << sqlite3_errmsg(db) << endl;
        sqlite3_finalize(stmt);
        return false;
    }
    cout << "[+]: Rendez-vous pris avec succès pour le " << date << " à " << time << "." << endl;
    sqlite3_finalize(stmt);
    return true;
}

bool Secretaire::annulerRdv(sqlite3* db){
    int rdv_id;
    cout << "[ ]: Entrez l'ID du rendez-vous à annuler : ";
    cin >> rdv_id;
    cin.ignore();

    string sql = "DELETE FROM RENDEZVOUS WHERE id = ?;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        cerr << "[ERROR] Failed to prepare statement: " << sqlite3_errmsg(db) << endl;
        return false;
    }

    sqlite3_bind_int(stmt, 1, rdv_id);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        cerr << "[ERROR] Failed to execute statement: " << sqlite3_errmsg(db) << endl;
        sqlite3_finalize(stmt);
        return false;
    }

    cout << "[+]: Rendez-vous annulé avec succès." << endl;
    sqlite3_finalize(stmt);
    return true;
}

void Secretaire::consulterRdv(sqlite3* db){
    int patient_id;
    cout << "[ ]: Entrez l'ID du patient pour consulter ses rendez-vous : ";
    cin >> patient_id;
    cin.ignore();

    string sql = "SELECT r.id, r.date, r.time, u.firstname, u.last_name "
                 "FROM RENDEZVOUS r "
                 "JOIN users u ON r.professionnel_id = u.id "
                 "WHERE r.patient_id = ? "
                 "ORDER BY r.date, r.time;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        cerr << "[ERROR] Failed to prepare statement: " << sqlite3_errmsg(db) << endl;
        return;
    }

    sqlite3_bind_int(stmt, 1, patient_id);

    cout << "[ ]: Rendez-vous pour le patient ID " << patient_id << " :" << endl;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int rdv_id = sqlite3_column_int(stmt, 0);
        string date = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        string time = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        string doc_firstname = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        string doc_lastname = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));

        cout << "\t - RDV ID: " << rdv_id << ", Date: " << date << ", Heure: " << time
             << ", Professionnel de santé: " << doc_firstname << " " << doc_lastname << endl;
    }

    sqlite3_finalize(stmt);
}

void Secretaire::menu(){
    int choice;
    do {
        cout << "\n=== Menu Secrétaire ===" << endl;
        cout << "1. Prendre un rendez-vous" << endl;
        cout << "2. Annuler un rendez-vous" << endl;
        cout << "3. Consulter les disponibilités des professionnels de santé" << endl;
        cout << "4. Consulter les rendez-vous d'un patient" << endl;
        cout << "5. Quitter" << endl;
        cout << "Choix: ";
        cin >> choice;
        cin.ignore();  // Pour consommer le caractère de nouvelle ligne restant

        switch (choice) {
            case 1:
                prendreRdv(db);
                break;
            case 2:
                annulerRdv(db);
                break;
            case 3:
                consulterDisponibilites(db);
                break;
            case 4:
                consulterRdv(db);
                break;
            case 5:
                cout << "Au revoir!" << endl;
                break;
            default:
                cout << "[!]: Choix invalide. Veuillez réessayer." << endl;
        }
    } while (choice != 5);
}
