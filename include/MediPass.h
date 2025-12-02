#ifndef MEDIPASS_H
#define MEDIPASS_H
#include <string>
#include <sqlite3.h>

#include "patient.h"
#include "profesionnel_de_sante.h"

class Pro_sante;

class User;

class Administrateur;   // forward declaration

class Patient;   // forward declaration

class DossierMedical;

using namespace std;

class MediPass
{
    public:
        MediPass(string DB_filename = "medipass.db");
        virtual ~MediPass();
        string getDbFilename() const;
        string create_db(sqlite3* db);
        void load_db(sqlite3* db);
        static int callback(void* data, int argc, char** argv, char** azColName);
        static int callback_names(void* data, int argc, char** argv, char** azColName);
        static int clallbackVector(void* data, int argc, char** argv, char** azColName);
        std::string get_current_user() const;
        string getTime() const;
        int print_banner() const;
        int run();
        int login(sqlite3* db);
        int logout();
        void load_user(sqlite3* db,vector<string> creds);
        int load_sante(sqlite3* db,vector<string> creds);
        int load_admin(sqlite3* db,vector<string> creds);
        Patient* load_patient(sqlite3* db,vector<string> creds);
        int load_secretaire(sqlite3* db,vector<string> creds);
        int getDossierId(sqlite3* db, int patientId);
        //DossierMedical* load_dossier();

        string getTimeDate();
        vector<string> getUserCreds(sqlite3* db, const string& firstname,const string& last_name) const;
        bool must_change_password(sqlite3* db, int user_id,string& current_password );
        void update_password(sqlite3* db, int user_id, const string& newPwd);
        void forceChangePassword(sqlite3* db, int user_id);

    protected:
        string db_filename;
        User* current_user=nullptr;

    private:
        bool create_user(sqlite33* db);
    };

#endif // MEDIPASS_H
