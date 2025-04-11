#include <iostream>
#include <string>
#include <map>
#include <vector>

class Board {
	public:
		// [1 Height = 1 Board Square] * same goes for width.
		Board(const int& height, const int& width) : _height(height), _width(width) {
			
			// Allocating memory for an array of pointers size height...
			this->_board = new char *[height];
			// Allocating memory for each row
			for (int i = 0; i < height; i++) {
				this->_board[i] = new char[width];
				for (int j = 0; j < width; j++) {
		        		this->_board[i][j] = '-'; // Initialize with '-'
		    		}
			}
			
		}
		
		~Board() {
			for(int i = 0; i < this->_height; i++) {
				delete[] _board[i];
			}
			
			delete[] _board;
		
		}
		
		void draw() {
			
			int jumps = this->_height;
			int counter = 0;
			
			for( int i = 0 ; i < this->_height ; i++ ) {
				for( int j = 0 ; j < this->_width ; j++ ) {
					std::cout << "[" << this->_board[i][j] << "]";
				}
				std::cout << std::endl;
			}
		}
		
		char** get_board() {
			return _board;
		}
		
	private:
		const int _height, _width;
		char **_board;
};

class Piece {
public:
	Piece(char graphic, std::string init_coord): _graphic(graphic), _current_coord(init_coord) {}
	void set_attribute(const std::string& attr_name, const std::vector<std::string>& attrs) {
		
		this->_attributes[attr_name] = attrs;
		
	}
	void assign_new_coordinate(std::string coord) {
		this->_current_coord = coord;
	}
	
	std::string get_current_coordinates() const {
		return _current_coord;
	}
	
	virtual void move(const Board& board) = 0;
	virtual ~Piece() {}
	
	
protected:

	std::map<std::string, std::vector<std::string>> _attributes;
	std::string _current_coord;
	const char _graphic;
	
		
};

class Rook : public Piece {
public:
	Rook(std::string init_coord): Piece('r', init_coord) {
		// - movement -
		// From 'this' piece's row and column,
		// Players can select any point in the x-axis or any point in the y-axis
		this->set_attribute("movement", {"this", "anypoint_x v anypoint_y"});
	}
	void move(const Board& board) override {
		std::cout << "Rook moves a max-x or max-y but only on one axis per turn" << std::endl;
	}
};



int main() {
	Board myBoard(8, 8);
	myBoard.draw();
	
	Rook myRook("A1");
	myRook.assign_new_coordinate("A5");
	std::cout << "Rook is at: " << myRook.get_current_coordinates() << std::endl;
	myRook.move(myBoard);

	
	return 0;
}
