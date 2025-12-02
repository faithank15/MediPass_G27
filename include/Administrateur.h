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
                   const std::string& dateNaissance,
                   const std::string& password,
                   const int telephone,
                   const std::string& created_by,
                   const std::string& created_at);

    // Gestion des utilisateurs
    void creerUtilisateur();
    void modifierAutorisation(int userId, const std::string& nouvelleAutorisation);
    void desactiverCompte(int userId);
    void activerCompte(int userId);
    void listerUtilisateurs();

    // Statistiques globales
    void afficherStatistiques();

    // Menu complet
    void menu();
};

#endif
