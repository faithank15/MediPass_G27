#include "MediPass.h"
#include <iostream>
#include <sqlite3.h>
#include "utilisateur.h"
#include "medecin.h"
#include "infirmier.h"
#include "Administrateur.h"
#include "patient.h"





MediPass::MediPass(string DB_filename)
{
    db_filename = DB_filename;
}

MediPass::~MediPass()
{

}

string MediPass::getDbFilename() const
{
    /*
    ** This function returns the database filename.
    */

    return db_filename;
}
std::string MediPass::get_current_user() const
{
    if (current_user != nullptr)
    {
        return current_user->getFirstname(); // Assure-toi que User a cette méthode
    }
    else
    {
        return "No user logged in.";
    }
}

int MediPass::callback(void* data, int argc, char** argv, char** azColName)
{
    /*
    ** This is a callback function for SQLite queries. It retrieves the count of rows from the query result
    ** and stores it in the provided data.
    */

    int* count = (int*)data;
    *count = atoi(argv[0]);
    return 0;

}

int MediPass::callback_names(void* data, int argc, char** argv, char** azColName)
{
    /*
    ** This is a callback function for SQLite queries. It retrieves names from the query result
    ** and stores them in the provided data vector.
    */

    std::vector<std::string>* names = (std::vector<std::string>*)data;
    names->push_back(std::string(argv[0]));
    names->push_back(std::string(argv[1]));

    return 0;
}

int callbackVector(void* data, int argc, char** argv, char** azColName)
{
    /*
    ** This is a callback function for SQLite queries. It retrieves values from the query result
    ** and stores them in the provided data vector.
    */

    std::vector<std::string>* values = (std::vector<std::string>*)data;
    for (int i = 0; i < argc; i++) {
        if (argv[i])
            values->push_back(std::string(argv[i]));
        else
            values->push_back("");  // éviter les crashs
    }

    return 0;
}

bool MediPass::must_change_password(sqlite3* db, int user_id, string& password) {
    sqlite3_stmt* stmt;
    const char* sql = "SELECT is_default_password FROM users WHERE id=?";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
        return false;

    sqlite3_bind_int(stmt, 1, user_id);

    int is_default = 0;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        is_default = sqlite3_column_int(stmt, 0);
    }

    sqlite3_finalize(stmt);

    return is_default != 0;  // vrai si l'utilisateur doit changer son mot de passe
}



void MediPass::update_password(sqlite3* db, int user_id, const std::string& newPwd) {
    sqlite3_stmt* stmt;
    const char* sql = "UPDATE users SET password=? WHERE id=?";

    sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
    sqlite3_bind_text(stmt, 1, newPwd.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 2, user_id);

    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
}

void MediPass::forceChangePassword(sqlite3* db, int user_id) {
    std::string newPwd;
    do {
        std::cout << "[!] Votre mot de passe est encore par défaut. Veuillez le changer : ";
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::getline(std::cin, newPwd);
        if(newPwd.empty()) {
            std::cout << "[!]: Mot de passe invalide, réessayez." << std::endl;
        }
    } while(newPwd.empty());

    // Mise à jour du mot de passe
    update_password(db, user_id, newPwd);

    // Réinitialiser le flag
    char* sql = sqlite3_mprintf("UPDATE users SET is_default_password=0 WHERE id=%d;", user_id);
    sqlite3_exec(db, sql, NULL, NULL, NULL);

    std::cout << "[+]: Mot de passe changé avec succès !" << std::endl;
}

void MediPass::load_db(sqlite3* db)
{
    // Créer les tables si elles n'existent pas
    create_db(db);

    // Vérifie s'il existe déjà un admin par défaut
    int admin_count = 0;
    sqlite3_stmt* stmt;
    const char* sql = "SELECT COUNT(*) FROM users "
                  "WHERE LOWER(firstname)='admin' "
                  "AND LOWER(last_name)='admin' "
                  "AND LOWER(role)='admin';";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "[ERROR] Failed to prepare admin check: " << sqlite3_errmsg(db) << std::endl;
        return;
    }

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        admin_count = sqlite3_column_int(stmt, 0);
    }
    sqlite3_finalize(stmt);

    //std::cout << "[DEBUG] admin_count = " << admin_count << std::endl;

    if (admin_count == 0) {
        // Crée l'admin par défaut une seule fois
        if (create_user(db, "admin", "admin", "admin", "admin", true, 0, "")) {
            std::cout << "[+]: Default admin created." << std::endl;
        } else {
            std::cerr << "[ERROR]: Failed to create default admin." << std::endl;
        }
    } else {
        std::cout << "[+]: Database loaded successfully with " << admin_count << " admin(s)." << std::endl;
    }
}



