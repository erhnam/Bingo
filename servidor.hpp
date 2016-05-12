#ifndef SERVIDOR_HPP
#define SERVIDOR_HPP

std::vector <Bingo> vectorBingos;                           //Vector para llevar control del numero de bingos
std::vector <Cliente> clientesTotales;                          //Vector de 4 jugadores standard (se pasará al bingo una vez completado)    

int c=0;
int espera=0;
char buffer[MSG_SIZE];

#endif