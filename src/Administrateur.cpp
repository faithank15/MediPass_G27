#include "Administrateur.h"
#include <iostream>
#include <string>
#include <sstream>
#include "MediPass.h"
#include "utilisateur.h"
#include <map>


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

    do{
    std::cout << "Type d'utilisateur a creer (admin / patient / professionel de sante) : ";
    std::getline(std::cin, type);
    if(type != "admin" && type != "patient" && type != "professionel de sante"){
        std::cout << "[!]: Type invalide. Veuillez reessayer." << std::endl;
        }
    } while(type != "admin" && type != "patient" && type != "professionel de sante");

    std::string prenom="", nom="", password="", role="", autorisation="",statut="",specialite="";
    int telephone=0;
    bool is_active=true;

    if(type=="professionel de sante") {
        std::cout << "Prénom : "; std::getline(std::cin, prenom);
        std::cout << "Nom : "; std::getline(std::cin, nom);
        std::cout << "Statut : "; std::getline(std::cin, statut);
        std::cout << "Spécialité : "; std::getline(std::cin, specialite);
        std::cout << "Téléphone : "; std::cin >> telephone;
        std::cout << "Autorisation (laisser vide pour l'autorisation par défaut): "; std::getline(std::cin, autorisation);

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
