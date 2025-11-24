#include "utilisateur.h"
#include "globals.h"
#include <sqlite3.h>
#include "MediPass.h"

// constructeur vide utilisé par load_user()
User::User()
    : id(0), firstname(""),last_name(""), password(""), role(""),
      active(false), telephone(0), created_by(""), created_at("") {}

User::User(MediPass* mp, sqlite3* db, const std::string& firstname,
         const std::string& last_name,
         const std::string& password,
         const std::string& role,
         bool is_active,
         int telephone,
         const std::string& created_by,
         const std::string& created_at,
         int id)
    : id(id),
      firstname(firstname),
      last_name(last_name),
      password(password),
      role(role),
      active(is_active),
      telephone(telephone),
      created_by(created_by),
      created_at(created_at),
      mediPass(mp),
      db(db) {userCount++;}

// Getters
int User::getId() const { return id; }
std::string User::getFirstname() const { return firstname; }
std::string User::getLast_name() const { return last_name; }
std::string User::getRole() const { return role; }
bool User::isActive() const { return active; }
int User::getTelephone() const { return telephone; }
std::string User::getCreatedBy() const { return created_by; }
std::string User::getCreatedAt() const { return created_at; }

bool User::verifyPassword(const std::string& pwd) const {
    return password == pwd;
}

void User::activate() { active = true; }
void User::deactivate() { active = false; }
