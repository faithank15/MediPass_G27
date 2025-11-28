#ifndef INFIRMIER_H
#define INFIRMIER_H

#include <string>
#include <chrono>
#include <vector>
#include "profesionnel_de_sante.h"

class Infirmier: public Pro_sante
{

public:
    Infirmier(MediPass* mp,sqlite3* db,std::string firstname,
            std::string last_name,
            std::string password,
            std::string role,
            bool active,
            int telephone,
            std::string created_by,
            std::string created_at,
            const std::string& autorisation = "A1",
            std::string statut);
    virtual ~Infirmier();
    void administrer_soin();
    void menu();
private:

};

#endif
