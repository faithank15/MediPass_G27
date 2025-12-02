#ifndef INFIRMIER_H
#define INFIRMIER_H

#include <string>
#include "profesionnel_de_sante.h"

class Infirmier : public Pro_sante
{
public:
    Infirmier(MediPass* mp,
              sqlite3* db,
              std::string firstname,
              std::string last_name,
              std::string dateNaissance,
              std::string password,
              std::string role,
              bool active,
              int telephone,
              std::string created_by,
              std::string created_at,
              const std::string& autorisation = "A3",
              const std::string& specialite = "");

    virtual ~Infirmier();

    void menu();
    void administrer_soin();

private:

};

#endif
