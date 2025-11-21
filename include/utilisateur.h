#ifndef USER_HPP
#define USER_HPP

#include <string>

class User {
public:
    User();  // Obligatoire pour MediPass

    User(int id,
         const std::string& username,
         const std::string& password,
         const std::string& role,
         bool is_active,
         int telephone,
         const std::string& created_by,
         const std::string& created_at);

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

    // Admin peut activer / désactiver
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
