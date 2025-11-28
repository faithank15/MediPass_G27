#include "infirmier.h"
#include <iostream>
#include "MediPass.h"
#include "profesionnel_de_sante.h"
#include "Soin.h"

class Soin; // Forward declaration

Infirmier::Infirmier(MediPass* mp,sqlite3* db,std::string firstname,
            std::string last_name,
            std::string password,
            std::string role,
            bool active,
            int telephone,
            std::string created_by,
            std::string created_at,
            const std::string& autorisation, 
            std::string statut)
     : Pro_sante(mp, db, firstname,last_name,password,active,telephone,created_by,created_at,autorisation,statut)
{
}

Infirmier::~Infirmier()
{
}

void Infirmier::menu(){
    std::cout << "Menu Infirmier" << std::endl;
    std::cout << "1. Administrer soin" << std::endl;
    std::cout << "2. Déconnexion" << std::endl;

    int choix;

    do{
        std::cout << "Choisissez une option: ";

        std::cin >> choix;

        switch(choix){
            case 1:
                administrer_soin();
                break;
            case 2:
                std::cout << "Au revoir!" << std::endl;
                break;
            default:
                std::cout << "Choix invalide." << std::endl;
                break;
        }
    } while(choix != 2);
}

void Infirmier::administrer_soin(){

    int patient_id;
    string type;
    string description;
    string patient_firstname;
    string patient_lastname;

    std::cout << "=======SOIN=======" << std::endl;
    std::cout << "\n\n[ ]: Veuillez entrer les informations du patient."
              << "\nID du patient: ";
    std::cin >> patient_id;
    std::cin.ignore(); // Pour consommer le caractère de nouvelle ligne restant
    std::cout << "\n Prénom du patient: ";
    std::getline(std::cin, patient_firstname);
    std::cout << "\n Nom du patient: ";
    std::getline(std::cin, patient_lastname);
    std::cout << "\n Type de soin: ";
    std::getline(std::cin, type);
    std::cout << "\n Description du soin: ";
    std::getline(std::cin, description);

    Soin soin(mp,db, this, patient_id, type, description);
}
