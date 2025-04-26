#include <iostream>
#include <string>
#include <vector>

/**
	After receiving a specified width and height, Board class generates a board.
	input:
		width - width length of the board as an integer.
		height - height length of the board as an integer.
*/
class Board {
public:
	Board(int width, int height) : _width_ci(width), _height_ci(height), _area_ci(width * height), _board_lstch(width * height, '-') {}
	
	// Prints a vector function
	void print_board() {
		int count_i = 0;
		
		for (_it = _board_lstch.begin(); _it != _board_lstch.end(); ++_it) {
			std::cout << *_it;
			++count_i;
			
			// Turns array square-shaped.
			if (count_i >= _width_ci) {std::cout << std::endl; count_i = 0;};
		}
	}
	
	// Displaying and updating the board...
	void display() {
		print_board();
	}
	
private:
	const int _width_ci, _height_ci, _area_ci;
	std::vector<char> _board_lstch;
	std::vector<char>::iterator _it; // Local iterator.

	
};


int main() {
	Board(8, 8).display();
	
}
