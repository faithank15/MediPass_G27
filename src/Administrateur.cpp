#include "Administrateur.h"
#include <iostream>
#include <string>
#include <sstream>
#include "MediPass.h"


Administrateur::Administrateur(MediPass* mp, sqlite3* db, const std::string& firstname,
                               const std::string& last_name,
                               const std::string& password){User(mp, db, firstname, last_name, password,"administrateur",true,NULL,NULL,NULL); }

// ------------------------------------------------------
// Creer un utilisateur (admin / sante / patient)
// ------------------------------------------------------
void Administrateur::creerUtilisateur() {
    std::string prenom, nom, username, password, role;

    std::cin.ignore();
    std::cout << "Prénom : "; std::getline(std::cin, prenom);
    std::cout << "Nom : "; std::getline(std::cin, nom);
    std::cout << "Nom d'utilisateur : "; std::getline(std::cin, username);
    std::cout << "Mot de passe : "; std::getline(std::cin, password);
    std::cout << "Rôle (admin / patient / professionel de sante) : "; std::getline(std::cin, role);

    // Construire la requête SQL
    std::stringstream ss;
    ss << "INSERT INTO users (firstname,last_name, password, role, is_active, created_by) "
       << "VALUES ('" << firstname << "', '" << last_name << "', '" << password << "', '" << role << "', 1, '"
       << this->getFirstname() << "');";

    std::string query = ss.str();
    char* errMsg = nullptr;
    if (sqlite3_exec(db, query.c_str(), nullptr, nullptr, &errMsg) != SQLITE_OK) {
        std::cerr << "Erreur SQL : " << errMsg << std::endl;
        sqlite3_free(errMsg);
    } else {
        std::cout << "Utilisateur créé avec succès !" << std::endl;
    }
}


// ------------------------------------------------------
// Modifier le role d un utilisateur
// ------------------------------------------------------
void Administrateur::modifierRole(int userId, const std::string& nouveauRole) {
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
void Administrateur::desactiverCompte(int userId) {
    std::string sql = "UPDATE users SET is_active=0 WHERE id=" + std::to_string(userId) + ";";
    char* errMsg = nullptr;
    sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errMsg);
    std::cout << "Compte " << userId << " desactive." << std::endl;
}

void Administrateur::activerCompte(int userId) {
    std::string sql = "UPDATE users SET is_active=1 WHERE id=" + std::to_string(userId) + ";";
    char* errMsg = nullptr;
    sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errMsg);
    std::cout << "Compte " << userId << " active." << std::endl;
}

// ------------------------------------------------------
// Statistiques globales
// ------------------------------------------------------
void Administrateur::afficherStatistiques() {
    int nbUsers = 0, nbPatients = 0;
    sqlite3_exec(db, "SELECT COUNT(*) FROM users;", MediPass::callback, &nbUsers, nullptr);
    sqlite3_exec(db, "SELECT COUNT(*) FROM patients;", MediPass::callback, &nbPatients, nullptr);
    std::cout << "Nombre total d'utilisateurs: " << nbUsers << std::endl;
    std::cout << "Nombre total de patients: " << nbPatients << std::endl;
}

// ------------------------------------------------------
// Menu de l�administrateur
// ------------------------------------------------------
void Administrateur::menu() {
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
            case 1: creerUtilisateur(); break;
            case 2: {
                int id; std::string role;
                std::cout << "ID utilisateur: "; std::cin >> id;
                std::cout << "Nouveau r�le: "; std::cin >> role;
                modifierRole(id, role);
                break;
            }
            case 3: {
                int id; char act;
                std::cout << "ID utilisateur: "; std::cin >> id;
                std::cout << "Activer(a)/D�sactiver(d)? "; std::cin >> act;
                if (act == 'a') activerCompte(id);
                else desactiverCompte(id);
                break;
            }
            case 4: afficherStatistiques(); break;
        }
    } while(choix != 5);
}
