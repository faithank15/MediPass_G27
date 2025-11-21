#include "MediPass.h"
#include <iostream>
#include <sqlite3.h>

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

}

int MediPass::print_banner() const
{
    /*
    ** This function prints the MediPass banner to the console.
    */
    
    cout << "******************************************" << endl;
    cout << "**__**__**********_*_*_____***************" << endl;
    cout << "*|  \\/  |********| (_)  __ \\**************" << endl;
    cout << "*| \\ / | ___  __| |_| |__) |_ _ ___ ___**" << endl;
    cout << "*| |\\/| |/ _ \\/ _` | |  ___/ _` / __/ __|*" << endl;
    cout << "*| |  | | __/ (_| | | | | (_| \\__ \\__ \\*" << endl;
    cout << "*|_|  |_|\\___|\\__,_|_|_|   \\__,_|___/___/*" << endl;
    cout << "******************************************" << endl;
    cout << "******************************************" << endl;
    return 0;
}

int MediPass::run(string db_filename)
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
                        // Load user details
                        User user;
                        MediPass::load_user(db, &user);
                        current_user = new User(user);
                        // User menu
                        User.menu();
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

string MediPass::create_db(sqlite3* db)
{

    sqlite3_exec(db, "PRAGMA foreign_keys = ON;", NULL, NULL, NULL);

    sqlite3_exec(db,
        "CREATE TABLE IF NOT EXISTS users ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "username TEXT NOT NULL,"
        "password TEXT NOT NULL,"
        "role TEXT NOT NULL,"
        "is_active INTEGER NOT NULL,"
        "telephone INTEGER,"
        "created_by TEXT,"
        "created_at DATETIME DEFAULT CURRENT_TIMESTAMP"
        ");",
        NULL, NULL, NULL
    );

    sqlite3_exec(db,
        "CREATE TABLE IF NOT EXISTS patients ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "first_name TEXT NOT NULL,"
        "last_name TEXT NOT NULL,"
        "date_of_birth DATE,",
        NULL, NULL, NULL);

    sqlite3_exec(db,
        "CREATE TABLE IF NOT EXISTS DOSSIERS_MEDICAUX ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "patient_id INTEGER NOT NULL,"
        "created_at DATETIME DEFAULT CURRENT_TIMESTAMP,"
        "FOREIGN KEY(patient_id) REFERENCES patients(id) ON DELETE SET NULL ON UPDATE CASCADE"
        ");",
        NULL, NULL, NULL);

    sqlite3_exec(db,
        "CREATE TABLE IF NOT EXISTS SOINS ( "
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "dossier_id INTEGER NOT NULL, "
        "description TEXT NOT NULL, "
        "date DATETIME DEFAULT CURRENT_TIMESTAMP, "
        "FOREIGN KEY(dossier_id) REFERENCES DOSSIERS_MEDICAUX(id) ON DELETE CASCADE ON UPDATE CASCADE "
        ");",
        NULL, NULL, NULL);

    sqlite3_exec(db,
        "CREATE TABLE IF NOT EXISTS CONSULTATIONS ( "
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "dossier_id INTEGER NOT NULL, "
        "notes TEXT, "
        "date DATETIME DEFAULT CURRENT_TIMESTAMP, "
        "FOREIGN KEY(dossier_id) REFERENCES DOSSIERS_MEDICAUX(id) ON DELETE CASCADE ON UPDATE CASCADE "
        ");",
        NULL, NULL, NULL);
    
    sqlite3_exec(db,
        "CREATE TABLE IF NOT EXISTS EXAMENS ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "consultation_id INTEGER NOT NULL,"
        "type TEXT NOT NULL, "
        "results TEXT,"
        "date DATETIME DEFAULT CURRENT_TIMESTAMP, "
        "FOREIGN KEY(consultation_id) REFERENCES CONSULTATIONS(id) ON DELETE CASCADE ON UPDATE CASCADE "
        ");",
        NULL, NULL, NULL);

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
        MediPass::create_user(db, "admin", "admin", "admin", true, 1234567890, NULL);
        MediPass::load_db(db);

    }else{
        cout << "[+]: Database loaded successfully with " << num_rows << " users." << endl;
    }
}

string MediPass::get_current_user() const
{
    if (current_user != nullptr)
    {
        return current_user->getUsername();
    }
    else
    {
        return "No user logged in.";
    }
}

int MediPass::login(sqlite3* db)
{

    int num_matches = 0;
    string username;
    string password;
    int count = 0;

    do{
        cout<<"====== LOGIN ======"<<endl;
        cout<<endl;
        cout<<"[ ]: Enter your username : ";
        cin>>username;
        cout<<"[ ]: Enter your password : ";
        cin>>password;
        char* sql = sqlite3_mprintf("SELECT COUNT(*) FROM users WHERE username='%q' AND password='%q' AND is_active=1;", username.c_str(), password.c_str());
        sqlite3_exec(db, sql, MediPass::callback, &num_matches, NULL);

        if(num_matches==0){

            cout<<"[!]: Username ou password incorrect reessayer"<<endl;
            count++;

        }
        if(count==3){
            return 0;
        }
    }while(num_matches==0);

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

void MediPass::load_user(sqlite3* db,User* user)
{
    /*
    ** This function redirects to the appropriate user loading function based on the current user's role.
    */

    enum UserType { PATIENT, SANTE, ADMIN };

    UserType user_type = current_user->getRole();

    switch (user_type){
        case PATIENT:
            MediPass::load_patient(db, current_user->getId(), dynamic_cast<Patient*>(user));
            break;
        case SANTE:
            MediPass::load_sante(db, current_user->getId(), dynamic_cast<Sante*>(user));
            break;
        case ADMIN:
            MediPass::load_admin(db, current_user->getId(), dynamic_cast<Admin*>(user));
            break;
        default:
            cout << "[!]: Unknown user role." << endl;
            break;
    }
}

int MediPass::load_patient(sqlite3* db, const int& patient_id, Patient* patient)
{
    /*
    ** This function loads patient details from the database into the provided Patient object.
    */

    // Implementation goes here

    return 0;
}

int MediPass::load_sante(sqlite3* db, const int& sante_id, Sante* sante)
{
    /*
    ** This function loads healthcare professional details from the database into the provided Sante object.
    */

    // Implementation goes here

    return 0;
}

int MediPass::load_admin(sqlite3* db, const int& admin_id, Admin* admin)
{
    /*
    ** This function loads admin details from the database into the provided Admin object.
    */

    // Implementation goes here

    return 0;
}