#include <iostream>
#include "hopfield.cpp"

using namespace std;

matriz matrix;

int main(int argc, char const *argv[])
{
	cout<<"Modelo de Hopfield: Reconocimiento de patrones"<<endl;

	/* *********************************************** 
	Definicion de los patrones de entrenamiento 
	**************************************************/ 
	cout<<"--------------------------------------"<<endl; 
	cout<<"----- Etapa de aprendizaje ---------"<<endl; 
	cout<<"--------------------------------------"<<endl; 
	// Lectura del tamaño de la matrix patron 
	int fil,col; 
	cout<<"Dimension del patron de aprendizaje"<<endl; 
	
	cout<<"Numero de filas: ";	cin>>fil; 
	cout<<"Numero de columnas: ";	cin>>col; 

	cout<<"------------------------------"<<endl; 
	// lectura del numero de patrones de entrenamiento 
	int M; 
	
	cout<<"Numero de patrones de entrenamiento: "; cin>>M; 
	
	cout<<"--------------------------------------"<<endl; 
	cout<<"Va a definir "<<M<<" patrones de entrenamiento de "<<fil<<" filas y "<<col<<" columnas cada uno"<<endl;

	// N --> numero de neuronas de la red: tamaño de los vectores de aprendizaje 
	const int N = fil*col;

	/* Declaracion de la matrix de Patrones P 
	de la forma E[M] = [e1,.., eN] */ 
	int **E = new int*[M];
	for(int i = 0; i < M; i++)
	    E[i] = new int[N];

	// Lectura de los patrones P[i] para i=0 hasta M patrones 
	int i, cont; 
	int f,c; 
	for (i=0;i<M;i++) { 
		cont=0; 
		cout<<"Introduzca un 1 para pixel negro y -1 para pixel blanco"<<endl; 
		cout<<"Patron num. "<<(i+1)<<"]"<<endl; 
		for (f=0;f<fil;f++) {
			for (c=0;c<col;c++){ 
				cout<<"E["<<f<<"]["<<c<<"] = ";cin>>E[i][cont++]; 
			}
		}
	} 
/*
	E[0][0] =-1 ; E[0][1] =1 ; E[0][2] =1 ; E[0][3] = 1; E[0][4] =1 ; E[0][5] =-1 ; 
	E[0][6] = 1; E[0][7] = -1; E[0][8] = -1; E[0][9] = -1; E[0][10] =-1 ; E[0][11] = 1;  
	E[0][12] = 1; E[0][13] = -1; E[0][14] = -1; E[0][15] =-1 ; E[0][16] =-1 ; E[0][17] = 1;  
	E[0][18] = 1; E[0][19] =1 ; E[0][20] = 1; E[0][21] = 1; E[0][22] = 1; E[0][23] = 1;  
	E[0][24] = 1; E[0][25] = -1; E[0][26] =-1 ; E[0][27] = -1; E[0][28] = -1; E[0][29] = 1;  
	E[0][30] = 1; E[0][31] = -1; E[0][32] = -1; E[0][33] = -1; E[0][34] = -1; E[0][35] = 1;  
	E[0][36] = 1; E[0][37] = -1; E[0][38] = -1; E[0][39] = -1; E[0][40] = -1; E[0][41] = 1;

	E[1][0] = 1; E[1][1] = 1; E[1][2] = 1; E[1][3] = 1; E[1][4] = 1; E[1][5] = -1; 
	E[1][6] = 1; E[1][7] = -1; E[1][8] = -1; E[1][9] = -1; E[1][10] =-1 ; E[1][11] = 1;  
	E[1][12] = 1; E[1][13] = -1; E[1][14] = -1; E[1][15] = -1; E[1][16] = -1; E[1][17] = 1;  
	E[1][18] = 1; E[1][19] = 1; E[1][20] = 1; E[1][21] = 1; E[1][22] = 1; E[1][23] = -1;  
	E[1][24] = 1; E[1][25] = -1; E[1][26] = -1; E[1][27] = -1; E[1][28] = -1; E[1][29] = 1;  
	E[1][30] = 1; E[1][31] = -1; E[1][32] = -1; E[1][33] = -1; E[1][34] = -1; E[1][35] = 1;  
	E[1][36] = 1; E[1][37] = 1; E[1][38] = 1; E[1][39] = 1; E[1][40] = 1; E[1][41] = -1; 

	E[2][0] = -1; E[2][1] = 1; E[2][2] = 1; E[2][3] = 1; E[2][4] = 1; E[2][5] = 1; 
	E[2][6] = 1; E[2][7] = -1; E[2][8] = -1; E[2][9] = -1; E[2][10] = -1; E[2][11] = -1;  
	E[2][12] = 1; E[2][13] = -1; E[2][14] = -1; E[2][15] = -1; E[2][16] = -1; E[2][17] = -1;  
	E[2][18] = 1 ; E[2][19] =-1 ; E[2][20] = -1; E[2][21] = -1; E[2][22] = -1; E[2][23] = -1;  
	E[2][24] = 1; E[2][25] = -1; E[2][26] = -1; E[2][27] = -1; E[2][28] = -1; E[2][29] = -1;  
	E[2][30] = 1; E[2][31] =-1 ; E[2][32] =-1 ; E[2][33] =-1 ; E[2][34] =-1 ; E[2][35] = -1;  
	E[2][36] = -1; E[2][37] = 1; E[2][38] = 1; E[2][39] = 1; E[2][40] = 1; E[2][41] = 1; 

	E[3][0] = 1; E[3][1] =1 ; E[3][2] =1 ; E[3][3] =1 ; E[3][4] =-1 ; E[3][5] = -1; 
	E[3][6] = 1; E[3][7] = -1; E[3][8] = -1; E[3][9] = -1; E[3][10] = 1; E[3][11] = -1;  
	E[3][12] = 1; E[3][13] = -1; E[3][14] = -1; E[3][15] =-1 ; E[3][16] = -1; E[3][17] =1 ;  
	E[3][18] = 1; E[3][19] = -1; E[3][20] = -1; E[3][21] = -1; E[3][22] = -1; E[3][23] = 1;  
	E[3][24] = 1; E[3][25] = -1; E[3][26] = -1; E[3][27] = -1; E[3][28] = -1; E[3][29] = 1;  
	E[3][30] = 1; E[3][31] = -1; E[3][32] = -1; E[3][33] = -1; E[3][34] = 1; E[3][35] = -1;  
	E[3][36] = 1; E[3][37] = 1; E[3][38] = 1; E[3][39] =1 ; E[3][40] = -1; E[3][41] =-1 ; 

*/


	cout<<endl;
	for (i=0;i<M;i++) { 
		cont=0; 
		for (f=0;f<fil;f++) {
			for (c=0;c<col;c++){ 
				if(E[i][cont++]==1){
					cout<<"0";
				}
				else cout<<"*";
			}
			cout<<endl;
		}
		cout<<endl;
		
	}


	/* *********************************************** 
	ETAPA DE APRENDIZAJE 
	Algoritmo de Hopfield 
	***********************************************/ 
	// Definicion de la matrix identidad de tamaño N*N 
	int j; 

	int **I = new int*[N];
	for(int i = 0; i < N; i++)
	    I[i] = new int[N];

	for (i=0;i<N;i++) {
		for (j=0;j<N;j++) {
			if (i==j) 
				I[i][j]= 1; 
			else 
				I[i][j] = 0;
		}
	}

	cout<<"aqui"<<endl;

	/******************************************************* 
	Para cada patron calcular la matrix de pesos de acuerdo a 
	la formula W = Sum (TE . E - I) 
	donde Sum es la sumatoria de 
	TE (matrix traspuesta del patron de entrada E) por la 
	entrada E menos la matrix identidad I 
	W es la matrix de pesos de N*N 
	************************************************* */ 
	//int **W = new int[N][N]; 

	int **W = new int*[N];
	for(int i = 0; i < N; i++){
	    W[i] = new int[N];
	}

	cout<<"aqui"<<endl;

	for (i=0;i<M;i++) { 
		// Aplicamos el parentesis de la formula anterior 
		//int **T = new int[N][1]; 

		int **T = new int*[N];
		for(int i = 0; i < N; i++)
		    T[i] = new int[1];
		cout<<"aqui"<<endl;
		//int **Ei = new int[1][N]; 

		int **Ei = new int*[1];
		for(int i = 0; i < 1; i++){
		    Ei[i] = new int[N];
		}
		cout<<"aqui"<<endl;
		for (j=0;j<N;j++) {
			Ei[0][j]= E[i][j]; 
		}

		matrix.traspuesta(Ei,1,N,T);  ////////////// CAMBIADO EL ORDEN DE N y 1
		//int **P = new int[N][N]; 
		cout<<"aqui"<<endl;
		int **P = new int*[N];
		for(int i = 0; i < N; i++){
		    P[i] = new int[N];
		}
		cout<<"aqui"<<endl;
		matrix.producto(T,Ei,N,N,1,P); /////////////////// cambiado el orden de Ei y T
		//int **OP = new int[N][N]; 

		int **OP = new int*[N];
		for(int i = 0; i < N; i++){
		    OP[i] = new int[N];
		}
		cout<<"aqui"<<endl;
		matrix.opuesta(I,N,N,OP); 
		//int **S = new int[N][N]; 

		int **S = new int*[N];
		for(int i = 0; i < N; i++){
		    S[i] = new int[N];
		}
		cout<<"aqui"<<endl;
		matrix.suma(P,OP,N,N,S); 
		matrix.suma(W,S,N,N,W); 
	}

	cout<<"aqui"<<endl;

	/* ********************************************** 
	********** ETAPA DE FUNCIONAMIENTO ************ 
	***********************************************/ 
	cout<<" "<<endl; 
	cout<<"----------------------------------"<<endl; 
	cout<<"----- Fase de Funcionamiento -----"<<endl; 
	cout<<"----------------------------------"<<endl; 
	cout<<"Introduzca una matrix de entrada de "<<fil<<" filas y "<<col<<" columnas"<<endl; 
	cout<<"Introduzca un 1 para pixel negro y -1 para pixel blanco"<<endl; 
	cout<<"Entrada: "<<endl; 
	/* Declaracion de la matrix de entrada Ent 
	de la forma Ent = [e1,.., eN] */ 
	//int **Ent = new int[1][N]; 

	int **Ent = new int*[1];
		for(int i = 0; i < 1; i++)
		    Ent[i] = new int[N];

	// Lectura de la entrada Ent 
	cont=0; 
	for (f=0;f<fil;f++) {
		for (c=0;c<col;c++) {
			cout<<"Ent["<<f<<"]["<<c<<"] = "; cin>>Ent[0][cont++];
		}
	}

	cout<<"Calculando salida "; 
	// Calcula una nueva salida mientras sea distinta de la anterior salida 
	// s(t+1) <> s(t) 
	//int **S = new int[1][N]; 

	int **S = new int*[1];
	for(int i = 0; i < 1; i++)
	    S[i] = new int[N];

	bool igual=false; 
	do { 
		cout<<".."; 
		// Aplicacion de la funcion escalon con desplazamiento 0 
		matrix.producto(Ent,W,1,4,4,S); 
		/* Transformacion de los valores de la salida S a valores discretos 1, -1 
		si S[i,j] < 0 entonces S[i,j]= -1 
		si S[i,j] >= 0 entonces S[i,j] = +1 */ 
		for (j=0;j<N;j++){ 
			if (S[0][j]<0) S[0][j]= -1; 
			else S[0][j]= 1; 
		}
		// Comparacion de las salidas en t y (t+1) 
		if (matrix.iguales(Ent,S,1,4)) 
			igual=true; 
		else 
		// La salida es la nueva entrada 
		for (j=0;j<N;j++) {
			Ent[0][j] = S[0][j]; 
		}
	} while (!igual);

// Impresión de la salida 
	cout<<" "<<endl; 
	cout<<"Salida:"<<endl; 
	for (j=0;j<N;j++) { 
		if ((j % col) == 0){ 
			cout<<""<<endl;
		} 
		cout<<S[0][j]<< " "; 
	} 
} 

