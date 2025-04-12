chess:
	g++ -fsanitize=address -g main.cpp -o chess
	chmod a+x chess
	./chess
	make delete
delete:
	rm chess
