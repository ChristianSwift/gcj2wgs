/*
** By TanChuan
** KML File reference: https://developers.google.com/kml/documentation/
*/

#include <string>
#include <fstream>
#include <vector>
#include <codecvt>
using namespace std;

#include <tchar.h>
#include "wgstochina.h"

static const wstring key_latitude = L"<latitude>";
static const wstring key_longitude = L"<longitude>";
static const wstring key_coordinate = L"<coordinates>";
static const wstring key_coordinate_c = L"</coordinates>";

typedef vector<wstring> vec_wstr;

void CorrectCoordinates(wstring &str, wstring::size_type idx)
{
    double lat = 0.0;
    double lon = 0.0;
    wstring::size_type num1;
    wstring::size_type num2;
    wchar_t new_Line[256] = {0};
    double chnlon = stod(str.substr(idx), &num1);
    double chnlat = stod(str.substr(idx + num1 + 1), &num2);
    GCJtoWGS(chnlat, chnlon, lat, lon);
    swprintf(new_Line, sizeof(new_Line)/sizeof(new_Line[0]), L"%.6f,%.6f", lon, lat);
    str.replace(idx, num1 + num2 + 1, new_Line);
}

/*
** Correct coordinates lines as follows:
** <coordinates>longitude, latitude, altitude</coordinates>
** <coordinates>longitude, latitude, altitude
** </coordinates>
*/
void CorrectCoordinates(vec_wstr &kml)
{
    bool in_coordiantes = false;
    for (vec_wstr::iterator it = kml.begin(); it != kml.end(); ++it)
    {
        wstring::size_type idx = it->find(key_coordinate);
        wstring::size_type idx_c = it->find(key_coordinate_c);
        if(idx != wstring::npos)
            in_coordiantes = true;
        if (in_coordiantes)
        {
            idx = it->find_first_of(L"-123456789");
            if (idx != wstring::npos)
                CorrectCoordinates(*it, idx);
        }
        if(idx_c != wstring::npos)
            in_coordiantes = false;
    }
}

int _tmain(int argc, _TCHAR* argv[])
{
    // support read/write utf-8 file
    const locale uft8_locale = locale(locale(""), new codecvt_utf8<wchar_t>());
    locale::global(uft8_locale);
    for (int i = 1; i < argc; ++i)
    {
        // open a kml file, read all lines in
        wstring filename(argv[i]);
        if (filename.compare(filename.size() - 4, 4, L".kml") != 0)
            continue;
        wifstream kml_file;
        kml_file.open(filename);
        if (!kml_file.is_open())
        {
            wprintf(L"Can not open file %s", argv[i]);
            continue;
        }

        wstring line;
        vec_wstr vkml;
        vkml.reserve(1000);
        while(kml_file.good())
        {
            getline(kml_file, line);
            vkml.push_back(line);
        }
        kml_file.close();

        // correct gcj coordinats to wgs
        CorrectCoordinates(vkml);

        // save corrected kml file
        wofstream converted_file;
        converted_file.open(filename.insert(filename.length() - 4, L"_xxx"));
        if (converted_file.is_open())
        {
            vec_wstr::iterator i = vkml.begin();
            for (; i != vkml.end() - 1; ++i)
                converted_file << *i << endl;
            converted_file << *i;
            converted_file.flush();
            converted_file.close();
        }
        vkml.clear();
    }

	return 0;
}