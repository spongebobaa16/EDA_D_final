floorplan: clean Solver.o B_tree.o main.o 
	g++ -o $@ -std=c++11 -g Solver.o B_tree.o main.o 

Solver.o: Solver.cpp
	g++ -c -std=c++11 -g Solver.cpp

B_tree.o: B_tree.cpp
	g++ -c -std=c++11 -g B_tree.cpp

main.o: main.cpp
	g++ -c -std=c++11 -g main.cpp

clean:
	rm -f *.o 
