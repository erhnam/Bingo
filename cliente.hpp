#ifndef CLIENTE_HPP
#define CLIENTE_HPP
#include <string.h>
#include <iostream>
#include <cstdio>

#define tamChar 15

class Cliente {

	private:

		int id;						//variable identificador que coincide con el socket
		char * usuario;				//variable donde guardará el nombre de usuario
		char * password;			//variable donde guardará el password de usuario
		bool usuActivado;			//variable para saber si el nombre del usuario ha sido introducido
		bool passActivado;			//variable para saber si el password del usuario ha sido introducido
		bool clienteLogueado;		//Variable para indicar que tanto la el usuario y el password han sido introducidos.
		bool estaJugando;     		//Variable que indica que el jugador esta jugando
		int ** carton;				//Variable donde será almacenado el carton que reciba del servidor.
		int partida;   				//Partida que juega el cliente
		bool linea1;
		bool linea2;
		bool linea3;
		bool registrado;
		bool cantaUnaLinea;
		bool cantaDosLineas;
		bool cantaBingo;			


	public:

		//Constructor

		Cliente(){

			usuario= new char [tamChar];
			password= new char [tamChar];
			id=0;
			partida=0;
			registrado=false;
			usuActivado=false;
			passActivado=false;
			clienteLogueado=false;
			estaJugando=false;
			linea1=false;
			linea2=false;
			linea3=false;
			cantaUnaLinea=false;
			cantaDosLineas=false;
			cantaBingo=false;			
			carton= new int *[3];
			for (int i = 0; i < 3; i++){
			    carton[i] = new int[9];
			}					
		}

		//operador igual
		Cliente& operator= (Cliente const & c){

			usuario= new char [tamChar];
			password= new char [tamChar];
			
			carton= new int *[3];
			for (int i = 0; i < 3; i++){
			    carton[i] = new int[9];
			}
			setRegistrado(c.getRegistrado());
			setUsuActivado(c.getUsuActivado());
			setPassActivado(c.getPassActivado());
			setClienteLogueado();
			setEstaJugando(c.getEstaJugando());

			setCantaBingo(c.getCantaBingo());
			setCantaDosLineas(c.getCantaDosLineas());
			setCantaUnaLinea(c.getCantaUnaLinea());
			setLinea1(c.getLinea1());
			setLinea2(c.getLinea2());
			setLinea3(c.getLinea3());

			setUsuario(c.getUsuario());
			setPassword(c.getPassword());
			setId(c.getId());
			setCarton(c.getCarton());
				
		}		

		//Getters
		int getClienteLogueado(){
			if(getUsuActivado() == true && getPassActivado() == true){
				clienteLogueado=true;
			}	
			return clienteLogueado;
		}
		inline int getId() const {return id;}
		inline int getPartida() const {return partida;}
		inline bool getEstaJugando() const {return estaJugando;}
		inline bool getUsuActivado() const {return usuActivado;}
		inline bool getPassActivado() const {return passActivado;}
		inline bool getRegistrado() const {return registrado;}		
		inline char * getUsuario() const {return usuario;}
		inline char * getPassword() const {return password;}
		inline int ** getCarton() const {return carton;}
		inline bool getLinea1() const {return linea1;}
		inline bool getLinea2() const {return linea2;}
		inline bool getLinea3() const {return linea3;}
		inline bool getCantaUnaLinea() const {return cantaUnaLinea;}
		inline bool getCantaDosLineas() const {return cantaDosLineas;}
		inline bool getCantaBingo() const {return cantaBingo;}


		//Setters
		inline void setId(int i){id=i;}
		inline void setUsuActivado(){
			usuActivado=true;
			setRegistrado();
		}
		inline void setPartida(int numPartida){partida=numPartida;}
		inline void setPassActivado(){passActivado=true;}
		inline void const setClienteLogueado(){clienteLogueado=true;}
		inline void setUsuario(char * u){strcpy(usuario,u);}
		inline void setPassword(char * p){strcpy(password,p);}
		inline void setLinea1(){linea1=true;}
		inline void setLinea2(){linea2=true;}
		inline void setLinea3(){linea3=true;}

		//Sobrecargar
		inline void setCarton(int ** c){carton=c;}
		inline void setRegistrado(){registrado=true;}
		inline void setEstaJugando(){estaJugando=true;}
		inline void setCantaUnaLinea(){cantaUnaLinea=true;}
		inline void setCantaDosLineas(){cantaDosLineas=true;}
		inline void setCantaBingo(){cantaBingo=true;}

		//Sobrecarga
		inline void setUsuActivado(bool b){
			usuActivado=b;
			setRegistrado();
		}
		inline void setClienteLogueado(const bool b){clienteLogueado=b;}		
		inline void setPassActivado(bool b){passActivado=b;}		
		inline void setLinea1(bool b){linea1=b;}
		inline void setLinea2(bool b){linea2=b;}
		inline void setLinea3(bool b){linea3=b;}		
		inline void setRegistrado(bool b){registrado=b;}
		inline void setEstaJugando(bool b){estaJugando=b;}
		inline void setCantaUnaLinea(bool b){cantaUnaLinea=b;}
		inline void setCantaDosLineas(bool b){cantaDosLineas=b;}
		inline void setCantaBingo(bool b){cantaBingo=b;}

		//Funciones
		void mostrarCarton(){

			int i,j;
			for (i=0;i<3;i++){
				for (j=0;j<9;j++){
					if(carton[i][j] == -1){
						std::cout << "| X ";
					}
					else {
						std::cout << "| " << carton[i][j] << " ";
					}
				}
					std::cout << "\n" << std::endl;
			}
		}		


};


#endif
