#ifndef USER_H
#define USER_H

#include <string>
#include <sqlite3.h>

class MediPass;

class User {
public:

    User();  // Obligatoire pour MediPass
    virtual ~User();
    virtual void menu()=0;
    User(MediPass* mp,sqlite3* db, const std::string& firstname,
         const std::string& last_name,
         const std::string& dateNaissance,
         const std::string& password,
         const std::string& role,
         bool is_active = true,
         int telephone = 00000000,
         const std::string& created_by = "",
         const std::string& created_at = "",
         const std::string& autorisation = "",
         const std::string& statut = "",
         const std::string& sécialité = "");

    // Getters
    int getId() const;
    std::string getFirstname() const;
    std::string getLast_name() const;
    std::string getRole() const;
    bool isActive() const;
    int getTelephone() const;
    std::string getCreatedBy() const;
    std::string getCreatedAt() const;

    // Pour login
    bool verifyPassword(const std::string& pwd) const;

    // Admin peut activer / d�sactiver
    void activate();
    void deactivate();

protected:
    int id;
    std::string firstname;
    std::string last_name;
    std::string dateNaissance;
    std::string password;
    std::string role;
    bool active;
    int telephone;
    std::string created_by;
    std::string created_at;
    MediPass* mp;
    sqlite3* db;
    std::string autorisation;
    std::string statut;
    std::string specialite;
private:
};



#endif //USER_H
