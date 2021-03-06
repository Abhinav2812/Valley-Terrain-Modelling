#pragma once
struct point{
    double x,y,z;
};
#define INF 1e2
point origin = {0,0,0};
point camera = {0,0,10};
point dirn = {0,0,0};
point bgcolor = {0,0,1};
point points[12] = {
    {-1.0,-1.0,1.0},
    {-1.0,1.0,1.0},
    {1.0,1.0,1.0},
    {1.0,-1.0,1.0},
    {-1.0,-1.0,-1.0},
    {-1.0,1.0,-1.0},
    {1.0,1.0,-1.0},
    {1.0,-1.0,-1.0},
    {-INF,-INF,-2},
    {-INF,INF,-2},
    {INF,INF,-2},
    {INF,-INF,-2},
};
float color[12][4] = {
    {1.0,1.0,1.0,1.},
    {1.0,0.0,1.0,1.},
    {1.0,1.0,0.0,1.},
    {0.0,1.0,1.0,1.},
    {0.0,0.0,1.0,1.},
    {0.0,1.0,0.0,1.},
    {1.0,0.0,0.0,1.},
    {0.0,0.0,0.0,1.},
    {0.0,1.0,0.0,1.},
    {0.0,1.0,0.0,1.},
    {0.0,1.0,0.0,1.},
    {0.0,1.0,0.0,1.},
};
