#include <iostream>
#include <string>
#include <map>
#include <vector>

class Piece {
public:
	void add_attribute(std::string attr_name, std::vector<std::string> attrs) {
		this->attributes[attr_name] = attrs;
	}
	void assign_new_coordinate(std::string coord) {
		this->current_coord = coord;
	}
	virtual void move(char** board) = 0;
	virtual ~Piece() {}
protected:
	std::map<std::string, std::vector<std::string>> attributes;
	std::string current_coord;
		
};

class Rook : public Piece {
public:
	void move(char** board) override {
		std::cout << "Rook moves a max-x or max-y but only on one axis per turn" << std::endl;
	}
};

int main() {
	Rook rook;
	
	// draw a board
	// for each of the 64 rows, draw 64 squares
	std::cout << std::endl;
	for (int i = 0; i < 8; i++){
		std::cout << '\n';
		for (int j = 0; j < 8; j++) {
			std::cout << "[  ]";
		}
		std::cout << std::endl;
		if (i < 7) {
			for (int k = 0; k < 8; k++) {
				std::cout << "--- ";
			}
			std::cout << std::endl;
		}
	}
	
	 // Example usage of Rook's move method
	    char** board = new char*[8];
	    for (int i = 0; i < 8; i++) {
		board[i] = new char[8];
	    }
	    rook.move(board);
	    for (int i = 0; i < 8; i++) {
		delete[] board[i];
	    }
	    delete[] board;

	
	return 0;
}
