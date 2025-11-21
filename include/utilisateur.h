#ifndef UTILISATEUR_H
#define UTILISATEUR_H

#include <string>

class Utilisateur{

public:

    class Invalid{};
    
    Utilisateur(const std::string& firstname,
                const std::string& last_name, 
                const std::string& numero_de_tel);

    // Accesseurs
    std::string obtenir_firstname() const{return firstname; };
    std::string obtenir_lastname() const {return last_name; };
    std::string obtenir_numero_de_tel() const {return num_telephone; }; 

private:

    std::string num_telephone ;
    std::string firstname;
    std::string last_name;
     // int id; 
    // std::string email;


};

#endif