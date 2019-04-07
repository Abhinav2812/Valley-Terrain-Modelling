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
extern int centerX, centerY;
extern void debugmat(glm::mat4 m);
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
        if(numChannels == 3)
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        }
        else
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
GLuint loadOBJ(const string &fileName, GLuint &VAO)
{
    glm::vec4 color(35.f/255.f,12.f/255.f,27.f/255.f,1.f);
    ifstream inFile(fileName.c_str());
    if(!inFile.is_open())
    {
        throw FileOpenError();
    }
    string line;
    vector<glm::vec3> v,vn;
    vector<glm::vec2> vt;
    vector<vertex> data;
    while(getline(inFile, line))
    {
        stringstream tmp;
        if(line[0]=='#'||line[0]=='o')
            continue;
        tmp<<line;
        glm::vec3 val;
        tmp>>line;
        if(line=="vn"||line=="v")
        {
            tmp>>val.x>>val.y>>val.z;
            if(line=="v")
            {
                v.push_back(val);
            }
            else if(line=="vn")
            {
                vn.push_back(val);
            }
            
        }
        else if(line=="vt")
        {
            tmp>>val.x>>val.y;
            vt.push_back(glm::vec2(val));
        }
        else if(line=="f")
        {
            char c;
            tmp>>val.x>>c>>val.y>>c>>val.z;
            data.push_back({v[val.x-1].x,v[val.x-1].y,v[val.x-1].z,color.x,color.y,color.z,color.w,vn[val.y-1].x,vn[val.y-1].y,vn[val.y-1].z,v[val.z-1].x,v[val.z-1].y});
            tmp>>val.x>>c>>val.y>>c>>val.z;
            data.push_back({v[val.x-1].x,v[val.x-1].y,v[val.x-1].z,color.x,color.y,color.z,color.w,vn[val.y-1].x,vn[val.y-1].y,vn[val.y-1].z,v[val.z-1].x,v[val.z-1].y});
            tmp>>val.x>>c>>val.y>>c>>val.z;
            data.push_back({v[val.x-1].x,v[val.x-1].y,v[val.x-1].z,color.x,color.y,color.z,color.w,vn[val.y-1].x,vn[val.y-1].y,vn[val.y-1].z,v[val.z-1].x,v[val.z-1].y});
        }
    }
    GLuint VBO;
    glGenBuffers(1, &VBO);
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertex)*data.size(), data.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(data[0]), 0);    
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(data[0]), (GLvoid*)(sizeof(float)*3));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(data[0]), (GLvoid*)(sizeof(float)*7));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(data[0]), (GLvoid*)(sizeof(float)*10));
    glEnableVertexAttribArray(3);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    return data.size();
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
    //y=-z,z=y;
    // up.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    // front.y = -sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    // front.z = sin(glm::radians(pitch));
    
    
    dirn = glm::normalize(front);
    // up = glm::normalize(cross(dirn,cross(up,dirn)));
    view = glm::lookAt(camera, camera+dirn, up);
    // view = glm::lookAt(-10.f*sundirn,glm::vec3(0.f,0.f,0.f),glm::vec3(0.f,1.f,1.f));
    glUniformMatrix4fv(glGetUniformLocation(programID, "view"), 1, GL_FALSE, glm::value_ptr(view));
}
void changeSize(int w, int h) {
    if (h == 0)
        h = 1;
    scrheight = h;
    scrwidth = w;
    float ratio =  w * 1.0 / h;
    centerX = w/2;
    centerY = max(1,h/2);
    glViewport(0, 0, w, h);
    projection = glm::perspective(45.0f, ratio, 0.1f, 100.0f);
    // projection = glm::ortho(-INF,+INF,-INF,+INF,.1f,100.f);
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
    return (1.5*corners+center+1.5*sides);
}
float height(float x, float y, float eps=1.f)
{
    const int mult1 = 1, mult2 = 1;
    // return sin(mult1*x+mult2*y);
    // return getNoise(x,y);
    // return 0;
    if(x>0&&x<19&&y>0&&y<19)
        return -2;
    return getSmoothNoise(x,y,eps);
}
glm::vec3 getNormal(float x, float y, float eps=.1f)
{
    float me = height(x,y,eps), up = height(x,y+eps,eps)-me, right= height(x+eps,y,eps)-me;
    float down = height(x,y-eps,eps)-me, left = height(x-eps,y,eps)-me;
    // glm::vec3 first = glm::normalize(glm::vec3(-right,eps,-up));//right*up
    // glm::vec3 second = glm::normalize(glm::vec3(left,-up,eps));//up*left
    // glm::vec3 third = glm::normalize(glm::vec3(left,down,eps));//left*down
    // glm::vec3 fourth = glm::normalize(glm::vec3(-right,down,eps));//down*right
    // glm::vec3 net = glm::normalize((first+second+third+fourth)/4.f);
    glm::vec3 net = glm::normalize(glm::vec3(right-left,2.f,up-down));
    return net;
}
GLuint generateTerrain(GLuint &VAO, float delta = 1, int inf = INF, float texMult = 0.3)
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
            buffer.push_back({x,height(x,y),y,0,1,0,1,normal.x,normal.y,normal.z,texMult*(x+inf),texMult*2*inf-texMult*(y+inf)});
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
GLuint generateWater(GLuint &VAO, float delta = 1, float startx = 0, float endx = 19 , float starty = 0, float endy = 19, float texMult = 0.3)
{
    GLuint VBO, EBO;
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glGenVertexArrays(1, &VAO);
    GLuint numVertex = 0;
    vector<vertex> buffer;
    vector<GLuint> ebo;
    vector<GLuint> previdxs,curridxs;
    for(float x=startx;x<=endx;x+=delta)
    {
        bool flag = 1;
        int loc = 0;
        for(float y=starty;y<=endy;y+=delta)
        {
            glm::vec3 normal = getNormal(x,y);
            buffer.push_back({x,-0.5,y,0,0.3,1,.5,0,1,0,0,0});
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
GLuint setupClouds()
{
    float vertices[20] = {
    -1.50, 1.50, 1.50,0.0,0.0,
     1.50, 1.50, 1.50,1.0,0.0,
     1.50, 1.50,-1.50,1.0,1.0,
    -1.50, 1.50,-1.50,0.0,1.0,
    };
    GLuint VAO, VBO;
    glGenBuffers(1, &VBO);
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float)*5, 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float)*5, (GLvoid*)(sizeof(float)*3));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    return VAO;
}
GLuint drawClouds(GLuint VAO, GLuint programID, GLuint texID)
{
    glUseProgram(programID);
    glBindTexture(GL_TEXTURE_2D, texID);
    glBindVertexArray(VAO);
    glm::mat4 view = glm::lookAt(
        glm::vec3(0.f,0.f,0.f),
        dirn,
        up
    );
    glUniformMatrix4fv(glGetUniformLocation(programID, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(programID, "proj"), 1, GL_FALSE, glm::value_ptr(projection));
    glEnable( GL_BLEND );  
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    for(glm::vec3 vec:cloudpos)
    {
        glm::mat4 model(1.f);
        model = glm::translate(model, vec);
        model = glm::translate(model, glm::vec3(.2f,0.f,.3f));
        model = glm::scale(model, glm::vec3(scaleFac,1.f,scaleFac));
        // cout<<scaleFac<<endl;
        // debugmat(model);
        glUniformMatrix4fv(glGetUniformLocation(programID, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glDrawArrays(GL_QUADS, 0, 4);
    }
    glDisable(GL_BLEND);  
    glBindVertexArray(0);
    glUseProgram(0);
}
GLuint drawSun(GLuint VAO, GLuint programID, GLuint texID)
{
    glUseProgram(programID);
    glBindTexture(GL_TEXTURE_2D, texID);
    glBindVertexArray(VAO);
    glm::mat4 view = glm::lookAt(
        glm::vec3(0.f,0.f,0.f),
        dirn,
        up
    );
    glUniformMatrix4fv(glGetUniformLocation(programID, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(programID, "proj"), 1, GL_FALSE, glm::value_ptr(projection));
    glEnable( GL_BLEND );  
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glm::mat4 model(1.f);
    model = glm::translate(model, -sundirn*2.5f);
    model = glm::scale(model, glm::vec3(scaleFac,1.f,scaleFac));
    // cout<<scaleFac<<endl;
    // debugmat(model);
    glUniformMatrix4fv(glGetUniformLocation(programID, "model"), 1, GL_FALSE, glm::value_ptr(model));
    glDrawArrays(GL_QUADS, 0, 4);
    glDisable(GL_BLEND);  
    glBindVertexArray(0);
    glUseProgram(0);
}
GLuint drawOBJ(GLuint VAO, int numVertices, GLuint programID, GLuint texID, glm::vec3 tr, glm::mat4 model=glm::mat4(1.f), glm::mat4 view=::view, glm::mat4 projection=::projection, bool shadow=false)
{
    glUseProgram(programID);
    if(!shadow)
        glBindTexture(GL_TEXTURE_2D, texID);
    glBindVertexArray(VAO);
    tr.y = height(tr.x,tr.z)-.3f;
    model = glm::translate(model, tr);
    glUniformMatrix4fv(glGetUniformLocation(programID, "model"), 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(glGetUniformLocation(programID, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(programID, "proj"), 1, GL_FALSE, glm::value_ptr(projection));
    glDrawArrays(GL_TRIANGLES, 0, numVertices);
    glBindVertexArray(0);
    glUseProgram(0);
}
void drawTrees(glm::mat4 model=glm::mat4(1.f), glm::mat4 view=::view, glm::mat4 projection=::projection, GLuint objProgramID=::objProgramID, bool shadow=false)
{
    float mult = 0.98;
    drawOBJ(objVAO, objNumVertices, objProgramID, objTexID, glm::vec3(0.f), model, view, projection, shadow);
    drawOBJ(objVAO, objNumVertices, objProgramID, objTexID, glm::vec3(6.2f*mult,0.f,7.5f*mult), model, view, projection, shadow);
    drawOBJ(objVAO, objNumVertices, objProgramID, objTexID, glm::vec3(7.8f*mult,0.f,-5.3f*mult), model, view, projection, shadow);
    // drawOBJ(objVAO, objNumVertices, objProgramID, objTexID, glm::vec3(-3.6f*mult,0.f,-2.8f*mult), model, view, projection, shadow);
    // drawOBJ(objVAO, objNumVertices, objProgramID, objTexID, glm::vec3(13.2f*mult,0.f,3.1f*mult), model, view, projection, shadow);
    // drawOBJ(objVAO, objNumVertices, objProgramID, objTexID, glm::vec3(18.2f*mult,0.f,-4.5f*mult), model, view, projection, shadow);
    drawOBJ(objVAO, objNumVertices, objProgramID, objTexID, glm::vec3(-15.8f*mult,0.f,-15.3f*mult), model, view, projection, shadow);
    drawOBJ(objVAO, objNumVertices, objProgramID, objTexID, glm::vec3(-12.6f*mult,0.f,-12.8f*mult), model, view, projection, shadow);
    drawOBJ(objVAO, objNumVertices, objProgramID, objTexID, glm::vec3(17.2f*mult,0.f,3.1f*mult), model, view, projection, shadow);
}
void drawTerrain()
{
    glUseProgram(programID);
    glUniform1i(glGetUniformLocation(programID,"theTexture"),0);
    glUniform1i(glGetUniformLocation(programID,"shadowTex"),1);
    glUniform1i(glGetUniformLocation(programID,"isWater"),0);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, shadowTexID);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texID);
    glBindVertexArray(terrainVAO);
    // GLuint uid = glGetUniformLocation(programID, "Trans");
    // glUniformMatrix4fv(uid, 1, GL_FALSE, glm::value_ptr(mvp));
    // debugmat(projection);
    // glDrawArrays(GL_TRIANGLES, 0, 3);
    glDrawElements(GL_TRIANGLES, terrainNumVertices, GL_UNSIGNED_INT, 0);
}
void drawWater()
{
    glEnable( GL_BLEND );  
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glUseProgram(programID);
    glUniform1i(glGetUniformLocation(programID,"isWater"),1);
    glUniform1f(glGetUniformLocation(programID,"t"),Time*.002f);
    glUniform3fv(glGetUniformLocation(programID, "viewPos"), 1, glm::value_ptr(camera));
    glBindVertexArray(waterVAO);
    glDrawElements(GL_TRIANGLES, waterNumVertices, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    glDisable(GL_BLEND);
}
int GenShadows(int width=800,int height=800)
{
    GLuint FBO;
    GLuint texID;
    int texWidth = 2000, texHeight = 2000;
    glGenFramebuffers(1, &FBO);
    glGenTextures(1, &texID);
    glBindTexture(GL_TEXTURE_2D, texID);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texWidth, texHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE,NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texID, 0);
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        throw FragmentbufferError();
    }
    glViewport(0,0,texWidth,texHeight);
    glm::mat4 model(1.f);
    glm::mat4 view = glm::lookAt(-10.f*sundirn,glm::vec3(0.f,0.f,0.f),glm::vec3(0.f,0.f,-1.f));
    glm::mat4 projection = glm::ortho(-INF,+INF,-INF,+INF, .1f, 100.f);
    GLuint progID = LoadProgram("vertex.vs","shadowfrag.frag");
    glUseProgram(progID);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    drawTrees(model, view, projection, progID, true);
    glUseProgram(0);
    // glBindTexture(GL_TEXTURE_2D, texID);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDeleteFramebuffers(1, &FBO);
    glClearColor(0.2,0.5,0.8,0.0);
    glClear(GL_COLOR_BUFFER_BIT);
    glViewport(0,0,scrwidth,scrheight);
    return texID;
}
void clampCam(glm::vec3 &cam)
{
    for(int i=0;i<3;i++)
        cam[i] = max(min(cameramax[i],cam[i]),cameramin[i]);
}