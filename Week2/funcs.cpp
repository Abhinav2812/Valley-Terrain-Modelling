#pragma once
#include<fstream>
#include<iostream>
#include<string>
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
    cout<<numChannels<<endl;
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
class Font{
    GLuint texID, VAO, verticesCount, programID;
    int lineHeight, base, scaleW,scaleH;
    struct{
        float x,y,width,height,xoffset,yoffset,xadvance;
    }buffer[256];
    int parseInt(const string &s)
    {
        bool eq = false;
        int val = 0, mult = 1;
        for(auto &i:s)
        {
            if(i=='=')
            {
                eq = true;
            }
            else if(eq)
            {
                if(i=='-')
                    mult = -1;
                else
                    val=val*10+(i-'0');
            }
        }
        return val*mult;
    }
    string parseStr(const string &s)
    {
        string ret;
        bool eq = false;
        for(auto &i:s)
        {
            if(i=='\"')
                eq = true;
            else if(eq)
            {
                ret.push_back(i);
            }
        }
        return ret;
    }
    public:
    Font()
    {
        this->programID = 0;
    }
    void loadFont(const string &fileName)
    {
        if(programID == 0)
            this->programID = LoadProgram("font.vs","font.frag");
        ifstream ifile(fileName);
        string s;
        getline(ifile, s); //ignore the info face...
        ifile>>s;//common
        ifile>>s;//lineHeight=
        lineHeight = parseInt(s);
        ifile>>s;//base=
        base = parseInt(s);
        ifile>>s;//scaleW=
        scaleW = parseInt(s);
        ifile>>s;//scaleH=
        scaleH = parseInt(s);
        ifile>>s>>s;//pages=,packed=,ignored
        ifile>>s>>s>>s;//page,id,file=...
        string texFile = parseStr(s);
        texID = loadTexture(texFile);
        ifile>>s>>s;//chars count=
        int numChars = parseInt(s);
        while(numChars--)
        {
            ifile>>s>>s;//char,id=
            int idx = parseInt(s);
            ifile>>s;//x=
            buffer[idx].x = parseInt(s);
            buffer[idx].x/= 1. * scaleW;
            ifile>>s;//y=
            buffer[idx].y = parseInt(s);
            buffer[idx].y/= 1. * scaleH;
            ifile>>s;//width=
            buffer[idx].width = parseInt(s);
            buffer[idx].width /= 1. * scaleW;            
            ifile>>s;//height=
            buffer[idx].height = parseInt(s);
            buffer[idx].height /= 1. * scaleH;            
            ifile>>s;//xoffset=
            buffer[idx].xoffset = parseInt(s);
            ifile>>s;//yoffset=
            buffer[idx].yoffset = parseInt(s);
            ifile>>s;//xadvance=
            buffer[idx].xadvance = parseInt(s);
            ifile>>s>>s;//page=,chnl=(ignore)
        }
    }
    void drawPrep(const string &s, float startX, float startY, float endX, float endY, float scale, float r=0.,float g=0., float b=0.)
    {
        float cursorX = startX, cursorY = startY;
        vector<vertex> vertices;
        for(auto &i:s)
        {
            if(i=='\n')
            {
                cursorY-=scale*this->lineHeight;
                cursorX=startX;
            }
            else
            {
                if(cursorX+scale*buffer[i].xadvance>endX)
                {
                    cursorY-=scale*this->lineHeight;
                    cursorX=startX;
                }
                float leftX = cursorX+scale*buffer[i].xoffset;
                float upY = cursorY-scale*buffer[i].yoffset;
                float rightX = leftX+scale*scaleW*buffer[i].width;
                float downY = upY-scale*scaleH*buffer[i].height;
                float leftXt = buffer[i].x;
                float upYt = buffer[i].y;
                float rightXt = buffer[i].x+buffer[i].width;
                float downYt = buffer[i].y+buffer[i].height;
                vertices.push_back({leftX,upY,-0.995,r,g,b,0,0,0,1,leftXt,upYt});
                vertices.push_back({rightX,upY,-0.995,r,g,b,0,0,0,1,rightXt,upYt});
                vertices.push_back({leftX,downY,-0.995,r,g,b,0,0,0,1,leftXt,downYt});
                vertices.push_back({leftX,downY,-0.995,r,g,b,0,0,0,1,leftXt,downYt});
                vertices.push_back({rightX,downY,-0.995,r,g,b,0,0,0,1,rightXt,downYt});
                vertices.push_back({rightX,upY,-0.995,r,g,b,0,0,0,1,rightXt,upYt});
                cursorX+=buffer[i].xadvance*scale;
            }
        }
        GLuint VBO;
        glGenBuffers(1,&VBO);
        glGenVertexArrays(1,&VAO);
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertex)*vertices.size(), vertices.data(), GL_STATIC_DRAW);
        this->verticesCount = vertices.size();
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertices[0]), 0);    
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(vertices[0]), (GLvoid*)(sizeof(float)*3));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(vertices[0]), (GLvoid*)(sizeof(float)*7));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(vertices[0]), (GLvoid*)(sizeof(float)*10));
        glEnableVertexAttribArray(3);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
    void drawNow(GLuint prevVert=0)
    {
        glBindTexture(GL_TEXTURE_2D, texID);
        glUseProgram(this->programID);
        glBindVertexArray(this->VAO);
        glDrawArrays(GL_TRIANGLES, 0, this->verticesCount);
        glBindVertexArray(0);
    }
};
//math funcs
void adjustCam()
{
    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    dirn = glm::normalize(front);
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
float getNoise(float x,float y){
    // return 0;
    int seed=1e7+321523;
    srand(seed-x*345-y*1752);
    return (double)(rand()%100 -50)/50.;
}
float getSmoothNoise(float x,float y, float c){
    // return 1;
    float corners =(getNoise(x+c,y+c) + getNoise(x-c,y-c) +getNoise(x-c,y+c)+getNoise(x+c,y-c))/16.;
    float sides = (getNoise(x,y+c) + getNoise(x,y-c) +getNoise(x-c,y)+getNoise(x+c,y))/8.;
    float center = getNoise(x,y)/4.;
    // cerr<<corners+sides+center<<endl;
    // return abs(corners+sides+center);
    return 2.5*(corners+center+sides);
}
float height(float x, float y, float eps=1.f)
{
    // const int mult1 = 1, mult2 = 1;
    // return sin(mult1*x+mult2*y);
    // return getNoise(x,y);
    return getSmoothNoise(x,y,eps);
}
glm::vec3 getNormal(float x, float y, float eps=.1f)
{
    float me = height(x,y,eps), up = height(x,y+eps,eps)-me, right= height(x+eps,y,eps)-me;
    float down = height(x,y-eps,eps)-me, left = height(x-eps,y,eps)-me;
    glm::vec3 first = glm::normalize(glm::vec3(-right,-up,eps));//right*up
    glm::vec3 second = glm::normalize(glm::vec3(left,-up,eps));//up*left
    glm::vec3 third = glm::normalize(glm::vec3(left,down,eps));//left*down
    glm::vec3 fourth = glm::normalize(glm::vec3(-right,down,eps));//down*right
    // glm::vec3 net = glm::normalize((first+second+third+fourth)/4.f);
    glm::vec3 net = glm::normalize(glm::vec3(right-left,up-down,2.f));
    return net;
}
GLuint generateTerrain(GLuint &VAO, float delta = 1, int inf = 20, float texMult = 0.3)
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
            glm::vec3 normal = getNormal(x,y);
            buffer.push_back({x,y,height(x,y),0,1,0,0,normal.x,normal.y,normal.z,texMult*(x+inf),texMult*(y+inf)});
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
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(buffer[0]), 0);    
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(buffer[0]), (GLvoid*)(sizeof(float)*3));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(buffer[0]), (GLvoid*)(sizeof(float)*7));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(buffer[0]), (GLvoid*)(sizeof(float)*10));
    glEnableVertexAttribArray(3);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    return ebo.size();   
}
// GLuint generateCloud()
// {
//     // vector<vertex> 
//     vector<glm::vec3> cloudpos;
//     cloudpos.push_back(glm::vec3());

// }