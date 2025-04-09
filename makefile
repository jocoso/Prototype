chess:
	g++ -o chess main.cpp
	chmod a+x chess
	./chess
	make delete
delete:
	rm chess
