#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <algorithm>

#include "profesionnel_de_sante.h"
#include "patient.h"

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

Pro_sante::Pro_sante(int id,
                     std::string username,
                     std::string password,
                     std::string role, 
                     bool active,
                     int telephone,
                     std::string created_by,
                     std::string created_at,
                     std::string autorisation, 
                     std::string statut)
:    User{username, password, role, active, telephone, created_by, created_at, id}, autorisation{autorisation}, statut{statut}
{
    if(!est_valide(autorisation, role, statut))
        throw Invalid{};
}



void Pro_sante::ajouter_date_disponible(const std::chrono::system_clock::time_point& date){
    liste_disponibilite.push_back(date);
}

    int id;
    std::string username;
    std::string password;
    std::string role;
    bool active;
    int telephone;
    std::string created_by;
    std::string created_at;

void  Pro_sante::menu(){
    int choix = 0;

    do {
        cout << "\n===== Menu Professionnel de Santé =====\n";
        cout << "1. Afficher mes informations\n";
        cout << "2. Afficher mes disponibilités\n";
        cout << "3. Ajouter une date de disponibilité\n";
        cout << "4. Voir mes patients en charge (placeholder)\n";
        cout << "0. Quitter\n";
        cout << "Votre choix : ";
        cin >> choix;

        switch (choix) {

        case 1: {
            cout << "\n--- Informations du professionnel ---\n";
            cout << "ID : " << getId() << "\n";
            cout << "Nom d'utilisateur : " << getUsername() << "\n";
            cout << "Rôle : " << getRole() << "\n";
            cout << "Autorisation : " << autorisation << "\n";
            cout << "Statut : " << statut << "\n";
            cout << "Téléphone : " << getTelephone() << "\n";
            cout << "Créé par : " << getCreatedBy() << "\n";
            cout << "Créé le : " << getCreatedAt() << "\n";
            cout << "---------------------------------------\n";
            break;
        }

        case 2: {
            cout << "\n--- Mes disponibilités ---\n";

            if (liste_disponibilite.empty()) {
                cout << "Aucune date enregistrée.\n";
            } else {
                for (const auto& d : liste_disponibilite) {
                    std::time_t t = chrono::system_clock::to_time_t(d);
                    cout << "- " << put_time(localtime(&t), "%Y-%m-%d %H:%M") << "\n";
                }
            }
            break;
        }

        case 3: {
            cout << "\n--- Ajouter une disponibilité ---\n";
            int annee, mois, jour, heure, minute;

            cout << "Année : ";
            cin >> annee;
            cout << "Mois  : ";
            cin >> mois;
            cout << "Jour  : ";
            cin >> jour;
            cout << "Heure : ";
            cin >> heure;
            cout << "Minute : ";
            cin >> minute;

            std::tm tm = {};
            tm.tm_year = annee - 1900;
            tm.tm_mon  = mois - 1;
            tm.tm_mday = jour;
            tm.tm_hour = heure;
            tm.tm_min  = minute;

            chrono::system_clock::time_point date =
                chrono::system_clock::from_time_t(std::mktime(&tm));

            ajouter_date_disponible(date);

            cout << "Date ajoutée avec succès !\n";
            break;
        }

        case 4:
            cout << "\n--- Patients en charge (placeholder) ---\n";
            cout << "Fonctionnalité en cours de construction.\n";
            break;

        case 0:
            cout << "Déconnexion...\n";
            break;

        default:
            cout << "Choix invalide, réessayez.\n";
            break;
        }

    } while (choix != 0);
}
