#include "MediPass.h"
#include <iostream>
#include <sqlite3.h>
#include "utilisateur.h"
#include "medecin.h"
#include "infirmier.h"
#include "Administrateur.h"




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

bool MediPass::must_change_password(sqlite3* db, int user_id, std::string& current_password) {
    sqlite3_stmt* stmt;
    const char* sql = "SELECT password FROM users WHERE id=?";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
        return false;

    sqlite3_bind_int(stmt, 1, user_id);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        current_password = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
    }

    sqlite3_finalize(stmt);

    // Conditions de première connexion
    return (current_password == "" || current_password == "admin" || current_password == "temp123");
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
    /*
    ** This function runs the main application loop.
    */

    MediPass::print_banner();
    sqlite3* db;
    int exit = sqlite3_open(db_filename.c_str(), &db); //c_str() converts string to const char* type
    if (exit)
    {
        cerr << "Error open DB " << sqlite3_errmsg(db) << endl;
        return 0;
    }
    else
    {
        cout << "[+]: Opened Database Successfully!" << endl;
    }

    MediPass::load_db(db);

    int choice;

    int login_status = 1;

    do{

        do{
            cout << "=== Main Menu ===" << endl;
            cout << endl;
            cout << "1-> Login" << endl;
            cout << "2-> Exit" << endl;
            cout << endl;
            cout << "#> ";
            cin >> choice;
        }while(choice != 1 && choice != 2);

        switch(choice){
            case 1:
                {
                    login_status = MediPass::login(db);
                    if(login_status == 1){
                        cout << "[+]: Login successful. Welcome " << MediPass::get_current_user() << "!" << endl;
                        //  Vérification première connexion

                        // User menu
                        current_user->menu();
                    }else{
                        cout << "[!]: Login failed after 3 attempts." << endl;
                    }
                }
                break;
            case 2:
                cout << "[+]: Exiting the application. Goodbye!" << endl;
                MediPass::logout();
                break;
        }

    }while(login_status == 0 || choice != 2);

    sqlite3_close(db);
    MediPass::~MediPass();
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
         "password TEXT NOT NULL,"
         "role TEXT NOT NULL,"
         "is_active INTEGER NOT NULL,"
         "telephone INTEGER,"
         "created_by TEXT,"
         "created_at DATETIME DEFAULT CURRENT_TIMESTAMP,"
         "autorisation TEXT,"
         "statut TEXT,"
         "specialite TEXT"
         ");"
        },
        {"PATIENTS",
         "CREATE TABLE IF NOT EXISTS PATIENTS ("
         "id INTEGER PRIMARY KEY AUTOINCREMENT,"
         "first_name TEXT NOT NULL,"
         "last_name TEXT NOT NULL,"
         "date_of_birth DATE"
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
            std::cout << "[OK] Table '" << table.name << "' created or already exists." << std::endl;
        }
    }

    return "[+]: Database created successfully.";
}

void MediPass::load_db(sqlite3* db)
{
    //int (*callback)(void*,int,char**,char**)= MediPass::callback;
    int num_rows = 0;
    sqlite3_exec(db, "SELECT COUNT(*) FROM users;",callback, &num_rows, NULL);
    if (num_rows == 0)
    {

        cout << "[!]: No database found" << endl;
        cout << "[+]: Creating a new database with default admin user..." << endl;
        MediPass::create_db(db);
        MediPass::create_user(db, "admin", "admin", "admin", "admin" ,true, 00000000,"");
        MediPass::load_db(db);

    }else{
        cout << "[+]: Database loaded successfully with " << num_rows << " users." << endl;
    }
}

string MediPass::get_current_user() const
{
    if (current_user != nullptr)
    {
        return current_user->getFirstname();
    }
    else
    {
        return "No user logged in.";
    }
}

int MediPass::login(sqlite3* db)
{

    int num_matches = 0;
    string fname;
    string lname;
    string password;
    int count = 0;

    do{
        cout<<"====== LOGIN ======"<<endl;
        cout<<endl;
        cout<<"[ ]: Enter your firstname : ";
        cin>>fname;
        cout<<"[ ]: Enter your lastname :";
        cin>>lname;
        cout<<"[ ]: Enter your password : ";
        cin>>password;
        char* sql = sqlite3_mprintf("SELECT COUNT(*) FROM users WHERE firstname='%q' AND last_name='%q' AND password='%q' AND is_active=1;",fname.c_str(), lname.c_str(), password.c_str());
        sqlite3_exec(db, sql, MediPass::callback, &num_matches, NULL);

        if(num_matches==0){

            cout<<"[!]: Username ou password incorrect reessayer"<<endl;
            count++;

        }
        if(count==3){
            return 0;
        }
    }while(num_matches==0);



    vector<string> creds = MediPass::getUserCreds(db, fname, lname, password);
    if(creds[5] != "1"){
        cout << "[!]: User account is inactive. Contact administrator." << endl;
        return 0;
    }
    MediPass::load_user(db,creds);

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

    string user_role = creds[4];

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

    string userStatut = creds[10];
    if (userStatut == "medecin") {
        current_user = new Medecin(this,db,creds[1],creds[2],creds[3],creds[4],creds[5]=="1",stoi(creds[6]),creds[7],creds[8],creds[9],creds[10],creds[11]);
    } else if (userStatut == "infirmier") {
        current_user = new Infirmier(this,db,creds[1],creds[2],creds[3],creds[4],creds[5]=="1",stoi(creds[6]),creds[7],creds[8],creds[9]);
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

    current_user = new Administrateur(this,db,creds[1],creds[2],creds[3]);

    cout << "[+]: Admin user loaded successfully." << endl;


    return 0;
}

void MediPass::create_user(sqlite3* db, const string& firstname,const string& last_name, const string& password, const string& role, const bool& is_active, const int& telephone, const string& created_by)
{

    /*
    ** This function creates a new user in the database with the provided details.
    */



    char* sql = sqlite3_mprintf("INSERT INTO users (firstname,last_name, password, role, is_active, telephone, created_by) VALUES ('%q','%q', '%q', '%q', %d, %d, '%q');",
                                firstname.c_str(),last_name.c_str(), password.c_str(), role.c_str(), is_active ? 1 : 0, telephone, created_by.c_str());

    int rc = sqlite3_exec(db, sql, NULL, NULL, NULL);
    if (rc != SQLITE_OK) {
        std::cerr << "[ERROR] Creating user: " << sqlite3_errmsg(db) << std::endl;
        return;
    }
    cout << "[+]: User " << firstname << " " << last_name << " created successfully." << endl;

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
