#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include "funcCliente.hpp"

FILE *fLog = NULL;

using namespace std;

//Recibe carton y lo guarda en matriz
void recibeCarton(char * buffer, int  m[3][9]){

	vector <int> list;				//Vector auxiliar para meter los numeros de la cadena							
	char * pch;					
	char str[250];
	int i,j,k=0;

	//Copiamos la cadena a otra auxiliar para trabajar con ella
	strcpy(str,buffer);

	//Convertimos char en int
	pch = strtok (str,"CARTON | ,;");

	while (pch != NULL){
		if(strcmp(pch,"X")==0){
			list.push_back(0);           
		}
		else{
			//cout << "Numero extraido: " << atoi(pch) << endl;    
			list.push_back(atoi(pch));
		}		
		pch = strtok (NULL, "| ,;");        
	}

	//Metemos vector en matriz

	for (i=0;i<3;i++){
		for (j=0;j<9;j++){
			m[i][j]=list[k];
			k++;
		}
	} 
		    

}

//Busca bola y la marca
int buscaBola(char * buffer, int m[3][9]){

	int i,j;
	char  * pch;
	char str[250];
	int bola;

	//Copiamos la cadena a otra auxiliar para trabajar con ella
	strcpy(str,buffer);
	

	//Nos quedamos con el char del numero obtenido
	pch = strtok (str," NÚMERO OBTENIDO - ");
	
	while (pch != NULL){
		//Convertimos char en int
		bola=atoi(pch);    						
		pch = strtok (NULL, " ");        
	}

	//Comprueba bola en carton / marca carton con un 0
	for (i=0; i<3;i++){
		for (j=0; j<9;j++){
			if(bola == m[i][j]){
		   		m[i][j]=-1*(m[i][j]);
				printf("\n\x1B[32mNúmero %d está en el cartón\x1B[0m \n\n", bola);							   		 				
			}
		}
	}

	return bola;

}


//Busca una linea
int buscaLinea(char * buffer, int lineas[], int m[3][9]){

int i,j;
int numLineas=0;

	//Busca una linea	
	for (i=0; i<3; i++){
		//inicializamos vector lineas
		lineas[i]=0;
		for (j=0; j<9; j++){
			//va buscando ceros en la linea
			if(m[i][j] < 0 && lineas[i] < 6){
				lineas[i]=lineas[i]+1;
			}
			//señala una linea porque ha llegado a 5 ceros
			if(lineas[i] == 5){
				numLineas++;
				//Hace que no compruebe mas esa linea
				lineas[i]=6;				
			}
		}
	}

	return numLineas;
	
}

//Comprueba las lineas que se llevan
void compruebaLineas(int numLineas,int sd,int m[3][9], bool &l1, bool &l2){

char buffer[250];

	//se canta primera linea
	if(l1==false && numLineas==1){		
		bzero(buffer,sizeof(buffer));
		strcpy(buffer,"UNA-LINEA");
        send(sd,buffer,sizeof(buffer),0);
	}

	//se canta segunda linea
	else if(l1==true && l2==false && numLineas<2){
		bzero(buffer,sizeof(buffer));
		strcpy(buffer,"DOS-LINEAS");
        send(sd,buffer,sizeof(buffer),0); 
	}
	//Hay bingo
	else if(l1==true && l2==true && numLineas==3){
		cout << "!!! BINGO !!!" << endl;
        bzero(buffer,sizeof(buffer));
		strcpy(buffer,"BINGO");
        send(sd,buffer,sizeof(buffer),0);   
	}

}

//Poner bola en el tablero
void ponerBolaTablero(int bola, int ** tablero){

	int unidades, decenas;

	//Calcular las unidades
	decenas=bola;
	decenas=(decenas/10)*10;
	unidades=(bola-decenas);

	//Insertarmos bola en el tablero

	// Caso de unidades 0 no puedes restarle uno
	if (unidades == 0){
		tablero[(decenas/10)-1][9]=bola;
	}	
	//Caso de unidades mayores de 1 restarle 1 para colocarlo en sitio correcto	
	else{ 
		tablero[decenas/10][unidades-1]=bola;
	}

}


//Mostrar Tablero
void mostrarTablero(int ** tablero){

	int i,j;

	for (i=0;i<9;i++){
		for (j=0;j<10;j++){
			std::cout << "| " << tablero[i][j];
		}
			std::cout << "\n" << std::endl;
	}

}

//Mostrar Carton
void mostrarCarton(int carton[][9]){

	int i,j;

	for (i=0;i<3;i++){
		for (j=0;j<9;j++){
			if(carton[i][j] < 0){
				printf("| \x1B[31m%d \x1B[0m", carton[i][j]*(-1));				
			}
			else if (carton[i][j] == 0){
				printf("| \x1B[34m%c \x1B[0m",'X');				
			}
			else {
				std::cout << "| " << carton[i][j] << " ";
			}
		}
			std::cout << "\n" << std::endl;
	}
}

// Función auxiliar, escritura de un log
void funcionLog(char *mensaje) {
	
	int resultado;
	char nombreFichero[100];
	char mensajeAEscribir[300];
	time_t t;

	// Abrir el fichero
	sprintf(nombreFichero,"log-cliente.txt");
	if(fLog==NULL){
		fLog = fopen(nombreFichero,"at");
		if(fLog==NULL){
			perror("Error abriendo el fichero de log");
			exit(1);
		}
	}

	// Obtener la hora actual
	t = time(NULL);
	struct tm * p = localtime(&t);
	strftime(mensajeAEscribir, 1000, "[%Y-%m-%d, %H:%M:%S]", p);

	// Vamos a incluir la hora y el mensaje que nos pasan
	sprintf(mensajeAEscribir, "%s ==> %s\n", mensajeAEscribir, mensaje);
	
	// Escribir finalmente en el fichero
	resultado = fputs(mensajeAEscribir,fLog);
	if ( resultado < 0)
		perror("Error escribiendo en el fichero de log");

	fclose(fLog);
	fLog=NULL;
}

