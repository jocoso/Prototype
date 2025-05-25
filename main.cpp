#include <iostream>
#include <stdexcept>
#include <algorithm>
#include <iterator>
#include <chrono>
#include <thread>
#include <vector>

namespace cc {
	// -- inelegant as fuck! and incredibly stupid.
	struct Coordinates;
	bool coords_equal(const Coordinates& comparing, const Coordinates& compared);
	int get_local_index( int x, int y, int width);
	void get_cartesian_coordinates(int local_bit, int width, int *cartesian_arr);
	
	
	struct Coordinates {
		int as_local;
		int as_cartesian[2] = {0,0};
		
		Coordinates(int x, int y, int width) {
			as_local = get_local_index(x, y, width);
			as_cartesian[0] = x; 
			as_cartesian[1] = y;
		}
		
		Coordinates(int local_bit, int width) {
			
			get_cartesian_coordinates(local_bit, width, as_cartesian);
			as_local = (as_cartesian[1] * width) + as_cartesian[0]; 
		}
		
		int* begin() { return as_cartesian; }
		int* end() { return as_cartesian + 2; }
		const int* begin() const {return as_cartesian;}
		const int* end() const {return as_cartesian + 2;}
		
	};

	bool coords_equal(const Coordinates& comparing, const Coordinates& compared) {
		bool same_locals = comparing.as_local == compared.as_local;
		bool same_cartesians = 
			std::equal(comparing.begin(), comparing.end(), compared.begin());
		return same_locals && same_cartesians;
		
	}
	
	int get_local_index( int x, int y, int width) {
		return (y * width) + x;
	}
	
	void get_cartesian_coordinates(int local_bit, int width, int *cartesian_arr) {
		cartesian_arr[1] = local_bit/width;
		cartesian_arr[0] = local_bit%width;
	}

};


class Console {
public:
	void draw(const std::string &words) {	
		std::cout << words << std::endl;
	}
	
	void clean() {
		#ifdef WINDOWS
			std::system("cls");
		#else
			std::system("clear");
		#endif
	}
};



class Typer {
public:
	Typer(Console& console) : _console(console), _buffer("") {}
	void type(std::string content, float delay_seconds) {
		int buffer_length = 0;
		using namespace std::this_thread;     // sleep_for, sleep_until
		using namespace std::chrono_literals; // nanoseconds, system_clock, seconds
		
		if(!_buffer.empty()) _buffer += "\n";
		
		
		while( buffer_length < content.length()) {
			auto delay = std::chrono::duration<float>(delay_seconds);
			sleep_for(delay);
			
			_console.clean();
			_console.draw(_buffer);
			_buffer += content[buffer_length++];
			
		}
		
	}
private:
	Console& _console;
	std::string _buffer;
};

int main() {
	// --> Program begins here.
	Console console;
	Typer typer = Typer(console);
	typer.type("Hello World!", .5);
	typer.type("I came with blackjacks. The hookers are late.", .1);
	
	return 0;
}
