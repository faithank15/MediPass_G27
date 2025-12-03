#include "Administrateur.h"
#include <iostream>
#include <string>
#include <sstream>
#include "MediPass.h"
#include "utilisateur.h"
#include <map>
#include <limits>


Administrateur::Administrateur(MediPass* mp, sqlite3* db, const std::string& firstname,
                               const std::string& last_name,
                               const std::string& dateNaissance,
                               const std::string& password,
                               const int telephone,
                               const std::string& created_by,
                               const std::string& created_at,
                               const std::string& autorisation,
                               const std::string& statut)
    : User(mp, db, firstname, last_name, dateNaissance, password, "admin", true,
           telephone, created_by, created_at, autorisation, statut) {}


// ------------------------------------------------------
// Création d'un utilisateur
// ------------------------------------------------------
void Administrateur::creerUtilisateur() {

    std::string type;

    std::map<std::string, std::string> defaultAut = {
        {"admin", "A1"},
        {"patient", "A4"},
        {"medecin", "A2"},
        {"infirmier", "A3"},
        {"secretaire", "A4"}
    };

    do {
        std::cout << "Type d'utilisateur a creer (admin / patient / secretaire / professionnel de sante) : ";
        std::getline(std::cin, type);

        if (type != "admin" && type != "patient" &&
            type != "secretaire" && type != "professionnel de sante") {
            std::cout << "[!]: Type invalide. Veuillez reessayer." << std::endl;
        }

    } while (type != "admin" && type != "patient" &&
             type != "secretaire" && type != "professionnel de sante");


    // Variables communes
    std::string prenom = "", nom = "", passw = "user";
    std::string statut = "", specialite = "", autorisation = "", dateNaissance = "";
    int telephone = 0;
    bool is_active = true;

    // -----------------------------
    // Professionnel de santé
    // -----------------------------
    if (type == "professionnel de sante") {

        std::cout << "Prénom : "; std::getline(std::cin, prenom);
        std::cout << "Nom : "; std::getline(std::cin, nom);
        std::cout << "Date de naissance (AAAA-MM-JJ) : "; std::getline(std::cin, dateNaissance);
        std::cout << "Statut (medecin / infirmier) : "; std::getline(std::cin, statut);
        std::cout << "Specialite : "; std::getline(std::cin, specialite);
        std::cout << "Téléphone : "; std::cin >> telephone;
        std::cin.ignore();

        autorisation = defaultAut[statut];

        std::string sql = sqlite3_mprintf(
        "INSERT INTO users (firstname, last_name, date_of_birth, password, role, is_active, telephone, created_by, created_at, autorisation, statut, specialite) "
        "VALUES('%q','%q','%q','%q','%q',%d,%d,'%q','%q','%q','%q','%q');",
        prenom.c_str(), nom.c_str(), dateNaissance.c_str(), passw.c_str(),
        type.c_str(),
        is_active ? 1 : 0, telephone,
        this->getFirstname().c_str(),
        mp->getTimeDate().c_str(),
        autorisation.c_str(), statut.c_str(), specialite.c_str()
        );

        char* errMsg = nullptr;
        if (sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errMsg) != SQLITE_OK) {
            std::cerr << "Erreur SQL : " << errMsg << std::endl;
            sqlite3_free(errMsg);
            return;
        }

    }

    // -----------------------------
    // Admin
    // -----------------------------
    else if (type == "admin") {
        std::cout << "Prénom : "; std::getline(std::cin, prenom);
        std::cout << "Nom : "; std::getline(std::cin, nom);
        std::cout << "Date de naissance (AAAA-MM-JJ) : "; std::getline(std::cin, dateNaissance);
        std::cout << "Téléphone : "; std::cin >> telephone;
        std::cin.ignore();

        statut = "admin basique";
        autorisation = defaultAut["admin"];

        std::string sql = sqlite3_mprintf(
        "INSERT INTO users (firstname, last_name, date_of_birth, password, role, is_active, telephone, created_by, created_at, autorisation, statut, specialite) "
        "VALUES('%q','%q','%q','%q','%q',%d,%d,'%q','%q','%q','%q','%q');",
        prenom.c_str(), nom.c_str(), dateNaissance.c_str(), passw.c_str(),
        type.c_str(),
        is_active ? 1 : 0, telephone,
        this->getFirstname().c_str(),
        mp->getTimeDate().c_str(),
        autorisation.c_str(), statut.c_str(), specialite.c_str()
        );

        char* errMsg = nullptr;
        if (sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errMsg) != SQLITE_OK) {
            std::cerr << "Erreur SQL : " << errMsg << std::endl;
            sqlite3_free(errMsg);
            return;
        }

    }

    // -----------------------------
    // Patient
    // -----------------------------
    else if (type == "patient") {
        std::cout << "Prénom : "; std::getline(std::cin, prenom);
        std::cout << "Nom : "; std::getline(std::cin, nom);
        std::cout << "Date de naissance (AAAA-MM-JJ) : "; std::getline(std::cin, dateNaissance);
        std::cout << "Téléphone : "; std::cin >> telephone;
        std::cin.ignore();

        statut = "patient";
        autorisation = "A4";

        std::string sql = sqlite3_mprintf(
            "INSERT INTO users (firstname, last_name, date_of_birth, password, role, is_active, telephone, created_by, created_at, autorisation, statut, specialite) "
            "VALUES('%q','%q','%q','%q','%q',%d,%d,'%q','%q','%q','%q','%q');",
            prenom.c_str(), nom.c_str(), dateNaissance.c_str(), passw.c_str(),
            type.c_str(),
            is_active ? 1 : 0, telephone,
            this->getFirstname().c_str(),
            mp->getTimeDate().c_str(),
            autorisation.c_str(), statut.c_str(), specialite.c_str()
        );
        char* errMsg = nullptr;
        if (sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errMsg) != SQLITE_OK) {
            std::cerr << "Erreur SQL : " << errMsg << std::endl;
            sqlite3_free(errMsg);
            return;
        }

        int id = sqlite3_last_insert_rowid(db);

        sql = sqlite3_mprintf(
            "INSERT INTO patients (patient_user_id) VALUES(%d);",
            id
        );
        if (sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errMsg) != SQLITE_OK) {
            std::cerr << "Erreur SQL : " << errMsg << std::endl;
            sqlite3_free(errMsg);
            return;
        }

        sql = sqlite3_mprintf(
            "INSERT INTO DOSSIERS_MEDICAUX (patient_id, created_at) VALUES(%d,'%q');",
            sqlite3_last_insert_rowid(db), mp->getTimeDate().c_str()
        );

        if (sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errMsg) != SQLITE_OK) {
            std::cerr << "Erreur SQL : " << errMsg << std::endl;
            sqlite3_free(errMsg);
            return;
        }

    }

    // -----------------------------
    // Secrétaire
    // -----------------------------
    else if (type == "secretaire") {
        std::cout << "Prénom : "; std::getline(std::cin, prenom);
        std::cout << "Nom : "; std::getline(std::cin, nom);
        std::cout << "Date de naissance (AAAA-MM-JJ) : "; std::getline(std::cin, dateNaissance);
        std::cout << "Téléphone : "; std::cin >> telephone;
        std::cin.ignore();

        statut = "secretaire";
        autorisation = "A4";

        std::string sql = sqlite3_mprintf(
            "INSERT INTO users (firstname, last_name, date_of_birth, password, role, is_active, telephone, created_by, created_at, autorisation, statut, specialite) "
            "VALUES('%q','%q','%q','%q','%q',%d,%d,'%q','%q','%q','%q','%q');",
            prenom.c_str(), nom.c_str(), dateNaissance.c_str(), passw.c_str(),
            type.c_str(),
            is_active ? 1 : 0, telephone,
            this->getFirstname().c_str(),
            mp->getTimeDate().c_str(),
            autorisation.c_str(), statut.c_str(), specialite.c_str()
        );
        char* errMsg = nullptr;
        if (sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errMsg) != SQLITE_OK) {
            std::cerr << "Erreur SQL : " << errMsg << std::endl;
            sqlite3_free(errMsg);
            return;
        }
        
    }


    // ------------------------------------------------------
    // INSERT utilisateur
    // ------------------------------------------------------

    

    std::cout << "[+] Utilisateur créé avec succès !" << std::endl;

    long newUserId = sqlite3_last_insert_rowid(db);


    // ------------------------------------------------------
    // Création patient → ajouter patient + dossier
    // ------------------------------------------------------
    if (type == "patient") {

        std::string sql2 = sqlite3_mprintf(
            "INSERT INTO patients (patient_user_id) VALUES(%d);",
            newUserId
        );
        char* errMsg = nullptr;

        if (sqlite3_exec(db, sql2.c_str(), nullptr, nullptr, &errMsg) != SQLITE_OK) {
            std::cerr << "Erreur lors de la création du patient : " << errMsg << std::endl;
            sqlite3_free(errMsg);
        } else {
            std::cout << "[+] Patient créé." << std::endl;
        }

        long patId = sqlite3_last_insert_rowid(db);

        std::string sql3 = sqlite3_mprintf(
            "INSERT INTO dossiers_medicaux (patient_id, created_at) VALUES(%d,'%q');",
            patId, mp->getTimeDate().c_str()
        );

        if (sqlite3_exec(db, sql3.c_str(), nullptr, nullptr, &errMsg) != SQLITE_OK) {
            std::cerr << "Erreur dossier médical : " << errMsg << std::endl;
            sqlite3_free(errMsg);
        } else {
            std::cout << "[+] Dossier médical créé." << std::endl;
        }
    }
}

