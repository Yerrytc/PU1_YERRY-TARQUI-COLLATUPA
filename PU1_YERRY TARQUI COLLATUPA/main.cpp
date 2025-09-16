#include <GL/glut.h>
#include <cmath>
#include <vector>
#include <cstdio>
using namespace std;

enum Alg { LINE_DIRECT, LINE_DDA, CIRCLE_PM, ELLIPSE_PM };

struct Figura {
    Alg alg;
    int x1,y1,x2,y2;
    float color[3];
    float grosor;
};

vector<Figura> figuras;

bool showGrid = true;
bool showAxes = true;
int gridSpacing = 50;

float currentColor[3] = {0,0,0};
float currentGrosor = 1;
Alg currentAlg = LINE_DIRECT;

int clickCount = 0;
int xStart, yStart;

void putPixel(int x, int y, float r, float g, float b, float grosor) {
    glColor3f(r,g,b);
    glPointSize(grosor);
    glBegin(GL_POINTS);
    glVertex2i(x,y);
    glEnd();
}

void drawLineDirect(int x1, int y1, int x2, int y2, float r,float g,float b,float grosor) {
    if (x1 == x2) {
        for (int y=min(y1,y2); y<=max(y1,y2); y++)
            putPixel(x1,y,r,g,b,grosor);
        return;
    }
    float m = (float)(y2-y1)/(x2-x1);
    if (fabs(m) <= 1) {
        float b0 = y1 - m*x1;
        for (int x=min(x1,x2); x<=max(x1,x2); x++) {
            int y = round(m*x+b0);
            putPixel(x,y,r,g,b,grosor);
        }
    } else {
        for (int y=min(y1,y2); y<=max(y1,y2); y++) {
            int x = round((y-y1)/m + x1);
            putPixel(x,y,r,g,b,grosor);
        }
    }
}

void drawLineDDA(int x1,int y1,int x2,int y2,float r,float g,float b,float grosor){
    int dx=x2-x1, dy=y2-y1;
    int steps = max(abs(dx),abs(dy));
    float xInc = dx/(float)steps, yInc=dy/(float)steps;
    float x=x1, y=y1;
    for(int i=0;i<=steps;i++){
        putPixel(round(x),round(y),r,g,b,grosor);
        x+=xInc; y+=yInc;
    }
}

void drawCirclePM(int xc,int yc,int r,float cr,float cg,float cb,float grosor){
    int x=0,y=r;
    int p=1-r;
    while(x<=y){
        putPixel(xc+x,yc+y,cr,cg,cb,grosor);
        putPixel(xc-x,yc+y,cr,cg,cb,grosor);
        putPixel(xc+x,yc-y,cr,cg,cb,grosor);
        putPixel(xc-x,yc-y,cr,cg,cb,grosor);
        putPixel(xc+y,yc+x,cr,cg,cb,grosor);
        putPixel(xc-y,yc+x,cr,cg,cb,grosor);
        putPixel(xc+y,yc-x,cr,cg,cb,grosor);
        putPixel(xc-y,yc-x,cr,cg,cb,grosor);
        x++;
        if(p<0) p+=2*x+1;
        else { y--; p+=2*(x-y)+1; }
    }
}

void drawEllipsePM(int xc,int yc,int rx,int ry,float cr,float cg,float cb,float grosor){
    int x=0,y=ry;
    long rx2=rx*rx, ry2=ry*ry;
    long p=ry2-(rx2*ry)+rx2/4;
    long dx=2*ry2*x, dy=2*rx2*y;

    while(dx<dy){
        putPixel(xc+x,yc+y,cr,cg,cb,grosor);
        putPixel(xc-x,yc+y,cr,cg,cb,grosor);
        putPixel(xc+x,yc-y,cr,cg,cb,grosor);
        putPixel(xc-x,yc-y,cr,cg,cb,grosor);
        if(p<0){ x++; dx+=2*ry2; p+=dx+ry2; }
        else{ x++; y--; dx+=2*ry2; dy-=2*rx2; p+=dx-dy+ry2; }
    }

    p=ry2*(x+0.5)*(x+0.5)+rx2*(y-1)*(y-1)-rx2*ry2;
    while(y>=0){
        putPixel(xc+x,yc+y,cr,cg,cb,grosor);
        putPixel(xc-x,yc+y,cr,cg,cb,grosor);
        putPixel(xc+x,yc-y,cr,cg,cb,grosor);
        putPixel(xc-x,yc-y,cr,cg,cb,grosor);
        if(p>0){ y--; dy-=2*rx2; p+=rx2-dy; }
        else{ y--; x++; dx+=2*ry2; dy-=2*rx2; p+=dx-dy+rx2; }
    }
}

void drawAxes(){
    glColor3f(0.7,0.7,0.7);
    glBegin(GL_LINES);
    glVertex2i(0,300);
    glVertex2i(800,300);
    glVertex2i(400,0);
    glVertex2i(400,600);
    glEnd();
}

void drawGrid(){
    glColor3f(0.9,0.9,0.9);
    glBegin(GL_LINES);
    for(int x=0;x<=800;x+=gridSpacing){
        glVertex2i(x,0); glVertex2i(x,600);
    }
    for(int y=0;y<=600;y+=gridSpacing){
        glVertex2i(0,y); glVertex2i(800,y);
    }
    glEnd();
}

