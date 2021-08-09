#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <windows.h>
#include "Map.h"

using std::string;
using namespace std;


int main() {
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
    cMap Map;

    if(!Map.readSXF("C:\\SXFCart\\M3833.SXF"))
        return 1;

    Map.setStructs();
    Map.writePassport("C:\\SXFCart\\Passport.txt");
    Map.writeDescriptor("C:\\SXFCart\\Descriptor.txt");
    Map.writeHeaders("C:\\SXFCart\\Header.txt");
    Map.writeMetrics("C:\\SXFCart\\Metrics.txt");
    return 0;
}

