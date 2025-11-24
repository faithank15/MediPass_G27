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

static const vector<string> AUTORISATIONS = {"A1", "A2", "A3"};              // Les différentes autorisations possibles, j'ai mis "ob" pour autorisations basique - nous allons compléter
static const vector<string> ROLES = {"Médecin Praticien", "Chef de Service", "Responsable d'unité fonctionnelle", "Directeur Médical"}; // Les différents status
static const vector<string> STATUTS = {"Actif", "Inactif", "Absent", "Suspendu"}; // les différents rôles hiérarchique et organisationnelle. Peut être que ce ,'est pas exactement à cette fin que nous avons créer cet attribut mais je l'ai interprété de cette façon pour staisfaire la dépendance



bool est_valide(const string& autorisation, const string& role, const string& statut){

    bool autorisation_trouve = find(AUTORISATIONS.begin(), AUTORISATIONS.end(), autorisation) != AUTORISATIONS.end();
    bool role_trouve = find(ROLES.begin(), ROLES.end(), role) != ROLES.end();
    bool statut_trouve = find(STATUTS.begin(), STATUTS.end(), statut) != STATUTS.end();

    return role_trouve && autorisation_trouve && statut_trouve;
}


Pro_sante::Pro_sante(MediPass* mp, sqlite3* db, const std::string& firstname, const std::string& last_name, const std::string& numero_de_tel, const std::string& autorisation, const std::string& role, const std::string& statut)
:    User(mp, db, firstname,last_name,NULL,role), autorisation{autorisation}, role{role}, statut{statut}
{
    if(!est_valide(autorisation, role, statut))
        throw Invalid{};
}



void Pro_sante::ajouter_date_disponible(const std::chrono::system_clock::time_point& date){
    liste_disponibilite.push_back(date);
}
