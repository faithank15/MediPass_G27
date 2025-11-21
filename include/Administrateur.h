#ifndef ADMINISTRATEUR_HPP
#define ADMINISTRATEUR_HPP

#include <string>
#include <vector>
#include "Utilisateur.hpp"
#include "Patient.hpp"
#include "MediPass.h"
#include <sqlite3.h>

class Administrateur : public Utilisateur {
public:
    Administrateur(const std::string& id,
                   const std::string& firstname,
                   const std::string& lastname,
                   const std::string& username,
                   const std::string& password);

    // Gestion des utilisateurs
    void creerUtilisateur(MediPass& mp, sqlite3* db);
    void modifierRole(MediPass& mp, sqlite3* db, int userId, const std::string& nouveauRole);
    void desactiverCompte(MediPass& mp, sqlite3* db, int userId);
    void activerCompte(MediPass& mp, sqlite3* db, int userId);

    // Statistiques globales
    void afficherStatistiques(MediPass& mp, sqlite3* db);

    // Menu complet
    void menuAdmin(MediPass& mp, sqlite3* db);
};

#endif
