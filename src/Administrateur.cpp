#include "Administrateur.h"
#include <iostream>

Administrateur::Administrateur(const std::string& id,
                               const std::string& username,
                               const std::string& password){User(id, username, password,"administrateur",true,NULL,NULL,NULL); }

// ------------------------------------------------------
// Creer un utilisateur (admin / sante / patient)
// ------------------------------------------------------
void Administrateur::creerUtilisateur(MediPass *mp, sqlite3* db) {
    std::string nom, prenom, username, password, role;
    std::cout << "Prenom: "; std::cin >> prenom;
    std::cout << "Nom: "; std::cin >> nom;
    std::cout << "Username: "; std::cin >> username;
    std::cout << "Password: "; std::cin >> password;
    std::cout << "Role (admin / sante / patient): "; std::cin >> role;

    mp.create_user(db, username, password, role, true, 0, this->getNomComplet());
    std::cout << "Utilisateur cree: " << prenom << " " << nom << " | Role: " << role << std::endl;
}

// ------------------------------------------------------
// Modifier le role d un utilisateur
// ------------------------------------------------------
void Administrateur::modifierRole(MediPass& mp, sqlite3* db, int userId, const std::string& nouveauRole) {
    std::string sql = "UPDATE users SET role='" + nouveauRole + "' WHERE id=" + std::to_string(userId) + ";";
    char* errMsg = nullptr;
    if (sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errMsg) != SQLITE_OK) {
        std::cerr << "Erreur SQL: " << errMsg << std::endl;
        sqlite3_free(errMsg);
    } else {
        std::cout << "Role de l'utilisateur " << userId << " modifie en " << nouveauRole << std::endl;
    }
}

// ------------------------------------------------------
// D�sactiver / activer compte
// ------------------------------------------------------
void Administrateur::desactiverCompte(MediPass& mp, sqlite3* db, int userId) {
    std::string sql = "UPDATE users SET is_active=0 WHERE id=" + std::to_string(userId) + ";";
    char* errMsg = nullptr;
    sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errMsg);
    std::cout << "Compte " << userId << " desactive." << std::endl;
}

void Administrateur::activerCompte(MediPass& mp, sqlite3* db, int userId) {
    std::string sql = "UPDATE users SET is_active=1 WHERE id=" + std::to_string(userId) + ";";
    char* errMsg = nullptr;
    sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errMsg);
    std::cout << "Compte " << userId << " active." << std::endl;
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
// Menu de l�administrateur
// ------------------------------------------------------
void Administrateur::menu(MediPass& mp, sqlite3* db) {
    int choix = 0;
    do {
        std::cout << "\n=== Menu Administrateur ===\n"
                  << "1. Cr�er un utilisateur\n"
                  << "2. Modifier r�le d�un utilisateur\n"
                  << "3. Activer/D�sactiver un utilisateur\n"
                  << "4. Statistiques globales\n"
                  << "5. D�connexion\n#> ";
        std::cin >> choix;

        switch(choix) {
            case 1: creerUtilisateur(mp, db); break;
            case 2: {
                int id; std::string role;
                std::cout << "ID utilisateur: "; std::cin >> id;
                std::cout << "Nouveau r�le: "; std::cin >> role;
                modifierRole(mp, db, id, role);
                break;
            }
            case 3: {
                int id; char act;
                std::cout << "ID utilisateur: "; std::cin >> id;
                std::cout << "Activer(a)/D�sactiver(d)? "; std::cin >> act;
                if (act == 'a') activerCompte(mp, db, id);
                else desactiverCompte(mp, db, id);
                break;
            }
            case 4: afficherStatistiques(mp, db); break;
        }
    } while(choix != 5);
}