void Administrateur::creerSuperAdministrateur() {

    std::string prenom = "", nom = "", passw = "admin",dateN="";
    int telephone = 0;

    std::cout << "[!] Creation d'un super administrateur :" << std::endl;
    std::cout << std::endl;
    std::cout << "Le super administrateur aura les droits complets d'administration sur le systeme." << std::endl;
    std::cout << std::endl;

    std::cout << "Vous confirmez la creation d'un super administrateur ? (o/n) : ";
    char confirm;
    std::cin >> confirm;
    std::cin.ignore();

    if (confirm != 'o' && confirm != 'O') {
        std::cout << "[!]: Creation annulee." << std::endl;
        return;
    }

    std::cout << "Prénom : "; std::getline(std::cin, prenom);
    std::cout << "Nom : "; std::getline(std::cin, nom);
    try
    {
        std::cout << "Téléphone : "; std::cin >> telephone;
    }
    catch(const std::exception& e)
    {
        std::cerr << "[!]: Entrée invalide pour le téléphone. Abandon de la création." << std::endl;
        return;
    }
    
    std::cin.ignore();
    std::cout << "Date de naissance (AAAA-MM-JJ) : "; std::getline(std::cin, dateN);
    

    std::string sql = sqlite3_mprintf(
        "INSERT INTO users (firstname, last_name, password, role, is_active, telephone, created_by, created_at, autorisation, statut) "
        "VALUES('%q','%q','%q','%q',%d,%d,'%q','%q','%q','%q');",
        prenom.c_str(), nom.c_str(), passw.c_str(),
        "admin",
        1, telephone,
        this->getFirstname().c_str(),
        mp->getTimeDate().c_str(),
        "A0", "super admin"
    );

    char* errMsg = nullptr;
    if (sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errMsg) != SQLITE_OK) {
        std::cerr << "Erreur SQL : " << errMsg << std::endl;
        sqlite3_free(errMsg);
        return;
    }

    std::cout << "[+] Super admin user created successfully." << std::endl;
}


