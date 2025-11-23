#include "consultation.h"

Consultation::Consultation(const std::string& notes, const std::chrono::system_clock::time_point& date_consultation)
    : notes(notes), date_consultation(date_consultation) {} 

std::string Consultation::getNotes() const {
    return notes;
}

std::chrono::system_clock::time_point Consultation::getDateConsultation() const {
    return date_consultation;
}

