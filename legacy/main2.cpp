#include<bits/stdc++.h>
#include<GL/glew.h>
#include<GL/freeglut.h>
#include "transformations.cpp"
#include "data.cpp"
#include "newdata.cpp"
using namespace std;
mutex io;
int frame = 0, Time = 0, timebase=0;
const int FPS = 120;
bool fullScreen = true, maxFPS = false, wireframe = false;
GLuint VBO;
void makeBuffers()
{
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertices[0]), 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(vertices[0]), (GLvoid*)(sizeof(float)*3));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}
void display()
{
    // glClearColor( bgcolor.x,bgcolor.y,bgcolor.z, 1 );
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glutSwapBuffers();
}
void keyboard(unsigned char c, int x, int y)
{
    // io.lock();
    translate(-origin.x,-origin.y,-origin.z);
    switch(c)
    {
        case 'Q':case 'q':exit(0);
        case 'x':scale(1./scale_ratio,1,1);break;
        case 'X':scale(scale_ratio,1,1);break;
        case 'y':scale(1,1./scale_ratio,1);break;
        case 'Y':scale(1,scale_ratio,1);break;
        case 'z':scale(1,1,1./scale_ratio);break;
        case 'Z':scale(1,1,scale_ratio);break;
        case 'w':case 'W':reflectXZ();break;
        case 'e':case 'E':reflectXY();break;
        case 'r':case 'R':reflectYZ();break;
        case 'j':case 'J':Shear(-scale_ratio,0,0);break;
        case 'l':case 'L':Shear(scale_ratio,0,0);break;
        case 'i':case 'I':Shear(0,-scale_ratio,0);break;
        case 'k':case 'K':Shear(0,scale_ratio,0);break;
        case 'o':case 'O':Shear(0,0,-scale_ratio);break;
        case 'm':case 'M':Shear(0,0,scale_ratio);break;
        case ';':translate(translate_val,0,0);break;
        case ':':translate(-translate_val,0,0);break;
        case '\'':translate(0,translate_val,0);break;
        case '\"':translate(0,-translate_val,0);break;
        case '/':translate(0,0,translate_val);break;
        case '?':translate(0,0,-translate_val);break;
        case '6':rotateZ(theta);break;
        case '4':rotateZ(-theta);break;
        case '2':rotateX(theta);break;
        case '8':rotateX(-theta);break;
        case '5':rotateY(theta);break;
        case '0':rotateY(-theta);break;
        case '.':rotate(6,2,0,theta);break;
        case '3':rotate(6,2,0,-theta);break;
        case '1':reflect(6,2,0);break;
        case 'f':case 'F':
        fullScreen = !fullScreen;
        if(fullScreen)
        {
            glutFullScreen();
        }
        else
        {
            glutPositionWindow(0,0);
            glutReshapeWindow(glutGet(GLUT_SCREEN_WIDTH), glutGet(GLUT_SCREEN_HEIGHT));
        }
        break;
        case '`':case '~':
        maxFPS = !maxFPS;
        // if(!maxFPS)
        // {
        //     timer(0);
        // }
        break;
        case ' ':
        wireframe = !wireframe;
        if(wireframe)
        {
            glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
        }
        else
        {
            glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
        }
        break;
    }
    translate(origin.x,origin.y,origin.z);
    // io.unlock();
    // glutPostRedisplay();
}
int main(int argc, char **argv)
{
    glutInit(&argc,argv);
    glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGBA|GLUT_DEPTH);
    glutCreateWindow("Valley Terrain Modelling");
    glutFullScreen();
    glutSetCursor(GLUT_CURSOR_NONE);
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glEnable(GL_DEPTH_TEST);
    glutMainLoop();
    return 0;
}
