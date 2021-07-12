//
// Created by maxso on 05.07.2021.
//

#include <iostream>
#include "Map.h"
#include <Windows.h>

using namespace std;

cMap::cMap():m_ptr_charMap(NULL), m_ptr_passport(NULL), m_ptr_descriptor(NULL),m_ptr_records(NULL){}

short cMap::readSXF(char *p_ptr_filepath)
{
    ifstream fileSXF;
    fileSXF.open(p_ptr_filepath, ios::binary);				    // Проверка на возможность чтения карты
    if (!fileSXF){
        cerr << "Error while opening file\n";
        fileSXF.close();
        return 0;
    }

    fileSXF.seekg(0, ios::end);							    // Нахождение размера карты
    unsigned long endPosition = fileSXF.tellg();
    fileSXF.seekg(0, ios::beg);
    m_ptr_charMap = new char[endPosition];				    // Выделение памяти под всю битовую карту
    fileSXF.read((char*)(m_ptr_charMap), endPosition);	// Сохранение сырой карты в массив
    fileSXF.close();

    cout << "Raw map was read\n";
    return 1;
}
void cMap::setStructs()
{
    // Паспорт в карте записан первым, устанавливаем указатель паспотра m_ptr_passport
    // в начало считанной сырой карты:
    m_ptr_passport = reinterpret_cast<rPassport*>(m_ptr_charMap);  //Считываем первые 400 байт информации
    // Сразу после паспорта записан дескриптор, устанавливаем указатель дескриптора
    // в позицию начала карты + размер паспорта:
    m_ptr_descriptor = reinterpret_cast<rDescriptor*>(m_ptr_charMap + sizeof(rPassport)); // Считываем 52 байта дескриптора после паспорта
    // В дескрипторе хранится информация о количестве записей карты.
    // Выделяем место под записи:
    m_ptr_records = new rRecord[m_ptr_descriptor->recordCount];


    unsigned long usefulCount;	// Количество байт сырых данных карты, которые
    // уже были интерпретированы как полезные данные

    usefulCount = sizeof(rPassport) + sizeof(rDescriptor);  //400+52 байта

    // Цикл по количеству записей в карте
    for(unsigned long i = 0; i < m_ptr_descriptor->recordCount; i ++)
    {
        m_ptr_records[i].header = reinterpret_cast<rHeader*>(m_ptr_charMap + usefulCount);
        m_ptr_records[i].points = (m_ptr_charMap + usefulCount + sizeof(rHeader));

        if (m_ptr_records[i].header->subobjectNumber >= 1 )
        {
            unsigned short* pCount = reinterpret_cast<unsigned short*>
                    (m_ptr_charMap + usefulCount + sizeof(rHeader) + m_ptr_records[i].header->metricLenght + sizeof(short));
        }
        // Увеличваем счетчик интерпретированных байт на размер размер записи
        usefulCount += m_ptr_records[i].header->length;
    }
}
short cMap::writePassport(char *p_ptr_filepath)
{
    ofstream outfile;
    outfile.open(p_ptr_filepath);
    if (!outfile){
        cerr << "Unable to write passport file\n";
        outfile.close();
        return 0;
    }

    outfile <<
            hex << "id = " << m_ptr_passport->id << "\n"
            << dec << "length = " << m_ptr_passport->passportLength << "\n"
            << hex << "edition = " << m_ptr_passport->edition << "\n"
            << dec << "controlSum = " << m_ptr_passport->controlSum << "\n"
            << "date = " << m_ptr_passport->date << "\n"
            << "nomenclature = " << m_ptr_passport->nomenclature << "\n"
            << "scale = " << m_ptr_passport->scale << "\n"
            << "listName = " << m_ptr_passport->name << "\n\n"
            // Info flags
            << "flag_status = "			<< static_cast<unsigned short>(m_ptr_passport->flag_status)			<< "\n"
            << "flag_precision = "		<< static_cast<unsigned short>(m_ptr_passport->flag_projection)		<< "\n"
            << "flag_realCoordinate = "	<< static_cast<unsigned short>(m_ptr_passport->flag_realCoordinate)	<< "\n"
            << "flag_codingType = "		<< static_cast<unsigned short>(m_ptr_passport->flag_codingType)		<< "\n"
            << "flag_generalTable = "	<< static_cast<unsigned short>(m_ptr_passport->flag_generalTable)	<< "\n"
            << "flag_signature = "		<< static_cast<unsigned short>(m_ptr_passport->flag_signature)		<< "\n"
            << "flag_precision = "		<< static_cast<unsigned short>(m_ptr_passport->flag_precision)		<< "\n"
            << "flag_specSort = "		<< static_cast<unsigned short>(m_ptr_passport->flag_specSort)		<< "\n"
            << "flag_reserveFlag = "	<< static_cast<unsigned short>(m_ptr_passport->flag_reserve)		<< "\n\n"

            << "classifier = "			<< m_ptr_passport->classifier			<< "\n\n"
            // Rectangular coordinates of corners
            << "XSouthWestCorner = "	 << m_ptr_passport->XSouthWestCorner	<< "\n"
            << "YSouthWestCorner = "	 << m_ptr_passport->YSouthWestCorner	<< "\n"
            << "XNorthWestCorner = "	 << m_ptr_passport->XNorthWestCorner	<< "\n"
            << "YNorthWestCorner = "	 << m_ptr_passport->YNorthWestCorner	<< "\n"
            << "XNorthEastCorner = "	 << m_ptr_passport->XNorthEastCorner	<< "\n"
            << "YNorthEastCorner = "	 << m_ptr_passport->YNorthEastCorner	<< "\n"
            << "XSouthEastCorner = "	 << m_ptr_passport->XSouthEastCorner	<< "\n"
            << "YSouthEastCorner = "	 << m_ptr_passport->YSouthEastCorner	<< "\n\n"
            // Geodetic coordinates of corners
            << "BSouthWestCorner = "	<< m_ptr_passport->BSouthWestCorner		<< "\n"
            << "LSouthWestCorner = "	<< m_ptr_passport->LSouthWestCorner		<< "\n"
            << "BNorthWestCorner = "	<< m_ptr_passport->BNorthWestCorner		<< "\n"
            << "LNorthWestCorner = "	<< m_ptr_passport->LNorthWestCorner		<< "\n"
            << "BNorthEastCorner = "	<< m_ptr_passport->BNorthEastCorner		<< "\n"
            << "LNorthEastCorner = "	<< m_ptr_passport->LNorthEastCorner		<< "\n"
            << "BSouthEastCorner = "	<< m_ptr_passport->BSouthEastCorner		<< "\n"
            << "LSouthEastCorner = "	<< m_ptr_passport->LSouthEastCorner		<< "\n\n"
            // Mathematical basis of list
            << "ellipsoidType = "		<< static_cast<unsigned short>(m_ptr_passport->ellipsoidType)		<< "\n"
            << "elevationSystem = "		<< static_cast<unsigned short>(m_ptr_passport->elevationSystem)		<< "\n"
            << "materialProjection = " << static_cast<unsigned short>(m_ptr_passport->materialProjection) << "\n"
            << "coordinateSystem = " << static_cast<unsigned short>(m_ptr_passport->coordinateSystem) << "\n"
            << "planeUnit = " << static_cast<unsigned short>(m_ptr_passport->planeUnit) << "\n"
            << "heightUnit = " << static_cast<unsigned short>(m_ptr_passport->heightUnit) << "\n"
            << "frameType = " << static_cast<unsigned short>(m_ptr_passport->frameType) << "\n"
            << "mapType = " << static_cast<unsigned short>(m_ptr_passport->mapType) << "\n\n"
            // Reference data on source material
            << "updateDate = " << m_ptr_passport->updateDate << "\n"
            << "materialKind = " << static_cast<unsigned short>(m_ptr_passport->materialKind) << "\n"
            << "materialType = " << static_cast<unsigned short>(m_ptr_passport->materialType) << "\n"
            << "reserve1 = " << m_ptr_passport->reserve1 << "\n"
            << "magneticDeclination = " << m_ptr_passport->magneticDeclination << "\n"
            << "avgMeridianConv = " << m_ptr_passport->avgMeridianConv << "\n"
            << "magneticChange = " << m_ptr_passport->magneticChange << "\n"
            << "declinationUpdate = " << m_ptr_passport->declinationUpdate << "\n"
            << "reserve2 = " << m_ptr_passport->reserve2 << "\n"
            << "reliefSection = " << m_ptr_passport->reliefSection << "\n"
            << "reserve3 = " << m_ptr_passport->reserve3 << "\n"
            << "deviceResolution = "	<< m_ptr_passport->deviceResolution		<< "\n\n"
            // Location of the frame on the device
            << "X_SW_FrameLoacation = "	<< m_ptr_passport->X_SW_FrameLoacation	<< "\n"
            << "Y_SW_FrameLoacation = "	<< m_ptr_passport->Y_SW_FrameLoacation	<< "\n"
            << "X_NW_FrameLoacation = "	<< m_ptr_passport->X_NW_FrameLoacation	<< "\n"
            << "Y_NW_FrameLoacation = "	<< m_ptr_passport->Y_NW_FrameLoacation	<< "\n"
            << "X_NE_FrameLoacation = "	<< m_ptr_passport->X_NE_FrameLoacation	<< "\n"
            << "Y_NE_FrameLoacation = "	<< m_ptr_passport->Y_NE_FrameLoacation	<< "\n"
            << "X_SE_FrameLoacation = "	<< m_ptr_passport->X_SE_FrameLoacation	<< "\n"
            << "Y_SE_FrameLoacation = "	<< m_ptr_passport->Y_SE_FrameLoacation	<< "\n\n"
            // Reference data on the projection of the source material
            << "firstMainParallel = "	<< m_ptr_passport->firstMainParallel	<< "\n"
            << "secondMainParallel = "	<< m_ptr_passport->secondMainParallel	<< "\n"
            << "axialMeridian = "		<< m_ptr_passport->axialMeridian		<< "\n"
            << "mainPointParallel = "	<< m_ptr_passport->mainPointParallel	<< "\n"
            << "poleLatitude = "		<< m_ptr_passport->poleLatitude			<< "\n"
            << "poleLongitude = "		<< m_ptr_passport->poleLongitude		<< "\n";
    outfile.close();
    return 1;
}

