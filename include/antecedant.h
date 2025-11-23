#ifndef ANTECEDANT_H
#define ANTECEDANT_H
#include <string>
class Antecedant
{
    public:
        Antecedant(const std::string& description, const std::string& date_diagnosis);
        std::string getDescription() const;
        std::string getDateDiagnosis() const;

    private:
        std::string description;
        std::string date_diagnosis;
};
#endif // ANTECEDANT_H