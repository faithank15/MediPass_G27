#include "User.hpp"

// constructeur vide utilisé par load_user()
User::User()
    : id(0), username(""), password(""), role(""),
      active(false), telephone(0), created_by(""), created_at("") {}

User::User(int id,
           const std::string& username,
           const std::string& password,
           const std::string& role,
           bool is_active,
           int telephone,
           const std::string& created_by,
           const std::string& created_at)
    : id(id),
      username(username),
      password(password),
      role(role),
      active(is_active),
      telephone(telephone),
      created_by(created_by),
      created_at(created_at) {}

// Getters
int User::getId() const { return id; }
std::string User::getUsername() const { return username; }
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
