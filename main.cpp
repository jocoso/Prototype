#include <iostream>
#include <string>
#include <vector>


//
/**
	After receiving a specified width and height, Board class generates a board.
	input:
		width - width length of the board as an integer.
		height - height length of the board as an integer.
*/
class Board {
public:
	Board(int width, int height) : _width(width), _height(height), _area(width * height), _board(width * height, '-') {}
	// 
	
	// Displaying the board...
	void display() {
		
		int count = 0;
		
		for (_it = _board.begin(); _it != _board.end(); ++_it) {
			std::cout << *_it;
			++count;
			
			// Turns array square-shaped.
			if (count >= _width) {std::cout << std::endl; count = 0;};
		}
	}
	
	//
private:
	const int _width, _height, _area;
	std::vector<char> _board;
	// 
	std::vector<char>::iterator _it; // Local iterator.
	//
	
};


int main() {
	Board(8, 8).display();
	
}
