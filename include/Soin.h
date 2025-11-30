#ifndef SOIN_H
#define SOIN_H
#include <sqlite3.h>
#include <string>

class MediPass;
class Infirmier;


class Soin
{
    public:
        Soin();
        Soin(MediPass* mp,sqlite3* db, Infirmier* infirmier = nullptr,
             int patient_id = 0,
             const std::string& type = "",
             const std::string& description = "");
        ~Soin();
        void afficher() const;
        int id;
        std::string type;
        std::string description;
        std::string date;
        int infirmier_id;
        int dossier_id;
    protected:
    


    private:
    sqlite3* db;
};

#endif // SOIN_H
