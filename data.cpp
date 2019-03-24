#pragma once
#include <glm/glm.hpp>
class FileOpenError{};
class ShaderLoadError{};
class GlewInitError{};
class ProgramLinkError{};
class TextureLoadError{};
struct vertex{
    float pos[3];
    float col[4];
    float normal[3];
    float tex[2];
};
#define INF 1e2
float yaw= -90.f,pitch=0.f;
glm::vec3 camera(0.f,0.f,10.f);
glm::vec3 dirn(0.f,0.f,-1.f);
glm::vec3 up(0.f,1.f,0.f);
glm::mat4 view(1.f), projection(1.f);
glm::vec3 sundirn = glm::normalize(glm::vec3(-1.f,-1.f,-1.f));
// vertex vertices[12] = {
//     {-1.0, 1.0, 1.0,1.0,0.0,1.0,1.0,0.0,0.0},
//     { 1.0, 1.0, 1.0,1.0,1.0,0.0,1.0,1.0,0.0},
//     { 1.0,-1.0, 1.0,0.0,1.0,1.0,1.0,1.0,1.0},
//     {-1.0,-1.0, 1.0,1.0,1.0,1.0,1.0,0.0,1.0},
//     {-1.0,-1.0,-1.0,0.0,0.0,1.0,1.0,0.0,0.0},
//     {-1.0, 1.0,-1.0,0.0,1.0,0.0,1.0,0.0,0.0},
//     { 1.0, 1.0,-1.0,1.0,0.0,0.0,1.0,0.0,0.0},
//     { 1.0,-1.0,-1.0,0.0,0.0,0.0,1.0,0.0,0.0},
//     {-INF,-INF,-2.0,0.0,1.0,0.0,1.0,0.0,0.0},
//     {-INF, INF,-2.0,0.0,1.0,0.0,1.0,0.0,0.0},
//     { INF, INF,-2.0,0.0,1.0,0.0,1.0,0.0,0.0},
//     { INF,-INF,-2.0,0.0,1.0,0.0,1.0,0.0,0.0},
// };
// unsigned int elements[] = {
//     0,1,2,
//     2,0,3,
// };
