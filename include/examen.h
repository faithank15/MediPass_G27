#ifndef EXAMEN_H
#define EXAMEN_H

#include <string>
#include <chrono>
#include <vector>

class Examen{
public: 

    Examen(std::string nom)
    : nom{nom}
    {
    }

    std::string obtenir_nom() { return nom; };
    
private:
    std::string nom;
};

#endif