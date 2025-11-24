#ifndef USER_HPP
#define USER_HPP

#include <string>
#include "globals.h"


class User {
public:
    // Déclaration de la variable statique - Nous en aurons besoin pour continuer - je pense
    static int userCount;
    User();  // Obligatoire pour MediPass
    virtual ~User();
    User(const std::string& username,
         const std::string& password,
         const std::string& role,
         bool is_active = true,
         int telephone = 00000000,
         const std::string& created_by = NULL,
         const std::string& created_at = NULL,
         int id = userCount);

    // Getters
    int getId() const;
    std::string getUsername() const;
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

private:
    int id;
    std::string username;
    std::string password;
    std::string role;
    bool active;
    int telephone;
    std::string created_by;
    std::string created_at;
};



#endif
