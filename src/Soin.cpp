#include "Soin.h"
#include "infirmier.h"
#include <sqlite3.h>
#include <iostream>
#include <vector>
#include "MediPass.h"


Soin::Soin(){}

Soin::Soin(MediPass* mp,sqlite3* db, Infirmier* infirmier,
             int patient_id,
             const std::string& type,
             const std::string& description)
{
    this->db = db;
    this->type = type;
    this->description = description;
    this->date = mp->getTimeDate();
    this->infirmier_id = infirmier->getId();
    dossier_id = 0;

    sqlite3_exec(db,
        sqlite3_mprintf("SELECT id FROM DOSSIERS_MEDICAUX WHERE patient_id = %d;", patient_id),
        MediPass::callback,
        &dossier_id,
        nullptr
    );

    sqlite3_exec(db,
        sqlite3_mprintf("INSERT INTO SOINS (dossier_id, type, description, date, infirmier_id) "
                        "VALUES (%d, '%q', '%q', '%q', %d);",
                        dossier_id, type.c_str(), description.c_str(), this->date.c_str(), this->infirmier_id),
        nullptr, nullptr, nullptr);
}

Soin::~Soin()
{
    //dtor
}

void Soin::afficher() const {

    std::vector<std::string> infirmier_info;
    sqlite3_exec(db,
        sqlite3_mprintf("SELECT first_name, last_name FROM users WHERE id = %d;", infirmier_id),
        MediPass::callback_names,
        &infirmier_info,
        nullptr
    );

    std::cout << "Soin administré le " << date
              << "\nInfirmier : " << infirmier_info[0] << " " << infirmier_info[1]
              << "\nInfirmier ID : " << infirmier_id
              << "\nType : " << type
              << "\nDescription : " << description
              << "\n-------------------------------\n";
}
