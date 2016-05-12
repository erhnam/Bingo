#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include "funcServidor.hpp"
#include "cliente.hpp"
#include "bingo.hpp"
#include <iostream>
#include <vector>

// Apuntador al fichero de log (se utilizará en el ejercicio resumen)
FILE *fLog = NULL;

using namespace std;

//Funcion manejadora de interrupcion
void sacarBolas (int sig) {  

	int j,i;

	alarm(6);
    
    for(i=0;i<c;i++){

	    //Saca bola
	    if(vectorBingos[i].sacarBola() == true){

			//envia bola a los clientes
			sprintf(buffer,"NÚMERO-OBTENIDO %d\n",vectorBingos[i].getBola());
			funcionLog(buffer);                                			
			for(j=0; j<(int)vectorBingos[i].getJugadores().size(); j++){
				send(vectorBingos[i].getJugadores()[j].getId(),buffer,strlen(buffer),0);
			}
		}
		else{
			//Se ignora la señal SIGALRM
            signal (SIGALRM, SIG_IGN);
            bzero(buffer,sizeof(buffer));			
			strcpy(buffer,"REINICIAR\n");
		}
	}    

}

//busqueda, devuelve en que partida de vingo está el jugador.
int buscaPartida(std::vector <Cliente> vector, int id){

	int i=0;

	for (i=0; i<(int)vector.size(); ++i){
		if(vector[i].getId() == id){
			return vector[i].getPartida();
		}
	}

	return -1;

}

//me devuelve la posición de donde se encuentra ese cliente
int busqueda(std::vector <Cliente> vector, int id){

	int i=0;

	for (i=0; i<(int)vector.size();i++){
		if(vector[i].getId() == id){
			return i;
		}
	}

	return -1;
}

//Funcion de salir cliente
void salirClienteBingo(int jug, int partida, int socket){

    vectorBingos[partida].getJugadores();

    int j=0;

    unsigned int tam=vectorBingos[partida].getJugadores().size();

    //sacamos al jugador de la partida.
    for (j=jug; j < tam - 1; j++){
        vectorBingos[partida].getJugadores()[j] = vectorBingos[partida].getJugadores()[j+1];
    }
    //Borramos ultima posicion y reducimos el tamaño
    vectorBingos[partida].getJugadores().pop_back();

    //Enviamos mensaje para que se salga y aparezca de nuevo en el menu
    bzero(buffer, sizeof(buffer));
	strcpy(buffer, "REINICIAR\n");
	funcionLog(buffer);                                
    send(vectorBingos[partida].getJugadores()[jug].getId(),buffer,strlen(buffer),0);

}

//Funcion de salir cliente
void salirCliente(int socket, fd_set * readfds, int * numClientes,  std::vector <Cliente> & arrayClientes){
  
    int j;
    
    //Re-estructurar el array de clientes
    for (j = 0; j < (int)arrayClientes.size() - 1; j++){
        if (arrayClientes[j].getId() == socket){
            break;
        }
    }
    for (; j < (int)(int)arrayClientes.size() - 1; j++){
        arrayClientes[j] = arrayClientes[j+1];
    }
    
    arrayClientes.pop_back();
    
    bzero(buffer,sizeof(buffer));
    sprintf(buffer,"Desconexión del cliente: %d\n",socket);
	funcionLog(buffer);                                

    for(j=0; j< (int)arrayClientes.size(); j++){
        if(arrayClientes[j].getId() != socket){
            send(arrayClientes[j].getId(),buffer,strlen(buffer),0);
        }
    }

        close(socket);
    FD_CLR(socket,readfds);
}

void manejador (int signum){
    printf("\nSe ha recibido la señal sigint\n");
	funcionLog(buffer);                                    
    signal(SIGINT,manejador);
    
    //Implementar lo que se desee realizar cuando ocurra la excepción de ctrl+c en el servidor
}

//Funcion que recoge los datos introducidos por teclado y lo guarda en un cliente
void registrarUsuario(char * datosUsuario, Cliente &cl){

	const char delim[2]=" ";
	char *tmp;
  	char * args[MAX_CADENA];
  	int num=0;

	/* Reservamos memoria para copiar la candena ... pero la memoria justa */
	char *str=new char[20];

	strcpy(str, datosUsuario);

	/* Extraemos la primera palabra */
    tmp=strtok(str, delim);
    
    do{
	    //Guardamos la palabra
		args[num]=tmp;
		num++;
	    
	    /* Extraemos la siguiente palabra */
	    tmp=strtok(NULL, delim);
	    
	}while (tmp!=NULL);

	//Se guardan datos en el cliente
    cl.setUsuario(args[2]);
    cl.setPassword(args[4]);
}

//Funcion que guarda usuario en un fichero
bool guardarUsuario(Cliente &cl){

	FILE * f;
	char usuario[15];
	char password[15];

	f=fopen("bbdd.txt","r");

	while(!feof(f)){

		fscanf(f, "%s %s", usuario, password);

		//se mira si el nombre está repetido
		if(strcmp(cl.getUsuario(),usuario) == 0){
			//usuario ya coincide
			char * n=NULL;
			cl.setUsuario(n);
			cl.setPassword(n);
			return false;
		}
	}

	fclose(f);

	f=fopen("bbdd.txt","a");
		fprintf(f, "%s %s\n", cl.getUsuario(), cl.getPassword());
	fclose(f);

    //Registramos al cliente
    cl.setRegistrado();
    //Usuario registrado satisfactoriamente
    return true;
	
}

