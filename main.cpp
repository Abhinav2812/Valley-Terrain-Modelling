#include<bits/stdc++.h>
#include<GL/freeglut.h>
#include "transformations.cpp"
#include "data.cpp"
#include "texture.cpp"
using namespace std;
mutex io;
int frame = 0, Time = 0, timebase=0;
const int FPS = 120;
bool fullScreen = true, maxFPS = false, wireframe = false;
void drawPoly(vector<int> indices)
{
    glBegin(GL_QUADS);
    for(auto i:indices)
    {
        glColor4fv(color[i]);
        glVertex3f(points[i].x,points[i].y,points[i].z);
    }
    glEnd();
}
void drawCube()
{
    drawPoly({0,3,2,1});
    drawPoly({2,3,7,6});
    drawPoly({0,4,7,3});
    drawPoly({1,2,6,5});
    drawPoly({4,5,6,7});
    drawPoly({0,1,5,4});
}
void adjustCam()
{
    glLoadIdentity();
    gluLookAt(  camera.x, camera.y, camera.z,
                dirn.x, dirn.y,  dirn.z,
                0.0f, 1.0f,  0.0f);
}
void changeSize(int w, int h) {
    if (h == 0)
        h = 1;
    float ratio =  w * 1.0 / h;
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glViewport(0, 0, w, h);
    gluPerspective(45.0f, ratio, 0.1f, 100.0f);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(  camera.x, camera.y, camera.z,
                0.0f, 0.0f,  0.0f,
                0.0f, 1.0f,  0.0f);
}
void fps()
{
    frame++;
	Time=glutGet(GLUT_ELAPSED_TIME);
    if (Time - timebase > 1000) {
        char s[1000];
		sprintf(s,"Valley Terrain Modelling FPS:%4.2f",
			frame*1000.0/(Time-timebase));
		timebase = Time;
		frame = 0;
        glutSetWindowTitle(s);
	}
}
void timer(int) {
    glutPostRedisplay();
    if(!maxFPS)
        glutTimerFunc(1000/FPS, timer, 0);
}
void display()
{
    glClearColor( bgcolor.x,bgcolor.y,bgcolor.z, 1 );
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    drawCube();
    glutSwapBuffers();
    if(maxFPS)
        glutPostRedisplay();
    fps();
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
        if(!maxFPS)
        {
            timer(0);
        }
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
void special_key(int key, int x, int y)
{

}
void mousetoCenter()
{
    static int centerX = glutGet(GLUT_WINDOW_WIDTH) / 2;
    static int centerY = glutGet(GLUT_WINDOW_HEIGHT) / 2;
    glutWarpPointer(centerX,centerY);
}
void mouseFunc(int button, int state, int x, int y)
{
    switch(button)
    {
        case 3:camera.z-=.1;break; //scroll down
        case 4:camera.z+=.1;break; //scroll up
        // case 5:camera.x-=.1;break; //scroll left
        // case 6:camera.x+=.1;break; //scroll right
        // case GLUT_RIGHT_BUTTON:camera.z++;break;
    }
    // io.lock();
    adjustCam();
    // io.unlock();
}
void mouseMovement(int x, int y) 
{
    static int centerX = glutGet(GLUT_WINDOW_WIDTH) / 2;
    static int centerY = glutGet(GLUT_WINDOW_HEIGHT) / 2;
    static bool warpCall = true;
    if(warpCall)
    {
        warpCall = false;
        return;
    }
    static float lastx = x;
    static float lasty = y;
    lastx = (float)x - lastx;
    lasty = (float)y - lasty;
    // io.lock();
    translate(-camera.x,-camera.y,-camera.z,true);
    if((float)x > lastx)
        rotateY(lastx*0.01,true);
    else
        rotateY((-lastx)*0.01,true);
    if((float)y > lasty)
        rotateX(lasty*0.01,true);
    else 
        rotateX(-lasty*0.01,true);
    translate(camera.x,camera.y,camera.z,true); 
    // io.unlock();
    if(1)
    {
        mousetoCenter();
        warpCall = true;
        x = centerX;
        y = centerY;
    }
    lastx = (float)x;
    lasty = (float)y;
}
int main(int argc, char **argv)
{
    glutInit(&argc,argv);
    glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGBA|GLUT_DEPTH);
    glutCreateWindow("Valley Terrain Modelling");
    glutFullScreen();
    glutSetCursor(GLUT_CURSOR_NONE);
    mousetoCenter();
    glutReshapeFunc(changeSize);
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(special_key);
    glutMotionFunc(mouseMovement);
    glutPassiveMotionFunc(mouseMovement);
    glutMouseFunc(mouseFunc);
    glEnable(GL_DEPTH_TEST);
    timer(0);
    glutMainLoop();
    return 0;
}