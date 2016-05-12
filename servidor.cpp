#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include "funcServidor.hpp"
#include "bingo.hpp"
#include "carton.hpp"
#include "cliente.hpp"
#include <vector>
#include <unistd.h>
#include <cstdio>
#include <iostream>


std::vector <Bingo> vectorBingos;                           //Vector para llevar control del numero de bingos
std::vector <Cliente> clientesTotales(0);                      //Vector de 4 jugadores standard (se pasará al bingo una vez completado)    
int c=0;													//Es auxiliar de i para poder enviar a la funcion esperarMensaje de forma global        													
char buffer[MSG_SIZE];

/*
 * El servidor ofrece el servicio de un chat
 */

using namespace std;

int main(){
  
	/*---------------------------------------------------- 
		Descriptor del socket y buffer de datos                
	-----------------------------------------------------*/
	int sd, new_sd;
    int salida;
    int numClientes = 0;
    int i,j;
	int recibidos;
    int on, ret;
    struct sockaddr_in sockname, from;
    char identificador[MSG_SIZE];
    socklen_t from_len;
    fd_set readfds, auxfds;
	vector <Cliente> vectorClientes;                           //Vector de 4 jugadores standard (se pasará al bingo una vez completado)
    int jugadores=0;                                           //Controla los 4 clientes por partida.
	int numPartida=0;                                          //Controla el numero de partida de bingo

	/* --------------------------------------------------
		Se abre el socket 
	---------------------------------------------------*/
  	sd = socket (AF_INET, SOCK_STREAM, 0);
	if (sd == -1){
		char error[100]="No se puede abrir el socket cliente\n";
		perror("No se puede abrir el socket cliente\n");
		funcionLog(error);
    	exit (1);	
	}
    
    // Activaremos una propiedad del socket que permitir· que otros
    // sockets puedan reutilizar cualquier puerto al que nos enlacemos.
    // Esto permitir· en protocolos como el TCP, poder ejecutar un
    // mismo programa varias veces seguidas y enlazarlo siempre al
    // mismo puerto. De lo contrario habrÌa que esperar a que el puerto
    // quedase disponible (TIME_WAIT en el caso de TCP)
    on=1;
    ret = setsockopt( sd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

	sockname.sin_family = AF_INET;
	sockname.sin_port = htons(2050);
	sockname.sin_addr.s_addr =  INADDR_ANY;

	if (bind (sd, (struct sockaddr *) &sockname, sizeof (sockname)) == -1){
		char error[100]="Error en la operación bind";
		perror("Error en la operación bind");
		funcionLog(error);		
		exit(1);
	}
	

   	/*---------------------------------------------------------------------
		De las peticiones que vamos a aceptar sólo necesitamos el 
		tamaño de su estructura, el resto de información (familia, puerto, 
		ip), nos la proporcionará el método que recibe las peticiones.
   	----------------------------------------------------------------------*/
	
    from_len = sizeof (from);


	if(listen(sd,1) == -1){
		char error[100]="Error en la operación de listen";
		perror("Error en la operación de listen");
		funcionLog(error);		
		exit(1);
	}
    
    //Inicializar los conjuntos fd_set
    FD_ZERO(&readfds);
    FD_ZERO(&auxfds);
    FD_SET(sd,&readfds);
    FD_SET(0,&readfds);
       	
    //Capturamos la señal SIGINT (Ctrl+c)
    signal(SIGINT,manejador);
    
    //Asociamos la señal SIGALRM con la función finalizar_bucle().
    signal (SIGALRM, sacarBolas);        

	/*-----------------------------------------------------------------------
		El servidor acepta una petición
	------------------------------------------------------------------------ */
	while(1){
        
        //Esperamos recibir mensajes de los clientes (nuevas conexiones o mensajes de los clientes ya conectados)
        
        auxfds = readfds;
        
        salida = select(FD_SETSIZE,&auxfds,NULL,NULL,NULL);
        
        if(salida > 0){            
            
            for(i=0; i<FD_SETSIZE; i++){
                
                //Buscamos el socket por el que se ha establecido la comunicación
                if(FD_ISSET(i, &auxfds)) {
                    
                    if( i == sd){

                        if((new_sd = accept(sd, (struct sockaddr *)&from, &from_len)) == -1){
                        	char error[100]="Error aceptando peticiones";
                            perror("Error aceptando peticiones");
							funcionLog(error);                            
                        }
                        else{                                
                            //El cliente ha sido aceptado para entrar al sistema
                            //Si hay huecos disponibles...
                            if(numClientes < MAX_CLIENTS){
                                //crea un cliente
                                Cliente cl = Cliente();
                                //se le asigna el descriptor
                                cl.setId(new_sd);
                                //inserta en vector de clientes totales del servidor
                                clientesTotales.push_back(cl);                      
                                //inserta el cliente en el vector que será usado para ir llenando las partidas de 4.              
                                numClientes++;

                                FD_SET(new_sd,&readfds);
                                strcpy(buffer, "Bienvenido al BINGO\n");
								funcionLog(buffer);                                
                                send(new_sd,buffer,strlen(buffer),0);
                                for(j=0; j<(numClientes-1);j++){
                                	if(clientesTotales[j].getPartida() == numPartida){
                                    	sprintf(buffer, "Nuevo Cliente conectado: %d\n",new_sd);
										funcionLog(buffer);                                
                                    	send(clientesTotales[j].getId(),buffer,strlen(buffer),0);
                                    }
                                }
                            }
                            else{
                                //No hay huecos disponibles.
                                strcpy(buffer,"Demasiados clientes conectados\n");
								funcionLog(buffer);                                                                
                                send(new_sd,buffer,strlen(buffer),0);
                                close(new_sd);
                            }   //Fin del else
                            
                        }   //Fin del else
                    
                    }   //Fin del if

                    else if (i == 0){
                        //Se ha introducido información de teclado por parte del servidor
                        bzero(buffer, sizeof(buffer));
                        fgets(buffer, sizeof(buffer),stdin);
                        
                        //Controlar si se ha introducido "SALIR", cerrando todos los sockets y finalmente saliendo del servidor. (implementar)
                        
                        if(strcmp(buffer,"SALIR\n") == 0){
                            for (j = 0; j < numClientes; j++){
                                send(clientesTotales[j].getId(), "Desconexion servidor\n", strlen("Desconexion servidor\n"),0);                                
                                close(clientesTotales[j].getId());
                                FD_CLR(clientesTotales[j].getId(),&readfds);
                            }
                                close(sd);
                                exit(-1);
                            
                        }   //Fin del if
                        
                    }   //Fin del else if

                    else{

						//Informacion que va a pedir el usuario
                        bzero(buffer,sizeof(buffer));
                        recibidos = recv(i,buffer,sizeof(buffer),0);

                        if(recibidos > 0 ){

                            /*---------------------------------------------------------------------
                                                    PETICIONES DEL CLIENTE
                            ----------------------------------------------------------------------*/

                            //Registrar Usuario
                            if(strncmp(buffer,"REGISTRO",8) == 0){
                                int jug=busqueda(clientesTotales,i);
                                if(clientesTotales[jug].getRegistrado() == false){                                    
                                    registrarUsuario(buffer,clientesTotales[jug]);
                                    //se comprueba si hay un nombre igual en la base de datos
                                    if(guardarUsuario(clientesTotales[jug]) == false){
                                        strcpy(buffer, "–ERR. Ya hay un usuario registrado con ese nombre.\n");
										funcionLog(buffer);                                
                                    }
                                    else{
                                        strcpy(buffer, "+Ok. Usuario registrado.\n");
										funcionLog(buffer);                                
                                    }
                                }
                                else{
                                    strcpy(buffer, "–ERR. El Usuario ya está registrado.\n");
									funcionLog(buffer);                                
                                }                                    
                            }

                            //Salir de una partida
                            else if (strcmp(buffer,"SALIR-PARTIDA") == 0){
                                int jug=busqueda(clientesTotales,i);
                                int par=buscaPartida(clientesTotales,i);
                                if(clientesTotales[jug].getEstaJugando() == true){
                                    //ponemos al jugador como que no está jugando.
                                    clientesTotales[jug].setEstaJugando(false);
                                    clientesTotales[jug].setLinea1(false);
                                    clientesTotales[jug].setLinea2(false);
                                    clientesTotales[jug].setLinea3(false);
                                    clientesTotales[jug].setCantaUnaLinea(false);
                                    clientesTotales[jug].setCantaDosLineas(false);
                                    clientesTotales[jug].setCantaBingo(false);
                                    int jug=busqueda(vectorBingos[par].getJugadores(),i);
                                    salirClienteBingo(jug,par,i);
                                }
                            }

                            //Loguearse con Usuario
                            else if(strncmp(buffer,"USUARIO",7) == 0){ 
                                int jug=busqueda(clientesTotales,i);
                                if (comprobarUsuario(buffer) == true && clientesTotales[jug].getUsuActivado() == false){
                                    clientesTotales[jug].setUsuario(buffer);                                        
                                    clientesTotales[jug].setUsuActivado();
                                    //se considera que el jugador esta registrado
                                    clientesTotales[jug].setRegistrado();
                                    //Envia mensaje al cliente
                                    strcpy(buffer, "+Ok. Usuario correcto\n");
									funcionLog(buffer);                                
                                }
                                else{
                                    if(clientesTotales[jug].getUsuActivado() == true){
                                        strcpy(buffer, "–ERR. Usuario ya activo\n");
										funcionLog(buffer);                                
                                    }
                                    else{
                                        strcpy(buffer, "–ERR. Usuario incorrecto\n");
										funcionLog(buffer);                                
                                    }
                                }
                            }

                            //Loguearse con Password
                            else if(strncmp(buffer,"PASSWORD",8) == 0){
                                int jug=busqueda(clientesTotales,i);								
                                if (clientesTotales[jug].getUsuActivado() == true){
	                                if (comprobarPassword(buffer,clientesTotales[jug].getUsuario()) == true){
                                        clientesTotales[jug].setPassActivado();
	                                    strcpy(buffer, "+Ok. Usuario validado\n");
										funcionLog(buffer);                                
	                                    send(i,buffer,strlen(buffer),0);                                    
	                                }
	                                else{
	                                    strcpy(buffer, "–ERR. Error en la validación\n");
										funcionLog(buffer);                                
	                                }
                            	}
                                //error al insertar password
								else{
                                    strcpy(buffer, "-ERR. Debe validar antes el usuario...\n");
									funcionLog(buffer);                                
								}

								if(clientesTotales[jug].getUsuActivado() == 1 && clientesTotales[jug].getPassActivado() == 1){
									clientesTotales[jug].setClienteLogueado();                                    
                                    strcpy(buffer, "+OK. Usuario conectado\n");
									funcionLog(buffer);                                
								}   
							}
                        
                            //Cantar linea
                            else if(strcmp(buffer,"UNA-LINEA") == 0){

                                //buscamos en que partida está el jugador
                                int par=buscaPartida(clientesTotales,i);

                                //buscamos en que posicion del vector de la partida está el jugador
                                int jug=busqueda(vectorBingos[par].getJugadores(),i);
                                //si el jugador esta jugando...
                                if(vectorBingos[par].getJugadores()[jug].getEstaJugando() == true){
                                    //Si no se ha cantado la linea 1
                                    if(vectorBingos[par].getLinea1() == false){
                                        //Comprueba la linea
                                        comprobarLinea(par, jug, vectorBingos[par].getBolaSacada());
                                        if(vectorBingos[par].getJugadores()[jug].getLinea1() || vectorBingos[par].getJugadores()[jug].getLinea2() || vectorBingos[par].getJugadores()[jug].getLinea3()){   
                                            //Hay linea
                                            //Al jugador le activo que ha cantado una linea
                                            vectorBingos[par].getJugadores()[jug].setCantaUnaLinea(true);
                                            //Activo en el bingo la linea 1
                                            vectorBingos[par].setLinea1(true);
                                            //envia mensaje a todos los jugadores                                                                                
                                            sprintf(buffer,"+Ok. Jugador %s ha cantado una línea\n",vectorBingos[par].getJugadores()[jug].getUsuario());
                                            funcionLog(buffer);                                        
                                            for(j=0; j<(int)vectorBingos[par].getJugadores().size(); j++){
                                                if(vectorBingos[par].getJugadores()[j].getId() != i){
                                                    //vectorBingos[par].getJugadores()[j].setCantaUnaLinea(true);
                                                    send(vectorBingos[par].getJugadores()[j].getId(),buffer,strlen(buffer),0);
                                                }                                
                                            }
                                        }
                                        else{   //Ha cantado linea sin serlo.
                                            sprintf(buffer,"-Err. El comando UNA-LINEA no procede\n");
    										funcionLog(buffer);                                
                                        } 
                                    }
                                    else{   //Si ya se ha cantado una linea
                                        sprintf(buffer,"-Err. El comando UNA-LINEA no procede\n");
                                        funcionLog(buffer);                                        
                                    }
                                }
                            }
                            //Cantar dos lineas
                             else if(strcmp(buffer,"DOS-LINEAS") == 0){

                                //buscamos en que partida está el jugador
                                int par=buscaPartida(clientesTotales,i);

                                //buscamos en que posicion del vector de la partida está el jugador
                                int jug=busqueda(vectorBingos[par].getJugadores(),i);
                                //si el jugador esta jugando...
                                if(vectorBingos[par].getJugadores()[jug].getEstaJugando() == true){
                                    //Busca una nueva linea (actualizando)
                                    comprobarLinea(par, jug, vectorBingos[par].getBolaSacada()); 
                                    //Comprueba que tiene dos lineas completas                                             
                                    if( (vectorBingos[par].getJugadores()[jug].getLinea1() && vectorBingos[par].getJugadores()[jug].getLinea2()) ||  
                                        (vectorBingos[par].getJugadores()[jug].getLinea1() && vectorBingos[par].getJugadores()[jug].getLinea3()) ||
                                        (vectorBingos[par].getJugadores()[jug].getLinea2() && vectorBingos[par].getJugadores()[jug].getLinea3())){

                                        //Hay dos lineas cantadas
                                        //Al jugador le activo que ha cantado dos lineas
                                        vectorBingos[par].getJugadores()[jug].setCantaDosLineas(true);

                                        //Si se ha cantado una linea y se comprueba la segunda
                                        if(vectorBingos[par].getLinea2() == false){

                                            //Activo en el bingo la linea 1
                                            vectorBingos[par].setLinea2(true);
                                            //envia mensaje a todos los jugadores                                              
                                            sprintf(buffer,"+Ok. Jugador %s ha cantado dos líneas\n",vectorBingos[par].getJugadores()[jug].getUsuario());
                                            funcionLog(buffer);                                        
                                            for(j=0; j<(int)vectorBingos[par].getJugadores().size(); j++){
                                                if(vectorBingos[par].getJugadores()[j].getId() != i){
                                                    //vectorBingos[par].getJugadores()[j].setCantaDosLinea(true);
                                                    send(vectorBingos[par].getJugadores()[j].getId(),buffer,strlen(buffer),0);
                                                }                                
                                            }
                                        }
                                        else{   //Ha cantado linea sin serlo.
                                            sprintf(buffer,"-Err. El comando DOS-LINEAS no procede\n");
                                            funcionLog(buffer);                                
                                        }                                                                                     
                                    }
                                    else{   //Si ya se ha cantado una linea
                                        sprintf(buffer,"-Err. El comando DOS-LINEAS no procede\n");
                                        funcionLog(buffer);                                        
                                    }
                                }
                            }

                            //Cantar Bingo
                            else if(strncmp(buffer,"BINGO",5) == 0){
                                //buscamos en que partida está el jugador
                                int par=buscaPartida(clientesTotales,i);
                                //buscamos en que jugicion del vector de la partida está el jugador
                                int jug=busqueda(vectorBingos[par].getJugadores(),i);
                                //si el jugador esta jugando...
                                if(vectorBingos[par].getJugadores()[jug].getEstaJugando() == true){
                                    //Compruebo que las 3 lineas estan completas
                                    comprobarLinea(par, jug, vectorBingos[par].getBolaSacada());

                                    if(vectorBingos[par].getJugadores()[jug].getLinea1() && vectorBingos[par].getJugadores()[jug].getLinea2() && vectorBingos[par].getJugadores()[jug].getLinea3() ) {
                                        //Se ignora la señal SIGALRM
                                        signal (SIGALRM, SIG_IGN);

                                        sprintf(buffer,"+Ok. Jugador %s ha cantado bingo\n",vectorBingos[par].getJugadores()[jug].getUsuario());
                                        funcionLog(buffer);                                

                                        //envia mensaje a los demas jugadores
                                        for(j=0; j<(int)vectorBingos[par].getJugadores().size(); j++){
	                                        send(vectorBingos[par].getJugadores()[j].getId(),buffer,strlen(buffer),0);                                                            
                                        }

                                        cout << "Fin de la partida " << endl;

                                        strcpy(buffer,"Fin de la partida\n");

                                        //mandamos mensaje de fin de partida
                                        for(j=0; j<(int)vectorBingos[par].getJugadores().size(); j++){
                                            send(vectorBingos[par].getJugadores()[j].getId(),buffer,strlen(buffer),0);                                                            
                                        }                                        

                                        int jug;

                                        //Sacamos a los jugadores de la partida reiniciando sus valores
                                        for(j=0;j<(int)vectorBingos[par].getJugadores().size();j++){
                                            //resetea jugadores en vector de clientes del servidor
                                            jug=busqueda(clientesTotales, vectorBingos[par].getJugadores()[j].getId());                                            
                                            clientesTotales[jug].setEstaJugando(false);
                                            clientesTotales[jug].setLinea1(false);
                                            clientesTotales[jug].setLinea2(false);
                                            clientesTotales[jug].setLinea3(false);
                                            clientesTotales[jug].setCantaUnaLinea(false);
                                            clientesTotales[jug].setCantaDosLineas(false);
                                            clientesTotales[jug].setCantaBingo(false);
                                        }

                                        for(j=0;j<(int)vectorBingos[par].getJugadores().size();j++){
                                            //salir del bingo
                                            bzero(buffer, sizeof(buffer));
                                            strcpy(buffer, "REINICIAR\n");
                                            funcionLog(buffer);                                
                                            send(vectorBingos[par].getJugadores()[j].getId(),buffer,strlen(buffer),0);
                                        }

                                        bzero(buffer, sizeof(buffer));
                                        vectorBingos[par].getJugadores().clear();                                                     
                                       
                                    }
                                    else{
                                        sprintf(buffer,"-Err. El comando BINGO no procede\n");
										funcionLog(buffer);                                                                        
                                    }
                                }     
                                else{   //No esta jugando...
                                    sprintf(buffer,"-Err. no estás jugando\n");
									funcionLog(buffer);                                
                                    send(vectorBingos[par].getJugadores()[jug].getId(),buffer,strlen(buffer),0);                                    
                                }
                            }                                                               

                            //Iniciar una partida                       
                            else if(strncmp(buffer, "INICIAR-PARTIDA",8) == 0){
                                int jugvector=busqueda(clientesTotales,i);
                                //Si el logueado ha falla rechaza al usuario
								if(clientesTotales[jugvector].getClienteLogueado() == false){
                                    strcpy(buffer, "-ERR. El usuario debe loguearse en el sistema...\n");
									funcionLog(buffer);                                
								}
                                //Acepta al usuario
                                else {
                                    //metemos los jugadores en un vector para pasarselo al bingo
                                    vectorClientes.push_back(clientesTotales[jugvector]);
                                    int jugcliente=busqueda(vectorClientes,i);
                                    //Aumenta un jugador y se queda esperando pasando al else
                                    if (clientesTotales[jugvector].getClienteLogueado() == true && vectorClientes.size() <= MAX_JUGADORES){
                                        //asignamos al jugador que esta jugando
                                        clientesTotales[jugvector].setEstaJugando(true);
                                        //asignamos al jugador el numero de partida
                                        clientesTotales[jugvector].setPartida(numPartida);
                                        //copiamos l
                                        vectorClientes[jugcliente]=clientesTotales[jugvector];
                                        //aumentamos el jugador para poder entrar en la partida                                                                
                                        jugadores++;
                                        
                                        sprintf(buffer,"+AVISO: %s se ha unido a la partida\n",vectorClientes[jugcliente].getUsuario());
										funcionLog(buffer);                                                                        
                                        //para mandarlo a todos.
                                        for(j=0; j<(int)vectorClientes.size(); j++){
                                            if(vectorClientes[j].getId() != i){    
                                                send(vectorClientes[j].getId(),buffer,strlen(buffer),0);
                                            }
                                        }

                                    /*---------------------------------------------------------------------
                                                                COMIENZA EL BINGO
                                    ----------------------------------------------------------------------*/

                                        if(jugadores == MAX_JUGADORES){

                                            //PONER AQUI EL REPARTO DE CARTONES

                                        	//Creamos el bingo y asignamos numero de partida (0-10)
                                            Bingo b = Bingo(numPartida,vectorClientes);

                                            //Reparte cartones
                                            for(j=0; j<MAX_JUGADORES; j++){
                                                sleep(1);
                                                strcpy(buffer,b.reparteCartones(j));
                                                send(b.getJugadores()[j].getId(),buffer,strlen(buffer),0);
                                            }  

                                            //agregamos el bingo al vector de partidas de bingo             
                                            vectorBingos.push_back(b);  
                                            //aumentamos identificador de bingo                                                    
                                            numPartida++;
                                            //reducimos jugadores de nuevo para nuevos clientes
                                            jugadores=0;
                                            //vaciamos vector clientes para generar otros nuevos 4 clientes   
                                            vectorClientes.clear();
                                                                                        
                                            //Mandamos mensaje de comenzar bingo
                           				 	sprintf(buffer,"\n+Ok. Empieza la partida en Bingo %d\n",numPartida);
											funcionLog(buffer);                                

                                            //para mandarlo a todos los de una partida
                                            int aux=buscaPartida(clientesTotales,i);

                                            //Asigna las partidas que hay a una variable global
                                            c=numPartida;

                                            for(j=0; j<MAX_JUGADORES; j++){
                                            	if(vectorBingos[aux].getJugadores()[j].getId() != i){
                                                	send(vectorBingos[aux].getJugadores()[j].getId(),buffer,strlen(buffer),0);
													funcionLog(buffer);                                
                                                }
                                            }     
                                          	
                                            alarm(6);

                                        }//Fin del if comienza bingo  
                                        
                                        else{
                                            //Se queda esperando
                                            strcpy(buffer, "+Ok. Petición Recibida. Quedamos a la espera de más jugadores\n");
											funcionLog(buffer);                                
                                        }//Fin del else de quedarse esperando 

                                    }//Fin del if de espera clientes 

                                }//Fin del else de aceptar

                            }//Fin del iniciar partida

                            //Mandar el mensaje
                            //me devuelve en que partida está el jugador
                            sprintf(identificador,"%d: %s",i,buffer);

                            for(j=0; j<(int)clientesTotales.size(); j++){
                                if(clientesTotales[j].getId() == i){
                                    send(clientesTotales[j].getId(),buffer,strlen(buffer),0);
									funcionLog(buffer);                                
                                }
                            }

                            bzero(buffer, sizeof(buffer));

                        }//Fin del if recibidos > 0

                        //Si el cliente introdujo SALIR
                        if(recibidos==0){
                            printf("El cliente %d, ha introducido SALIR\n", i);
                            sprintf(buffer,"El cliente %d, ha introducido SALIR\n", i);
							funcionLog(buffer);
                            salirCliente(i,&readfds,&numClientes,clientesTotales);                                  
                            //Eliminar ese socket
                        }

                    }//Fin del else

                }//Fin del if

            }//FIn del for

        }//Fin del if

	}//Fin del while

	close(sd);

    return 0;	
}