short cMap::writeDescriptor(char* p_ptr_filepath)
{
    ofstream outfile;
    outfile.open(p_ptr_filepath);
    if (!outfile){
        cerr << "Unable to write descriptor file\n";
        outfile.close();
        return 0;
    }

    outfile <<
            hex	<< "id = "				<< m_ptr_descriptor->id					<< "\n"
            <<	dec	<< "length = "		<< m_ptr_descriptor->length				<< "\n"
            << "nomenclature = "		<< m_ptr_descriptor->nomenclature		<< "\n"
            << "recodrCount = "			<< m_ptr_descriptor->recordCount		<< "\n\n"

            << "flag_status = "			<< static_cast<unsigned short>(m_ptr_descriptor->flag_status)			<< "\n"
            << "flag_projection = "		<< static_cast<unsigned short>(m_ptr_descriptor->flag_projection)		<< "\n"
            << "flag_realCoordinate = "	<< static_cast<unsigned short>(m_ptr_descriptor->flag_realCoordinate)	<< "\n"
            << "flag_codingType = "		<< static_cast<unsigned short>(m_ptr_descriptor->flag_codingType)		<< "\n"
            << "flag_generalTable = "	<< static_cast<unsigned short>(m_ptr_descriptor->flag_generalTable)		<< "\n"
            << "flag_signature = "		<< static_cast<unsigned short>(m_ptr_descriptor->flag_signature)		<< "\n"
            << "flag_reserve = "		<< static_cast<unsigned short>(m_ptr_descriptor->flag_reserve)			<< "\n\n"

            << "objectsClassifier = "	<< m_ptr_descriptor->objectsClassifier	<< "\n"
            << "semanticsClassifier = "	<< m_ptr_descriptor->semanticsClassifier<< "\n\n";
    outfile.close();
    return 1;
}

