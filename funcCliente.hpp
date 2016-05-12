#ifndef FUNCCLIENTE_HPP
#define FUNCCLIENTE_HPP

int buscaLinea(char * buffer, int lineas[],int m[3][9]);
void compruebaLineas(int numLineas, int sd,int m[3][9], bool &l1, bool &l2);
int buscaBola(char * buffer, int m[3][9]);
void recibeCarton(char * buffer, int m[3][9]);
void mostrarTablero(int ** tablero);
void ponerBolaTablero(int bola, int ** tablero);
void mostrarCarton(int carton[][9]);
void funcionLog(char *mensaje);



#endif