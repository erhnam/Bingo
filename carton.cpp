#include <iostream>
#include <cstdio>
#include <cstdlib>
#include "carton.hpp"
#include <time.h>


//Funcion que genera el carton.
void Carton::generarEspaciosVacios(){
	
	//semilla para el aleatorio
	srand(time(NULL)); 

	//variables
	int i,j,k,n,x;
	int vacios[getTamVacios()];
	int ** carton=getCarton();
	int salen[4];

	//Se genera vector con los 9 columnas
	for (i=0;i<getTamVacios();i++){
		vacios[i]=i;
	}

	//Generar los huecos vacios.
	for(i=0;i<3;i++){
		for(j=0;j<4;j++){
			//genena un numero aleatorio para crear huecos vacios
			x=rand() % getTamVacios();
			//asigna el valor del vector numeros respecto del numero salido			
			n=vacios[x];			
			//quitamos el valor del vector
			for (k=x;k<getTamVacios();k++){
				vacios[k]=vacios[k+1];
			}
			//reducimos el tamaño del vector.			
			setTamVacios(getTamVacios()-1);
			//asignamos espacio vacio al carton
			carton[i][n]=-1;
			salen[j]=n;
		}
		//Restauramos los valores para la siguiente fila
		setTamVacios(numCol);
		for (j=0;j<getTamVacios();j++){
			vacios[j]=j;
		}		
	}
	
	//Creamos el carton con los -1
	setCarton(carton);

}

void Carton::asignarNumerosCarton(){
	
	int i,j,k,n,aux;
	int * columnas=getColumnas();
	int ** carton=getCarton();

	//Asignamos numeros al carton evitando los espacios vacios.
	for(k=0;k<numCol;k++){	
		
		i=0;
		
		//Creamos los numeros por columnas almacenados en un vector
		while(i!=3){		
			n=1+rand() % 10 + (k*10);

			if (i==0){
				columnas[i]=n;
				i++;	
			}

			if (i==1 && n != columnas[i-1]){
				columnas[i]=n;
				i++;	
			}
			
			if (i==2 && n != columnas[i-1] && n != columnas[i-2]){
				columnas[i]=n;
				i++;	
			}			
		}

	    //Ordenar vector
		for (i=0;i<3;i++){
			for (j=0;j<2;j++){
		        if (columnas[j] > columnas[j+1]){
		            aux=columnas[j];
		            columnas[j]=columnas[j+1];
		            columnas[j+1]=aux;
		        }
	        }
	    }

	    i=0;
	    j=k;

	    //Metemos numeros en carton 
	    while(i<3 && j<9){	
	    	//Si hay un espacio vacio pasa a siguiente fila
		    if(carton[i][j]==-1){
		    	i++;
		    }
		    //Si no hay espacio vacio guarda numero y pasa a siguiente fila
		    else{
				carton[i][k]=columnas[i];	
		  		i++;  	
		  	    }
		}
	}

	setCarton(carton);

}

void Carton::generarCarton(){
	
	srand(time(NULL)); 
	generarEspaciosVacios();
	asignarNumerosCarton();
}

void Carton::imprimeCarton(){

	int i,j;
	char x='X';

	for (i=0;i<numFil;i++){
		for (j=0;j<numCol;j++){
			if(carton[i][j]==-1){
				std::cout << "|  " << x;			
			}
			else{
				std::cout << "|  " << carton[i][j];
			}
		}
		std::cout << "\n" << std::endl;
	}
}
