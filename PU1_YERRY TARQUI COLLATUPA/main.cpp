#include <GL/glut.h>
#include <cmath>
#include <vector>
using namespace std;

enum Alg { LINE_DIRECT, LINE_DDA, CIRCLE_PM, ELLIPSE_PM };
struct Figura {
    Alg alg;
    int x1,y1,x2,y2;
    float color[3];
    float grosor;
};

vector<Figura> figuras;

void dibujarPunto(int x,int y){
    glBegin(GL_POINTS);
    glVertex2i(x,y);
    glEnd();
}

void lineaDirecta(int xi,int yi,int xf,int yf){
    if(xi==xf){ for(int y=min(yi,yf); y<=max(yi,yf); y++) dibujarPunto(xi,y); return; }
    float m=(float)(yf-yi)/(xf-xi);
    if(abs(m)<=1){
        if(xi>xf){ swap(xi,xf); swap(yi,yf); }
        for(int x=xi;x<=xf;x++) dibujarPunto(x,(int)round(yi+m*(x-xi)));
    } else {
        if(yi>yf){ swap(xi,xf); swap(yi,yf); }
        float im=(float)(xf-xi)/(yf-yi);
        for(int y=yi;y<=yf;y++) dibujarPunto((int)round(xi+im*(y-yi)),y);
    }
}

void lineaDDA(int xi,int yi,int xf,int yf){
    int dx=xf-xi, dy=yf-yi, steps=max(abs(dx),abs(dy));
    float x=xi, y=yi, xinc=dx/(float)steps, yinc=dy/(float)steps;
    for(int i=0;i<=steps;i++){ dibujarPunto((int)round(x),(int)round(y)); x+=xinc; y+=yinc; }
}

void circuloPM(int xc,int yc,int r){
    int x=0,y=r,p=1-r;
    while(x<=y){
        dibujarPunto(xc+x,yc+y); dibujarPunto(xc-x,yc+y);
        dibujarPunto(xc+x,yc-y); dibujarPunto(xc-x,yc-y);
        dibujarPunto(xc+y,yc+x); dibujarPunto(xc-y,yc+x);
        dibujarPunto(xc+y,yc-x); dibujarPunto(xc-y,yc-x);
        x++; p<0?p+=2*x+1:(y--,p+=2*(x-y)+1);
    }
}

void elipsePM(int xc,int yc,int rx,int ry){
    int x=0,y=ry,rx2=rx*rx,ry2=ry*ry;
    int px=0,py=2*rx2*y;
    int p=round(ry2-rx2*ry+0.25*rx2);
    while(px<py){
        dibujarPunto(xc+x,yc+y); dibujarPunto(xc-x,yc+y);
        dibujarPunto(xc+x,yc-y); dibujarPunto(xc-x,yc-y);
        x++; px+=2*ry2; if(p<0)p+=ry2+px; else y--,py-=2*rx2,p+=ry2+px-py;
    }
    p=round(ry2*(x+0.5)*(x+0.5)+rx2*(y-1)*(y-1)-rx2*ry2);
    while(y>=0){
        dibujarPunto(xc+x,yc+y); dibujarPunto(xc-x,yc+y);
        dibujarPunto(xc+x,yc-y); dibujarPunto(xc-x,yc-y);
        y--; py-=2*rx2; if(p>0)p+=rx2-py; else x++,px+=2*ry2,p+=rx2-py+px;
    }
}

void display(){
    glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(0,0,0);
    glPointSize(2.0);
    lineaDirecta(100,100,700,500);
    circuloPM(400,300,100);
    glFlush();
}

void init(){
    glClearColor(1,1,1,1);
    gluOrtho2D(0,800,0,600);
}

int main(int argc,char** argv){
    glutInit(&argc,argv);
    glutInitDisplayMode(GLUT_SINGLE|GLUT_RGB);
    glutInitWindowSize(800,600);
    glutInitWindowPosition(100,100);
    glutCreateWindow("Commit 2 - Algoritmos");
    init();
    glutDisplayFunc(display);
    glutMainLoop();
    return 0;
}