//Comprobar Usuario Registrado
bool comprobarUsuario(char * buffer){

	FILE * f;
	char usuario[15];
	char password[15];
	const char delim[2]=" ";
	char *tmp;
  	char * args[MAX_CADENA];
  	int num=0;

	/* Reservamos memoria para copiar la candena ... pero la memoria justa */
	char *str= new char[strlen(buffer)];
	strcpy(str, buffer);

	/* Extraemos la primera palabra */
    tmp=strtok(str, delim);
    
    do{
	    //Guardamos la palabra
		args[num]=tmp;
		num++;
	    
	    /* Extraemos la siguiente palabra */
	    tmp=strtok(NULL, delim);
	    
    }while (tmp!=NULL);

    strcpy(buffer,args[1]);

	f=fopen("bbdd.txt","r");
		
	while(!feof(f)){
		
		fscanf(f, "%s %s", usuario, password);
		if(strcmp(buffer,usuario) == 0){
			return true;
		}
	}

	fclose(f);

	return false;
}


//Comprobar Password del usuario registrado
bool comprobarPassword (char * buffer, char * usuario){

	FILE * f;
	char usu[15];
	char password[15];
	const char delim[2]=" ";
	char *tmp;
  	char * args[MAX_CADENA];
  	int num=0;
	char *str= new char [250];

	strcpy(str, buffer);
	/* Extraemos la primera palabra */
    tmp=strtok(str, delim);

    do{
	    //Guardamos la palabra
		args[num]=tmp;
		num++;
	    /* Extraemos la siguiente palabra */
	    tmp=strtok(NULL, delim);
	    
    }while (tmp!=NULL);

    strcpy(buffer,args[1]);

	f=fopen("bbdd.txt","r");
		
		while(!feof(f)){
			
			fscanf(f, "%s %s", usu, password);
			if(strcmp(buffer,password) == 0 && strcmp(usu,usuario) == 0){
				return true;
			}
		}

	fclose(f);

	return false;
}


//Funcion que comprueba las lineas
void comprobarLinea(int par, int pos,  vector <int> bolas){

	int i,j;
	int ** m;
	int linea1[5];					//Linea 1 del carton
	int linea2[5];					//Linea 2 del carton
	int linea3[5];					//Linea 3 del carton
	int aciertos1=0;
	int aciertos2=0;
	int aciertos3=0;


	//Incializa matriz
	m=new int *[3];
	for (int i = 0; i < 3; i++){
	    m[i] = new int[9];
	}

	//Extrae el carton del jugador correspondiente
	m=vectorBingos[par].getJugadores()[pos].getCarton();

	cout << "\nComprobando linea...\n" << endl;

	//para el log
	char buf[100]="\nComprobando linea...\n";
	funcionLog(buf);                                

	//Muestro Carton
	//vectorBingos[par].getJugadores()[pos].mostrarCarton();


	/***************************************
					LINEA 1
	***************************************/

	//Se dercarta la fila 1 del carton si se ha cantado antes
	if(vectorBingos[par].getJugadores()[pos].getLinea1() == false){
		//Extrae los numeros de fila 1 que no sean -1
		for(i=0,j=0; i<9 ; i++){
			if(m[0][i] != -1){
				linea1[j]=m[0][i];
				j++;
			}
		}

		//comprueba que los numeros estan
		for (i=0; i<5; i++){
			for(j=0; j<(int)bolas.size(); j++){
				if(linea1[i] == bolas[j]){
					linea1[i]=0;
					aciertos1++;
					//el jugador tiene linea
					if(aciertos1==5){
						vectorBingos[par].getJugadores()[pos].setLinea1(true);						
					}				
				}
			}
		}

	}

	/***************************************
					LINEA 2
	***************************************/

	//Se dercarta la fila 2 del carton si se ha cantado antes

	if(vectorBingos[par].getJugadores()[pos].getLinea2() == false){

		//Extrae los numeros de fila 2 que no sean -1
		for(i=0, j=0; i<9 ; i++){
			if(m[1][i] != -1){
				linea2[j]=m[1][i];
				j++;
			}
		}

		//comprueba que los numeros estan
		for (i=0; i<5; i++){
			for(j=0; j<(int)bolas.size(); j++){
				if(linea2[i] == bolas[j]){
					linea2[i]=0;
					aciertos2++;
					//el jugador tiene linea
					if(aciertos2==5){
						vectorBingos[par].getJugadores()[pos].setLinea2(true);
					}										
				}
			}
		}
	}

	/***************************************
					LINEA 3
	***************************************/

	//Se dercarta la fila 3 del carton si se ha cantado antes
	if(vectorBingos[par].getJugadores()[pos].getLinea3() == false){

		//Extrae los numeros de fila 3 que no sean -1
		for(i=0, j=0; i<9 ; i++){
			if(m[2][i] != -1){
				linea3[j]=m[2][i];
				j++;
			}
		}

		//comprueba que los numeros estan
		for (i=0; i<5; i++){
			for(j=0; j<(int)bolas.size(); j++){
				if(linea3[i] == bolas[j]){
					linea3[i]=0;
					aciertos3++;
					//el jugador tiene linea					
					if(aciertos3==5){
						vectorBingos[par].getJugadores()[pos].setLinea3(true);
					}									
				}
			}
		}
	}


}

// Función auxiliar, escritura de un log
void funcionLog(char *mensaje) {
	
	int resultado;
	char nombreFichero[100];
	char mensajeAEscribir[300];
	time_t t;

	// Abrir el fichero
	sprintf(nombreFichero,"log-servidor.txt");
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