int MediPass::print_banner() const
{
    /*
    ** This function prints the MediPass banner to the console.
    */

    cout << "******************************************" << endl;
    cout << "**__**__**********_*_*_____***************" << endl;
    cout << "*|  \\/  |********| (_)  __ \\**************" << endl;
    cout << "*| \\ /  |  ___  __| |_| |__) |_ _ ___ ___**" << endl;
    cout << "*| |\\/| |/ _ \\/ _` | |  ___/ _` / __/ __|*" << endl;
    cout << "*| |  | | __/ (_| | | | | (_| \\__ \\__ \\*" << endl;
    cout << "*|_|  |_|\\___|\\__,_|_|_|   \\__,_|___/___/*" << endl;
    cout << "******************************************" << endl;
    cout << "******************************************" << endl;
    return 0;
}

int MediPass::run()
{
    print_banner();

    sqlite3* db;
    if (sqlite3_open(db_filename.c_str(), &db)) {
        cerr << "Error opening DB: " << sqlite3_errmsg(db) << endl;
        return 0;
    }
    cout << "[+]: Database opened successfully!" << endl;

    // Charger la DB et créer l'admin par défaut uniquement si nécessaire
    load_db(db);

    int choice;
    do {
        cout << "\n=== Main Menu ===\n"
             << "1-> Login\n"
             << "2-> Exit\n"
             << "#> ";
        std::cin >> choice;

        switch(choice) {
            case 1:
                if (login(db) == 1) {
                    // login() gère l'accès au menu utilisateur
                } else {
                    cout << "[!]: Login failed after 3 attempts." << endl;
                }
                break;

            case 2:
                cout << "[+]: Exiting the application. Goodbye!" << endl;
                if (current_user) logout();
                break;

            default:
                cout << "[!]: Invalid choice." << endl;
                break;
        }

    } while (choice != 2);

    sqlite3_close(db);
    return 0;
}


std::string MediPass::create_db(sqlite3* db)
{
    int rc;
    char* errmsg = nullptr;

    // Activer les clés étrangères
    rc = sqlite3_exec(db, "PRAGMA foreign_keys = ON;", NULL, NULL, &errmsg);
    if (rc != SQLITE_OK) {
        std::cerr << "[ERROR] PRAGMA foreign_keys: " << errmsg << std::endl;
        sqlite3_free(errmsg);
        return "[ERROR]: Failed to set PRAGMA foreign_keys.";
    }

    // Tableau de paires {nom_table, requête}
    struct Table {
        std::string name;
        std::string sql;
    };

        Table tables[] = {
        {"USERS",
         "CREATE TABLE IF NOT EXISTS USERS ("
         "id INTEGER PRIMARY KEY AUTOINCREMENT,"
         "firstname TEXT NOT NULL,"
         "last_name TEXT NOT NULL,"
         "date_of_birth TEXT,"
         "password TEXT NOT NULL,"
         "role TEXT NOT NULL,"
         "is_active INTEGER NOT NULL,"
         "telephone INTEGER,"
         "created_by TEXT,"
         "created_at DATETIME DEFAULT CURRENT_TIMESTAMP,"
         "autorisation TEXT,"
         "statut TEXT,"
         "specialite TEXT,"
         "is_default_password INTEGER NOT NULL DEFAULT 1,"
         "UNIQUE(firstname, last_name, role)"
         ");"
        },
        {"PATIENTS",
         "CREATE TABLE IF NOT EXISTS PATIENTS ("
         "id INTEGER PRIMARY KEY AUTOINCREMENT,"
         "patient_user_id INTEGER UNIQUE,"
         "FOREIGN KEY(patient_user_id) REFERENCES USERS(id) ON DELETE CASCADE ON UPDATE CASCADE"
         ");"
        },
        {"DOSSIERS_MEDICAUX",
         "CREATE TABLE IF NOT EXISTS DOSSIERS_MEDICAUX ("
         "id INTEGER PRIMARY KEY AUTOINCREMENT,"
         "patient_id INTEGER,"
         "created_at DATETIME DEFAULT CURRENT_TIMESTAMP,"
         "FOREIGN KEY(patient_id) REFERENCES patients(id) ON DELETE SET NULL ON UPDATE CASCADE"
         ");"
        },
        {"SOINS",
         "CREATE TABLE IF NOT EXISTS SOINS ("
         "id INTEGER PRIMARY KEY AUTOINCREMENT,"
         "dossier_id INTEGER NOT NULL,"
         "description TEXT NOT NULL,"
         "date DATETIME DEFAULT CURRENT_TIMESTAMP,"
         "infirmier_id INTEGER,"
         "FOREIGN KEY(infirmier_id) REFERENCES USERS(id) ON DELETE SET NULL ON UPDATE CASCADE,"
         "FOREIGN KEY(dossier_id) REFERENCES DOSSIERS_MEDICAUX(id) ON DELETE CASCADE ON UPDATE CASCADE"
         ");"
        },
        {"CONSULTATIONS",
         "CREATE TABLE IF NOT EXISTS CONSULTATIONS ("
         "id INTEGER PRIMARY KEY AUTOINCREMENT,"
         "dossier_id INTEGER NOT NULL,"
         "notes TEXT,"
         "date DATETIME DEFAULT CURRENT_TIMESTAMP,"
         "FOREIGN KEY(dossier_id) REFERENCES DOSSIERS_MEDICAUX(id) ON DELETE CASCADE ON UPDATE CASCADE"
         ");"
        },
        {"EXAMENS",
         "CREATE TABLE IF NOT EXISTS EXAMENS ("
         "id INTEGER PRIMARY KEY AUTOINCREMENT,"
         "consultation_id INTEGER NOT NULL,"
         "type TEXT NOT NULL,"
         "results TEXT,"
         "date DATETIME DEFAULT CURRENT_TIMESTAMP,"
         "FOREIGN KEY(consultation_id) REFERENCES CONSULTATIONS(id) ON DELETE CASCADE ON UPDATE CASCADE"
         ");"
        }
    };

    // Exécuter chaque requête et vérifier
    for (const auto& table : tables) {
        rc = sqlite3_exec(db, table.sql.c_str(), NULL, NULL, &errmsg);
        if (rc != SQLITE_OK) {
            std::cerr << "[ERROR] Creating table '" << table.name << "': " << errmsg << std::endl;
            sqlite3_free(errmsg);
            return "[ERROR]: Failed to create table " + table.name;
        } else {
            //std::cout << "[OK] Table '" << table.name << "' created or already exists." << std::endl;
        }
    }

    return "[+]: Database created successfully.";
}


