#define GLUT_DISABLE_ATEXIT_HACK

#include <iostream>
#include <GL/glut.h>
#include <vector>
#include <stack>
#include <math.h>
#include <time.h>
#include "grafo.cpp"

using namespace std;

#define KEY_ESC 27

vector<pair<int,int>> lista_puntos, ruta;
grafo graf(1500);
bool i_circle;

void displayGizmo()
{
	glBegin(GL_LINES);
	glColor3d(255,0,0);
	glVertex2d(500, 0);
	glVertex2d(500, 300);

	glColor3d(255,0,0);
	glVertex2d(500, 300);
	glVertex2d(500, 600);

	glColor3d(255, 0, 0);
	glVertex2d(0, 300);
	glVertex2d(500, 300);

	glColor3d(255, 0, 0);
	glVertex2d(500, 300);
	glVertex2d(1000, 300);

	glEnd();
}



void OnMouseClick(int button, int state, int x, int y)
{
  if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
  {

    cout<<"mouse pos: "<<x<<" - "<<y<<endl;
    //ruta=graf.buscar(graf.nodos[0], graf.nodos[430]);
    ruta=graf.buscar(graf.nodos[0], graf.nodos[636]);
    i_circle=true;

  }
  if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
  {
  	

  	if (lista_puntos.size()==2)
  	{
  		true;
  	}
  }
}

void OnMouseMotion(int x, int y)
{
     //opcional 
	 //hacer algo x,z cuando se mueve el mouse
}

void idle(){ // AGREGAR ESTA FUNCION
	glutPostRedisplay();
}

//funcion llamada a cada imagen
void glPaint(void) {

	//El fondo de la escena al color initial
	glClear(GL_COLOR_BUFFER_BIT); //CAMBIO
	glLoadIdentity();
	glOrtho(0.0f,  1000.0f, 0.0f, 600.0f, -1.0f, 1.0f);
    
	//dibujar quadTree (qt->draw())
	//cout<<"awed"<<endl;
	graf.print_points();
	//cout<<"print-pints"<<endl;
	graf.print_node();
	//cout<<"print-pints"<<endl;

	if(i_circle){
		graf.print_ruta(ruta);
	}

	//graf.print(graf.nodos[0]);
	

	//dibuja el gizmo
	displayGizmo();

	//doble buffer, mantener esta instruccion al fin de la funcion
	glutSwapBuffers();
}

void init_GL(void) {
	//Color del fondo de la escena
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f); //(R, G, B, transparencia) en este caso un fondo negro

	//modo projeccion
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
}

//en el caso que la ventana cambie de tamaño
GLvoid window_redraw(GLsizei width, GLsizei height) {
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();


}

GLvoid window_key(unsigned char key, int x, int y) {
	switch (key) {
	case KEY_ESC:
		exit(0);
		break;

	default:
		break;
	}

}

int main(int argc, char** argv)
{
	
	srand(time(NULL));
	
	cout << "grafo" << endl;
	int contador=0;
	for (int y = 20; y <= 600; y=y+20)
	{
		for (int x = 20; x <= 1000; x=x+20)
		{
			graf.insert_nodo(x,y,contador);
			contador++;
		}
	}
	
	for (int y = 0; y < 29; ++y)
	{
		for (int x = 0; x < 49; ++x)
		{
			graf.insert_edge(graf.nodos[(50*y)+x]->pos,graf.nodos[((50*y)+x)+1]->pos,rand()%100);
			graf.insert_edge(graf.nodos[(50*y)+x]->pos,graf.nodos[(50*(y+1))+x]->pos,rand()%100);
			graf.insert_edge(graf.nodos[(50*y)+x]->pos,graf.nodos[(50*(y+1))+x+1]->pos,rand()%100);
		}
	}

	int random=0, ps=375;

	for (int i = 0; i < 5; ++i)
	{
		ps=ps+50;
		for (int i = 0; i < 10; ++i)
		{
			//random=rand()%1000;
			for (int j = 0; j < graf.nodos.size(); ++j)
			{
				graf.adyacencia[ps+i][j]=0;
				graf.adyacencia[j][ps+i]=0;
			}
		}
	}
		

	//graf.print();

	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(1000, 600); //tamaño de la ventana
	glutInitWindowPosition(100, 100); //posicion de la ventana
	glutCreateWindow("TP2 bis OpenGL : Bresenham"); //titulo de la ventana

	init_GL(); //funcion de inicializacion de OpenGL
	
	glutDisplayFunc(glPaint);

	glutReshapeFunc(&window_redraw);
	// Callback del teclado
	glutKeyboardFunc(&window_key);
	glutMouseFunc(&OnMouseClick);
	glutMotionFunc(&OnMouseMotion);
	glutIdleFunc(&idle);

	
	//qt = new quadTree();
	glutMainLoop();

	return 0;
}