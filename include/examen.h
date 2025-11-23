#ifndef EXAMEN_H
#define EXAMEN_H
#include <string>
#include <chrono>

class Examen
{
    public:
        Examen(const std::string& type, const std::chrono::system_clock::time_point& date_examen, const std::string& resultat);
        std::string getType() const;
        std::chrono::system_clock::time_point getDateExamen() const;
        std::string getResultat() const;

    private:
        std::string type;
        std::chrono::system_clock::time_point date_examen;
        std::string resultat;
};
#endif // EXAMEN_H