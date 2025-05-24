start:
	g++ -g -fsanitize=address -o game main.cpp
	chmod a+x game
	./game
clean:
	rm game
