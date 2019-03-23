#pragma once
#include<bits/stdc++.h>
#include "data.cpp"
const double scale_ratio = 1.1;
const double translate_val = 0.1;
const double theta = M_PI/90.;
void translate(float X, float Y, float Z, bool moveOrigin=false)
{
    for(auto &i:points)
    {
        i.x+=X;
        i.y+=Y;
        i.z+=Z;
    }
    if(moveOrigin)
    {
        origin.x+=X;
        origin.y+=Y;
        origin.z+=Z;
    }
}
void scale(float X, float Y, float Z)
{
    for(auto &i:points)
    {
        i.x *= X;
        i.y *= Y;
        i.z *= Z;
    }
}
void rotateX(float theta, bool moveorigin=false)
{
    point avg = {0,0,0};
    for(auto &i:points)
    {
        point old;
        old.y=i.y;
        old.z=i.z;
        i.y = old.y*cos(theta)-old.z*sin(theta);
        i.z = old.y*sin(theta)+old.z*cos(theta);
        avg.y+=i.y;
        avg.z+=i.z;
    }
    avg.y/=sizeof(points)/sizeof(point);
    avg.z/=sizeof(points)/sizeof(point);
    if(moveorigin)
    {
        point old;
        old.y=origin.y;
        old.z=origin.z;
        origin.y = old.y*cos(theta)-old.z*sin(theta);
        origin.z = old.y*sin(theta)+old.z*cos(theta);
    }
}
void rotateY(float theta, bool moveOrigin=false)
{
    point avg = {0,0,0};
    for(auto &i:points)
    {
        point old;
        old.x=i.x;
        old.z=i.z;
        i.z = old.z*cos(theta)-old.x*sin(theta);
        i.x = old.z*sin(theta)+old.x*cos(theta);
        avg.x+=i.x;
        avg.z+=i.z;
    }
    avg.x/=sizeof(points)/sizeof(point);
    avg.z/=sizeof(points)/sizeof(point);
    if(moveOrigin)
    {
        point old;
        old.x=origin.x;
        old.z=origin.z;
        origin.z = old.z*cos(theta)-old.x*sin(theta);
        origin.x = old.z*sin(theta)+old.x*cos(theta);
    }
}
void rotateZ(float theta)
{
    for(auto &i:points)
    {
        point old;
        old.x=i.x;
        old.y=i.y;
        i.x = old.x*cos(theta)-old.y*sin(theta);
        i.y = old.x*sin(theta)+old.y*cos(theta);
    }
}
void rotate(float A, float B, float C, float phi)
{
    float L = sqrt(A*A+B*B+C*C), K = sqrt(B*B+C*C);
    rotateX(acos(C/K));
    rotateY(acos(K/L));
    rotateZ(phi);
    rotateY(-acos(K/L));
    rotateX(-acos(C/K));
}
void reflectXZ()
{
    for(auto &i:points)
    {
        i.y*=-1;
    }   
}
void reflectYZ()
{
    for(auto &i:points)
    {
        i.x*=-1;
    }   
}
void reflectXY()
{
    for(auto &i:points)
    {
        i.z*=-1;
    }   
}
void reflect(float A, float B, float C)
{
    float L = sqrt(A*A+B*B+C*C), K = sqrt(B*B+C*C);
    rotateX(acos(C/K));
    rotateY(acos(K/L));
    reflectXY();
    rotateY(-acos(K/L));
    rotateX(-acos(C/K));
}

void Shear(float scalex, float scaley, float scalez)
{
    for(auto &i:points)
    {
        point old;
        old.x = i.x;
        old.y = i.y;
        old.z = i.z;
        i.x = old.x+scaley*old.y+scalez*old.z;
        i.y = scalex*old.x+old.y+scalez*old.z;
        i.z = scalex*old.x+scaley*old.y+old.z;
    }
}