// ------------------------------------------------------
// Modifier autorisation
// ------------------------------------------------------
void Administrateur::modifierAutorisation(int userId, const std::string& nouvelleAutorisation) {

    // Construire la requête SQL
    std::string sql = "UPDATE users SET autorisation='" + nouvelleAutorisation +
                      "' WHERE id=" + std::to_string(userId) + ";";

    char* errMsg = nullptr;

    // Exécuter la requête
    if (sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errMsg) != SQLITE_OK) {
        std::cerr << "[!] Erreur SQL : " << errMsg << std::endl;
        sqlite3_free(errMsg);
    } else {
        std::cout << "[+] Autorisation modifiée avec succès." << std::endl;
    }
}




// ------------------------------------------------------
// Liste utilisateurs
// ------------------------------------------------------
void Administrateur::listerUtilisateurs() {
    const char* sql = "SELECT id, firstname, last_name, role, is_active FROM users;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "[ERROR] Impossible de préparer la requête : "
                  << sqlite3_errmsg(db) << std::endl;
        return;
    }

    std::cout << "\n=== Liste des utilisateurs ===\n";
    std::cout << "ID | Prénom | Nom | Rôle | Actif\n";
    std::cout << "----------------------------------\n";

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        const char* prenom = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        const char* nom = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        const char* role = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        int actif = sqlite3_column_int(stmt, 4);

        std::cout << id << " | " << prenom << " | " << nom
                  << " | " << role << " | " << (actif ? "Oui" : "Non") << "\n";
    }

    sqlite3_finalize(stmt);
}



// ------------------------------------------------------
// Activation / désactivation
// ------------------------------------------------------
void Administrateur::desactiverCompte(int userId) {
    sqlite3_exec(db, ("UPDATE users SET is_active=0 WHERE id="+std::to_string(userId)).c_str(),
                 nullptr, nullptr, nullptr);

    std::cout << "[+] Compte désactivé.\n";
}

void Administrateur::activerCompte(int userId) {
    sqlite3_exec(db, ("UPDATE users SET is_active=1 WHERE id="+std::to_string(userId)).c_str(),
                 nullptr, nullptr, nullptr);

    std::cout << "[+] Compte activé.\n";
}



