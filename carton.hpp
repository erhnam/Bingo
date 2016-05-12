#ifndef CARTON_HPP
#define CARTON_HPP

	#define numFil 3
	#define numCol 9

class Carton {

	private: 

		int cliente;					//Variable para saber a que cliente le corresponde el carton
		int ** carton;					//Variable para almacenar el carton
		int * columnas;					//variable vector para almacenar los numeros de una columna
		int * vacios;					//variable vector para almacenar los numeros vacios de una fila
		int tamVacios;					//variable para controlar el numero de numeros vacios.

	public:

		//Constructor		
		Carton(){
			tamVacios=numCol;
 			vacios=new int[tamVacios];
		 	columnas=new int[numFil];
		 	//Inicializa Carton
		 	carton=new int *[numFil];
			for (int i = 0; i < numFil; i++){
			    carton[i] = new int[numCol];
			}
		}
		
		//Metodos
		
		//Getters
		inline int getTamVacios(){return tamVacios;}
		inline int * getColumnas(){return columnas;}
		inline int * getVacios(){return vacios;}
		inline int ** getCarton(){return carton;}
		inline int getCliente(){return cliente;}

		//Setters
		inline void setTamVacios(int t){tamVacios=t;}
		inline void setCarton(int ** c){carton=c;}
		inline void setCliente(int c){cliente=c;}
		
		void generarCarton ();
		void generarEspaciosVacios();
		void asignarNumerosCarton();
		void imprimeCarton();



};

//Prototipo de funciones


#endif
