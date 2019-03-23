#pragma once
#include<fstream>
#include<iostream>
#include<vector>
#include<sstream>
#include<GL/glew.h>
#include<GL/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "data.cpp"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
using namespace std;
extern GLuint programID;
//io funcs
GLuint loadTexture(const string &fileName, GLenum repeatMode = GL_REPEAT)
{
    GLuint texID;
    glGenTextures(1, &texID);
    glBindTexture(GL_TEXTURE_2D, texID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, repeatMode);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, repeatMode);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    int width, height, numChannels;
    unsigned char *data = stbi_load(fileName.c_str(), &width, &height, &numChannels, 0);
    assert(data!=NULL);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        throw TextureLoadError();
    }
    stbi_image_free(data);
    return texID;
}
const string readFile(const string &fileName)
{
    ifstream inFile(fileName.c_str());
    if(!inFile.is_open())
    {
        throw FileOpenError();
    }
    stringstream ss;
    ss << inFile.rdbuf();
    string ret = ss.str();
    inFile.close();
    return ret;
}
GLuint loadShader(const GLenum &type, const string &fileName)
{
    int shaderID = glCreateShader(type);
    string code = readFile(fileName);
    const char* codeptr = code.c_str();
    glShaderSource(shaderID, 1, &codeptr, nullptr);
    glCompileShader(shaderID);
    GLint compileStatus;
    glGetShaderiv(shaderID, GL_COMPILE_STATUS, &compileStatus);
    if(compileStatus == GL_FALSE)
    {
        GLint logSize;
        glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &logSize);
        char* error = (char*)alloca((logSize+1)*sizeof(char));
        error[logSize] = '\0';
        glGetShaderInfoLog(shaderID, logSize, &logSize, error);
        stringstream errorMsg;
        switch(type)
        {
            case GL_VERTEX_SHADER: errorMsg <<"Vertex ";break;
            case GL_FRAGMENT_SHADER: errorMsg <<"Fragment ";break;
            case GL_GEOMETRY_SHADER: errorMsg <<"Geometry ";break;
        }
        errorMsg << "Shader Failed to Compile:-\n" << error << "\n";
        cerr<<errorMsg.str()<<endl;
        throw ShaderLoadError();
    }
    return shaderID;
}
GLuint LoadProgram(const string &vertexShader, const string &fragmentShader)
// Loads program with given shaders attached
{
    GLuint vertexShaderID  = loadShader(GL_VERTEX_SHADER, vertexShader);
    GLuint fragmentShaderID = loadShader(GL_FRAGMENT_SHADER, fragmentShader);
    GLuint programID = glCreateProgram();
    glAttachShader(programID,vertexShaderID);
    glAttachShader(programID,fragmentShaderID);
    glLinkProgram(programID);
    GLint compileStatus;
    glGetProgramiv(programID, GL_LINK_STATUS, &compileStatus);
    if(compileStatus == GL_FALSE)
    {
        GLint logSize;
        glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &logSize);
        char* error = (char*)alloca((logSize+1)*sizeof(char));
        error[logSize] = '\0';
        glGetProgramInfoLog(programID, logSize, &logSize, error);
        stringstream errorMsg;
        errorMsg << "Program Link Failed:-\n" << error << "\n";
        cerr<<errorMsg.str()<<endl;
        throw ProgramLinkError();
    }
    glDeleteShader(vertexShaderID);
    glDeleteShader(fragmentShaderID);
    return programID;
}
//math funcs
void adjustCam()
{
    view = glm::lookAt(camera, camera+dirn, up);
    glUniformMatrix4fv(glGetUniformLocation(programID, "view"), 1, GL_FALSE, glm::value_ptr(view));
}
void changeSize(int w, int h) {
    if (h == 0)
        h = 1;
    float ratio =  w * 1.0 / h;
    glViewport(0, 0, w, h);
    projection = glm::perspective(45.0f, ratio, 0.1f, 100.0f);
    glUniformMatrix4fv(glGetUniformLocation(programID, "proj"), 1, GL_FALSE, glm::value_ptr(projection));
}
std::vector<int> triangulate(const std::vector<int> &vert)
{
    std::vector<int> ans;
    ans.reserve(max<int>(0,vert.size()*3-6));
    for(int i=1;i+1<vert.size();i++)
    {
        ans.emplace_back(vert[0]);
        ans.emplace_back(vert[i]);
        ans.emplace_back(vert[i+1]);
    }
    return ans;
}
float height(float x, float y)
{
    const int mult1 = 1, mult2 = 1;
    return mult1*sin(x)+mult2*cos(y);
}
GLuint generateTerrain(GLuint &VAO, float delta = 0.1, int inf = 10, float texMult = 0.3)
{
    GLuint VBO, EBO;
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glGenVertexArrays(1, &VAO);
    GLuint numVertex = 0;
    vector<vertex> buffer;
    vector<GLuint> ebo;
    vector<GLuint> previdxs,curridxs;
    for(float x=-inf;x<=+inf;x+=delta)
    {
        bool flag = 1;
        int loc = 0;
        for(float y=-inf;y<=+inf;y+=delta)
        {
            buffer.push_back({x,y,height(x,y),0,1,0,0,texMult*(x+inf),texMult*(y+inf)});
            if(previdxs.size())
            {
                if(loc!=0)
                {
                    ebo.push_back(numVertex-1);
                    ebo.push_back(numVertex);
                    ebo.push_back(previdxs[loc]);
                }
                if(loc+1<previdxs.size())
                {
                    ebo.push_back(previdxs[loc]);
                    ebo.push_back(previdxs[loc+1]);
                    ebo.push_back(numVertex);
                }
                loc++;
            }
            curridxs.push_back(numVertex);
            numVertex++;
        }
        swap(curridxs,previdxs);
        curridxs.clear();
    }
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertex)*buffer.size(), buffer.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)*ebo.size(), ebo.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertices[0]), 0);    
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(vertices[0]), (GLvoid*)(sizeof(float)*3));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(vertices[0]), (GLvoid*)(sizeof(float)*7));
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    return ebo.size();   
}