void display(){
    glClear(GL_COLOR_BUFFER_BIT);
    if(showGrid) drawGrid();
    if(showAxes) drawAxes();

    for(auto &f: figuras){
        switch(f.alg){
            case LINE_DIRECT: drawLineDirect(f.x1,f.y1,f.x2,f.y2,f.color[0],f.color[1],f.color[2],f.grosor); break;
            case LINE_DDA: drawLineDDA(f.x1,f.y1,f.x2,f.y2,f.color[0],f.color[1],f.color[2],f.grosor); break;
            case CIRCLE_PM:{
                int dx=f.x2-f.x1, dy=f.y2-f.y1;
                int r = (int)round(sqrt(dx*dx+dy*dy));
                drawCirclePM(f.x1,f.y1,r,f.color[0],f.color[1],f.color[2],f.grosor);
            } break;
            case ELLIPSE_PM:{
                int rx=abs(f.x2-f.x1), ry=abs(f.y2-f.y1);
                drawEllipsePM(f.x1,f.y1,rx,ry,f.color[0],f.color[1],f.color[2],f.grosor);
            } break;
        }
    }
    glFlush();
}

void mouse(int button,int state,int x,int y){
    if(button==GLUT_LEFT_BUTTON && state==GLUT_DOWN){
        if(clickCount==0){ xStart=x; yStart=600-y; clickCount=1; }
        else{
            Figura f;
            f.alg=currentAlg;
            f.x1=xStart; f.y1=yStart;
            f.x2=x; f.y2=600-y;
            f.color[0]=currentColor[0];
            f.color[1]=currentColor[1];
            f.color[2]=currentColor[2];
            f.grosor=currentGrosor;
            figuras.push_back(f);
            clickCount=0;
            glutPostRedisplay();
        }
    }
}

void keyboard(unsigned char key,int,int){
    switch(key){
        case 'g': case 'G': showGrid=!showGrid; break;
        case 'e': case 'E': showAxes=!showAxes; break;
        case 'c': case 'C': figuras.clear(); break;
        case 27: exit(0);
    }
    glutPostRedisplay();
}

// ---- Menú ----
void menuAlg(int op){ currentAlg=(Alg)op; }
void menuColor(int op){
    switch(op){
        case 0: currentColor[0]=0; currentColor[1]=0; currentColor[2]=0; break;
        case 1: currentColor[0]=1; currentColor[1]=0; currentColor[2]=0; break;
        case 2: currentColor[0]=0; currentColor[1]=1; currentColor[2]=0; break;
        case 3: currentColor[0]=0; currentColor[1]=0; currentColor[2]=1; break;
    }
}
void menuGrosor(int op){
    switch(op){
        case 1: currentGrosor=1; break;
        case 2: currentGrosor=2; break;
        case 3: currentGrosor=3; break;
        case 5: currentGrosor=5; break;
    }
}
void menuVista(int op){
    switch(op){
        case 0: showGrid=!showGrid; break;
        case 1: showAxes=!showAxes; break;
    }
}
void menuHerr(int op){
    switch(op){
        case 0: figuras.clear(); break;
        case 1: if(!figuras.empty()) figuras.pop_back(); break;
    }
}

void createMenu(){
    int menuDibujo=glutCreateMenu(menuAlg);
    glutAddMenuEntry("Recta (Directo)", LINE_DIRECT);
    glutAddMenuEntry("Recta (DDA)", LINE_DDA);
    glutAddMenuEntry("Circulo (Punto Medio)", CIRCLE_PM);
    glutAddMenuEntry("Elipse (Punto Medio)", ELLIPSE_PM);

    int menuColorId=glutCreateMenu(menuColor);
    glutAddMenuEntry("Negro",0);
    glutAddMenuEntry("Rojo",1);
    glutAddMenuEntry("Verde",2);
    glutAddMenuEntry("Azul",3);

    int menuGrosorId=glutCreateMenu(menuGrosor);
    glutAddMenuEntry("1 px",1);
    glutAddMenuEntry("2 px",2);
    glutAddMenuEntry("3 px",3);
    glutAddMenuEntry("5 px",5);

    int menuVistaId=glutCreateMenu(menuVista);
    glutAddMenuEntry("Mostrar/Ocultar cuadrícula",0);
    glutAddMenuEntry("Mostrar/Ocultar ejes",1);

    int menuHerrId=glutCreateMenu(menuHerr);
    glutAddMenuEntry("Limpiar lienzo",0);
    glutAddMenuEntry("Borrar última figura",1);

    int menu=glutCreateMenu([](int){});
    glutAddSubMenu("Dibujo",menuDibujo);
    glutAddSubMenu("Color",menuColorId);
    glutAddSubMenu("Grosor",menuGrosorId);
    glutAddSubMenu("Vista",menuVistaId);
    glutAddSubMenu("Herramientas",menuHerrId);

    glutAttachMenu(GLUT_RIGHT_BUTTON);
}

void init(){
    glClearColor(1,1,1,1);
    gluOrtho2D(0,800,0,600);
}

int main(int argc,char**argv){
    glutInit(&argc,argv);
    glutInitDisplayMode(GLUT_SINGLE|GLUT_RGB);
    glutInitWindowSize(800,600);
    glutCreateWindow("Commit 5 - Version Final");
    init();
    createMenu();

    glutDisplayFunc(display);
    glutMouseFunc(mouse);
    glutKeyboardFunc(keyboard);
    glutPassiveMotionFunc([](int x,int y){ });

    glutMainLoop();
    return 0;
}
