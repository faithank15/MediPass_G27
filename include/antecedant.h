#ifndef ANTECEDENT_H
#define ANTECEDENT_H

#include <string>

class Antecedant {
private:
    std::string description;
    std::string date;

public:
    Antecedant();
    Antecedant(const std::string& description,
               const std::string& date);

    // Getters
    std::string getDescription() const;
    std::string getDate() const;

    // Setters
    void setDescription(const std::string& d);
    void setDate(const std::string& d);

    void afficher() const;
};

#endif
