#ifndef EXAMEN_H
#define EXAMEN_H

#include <string>

class Examen {
private:
    
public:
    Examen();
    Examen(const std::string& date,
           const std::string& typeExamen,
           const std::string& resultat);

    // Getters
    std::string getDate() const;
    std::string getTypeExamen() const;
    std::string getResultat() const;

    // Setters
    void setDate(const std::string& d);
    void setTypeExamen(const std::string& t);
    void setResultat(const std::string& r);

    void afficher() const;

    std::string date;
    std::string typeExamen;
    std::string resultat;

    int id;

};

#endif