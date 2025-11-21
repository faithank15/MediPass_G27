#include "Administrateur.hpp"
#include <iostream>

Administrateur::Administrateur(const std::string& id,
                               const std::string& firstname,
                               const std::string& lastname,
                               const std::string& username,
                               const std::string& password)
    : Utilisateur(id, firstname, lastname, username, password) {}

// ------------------------------------------------------
// Créer un utilisateur (admin / sante / patient)
// ------------------------------------------------------
void Administrateur::creerUtilisateur(MediPass& mp, sqlite3* db) {
    std::string nom, prenom, username, password, role;
    std::cout << "Prénom: "; std::cin >> prenom;
    std::cout << "Nom: "; std::cin >> nom;
    std::cout << "Username: "; std::cin >> username;
    std::cout << "Password: "; std::cin >> password;
    std::cout << "Rôle (admin / sante / patient): "; std::cin >> role;

    mp.create_user(db, username, password, role, true, 0, this->getNomComplet());
    std::cout << "Utilisateur créé: " << prenom << " " << nom << " | Rôle: " << role << std::endl;
}

// ------------------------------------------------------
// Modifier le rôle d’un utilisateur
// ------------------------------------------------------
void Administrateur::modifierRole(MediPass& mp, sqlite3* db, int userId, const std::string& nouveauRole) {
    std::string sql = "UPDATE users SET role='" + nouveauRole + "' WHERE id=" + std::to_string(userId) + ";";
    char* errMsg = nullptr;
    if (sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errMsg) != SQLITE_OK) {
        std::cerr << "Erreur SQL: " << errMsg << std::endl;
        sqlite3_free(errMsg);
    } else {
        std::cout << "Rôle de l'utilisateur " << userId << " modifié en " << nouveauRole << std::endl;
    }
}

// ------------------------------------------------------
// Désactiver / activer compte
// ------------------------------------------------------
void Administrateur::desactiverCompte(MediPass& mp, sqlite3* db, int userId) {
    std::string sql = "UPDATE users SET is_active=0 WHERE id=" + std::to_string(userId) + ";";
    char* errMsg = nullptr;
    sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errMsg);
    std::cout << "Compte " << userId << " désactivé." << std::endl;
}

void Administrateur::activerCompte(MediPass& mp, sqlite3* db, int userId) {
    std::string sql = "UPDATE users SET is_active=1 WHERE id=" + std::to_string(userId) + ";";
    char* errMsg = nullptr;
    sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errMsg);
    std::cout << "Compte " << userId << " activé." << std::endl;
}

// ------------------------------------------------------
// Statistiques globales
// ------------------------------------------------------
void Administrateur::afficherStatistiques(MediPass& mp, sqlite3* db) {
    int nbUsers = 0, nbPatients = 0;
    sqlite3_exec(db, "SELECT COUNT(*) FROM users;", MediPass::callback, &nbUsers, nullptr);
    sqlite3_exec(db, "SELECT COUNT(*) FROM patients;", MediPass::callback, &nbPatients, nullptr);
    std::cout << "Nombre total d'utilisateurs: " << nbUsers << std::endl;
    std::cout << "Nombre total de patients: " << nbPatients << std::endl;
}

// ------------------------------------------------------
// Menu de l’administrateur
// ------------------------------------------------------
void Administrateur::menuAdmin(MediPass& mp, sqlite3* db) {
    int choix = 0;
    do {
        std::cout << "\n=== Menu Administrateur ===\n"
                  << "1. Créer un utilisateur\n"
                  << "2. Modifier rôle d’un utilisateur\n"
                  << "3. Activer/Désactiver un utilisateur\n"
                  << "4. Statistiques globales\n"
                  << "5. Déconnexion\n#> ";
        std::cin >> choix;

        switch(choix) {
            case 1: creerUtilisateur(mp, db); break;
            case 2: {
                int id; std::string role;
                std::cout << "ID utilisateur: "; std::cin >> id;
                std::cout << "Nouveau rôle: "; std::cin >> role;
                modifierRole(mp, db, id, role);
                break;
            }
            case 3: {
                int id; char act;
                std::cout << "ID utilisateur: "; std::cin >> id;
                std::cout << "Activer(a)/Désactiver(d)? "; std::cin >> act;
                if (act == 'a') activerCompte(mp, db, id);
                else desactiverCompte(mp, db, id);
                break;
            }
            case 4: afficherStatistiques(mp, db); break;
        }
    } while(choix != 5);
}
