#include <iostream>
#include <stdexcept>
#include <algorithm>
#include <iterator>

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
	void draw(std::string words) {
		if(words.empty())
			throw std::invalid_argument("Invalid input: empty string.");
		else {
			std::cout << words << std::endl;
		}
	}
	
	void clean() {
		#ifdef WINDOWS
			std::system("cls");
		#else
			std::system("clear");
		#endif
	}
};

int main() {
	// --> Program begins here.
	cc::Coordinates coordinate_1 = cc::Coordinates(0, 0, 4);
	cc::Coordinates coordinate_2 = cc::Coordinates(0, 4);
	
	if(cc::coords_equal(coordinate_1, coordinate_2))
		std::cout << "Test Passed!" << std::endl;
	else
		std::cout << "Test Failed!" << std::endl;
		
	cc::Coordinates coordinate_3 = cc::Coordinates(2, 1, 4);
	cc::Coordinates coordinate_4 = cc::Coordinates(6, 4);
	
	if(cc::coords_equal(coordinate_3, coordinate_4))
		std::cout << "Test Passed!" << std::endl;
	else
		std::cout << "Test Failed!" << std::endl;
		
	Console console;
	
	try{
		console.draw("Rosa roja");
		std::cout << "Test Passed!" << std::endl;
	} catch(std::invalid_argument e) {
		std::cout << "Test Failed! " << e.what() << std::endl;
	}
	
	console.clean();
	
	return 0;
}