int MediPass::login(sqlite3* db)
{
    int num_matches = 0;
    std::string fname, lname, password;
    int count = 0;

    do {
        std::cout << "====== LOGIN ======" << std::endl;
        std::cout << "[ ]: Enter your firstname : ";
        std::cin >> fname;
        std::cout << "[ ]: Enter your lastname : ";
        std::cin >> lname;
        std::cout << "[ ]: Enter your password : ";
        std::cin >> password;

        char* sql = sqlite3_mprintf(
            "SELECT COUNT(*) FROM users "
            "WHERE firstname='%q' AND last_name='%q' AND password='%q' AND is_active=1;",
            fname.c_str(), lname.c_str(), password.c_str()
        );

        sqlite3_exec(db, sql, MediPass::callback, &num_matches, nullptr);

        if (num_matches == 0) {
            std::cout << "[!]: Username ou password incorrect. Veuillez reessayer." << std::endl;
            count++;
        }

        if (count == 3) {
            std::cout << "[!]: Nombre maximal de tentatives atteint. Login échoué." << std::endl;
            return 0;
        }

    } while (num_matches == 0);

    // Récupérer les informations complètes de l'utilisateur
    std::vector<std::string> creds = MediPass::getUserCreds(db, fname, lname, password);

    // Vérifier si l'utilisateur est actif
    if (creds[6] != "1") {
        std::cout << "[!]: User account is inactive. Contact administrator." << std::endl;
        return 0;
    }

    MediPass::load_user(db, creds);
    int user_id = std::stoi(creds[0]);
    std::string role = creds[5];

    // Vérifier si le mot de passe est par défaut pour admin ou professionnel de santé
    if (role == "admin" || role == "professionnel de sante") {
        std::string current_password;
        if (must_change_password(db, user_id, current_password)) {
            forceChangePassword(db, user_id);
        }
    }

    // Message de bienvenue
    std::cout << "[+]: Login successful. Welcome " << get_current_user() << "!" << std::endl;

    // Accès au menu utilisateur
    current_user->menu();

    return 1;
}


int MediPass::logout()
{
    /*
    ** This function logs out the current user.
    */

    if (current_user != nullptr)
    {
        delete current_user;
        current_user = nullptr;
        cout << "[+]: User logged out successfully." << endl;
        return 1;
    }
    else
    {
        cout << "[!]: No user is currently logged in." << endl;
        return 0;
    }
}

void MediPass::load_user(sqlite3* db,vector<string> creds)
{
    /*
    ** This function redirects to the appropriate user loading function based on the current user's role.
    */

    string user_role = creds[5];

    if (user_role == "patient") {
        cout << "[!]: Patient loading not implemented yet." << endl;
    } else if (user_role == "professionnel de sante") {
        MediPass::load_sante(db, creds);
    } else if (user_role == "admin") {
        MediPass::load_admin(db, creds);
    } else {
        cout << "[!]: Unknown user role." << endl;
    }
}

