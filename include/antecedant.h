#ifndef ANTECEDENT_H
#define ANTECEDENT_H

#include <string>

class Antecedant {
private:
    
    
public:
    int id;
    std::string description;
    std::string date;
    std::string type;

    Antecedant();
    Antecedant(int id,const std::string& type,
               const std::string& date,
               const std::string& description);

    // Getters
    std::string getDescription() const;
    std::string getDate() const;
    std::string getType() const;

    // Setters
    void setDescription(const std::string& d);
    void setDate(const std::string& d);
    void setType(const std::string& d);

    void afficher() const;
};

#endif
