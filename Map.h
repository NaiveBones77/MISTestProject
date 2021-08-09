//
// Created by maxso on 05.07.2021.
//

#ifndef MAP_H
#define MAP_H

#include <fstream>
#include "Structs.h"

class cMap
{
private:
    char *m_ptr_charMap;
    rPassport *m_ptr_passport;
    rDescriptor *m_ptr_descriptor;
    rRecord *m_ptr_records;

public:
    cMap();


    short readSXF(char* p_ptr_filepath);            // Прочитать карту в битовом режиме и сохранить в m_ptr_charMap

    void setStructs();					            // Установить значения структур rPassport rDescriptor rRecord

    short writePassport(char *p_ptr_filepath);		// Записать данные паспорта в файл
    short writeDescriptor(char *p_ptr_filepath);    // Записать данные дескриптора в файл
    short writeHeaders(char *p_ptr_filepath);		// Записать данные заголовков в файл
    short writeMetrics(char *p_ptr_filepath);
};

#endif //MAP_H
