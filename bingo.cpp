#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <time.h>
#include "bingo.hpp"

//Crear bombo
void Bingo::crearbombo(){

	unsigned int i=0;

	for (i=0 ;i<90;i++){
		getBombo().push_back(i+1);
	}

}

//Inicializar tablero
void Bingo::inicializarTablero(){

	int i,j;
	int ** tablero=getTablero();

	for (i=0;i<9;i++){
		for (j=0;j<numColumnas;j++){
			tablero[i][j]=0;
		}
	}

	setTablero(tablero);	
}


//Sacar bombo
bool Bingo::sacarBola(){

	srand(time(NULL)); 
    int posicion=0;

	if ((int)getBombo().size() != 0){

		//genena un numero aleatorio
		posicion=rand() % (int)getBombo().size()-1;

		//asigna valor de la bola correspondiente en el bombo
		bola=getBombo()[posicion];

		//seteamos la bola
		setBola(bola);

		//borramos el numero salido
		getBombo().erase(getBombo().begin()+ posicion);

		//Guardamos la bola en el vector para su comprobacion de lineas y bingo y aumentamos el numero de bolas que han salido
		getBolaSacada().push_back(bola);

		return true;		
	}

	else{
		printf("No hay mas bolas\n");
		return false;
	}
}

//Poner bola en el tablero
void Bingo::ponerBolaTablero(){

	int unidades, decenas;
	int ** tablero=getTablero();
	
	//Calcular las unidades
	decenas=getBola();
	decenas=(decenas/10)*10;
	unidades=(getBola()-decenas);

	//Insertarmos bola en el tablero

	// Caso de unidades 0 no puedes restarle uno
	if (unidades == 0){
		tablero[(decenas/10)-1][9]=getBola();
	}	
	//Caso de unidades mayores de 1 restarle 1 para colocarlo en sitio correcto	
	else{ 
		tablero[decenas/10][unidades-1]=getBola();
	}

	setTablero(tablero);

}


//Mostrar Tablero
void Bingo::mostrarTablero(){

	int i,j;

	for (i=0;i<numFilas;i++){
		for (j=0;j<numColumnas;j++){
			std::cout << "| " << tablero[i][j];
		}
			std::cout << "\n" << std::endl;
	}

}

//Mostrar Bombo
void Bingo::mostrarBombo(){

	unsigned int i=0;

	std::cout << "{";
	for (i=0; i<getBombo().size();i++){
		std::cout << getBombo()[i] << ",";		
	}
	std::cout << "}\n" << std::endl;
}

//ReparteCartones
char * Bingo::reparteCartones(int numJugador){

	char * buffer=new char[250];
	char * num=new char[3];	
	int ** m;
	vector <Cliente> j=getJugadores();
	Cliente cl=j[numJugador];

	//Se crea Carton
	Carton c= Carton();
	//Se genera el carton
	c.generarCarton();

	//se mete el carton en el vector jugadores(clientes)
	cl.setCarton(c.getCarton());

	//Se mete jugador en el vector
	j[numJugador]=cl;

	//Seteamos el vector
	setJugadores(j);

	//se obtiene el carton y se pasa a matriz
	m=j[numJugador].getCarton();

	//Comienzo de la cadena
	strcpy(buffer,"CARTON | ");

	//Pasamos a char
	for (int i=0; i<numFil; i++){
		for (int j=0; j<numCol;j++){
			if(m[i][j] != -1){
				sprintf(num, "%d,", m[i][j]);
	 			strcat(buffer,num);
	 		}
	 		else{
	 			strcat(buffer,"X,");	 			
	 		}
		}
		
		buffer[strlen(buffer)-1]='\0';
 		strcat(buffer,";");
	}

	buffer[strlen(buffer)-1]='\0';
 	strcat(buffer,"|");
 	return buffer;

}