// ------------------------------------------------------
// Statistiques détaillées
// ------------------------------------------------------
void Administrateur::afficherStatistiques() {
    std::cout << "\n===== STATISTIQUES DU SYSTÈME =====\n";

    auto getInt = [&](const std::string& query) {
        int value = 0;
        sqlite3_exec(db, query.c_str(), MediPass::callback, &value, nullptr);
        return value;
    };

    // ---- STATISTIQUES GLOBALES ----
    int nbUsers      = getInt("SELECT COUNT(*) FROM users;");
    int nbActive     = getInt("SELECT COUNT(*) FROM users WHERE is_active = 1;");
    int nbInactive   = getInt("SELECT COUNT(*) FROM users WHERE is_active = 0;");
    int nbPatients   = getInt("SELECT COUNT(*) FROM patients;");
    int nbDocs       = getInt("SELECT COUNT(*) FROM users WHERE statut = 'medecin';");
    int nbInf        = getInt("SELECT COUNT(*) FROM users WHERE statut = 'infirmier';");
    int nbSec        = getInt("SELECT COUNT(*) FROM users WHERE statut = 'secretaire';");
    int nbAdmin      = getInt("SELECT COUNT(*) FROM users WHERE role = 'admin';");
    int nbConsult    = getInt("SELECT COUNT(*) FROM CONSULTATIONS;");
    int nbDossiers   = getInt("SELECT COUNT(*) FROM DOSSIERS_MEDICAUX;");

    // Moyenne consultations par patient
    double moyConsult = 0;
    if (nbPatients > 0) moyConsult = (double)nbConsult / nbPatients;

    std::cout << "\n--- Comptes Utilisateurs ---\n";
    std::cout << "Total d'utilisateurs : " << nbUsers << "\n";
    std::cout << "Actifs : " << nbActive << "\n";
    std::cout << "Inactifs : " << nbInactive << "\n";

    std::cout << "\n--- Répartition par rôles ---\n";
    std::cout << "Administrateurs : " << nbAdmin << "\n";
    std::cout << "Médecins : " << nbDocs << "\n";
    std::cout << "Infirmiers : " << nbInf << "\n";
    std::cout << "Secrétaires : " << nbSec << "\n";
    std::cout << "Patients : " << nbPatients << "\n";

    std::cout << "\n--- Données médicales ---\n";
    std::cout << "Nombre total de consultations : " << nbConsult << "\n";
    std::cout << "Nombre total de dossiers médicaux : " << nbDossiers << "\n";
    std::cout << "Consultations moyennes par patient : " << moyConsult << "\n";

    // ---- STATISTIQUES PAR SPÉCIALITÉ ----
    std::cout << "\n--- Spécialités médicales ---\n";

    const char* sql =
        "SELECT specialite, COUNT(*) "
        "FROM users WHERE statut = 'medecin' "
        "GROUP BY specialite;";

    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {

        while (sqlite3_step(stmt) == SQLITE_ROW) {
            const unsigned char* spec = sqlite3_column_text(stmt, 0);
            int nb = sqlite3_column_int(stmt, 1);

            std::cout << (spec ? (const char*)spec : "Non spécifié")
                      << " : " << nb << " médecin(s)\n";
        }
    }
    sqlite3_finalize(stmt);

    std::cout << "\n===== FIN DES STATISTIQUES =====\n";
}




// ------------------------------------------------------
// Menu
// ------------------------------------------------------
void Administrateur::menu() {
    int choix = 0;

    do {
        if(this->autorisation!="A0"&&this->autorisation!="A1"){
            std::cout << "[!]: Vous n'avez pas les autorisations necessaires pour acceder a l'administration.\n";
            choix=6;
            continue;
        }
        std::cout << "\n====================================================\n"
                  << "             Menu Administrateur (" << this->getFirstname() << ")           \n"
                  << "====================================================\n";
        if(this->autorisation=="A0"){
            std::cout << "0. Creer un super administrateur\n";
        }
        std::cout << "1. Creer un utilisateur\n"
                  << "2. Modifier autorisation d'un utilisateur\n"
                  << "3. Activer/Desactiver un utilisateur\n"
                  << "4. Statistiques detaillees\n"
                  << "5. Liste des utilisateurs\n"
                  << "6. Deconnexion\n#> ";

        std::cin >> choix;
        std::cin.ignore();

        switch (choix) {
        
        case 0:
            if(this->autorisation=="A0"){
                creerSuperAdministrateur();
                break;
            }

        case 1: creerUtilisateur(); break;

        case 2: {
            int id;
            std::string nouvelleAutorisation;
            std::cout << "ID utilisateur: ";
            std::cin >> id;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // nettoyer le buffer
            std::cout << "Nouvelle autorisation (ex: A1, A2, A3, A4) : ";
            std::getline(std::cin, nouvelleAutorisation);
            modifierAutorisation(id, nouvelleAutorisation);
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

        case 6:
            mp->logout();
            std::cout << "[+]: Déconnexion réussie.\n";
            break;

        case 5: listerUtilisateurs(); break;

        default:
            std::cout << "[!]: Choix invalide.\n";
        }

    } while (choix != 6);
}
