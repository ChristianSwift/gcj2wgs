/*
 * reference: http://blog.csdn.net/coolypf/article/details/8686588
*/

#include <cmath>

// Krasovsky 1940
//
// a = 6378245.0, 1/f = 298.3
// b = a * (1 - f)
// ee = (a^2 - b^2) / a^2;
const double a = 6378245.0;
const double ee = 0.00669342162296594323;
const double pi = 3.14159265358979324;

double TransformLat(double x, double y)
{
    double result = -100.0 + 2.0*x + 3.0*y + 0.2*y*y + 0.1*x*y + 0.2*sqrt(abs(x));
    result += (20.0*sin(6.0*pi*x) + 20.0*sin(2.0*pi*x))*2.0/3.0;
    result += (20.0*sin(pi*y) + 40.0*sin(pi/3.0*y))*2.0/3.0;
    result += (160.0*sin(pi/12.0*y) + 320.0*sin(pi/30.0*y))*2.0/3.0;
    return result;
}

double TransformLon(double x, double y)
{
    double result = 300.0 + x + 2.0*y + 0.1*x*x + 0.1*x*y + 0.1*sqrt(abs(x));
    result += (20.0*sin(6.0*pi*x) + 20.0*sin(2.0*pi*x))*2.0/3.0;
    result += (20.0*sin(pi*x) + 40.0*sin(pi/3.0*x))*2.0/3.0;
    result += (150.0*sin(pi/12.0*x) + 300.0*sin(pi/30.0*x))*2.0/3.0;
    return result;
}

bool OutOfChina(double lat, double lon)
{
    if (lon < 72.004 || lon > 137.8347)
        return true;
    if (lat < 0.8293 || lat > 55.8271)
        return true;
    return false;
}

// World Geodetic System ===> Mars Geodetic System
// In China only
void WGStoGCJ(double wgslat, double wgslon, double &chnlat, double &chnlon)
{
    if (OutOfChina(wgslat, wgslon))
    {
        chnlat = wgslat;
        chnlon = wgslon;
        return;
    }

    double lat = TransformLat(wgslon - 105.0, wgslat - 35.0);
    double lon = TransformLon(wgslon - 105.0, wgslat - 35.0);
    double rad_lat = pi / 180.0 * wgslat;
    double magic = sin(rad_lat);
    magic = 1 - ee*magic*magic;
    double sqrt_magic = sqrt(magic);
    lat = (180.0*lat) / (pi * (a*(1 - ee)) / (magic*sqrt_magic));
    lon = (180.0*lon) / (pi * a * cos(rad_lat) / sqrt_magic);
    chnlat = wgslat + lat;
    chnlon = wgslon + lon;
}

// Assume the coordinates that very close has similar offset
void GCJtoWGS(double chnlat, double chnlon, double &wgslat, double &wgslon)
{
    double lat;
    double lon;
    WGStoGCJ(chnlat, chnlon, lat, lon);
    wgslat = chnlat - (lat - chnlat);
    wgslon = chnlon - (lon - chnlon);
}