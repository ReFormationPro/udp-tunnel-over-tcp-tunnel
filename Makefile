all: tunnel

tunnel: main.cpp utils.cpp
	g++ -g main.cpp utils.cpp -lpthread -o tunnel

clean:
	rm -f tunnel

