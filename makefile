run: prepare
	clear
	./main
prepare: 
	g++ -pthread -o main main.cpp -lncurses 