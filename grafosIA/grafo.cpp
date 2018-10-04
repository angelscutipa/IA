#define GLUT_DISABLE_ATEXIT_HACK

#include <iostream>
#include <GL/glut.h>
#include <vector>
#include <stack>
#include <math.h>
#include <time.h>

using namespace std;

void displayGizmo2(int xi, int xf, int yi, int yf){
	glBegin(GL_LINES);
	glColor3d(0,255,0);
	glVertex2d(xi, yi);
	glVertex2d(xf, yf);
	glEnd();
}

void displayGizmo3(int xi, int xf, int yi, int yf){
	glBegin(GL_LINES);
	glColor3d(150,224,242);
	glVertex2d(xi, yi);
	glVertex2d(xf, yf);

	glEnd();
}

void displayGizmoP(int x, int y){
	glBegin(GL_POINTS);
	//glColor3f(0,255,0);
	glColor3f(150,224,242);
    glVertex2d(x,y);
    glEnd();
}



class nodo
{
public:
	int x, y, pos;
	int distancia;
	bool visitado=false;
	nodo(int _x, int _y, int _pos){
		x=_x;
		y=_y;
		pos=_pos;
	}
	void burbuja(vector<nodo*> &m_edges){
		if(m_edges.size()>0){
			for (int i = 0; i < m_edges.size()-1; ++i)
			{
				for (int j = 0; j < m_edges.size()-1; ++j)
				{
					if(m_edges[j]->distancia>m_edges[j+1]->distancia){
						nodo* temp;
						temp=m_edges[j];
						m_edges[j]=m_edges[j+1];
						m_edges[j+1]=temp;
					}
				}
			}
		}
	}
	~nodo(){};
	
};



class grafo
{
public:
	int ctd_nodos;
	vector< nodo* > nodos;
	vector< vector< int > > adyacencia;

	bool is_ruta=false;

	grafo(int cant);

	void insert_nodo(int x,int y,int pos);
	void insert_edge(int pos1, int pos2, int peso);
	vector< pair <int, int> > buscar(nodo *a, nodo *b);
	void visit_nodo(nodo *p, stack< nodo* > &ruta, vector< pair <int,int> > &camino );
	void print();
	void print_points();
	void print_node();
	void print_ruta(vector< pair<int,int> > ruta);

	~grafo(){};

};

grafo::grafo(int cant){
	ctd_nodos=cant;

	for (int i = 0; i < ctd_nodos; ++i)
	{
		vector<int> angel;
		for (int j = 0; j < ctd_nodos; ++j)
		{
			angel.push_back(0);
		}
		adyacencia.push_back(angel);
	}

}

void grafo::insert_nodo(int x, int y, int pos){
	nodo* temp;
	temp=new nodo(x,y,pos);
	nodos.push_back(temp);
}

void grafo::insert_edge(int pos1, int pos2, int peso){
	adyacencia[pos1][pos2]=peso;
	adyacencia[pos2][pos1]=peso;
}


void grafo::visit_nodo(nodo *p, stack< nodo* > &ruta, vector< pair <int,int> > &camino ){
	if (p!=NULL || (*p).visitado==false)
	{
		(*p).visitado=true;
		cout<<(*p).x<<"   "<<(*p).y<<endl;
		pair<int,int> a;
		a.first=(*p).x;
		a.second=(*p).y;
		camino.push_back(a);

		if ((p) && (*p).distancia==0)
		{
			//cout<<"hola que tal  omo" <<endl;
			is_ruta=true;
		}
		
		//int s;cin>>s;
		stack< nodo* > actuales;

		vector<nodo*> hijos;


		for (int i = 0; i < ctd_nodos; ++i)
		{
			if (adyacencia[(*p).pos][i]>0)
			{
				hijos.push_back(nodos[i]);
			}
		}

		(*p).burbuja(hijos);

		for (int i = hijos.size()-1; i >= 0 ; --i)
		{
			actuales.push(hijos[i]);
		}		

		/*if ((*p).m_edges.size()==0)
		{
			ruta.pop();
			return;
		}*/
		while(!actuales.empty()){
			if (is_ruta==true)
			{
				return;
			}
			ruta.push(actuales.top());
			actuales.pop();
			visit_nodo(ruta.top(), ruta, camino);
				
		}
	}
	else return;
}

vector< pair <int, int> > grafo::buscar(nodo *a, nodo *b){
	nodo *temp;
	temp=a;

	vector<float> distancias; 
	float rad, d_a;
	pair<int, int> pos_menor;
	for (int i = 0; i < nodos.size(); ++i)
	{
		rad=sqrt(((((*b).x)-(nodos[i]->x ))*(( (*b).x )-( nodos[i]->x  )) ) + ((( (*b).y )-( nodos[i]->y ))*(( (*b).y )-( nodos[i]->y )) ));
		nodos[i]->distancia=rad;
		//distancias.push_back(rad);
	}
	
	stack< nodo* > ruta;

	vector< pair <int, int> > camino;


	visit_nodo(a, ruta, camino);

	cout<<"sali de buscar ruta"<<endl;

	vector< pair <int, int> > dibujar;


	for (int i = 0; i < ruta.size()-1; ++i)
	{
		cout<<"entre hola que pasa"<<endl;
		cout<<"["<<ruta.top()->x<<" ; "<<ruta.top()->y<<"]  ";
		pair<int,int> nodo;
		nodo.first=ruta.top()->x;
		nodo.second=ruta.top()->y;
		dibujar.push_back(nodo);
		ruta.pop();

	}
	return camino;
}


void grafo::print(){
	
	cout<<nodos.size()<<endl;

	cout<<"   ";

	for (int i = 0; i < nodos.size(); ++i)
	{
		cout<<nodos[i]->pos<<"  ";
	}
	cout<<endl;
	for (int i = 0; i < nodos.size(); ++i)
	{
		cout<<nodos[i]->pos<<": ";
		for (int j = 0; j < nodos.size(); ++j)
		{
			cout<<adyacencia[i][j]<<"  ";
		}
		cout<<endl;
	}
}

void grafo::print_points(){
	for (int i = 0; i < this->nodos.size(); ++i)
	{
		//cout<<nodos[i]->x<<"   "<<nodos[i]->y<<endl;
		displayGizmoP(nodos[i]->x,nodos[i]->y);
		
	}
	//cout<<"-------------------------------------------"<<endl;
}

void grafo::print_node(){
	//cout<<nodos[1]->m_edges.size()<<endl;
	
	for (int j = 0; j < this->nodos.size(); ++j)
	{
		vector<nodo*> hijos;
		
		for (int i = 0; i < ctd_nodos; ++i)
		{
			if (adyacencia[nodos[j]->pos][i]>0)
			{
				hijos.push_back(nodos[i]);
			}
		}
		for(int i=0; i<hijos.size(); i++){
			displayGizmo2((*nodos[j]).x, hijos[i]->x, nodos[j]->y, hijos[i]->y);
		}
		
	}
}
void grafo:: print_ruta(vector< pair<int,int> > ruta){
	for (int i = 0; i < ruta.size()-1; ++i)
	{
		displayGizmo3(ruta[i].first,ruta[i+1].first,ruta[i].second, ruta[i+1].second);
	}
}