#ifndef BINGO_HPP
#define BINGO_HPP

#include <vector>
#include "carton.hpp"
#include "cliente.hpp"
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>

	#define numFilas 9
	#define numColumnas 10
	#define numJugadores 4

using namespace std;	

class Bingo {

	private:

		int bola;							//Variable que recoge el valor de la bola.
		int partida;						//Variable que indica el numero de partida.
		vector <Cliente> jugadores;			//Variable vector que contiene los 4 clientes que estan jugando.
		vector <int> bolaSacada;			//Se almacena las bolas que salen del bombo para poder comprobarlas con el carton mejor que con matriz
		int tam;							//Variable que indica el tamaño del vector.
    	int tamTablero;						//Variable que indica el tamaño del tablero.
		vector <int> bombo;					//Variable vector que contiene todos los numeros del 1-90 para representar el bombo
		int ** tablero;						//Variable para crear matriz del tablero donde se comprueba que la bola ya ha salido antes.
		bool linea1;						//Cuando sea true es que se ha cantado una linea
		bool linea2;						//Cuando sea true es que se ha cantado una linea
		bool bingo;							//Cuando sea true se ha cantado bingo y acaba el juego
		int ** carton;						//Variable donde será almacenado el carton que reciba del servidor.		
		int numLineasCantadas;
	public:

		//Constructor
		Bingo(int numPartida, std::vector <Cliente> &jug){
			partida=numPartida;
			
			for (unsigned int i=0; i<jug.size(); i++){
				jugadores.push_back(jug[i]);
			}
			
			tamTablero=90;
			linea1=false;
			linea2=false;
			bingo=false;
			numLineasCantadas=0;

			//Inicializa Tablero
			tablero= new int *[numFilas];
			for (int i = 0; i < numFilas; i++){
			    tablero[i] = new int[numColumnas];
			}

			//Incializa carton
			carton=new int *[numFil];

				for (int i = 0; i < numFil; i++){
				    carton[i] = new int[numCol];
				}

            //Creamos el bombo
            crearbombo();

            //Creamos e inicializamos el Tablero
            inicializarTablero();				
		}

		//Getters
		inline vector <Cliente> & getJugadores(){return jugadores;}		
		inline int getPartida(){return partida;}
		inline vector <int> & getBombo(){return bombo;}
		inline int ** getTablero(){return tablero;}
		inline int getTamTablero(){return tamTablero;}
		inline int getBola(){return bola;}
		inline bool getLinea1(){return linea1;}
		inline bool getLinea2(){return linea2;}
		inline bool getBingo(){return bingo;}
		inline int ** getCarton(){return carton;}
		inline vector <int> & getBolaSacada(){return bolaSacada;}
		inline int getNumLineasCantadas(){return numLineasCantadas;}

		//Setters
		inline void setJugadores(vector <Cliente> &jug){jugadores=jug;}
		inline void setBola(int b){bola=b;}
		inline void setTamTablero(int t){tamTablero=t;}
		inline void setTablero (int ** t) {tablero=t;}
		inline void setLinea1(bool b){linea1=b;}
		inline void setLinea2(bool b){linea2=b;}
		inline void setBingo(bool b){bingo=b;}
		inline void setCarton(int ** c){carton=c;}
		inline void setBombo (vector <int> &b){bombo=b;}
		inline void setNumLineasCantadas (){numLineasCantadas++;}
		inline void setBolaSacada(vector <int> & bs){bolaSacada=bs;}

		//Funciones
		void crearbombo();
		void inicializarTablero();
		bool sacarBola();
		void limpiarceros();
		void mostrarTablero();
		void ponerBolaTablero();
		void mostrarBombo();
		char * reparteCartones(int numJugador);

};



#endif
