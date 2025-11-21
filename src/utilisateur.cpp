#include <iostream>
#include <regex>
#include "utilisateur.h"

using namespace std;

bool numero_est_valide(const string& num_tel){
    regex motif_numero(R"((?:\+?229\s*)?01[0-9]{8})");
    smatch resultat; 
    if(!regex_match(num_tel, resultat, motif_numero))
        return false;
    return true;
}


Utilisateur::Utilisateur(const string& firstname,
                         const string& last_name, 
                         const string& numero_de_tel)

:   firstname{firstname}, last_name{last_name}, num_telephone{numero_de_tel}
{
    if(!numero_est_valide(num_telephone))
        throw Invalid{};
}


// int main() {
//     try {
//     Utilisateur u("Pito", "Kossi", "+229 0123456789");
//     cout << "OK numéro valide\n";
// }
// catch (Utilisateur::Invalid&) {
//     cout << "Numéro invalide\n";
// }

// }