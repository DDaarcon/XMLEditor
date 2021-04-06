xmleditor: units.o interface.o main.o
	g++ units.o interface.o main.o -o xmleditor
units.o:
	g++ -c units.cpp -o units.o
interface.o:
	g++ -c interface.cpp -o interface.o
main.o:
	g++ -c main.cpp -o main.o