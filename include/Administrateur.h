#ifndef ADMINISTRATEUR_HPP
#define ADMINISTRATEUR_HPP

#include <string>
#include <vector>
#include "utilisateur.h"
#include "patient.h"
#include <sqlite3.h>

class MediPass;   // forward declaration


class Administrateur :virtual public User{
public:
    Administrateur(MediPass* mp, sqlite3* db, const std::string& firstname,
                   const std::string& last_name,
                   const std::string& password);

    // Gestion des utilisateurs
    void creerUtilisateur();
    void modifierRole(int userId, const std::string& nouveauRole);
    void desactiverCompte(int userId);
    void activerCompte(int userId);

    // Statistiques globales
    void afficherStatistiques();

    // Menu complet
    void menu();
};

#endif
