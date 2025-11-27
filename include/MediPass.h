#ifndef MEDIPASS_H
#define MEDIPASS_H
#include <string>
#include <sqlite3.h>

#include "patient.h"
#include "profesionnel_de_sante.h"

class Pro_sante;

class User;

class Administrateur;   // forward declaration

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
        string get_current_user() const;
        string getTime() const;
        int print_banner() const;
        int run();
        int login(sqlite3* db);
        int logout();
        void load_user(sqlite3* db,vector<string> creds);
        int load_sante(sqlite3* db,vector<string> creds);
        int load_admin(sqlite3* db,vector<string> creds);
        string getTimeDate();
        vector<string> getUserCreds(sqlite3* db, const string& firstname,const string& last_name,const string& password) const;
        bool must_change_password(sqlite3* db, int user_id, std::string& current_password);
        void update_password(sqlite3* db, int user_id, const std::string& newPwd);

    protected:
        string db_filename;
        User* current_user=nullptr;

    private:
        void create_user(sqlite3* db, const string& firstname,const string& last_name, const string& password, const string& role, const bool& is_active, const int& telephone, const string& created_by);

};

#endif // MEDIPASS_H
