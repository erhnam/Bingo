#ifndef FUNCSERVIDOR_HPP
#define FUNCSERVIDOR_HPP

#define MAX_CADENA 100
#define MAX_JUGADORES 4	//(0,1,2,3) 4 jugadores
#define MAX_CLIENTS 40
#define MSG_SIZE 250
#define MAX_PARTIDAS 10

#include "cliente.hpp"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <vector>
#include "bingo.hpp"

extern int c;
extern char buffer[MSG_SIZE];
extern std::vector <Bingo> vectorBingos;                           //Vector para llevar control del numero de bingos

int buscaPartida(std::vector <Cliente> vector, int id);
int busqueda(std::vector <Cliente> vector, int id);
void registrarUsuario(char * datosUsuario, Cliente &cl);
bool guardarUsuario (Cliente &cl);
bool comprobarUsuario (char * buffer);
bool comprobarPassword (char * buffer, char * usuario);
void manejador (int signum);
void salirCliente(int socket, fd_set * readfds, int * numClientes, std::vector <Cliente> & arrayClientes);
void sacarBolas (int sig);
void comprobarLinea(int par, int pos, vector <int> bolas);
void funcionLog(char *mensaje);
void salirClienteBingo(int jug, int partida,int socket);

#endif
