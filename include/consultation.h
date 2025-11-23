#ifndef CONSULTATION_H
#define CONSULTATION_H
#include <string>
#include <chrono>
class Consultation
{
    public:
        Consultation(const std::string& notes, const std::chrono::system_clock::time_point& date_consultation);
        std::string getNotes() const;
        std::chrono::system_clock::time_point getDateConsultation() const;

    private:
        std::string notes;
        std::chrono::system_clock::time_point date_consultation;
};
#endif // CONSULTATION_H