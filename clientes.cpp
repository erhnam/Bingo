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

using namespace std;

int main(){
  
	/*---------------------------------------------------- 
		Descriptor del socket y buffer de datos                
	-----------------------------------------------------*/
	int sd;
    int salida;
    int fin = 0;
	struct sockaddr_in sockname;
	socklen_t len_sockname;
    fd_set readfds, auxfds;
	char buffer[250];										//Buffer de datos recibidos
    int carton[3][9];											//Carton del jugador
	int lineas[3];											//Marca si se llega a 5 numeros con 0 (1 linea)
	int numLineas=0;										//Lleva la cuenta de lineas que se lleva.
	bool linea1=false;
	bool linea2=false;
	int ** tablero;
	int bola;
	int opcion=0;
	char * nombre=new char [20];
	char * password=new char [20];	
	int numbola=0;	

	//Inicializa Tablero
	tablero= new int *[9];
	for (int i = 0; i < 9; i++){
	    tablero[i] = new int;
	}

	/* --------------------------------------------------
		Se abre el socket 
	---------------------------------------------------*/
  	sd = socket (AF_INET, SOCK_STREAM, 0);
	if (sd == -1){
		perror("No se puede abrir el socket cliente\n");
    		exit (1);	
	}

	/* ------------------------------------------------------------------
		Se rellenan los campos de la estructura con la IP del 
		servidor y el puerto del servicio que solicitamos
	-------------------------------------------------------------------*/
	sockname.sin_family = AF_INET;
	sockname.sin_port = htons(2050);
	sockname.sin_addr.s_addr =  inet_addr("127.0.0.1");

	/* ------------------------------------------------------------------
		Se solicita la conexión con el servidor
	-------------------------------------------------------------------*/
	len_sockname = sizeof(sockname);
	
	if (connect(sd, (struct sockaddr *)&sockname, len_sockname) == -1){
		perror ("Error de conexión\n");
		exit(1);
	}
    
    //Inicializamos las estructuras
    FD_ZERO(&auxfds);
    FD_ZERO(&readfds);
    
    FD_SET(0,&readfds);
    FD_SET(sd,&readfds);

    
	/* ------------------------------------------------------------------
		Se transmite la información
	-------------------------------------------------------------------*/
	do{
        auxfds = readfds;
        salida = select(sd+1,&auxfds,NULL,NULL,NULL);

        //Tengo mensaje desde el servidor
        if(FD_ISSET(sd, &auxfds)){

            bzero(buffer,sizeof(buffer));
            recv(sd,buffer,sizeof(buffer),0);

	    	funcionLog(buffer);

		    //lo que se recibe del sevidor. (otros clientes)
            printf("\n%s\n",buffer);	

            //Se ha recibido el carton almacenado en vector
			if(strncmp(buffer,"CARTON |",5) == 0){

         	   recibeCarton(buffer, carton);

	        }
	
		    if(strncmp(buffer,"NÚMERO-OBTENIDO",5)==0){

		    	numbola++;

		    	//Busca bola
		    	bola=buscaBola(buffer,carton);

		    	//Muestra carton
		    	mostrarCarton(carton);

		    	//Poner bola en el tablero
				//ponerBolaTablero(bola, tablero);

		    	//busca lineas
				numLineas=buscaLinea(buffer, lineas, carton);

				//Comprueba lineas	    	
				//compruebaLineas(numLineas,sd, m, linea1, linea2);
		    }    

		    if(strcmp(buffer,"REINICIAR\n")==0) {

		    	opcion=0;
            	bzero(buffer,sizeof(buffer));
            	system("clear");

			}

		    if(strcmp(buffer,"-ERR. El usuario debe loguearse en el sistema...\n")==0) {

            	system("clear");
	            printf("\n%s\n",buffer);
		    	opcion=0;

			}

		    if(strncmp(buffer,"+Ok. Jugador %s ha cantado una línea\n",12) == 0){
		    	if(linea1==true){
		    		linea2=true;
		    	}
		    	else{
		    		linea1=true;
		    	}
		   	}    

            if(strcmp(buffer,"Demasiados clientes conectados\n") == 0){
                fin=1;
            }
            
            if(strcmp(buffer,"Desconexion servidor\n") == 0){
                fin=1;
            }
            
        }	//Fin del if

     	if(opcion != 4){

			cout << "Elija una opcion"  	 << endl;
			cout << "0 - SALIR" 			 << endl;
			cout << "1 - REGISTRAR USUARIO " << endl;
			cout << "2 - LOGUEAR USUARIO" 	 << endl;
			cout << "3 - PASSWORD " 		 << endl;
			cout << "4 - INICIAR PARTIDA" 	 << endl;
		
			cin >> opcion;

        //He introducido información por teclado
  

			switch(opcion){

				case 0:
						system("clear");
				        strcpy(buffer,"SALIR");
				        send(sd,buffer,sizeof(buffer),0);  					        
				        fin=1;
				break;						

				case 1:
						system("clear");					
						cout << "Inserte un nombre: ";
						cin >> nombre;
						cout << "Inserte un password: ";
						cin >> password;
				        bzero(buffer,sizeof(buffer));							
                        sprintf(buffer,"REGISTRO –u %s -p %s",nombre, password);
				        send(sd,buffer,sizeof(buffer),0);  
				break;

				case 2:
						system("clear");					
						cout << "Inserte usuario: ";
						cin >> nombre;
				        bzero(buffer,sizeof(buffer));							
                        sprintf(buffer,"USUARIO %s",nombre);
				        send(sd,buffer,sizeof(buffer),0); 
				break;			

				case 3:
						system("clear");					
						cout << "Inserte password: ";
						cin >> password;
				        bzero(buffer,sizeof(buffer));							
                        sprintf(buffer,"PASSWORD %s",password);
				        send(sd,buffer,sizeof(buffer),0); 
				break;

				case 4:
						system("clear");					
				        //bzero(buffer,sizeof(buffer));							
                        strcpy(buffer,"INICIAR-PARTIDA");
				        send(sd,buffer,sizeof(buffer),0); 
				break;

				default:
						cout << "Elija una opción correcta." << endl;

			}	//Fin del switch

		}	//Fin del if

		//Se introduce informacion por el teclado.
		if(FD_ISSET(0,&auxfds)){

	        bzero(buffer,sizeof(buffer));
	        fgets(buffer,sizeof(buffer),stdin);

	        //Quitamos el \n
	        buffer[strlen(buffer)-1]='\0';
	        
	        if(strcmp(buffer,"SALIR") == 0){
	            fin = 1;
	        }

	        if(strcmp(buffer,"REINICIAR\n") == 0){
	            opcion=0;
            	bzero(buffer,sizeof(buffer));
            	system("clear");	            
	        }
   
	        send(sd,buffer,sizeof(buffer),0);   
	    }	//fin del if


    }while(fin == 0); //Fin del Do{

    close(sd);

	return 0;

}