int MediPass::load_sante(sqlite3* db, vector<string> creds)
{
    /*
    ** This function loads healthcare professional details from the database into the provided Sante object.
    */

    string userStatut = creds[11];
    if (userStatut == "medecin") {
        current_user = new Medecin(this,db,creds[1],creds[2],creds[3],creds[4],creds[6]=="1",stoi(creds[7]),creds[8],creds[9],creds[11],creds[12]);
    } else if (userStatut == "infirmier") {
        current_user = new Infirmier(this,db,creds[1],creds[2],creds[3],creds[4],creds[5],creds[6]=="1",stoi(creds[7]),creds[8],creds[9],creds[10],creds[12]);
    } else {
        cout << "[!]: Statut de professionel de sante inconnu." << endl;
    }

    return 0;
}

int MediPass::load_admin(sqlite3* db,vector<string> creds)
{
    /*
    ** This function loads admin details from the database into the provided Admin object.
    */

    current_user = new Administrateur(this,db,creds[1],creds[2],creds[3],creds[4],stoi(creds[7]),creds[8],creds[9]);

    cout << "[+]: Admin user loaded successfully." << endl;


    return 0;
}

Patient* MediPass::load_patient(sqlite3* db, vector<string> creds) {

    /*
    **
    */

    Patient* p = new Patient(this,db,stoi(creds[0]),creds[1],creds[2],creds[3],creds[6]=="1",stoi(creds[7]),creds[8],creds[9]);

    //vecto


}

// Modifiée pour retourner vrai si l'utilisateur a été créé, faux sinon
bool MediPass::create_user(sqlite3* db, const string& firstname,
                           const string& last_name,
                           const string& password,
                           const string& role,
                           const bool& is_active,
                           const int& telephone,
                           const string& created_by)
{
    // Vérifie si l'utilisateur existe déjà
    sqlite3_stmt* stmt;
    const char* check_sql = "SELECT COUNT(*) FROM users WHERE firstname=? AND last_name=? AND role=?;";

    if (sqlite3_prepare_v2(db, check_sql, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "[ERROR] Failed to prepare check statement: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    sqlite3_bind_text(stmt, 1, firstname.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, last_name.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, role.c_str(), -1, SQLITE_STATIC);

    int exists = 0;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        exists = sqlite3_column_int(stmt, 0);
    }

    sqlite3_finalize(stmt);

    if (exists > 0) {
        std::cout << "[!]: User " << firstname << " " << last_name
                  << " with role " << role << " already exists. Creation refused." << std::endl;
        return false;
    }

    // Insère l'utilisateur
    sqlite3_stmt* insert_stmt;
    const char* insert_sql = "INSERT INTO users (firstname, last_name, password, role, is_active, telephone, created_by, is_default_password) "
                             "VALUES (?, ?, ?, ?, ?, ?, ?, 1);";

    if (sqlite3_prepare_v2(db, insert_sql, -1, &insert_stmt, nullptr) != SQLITE_OK) {
        std::cerr << "[ERROR] Failed to prepare insert statement: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    sqlite3_bind_text(insert_stmt, 1, firstname.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(insert_stmt, 2, last_name.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(insert_stmt, 3, password.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(insert_stmt, 4, role.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int(insert_stmt, 5, is_active ? 1 : 0);
    sqlite3_bind_int(insert_stmt, 6, telephone);
    sqlite3_bind_text(insert_stmt, 7, created_by.c_str(), -1, SQLITE_STATIC);

    if (sqlite3_step(insert_stmt) != SQLITE_DONE) {
        std::cerr << "[ERROR] Creating user: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(insert_stmt);
        return false;
    }

    sqlite3_finalize(insert_stmt);
    std::cout << "[+]: User " << firstname << " " << last_name << " created successfully." << std::endl;
    return true;
}






string MediPass::getTimeDate()
{
    /*
    ** This function returns the current date and time as a formatted string.
    */

    time_t now = time(0);
    char buf[80];
    strftime(buf, sizeof(buf), "%Y-%m-%d %X", localtime(&now));
    return string(buf);
}

vector<string> MediPass::getUserCreds(sqlite3* db, const string& firstname,const string& last_name,const string& password) const
{
    /*
    ** This function retrieves user credentials from the database based on the provided firstname, lastname, and password.
    */

    vector<string> creds;
    char* sql = sqlite3_mprintf("SELECT * FROM users WHERE firstname='%q' AND last_name='%q' AND password='%q' AND is_active=1;",
                                firstname.c_str(), last_name.c_str(), password.c_str());

    sqlite3_exec(db, sql, callbackVector, &creds, NULL);

    //cout  << "[+]: Number of credentials retrieved: " << creds.size() << endl;

    return creds;
}
