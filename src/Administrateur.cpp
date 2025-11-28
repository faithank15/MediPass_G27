#include "Administrateur.h"
#include <iostream>
#include <string>
#include <sstream>
#include "MediPass.h"
#include "utilisateur.h"
#include <map>
#include <limits>



Administrateur::Administrateur(MediPass* mp,
                                sqlite3* db,
                                const std::string& firstname,
                                const std::string& last_name,
                                const std::string& password
                            ) : User(mp, db, firstname, last_name, password, "admin", true) {}
// ------------------------------------------------------
// Creer un utilisateur (admin / sante / patient)
// ------------------------------------------------------
void Administrateur::creerUtilisateur() {

    std::string type;

    std::map<std::string, std::string> defaultAut = {
        {"admin", "A3"},
        {"patient", ""},
        {"medecin", "A2"},
        {"infirmier", "A1"}
    };

    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    do {
    std::cout << "Type d'utilisateur a creer (admin / patient / professionnel de sante) :  ";
    std::getline(std::cin, type);
    if(type != "admin" && type != "patient" && type != "professionnel de sante"){
        std::cout << "[!]: Type invalide. Veuillez reessayer." << std::endl;
        }
    } while(type != "admin" && type != "patient" && type != "professionnel de sante");

    std::string prenom="", nom="", password="", role="", autorisation="",statut="",specialite="";
    int telephone=0;
    bool is_active=true;

    if(type=="professionnel de sante") {
        std::cout << "Prenom : "; std::getline(std::cin, prenom);
        std::cout << "Nom : "; std::getline(std::cin, nom);
        std::cout << "Statut : "; std::getline(std::cin, statut);
        std::cout << "Spécialité : "; std::getline(std::cin, specialite);
        std::cout << "Téléphone : "; std::cin >> telephone;
        std::cout << "Autorisation (laisser vide pour l'autorisation par défaut): ";  std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); std::getline(std::cin, autorisation);

        if(autorisation.empty()){
            autorisation = defaultAut[statut];
        }else if(autorisation != "A1" && autorisation != "A2"){
            std::cout << "[!]: Autorisation invalide. Attribution de l'autorisation par défaut." << std::endl;
            autorisation = defaultAut[statut];
        }else if(autorisation == "A1"){
            std::cout << "[!]: L'utilisateur pourra consulter les antécedents et l'historique de soins des patients. Il pourra aussi ajouter des soins." << std::endl;
            std::cout << "[!]: Il ne pourra pas créer/modifier/supprimer des dossiers médicaux ni créer des utilisateurs." << std::endl;
            std::cout << "[!]: Vous confirmer le niveau d'autorisation (O/N) ? ";
            char confirmation;
            std::cin >> confirmation;
            if (confirmation != 'O' && confirmation != 'o') {
                autorisation = defaultAut[statut];
            }
        }else if(autorisation == "A2"){
            std::cout << "[!]: L'utilisateur pourra tout faire sauf créer des utilisateurs." << std::endl;
            std::cout << "[!]: Vous confirmer le niveau d'autorisation (O/N) ? ";
            char confirmation;
            std::cin >> confirmation;
            if (confirmation != 'O' && confirmation != 'o') {
                autorisation = defaultAut[statut];
            }
        }
    }else if(type=="admin"){
        std::cin.ignore();
        std::cout << "Prénom : "; std::getline(cin, prenom);
        std::cout << "Nom : "; std::getline(cin, nom);
        std::cout << "Téléphone : "; std::cin >> telephone;
    }else if(type=="patient"){
        std::cin.ignore();
        std::cout << "Prénom : "; std::getline(std::cin, prenom);
        std::cout << "Nom : "; std::getline(std::cin, nom);
    }
    if(password.empty()) {
    password = "temp123"; // mot de passe par défaut
}

    // Construire la requête SQL
    std::string sql = sqlite3_mprintf(
        "INSERT INTO users (firstname, last_name, password, role, is_active, telephone,created_by,created_at, autorisation, statut, specialite) "
        "VALUES ('%q', '%q', '%q', '%q', %d, %d, '%q', '%q', '%q', '%q', '%q');",
        prenom.c_str(), nom.c_str(), password.c_str(),
        type.c_str(),
        is_active ? 1 : 0, telephone,
        this->getFirstname().c_str(), "CURRENT_TIMESTAMP",
        mp->getTimeDate().c_str(),
        autorisation.c_str(), statut.c_str(), specialite.c_str()
    );
    char* errMsg = nullptr;
    if (sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errMsg) != SQLITE_OK) {
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

void Administrateur::listerUtilisateurs() {
    const char* sql = "SELECT id, firstname, last_name, role, is_active FROM users;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "[ERROR] Impossible de préparer la requête : " << sqlite3_errmsg(db) << std::endl;
        return;
    }

    std::cout << "\n=== Liste des utilisateurs ===\n";
    std::cout << "ID | Prénom | Nom | Rôle | Actif\n";
    std::cout << "----------------------------------\n";

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        const unsigned char* prenom = sqlite3_column_text(stmt, 1);
        const unsigned char* nom = sqlite3_column_text(stmt, 2);
        const unsigned char* role = sqlite3_column_text(stmt, 3);
        int actif = sqlite3_column_int(stmt, 4);

        std::cout << id << " | "
                  << (prenom ? reinterpret_cast<const char*>(prenom) : "") << " | "
                  << (nom ? reinterpret_cast<const char*>(nom) : "") << " | "
                  << (role ? reinterpret_cast<const char*>(role) : "") << " | "
                  << (actif ? "Oui" : "Non") << "\n";
    }

    sqlite3_finalize(stmt);
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
// Menu de l'administrateur
// ------------------------------------------------------
void Administrateur::menu() {
    int choix = 0;
    do {
        std::cout << "\n=== Menu Administrateur ===\n"
                  << "1. Créer un utilisateur\n"
                  << "2. Modifier rôle d'un utilisateur\n"
                  << "3. Activer/Désactiver un utilisateur\n"
                  << "4. Statistiques globales\n"
                  << "5. Déconnexion\n"
                  << "6. Liste Utilisateur\n#> ";

        std::cin >> choix;

        switch(choix) {
            case 1: creerUtilisateur(); break;
            case 2: {
                int id; std::string role;
                std::cout << "ID utilisateur: "; std::cin >> id;
                std::cout << "Nouveau rôle: "; std::cin >> role;
                modifierRole(id, role);
                break;
            }
            case 3: {
                int id; char act;
                std::cout << "ID utilisateur: "; std::cin >> id;
                std::cout << "Activer(a)/Désactiver(d)? "; std::cin >> act;
                if (act == 'a') activerCompte(id);
                else desactiverCompte(id);
                break;
            }
            case 4: afficherStatistiques(); break;
            case 5:
                mp->logout();  // <-- Déconnexion effective
                std::cout << "[+]: Déconnexion réussie.\n";
                break;
            case 6: listerUtilisateurs(); break;
            default:
                std::cout << "[!]: Choix invalide.\n";
                break;

        }

    } while(choix != 5);
}
