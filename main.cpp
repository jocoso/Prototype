#include <iostream>
#include <string>
#include <map>
#include <vector>

class Board;

class Piece {
public:
	
	Piece(const char stamp, std::string init_coord): _stamp(stamp), _current_coord(init_coord) {}
	
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
	
	char stamp() const {
		return this->_stamp;
	}
	
protected:
	std::map<std::string, std::vector<std::string>> _attributes;
	std::string _current_coord;
	const char _stamp;
	
		
};

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
			

			for( int i = 0 ; i < this->_height ; i++ ) {
				std::cout << i+1 << " - ";
				for( int j = 0 ; j < this->_width ; j++ ) {
					std::cout << "[" << this->_board[i][j] << "]";
				}
				std::cout << std::endl;
			}
			
			std::cout << " - - ";
			for( char j = 'A' ; j < (char)(65 + this->_width) ; j++ ) { 
				std::cout << j << "  ";
			}
			std::cout << std::endl;
		}
		
		void transfer(Piece &piece, std::string new_coords) {
			
		}
		
		char** get_board() {
			return _board;
		}
		
		void add_piece(Piece &piece) {
			std::string coord = piece.get_current_coordinates();
			int row = coord[1] - '1';
			int col = coord[0] - 'A';
			
			if(row >= 0 && row < _height && col >= 0 && col < _width) {
				if(_board[row][col] == '-') {
					_board[row][col] = piece.stamp();
				}
			}
		} 
		
		
		
	private:
		const int _height, _width;
		char **_board;
};



class Rook : public Piece {
public:
	Rook(std::string init_coord): Piece('r', init_coord), _has_moved(false) {
		// - movement -
		// From 'this' piece's row and column,
		// Players can select any point in the x-axis or any point in the y-axis
		this->set_attribute("movement", {"this", "anypoint_x v anypoint_y"});
	}
	void move(const Board& board) override {

			
	}
	
private:
	bool _has_moved;
};



int main() {
	Board myBoard(8, 8);
	
	Rook rightRook("A1");
	rightRook.assign_new_coordinate("H8");
	std::cout << "Rook is at: " << rightRook.get_current_coordinates() << std::endl;
	
	myBoard.add_piece(rightRook);
	myBoard.draw();
	
	return 0;
}
