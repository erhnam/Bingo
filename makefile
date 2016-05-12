.PHONY : clean

OBJECTOSCLIENTE = clientes.o funcCliente.o
OBJECTOSSERVIDOR = servidor.o bingo.o funcServidor.o carton.o

all: servidor clientes clean

servidor: $(OBJECTOSSERVIDOR)
	g++ -o servidor $(OBJECTOSSERVIDOR)

servidor.o: servidor.cpp
	g++ -c -Wall servidor.cpp

bingo.o: bingo.cpp
	g++ -c -Wall bingo.cpp

funcServidor.o: funcServidor.cpp
	g++ -c -Wall funcServidor.cpp

carton.o: carton.cpp
	g++ -c -Wall carton.cpp

clientes: $(OBJECTOSCLIENTE)
	g++ -o clientes $(OBJECTOSCLIENTE)

clientes.o: clientes.cpp
	g++ -c -Wall clientes.cpp

funcCliente.o: funcCliente.cpp
	g++ -c -Wall funcCliente.cpp

clean:
	@echo Borrando ficheros.o ...
	rm *.o 
	@echo "Ejecute <./cliente> y <./servidor> en terminales distintas para jugar al bingo."	
	@echo  Terminado

