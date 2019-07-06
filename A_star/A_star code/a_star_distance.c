/* 
code for heuristic distance evaluation for A-star

Authors : Lawrence Adu-Gyamfi, Alexis Oger

*/

#include <stdio.h>
#include <math.h>

#define PI 3.14159265
#include "astar.h"

//-------------------------------------------------------------------------
double distance_h, distance_d;
double R = 6371e3; // Earth radius in metres
double val = PI / 180;

// Function to calculate distance btn two nodes using the haversine formula
// Result is in metres
double heuristic(node node1, node node2)
{
    double lat_1 = node1.lat ;
    double lon_1 = node1.lon ;
    double lat_2 = node2.lat ;
    double lon_2 = node2.lon ;

    double delta_lat = (lat_2 - lat_1) * val;
    double delta_lon = (lon_2 - lon_1) * val;

    double a = sin(delta_lat/2) * sin(delta_lat/2) + 
    cos(lat_1* val)*cos(lat_2*val)*sin(delta_lon/2)*sin(delta_lon/2);

    double c = 2 * atan2(sqrt(a), sqrt(1-a));

    distance_h = R * c;

    return distance_h;

}


// Function to calculate the straight distance btn 2 nodes using the equirectangular
// projection and pythagoras theorem. Distance is in metres.
double distance(node node1, node node2)
{
    double lat_1 = node1.lat ;
    double lon_1 = node1.lon ;
    double lat_2 = node2.lat ;
    double lon_2 = node2.lon ;

    double delta_lat = (lat_2 - lat_1) * val;
    double delta_lon = (lon_2 - lon_1) * val;
    double x = delta_lon * (lat_2 + lat_1)*val;
    double y = delta_lat;

    distance_d = R * sqrt(x*x + y*y);

    return distance_d;
}
