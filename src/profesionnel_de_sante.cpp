#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <sqlite3.h>
#include "MediPass.h"

#include "profesionnel_de_sante.h"
#include "patient.h"
#include "utilisateur.h"

using namespace std;

static const vector<string> AUTORISATIONS = {"A1", "A2", "A3"};                                 // Les différentes autorisations possibles, j'ai mis "ob" pour autorisations basique - nous allons compléter
static const vector<string> ROLES = {"professionnel de sante", "admin", "patient"};             // Les différents status
static const vector<string> STATUTS = {"infirmier", "medecin"};                                 // les différents rôles hiérarchique et organisationnelle. Peut être que ce ,'est pas exactement à cette fin que nous avons créer cet attribut mais je l'ai interprété de cette façon pour staisfaire la dépendance



bool est_valide(const string& autorisation, const string& role, const string& statut){

    bool autorisation_trouve = find(AUTORISATIONS.begin(), AUTORISATIONS.end(), autorisation) != AUTORISATIONS.end();
    //bool role_trouve = find(ROLES.begin(), ROLES.end(), role) != ROLES.end();
    bool statut_trouve = find(STATUTS.begin(), STATUTS.end(), statut) != STATUTS.end();

    return autorisation_trouve && statut_trouve;
}


Pro_sante::Pro_sante(MediPass* mp, sqlite3* db, const std::string& firstname,
              const std::string& last_name,
              std::string dateNaissance,
              std::string password,
              bool active,
              int telephone,
              std::string created_by,
              std::string created_at,
              std::string autorisation,
              std::string statut,
              std::string specialite)
:    User(mp, db, firstname,last_name,dateNaissance,password,"professionnel de sante",active,telephone,created_by,created_at,autorisation,statut,specialite)
{
                std::cout << "[!]: Constructeur pro medecin atteint" << std::endl;

    if(!est_valide(autorisation, role, statut))
        throw Invalid{};
}



void Pro_sante::ajouter_date_disponible(const std::chrono::system_clock::time_point& date){
    liste_disponibilite.push_back(date);
}