short cMap::writeHeaders(char* p_ptr_filepath)
{

    ofstream outfile;
    outfile.open(p_ptr_filepath);
    if (!outfile){
        cerr << "Unable to write header file\n";
        outfile.close();
        return 0;
    }
    for (int i = 0; i < m_ptr_descriptor->recordCount; i++)
    {
        outfile << "---------------------------------------------"			 		            << "\n"
                << "Record [" 		    << i << "]"								                << "\n"
                << "---------------------------------------------"					            << "\n"
                <<	hex	<< "id = "				<< m_ptr_records[i].header->id					<< "\n"
                <<	dec	<< "length = "			<< m_ptr_records[i].header->length				<< "\n"
                << "metricLenght = "			<< m_ptr_records[i].header->metricLenght		<< "\n"
                << "classificationCode = "		<< m_ptr_records[i].header->classificationCode	<< "\n\n"

                << "numberInGroup = "			<< m_ptr_records[i].header->numberInGroup		<< "\n"
                << "groupNumber = "				<< m_ptr_records[i].header->groupNumber			<< "\n\n"

                << "flag_localization = "		<< m_ptr_records[i].header->flag_localization	<< "\n"
                << "flag_frameExit = "			<< m_ptr_records[i].header->flag_frameExit		<< "\n"
                << "flag_insularity = "			<< m_ptr_records[i].header->flag_insularity		<< "\n"
                << "flag_semantics = "			<< m_ptr_records[i].header->flag_semantics		<< "\n"
                << "flag_metricSize = "			<< m_ptr_records[i].header->flag_metricSize	    << "\n"
                << "flag_bindingVector = "		<< m_ptr_records[i].header->flag_bindingVector	<< "\n"
                << "flag_UNICODE = "			<< m_ptr_records[i].header->flag_UNICODE		<< "\n"
                << "flag_reserve = "			<< m_ptr_records[i].header->flag_reserve		<< "\n\n"

                << "flag_metricFormat = "		<< m_ptr_records[i].header->flag_metricFormat	<< "\n"
                << "flag_dimension = "			<< m_ptr_records[i].header->flag_dimension		<< "\n"
                << "flag_metricType = "		<< m_ptr_records[i].header->flag_metricType	        << "\n"
                << "flag_metricText = "			<< m_ptr_records[i].header->flag_metricText		<< "\n"
                << "flag_sign = "				<< m_ptr_records[i].header->flag_sign			<< "\n"
                << "flag_scalableGraphics = "	<< m_ptr_records[i].header->flag_scalableGraphics<< "\n"
                << "flag_metricSpline = "		<< m_ptr_records[i].header->flag_metricSpline	<< "\n\n"

                << "bottomBorder = "			<< m_ptr_records[i].header->bottomBorder		<< "\n"
                << "topBorder = "				<< m_ptr_records[i].header->topBorder			<< "\n\n"

                << "metricPointsCountBig = "	<< m_ptr_records[i].header->metricPointsCountBig<< "\n"
                << "subobjectNumber = "			<< m_ptr_records[i].header->subobjectNumber		<< "\n"
                << "metricPointsCount = "		<< m_ptr_records[i].header->metricPointsCount	<< "\n\n";
    }
    outfile.close();
    return 1;
}



