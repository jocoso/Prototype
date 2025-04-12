#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <memory>
#include <sstream>

class Board;
class Chess;
class Piece;




class Piece {
public:
	
	Piece(std::string pce_stamp, std::string pce_name, std::string pce_init_coord): _stamp(pce_stamp), _name(pce_name), _current_coord(pce_init_coord) {}
	
	void set_attribute(const std::string& attr_name, const std::vector<std::string>& attrs) {
		_attributes[attr_name] = attrs;	
	}
	void assign_new_coordinate(std::string coord) {
		_current_coord = coord;
	}
	std::string get_current_coordinates() const {
		return _current_coord;
	}
	
	virtual void move(const Board& board) = 0;
	virtual ~Piece() {}
	
	std::string stamp() const {
		return _stamp;
	}
	
	std::string get_name() const {
		return _name;
	}
	
protected:
	std::map<std::string, std::vector<std::string>> _attributes;
	std::string _current_coord;
	std::string _name;
	std::string _stamp;
	
		
};

class Board {
	public:
		// [1 Height = 1 Board Square] * same goes for width.
		Board(const int& height, const int& width) : _height(height), _width(width), _cursor_coords("A1") {
			
			_board.resize(height, std::vector<std::string>(width, "-"));
			
		}
		
		
		Board(const Board& other) : 
			_height(other._height), 
			_width(other._width), 
			_cursor_coords(other._cursor_coords), 
			_board(other._board) {}
		
		~Board() = default;
		
		// Draws board on the console screen.
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
		
		void update() {
		
	
		
			
		}
		
		std::string& get_square_at(const std::string coords) {
			int sqr_row = coords[1] - '1';
			int sqr_col = coords[0] - 'A';
			
			if(sqr_row >= 0 && sqr_row < _height && sqr_col >= 0 && sqr_col < _width) {
        			
            			return _board[sqr_row][sqr_col];
				
				
        		} else {
        			throw std::out_of_range("Coordinates are out of bounds.");
    			}
    			

		}
		
		void transfer(Piece &piece, std::string new_coords) {
			
		}
		
		std::vector<std::vector<std::string>>& get_board() {
			return _board;
		}
		
		void add_piece(Piece &piece) {
			
			std::string* piece_sqr_ptr = &this->get_square_at(piece.get_current_coordinates());
			
			if(piece_sqr_ptr != nullptr && *piece_sqr_ptr == "-") {
				
				*piece_sqr_ptr = piece.stamp();
				
			} else {
				std::cout << "Cannot add piece " << piece.get_name() << " to the board." << std::endl;
			}
		} 
		
		
		
	private:
		const int _height, _width;
		std::string _cursor_coords;
		std::vector<std::vector<std::string>> _board;
		std::stringstream _ss;
		
};

class Chess {
public:
	Chess(Board&& board) : _board(std::make_unique<Board>(std::move(board))) {
	}
	~Chess() = default;
private:
	std::unique_ptr<Board> _board;
};

class Rook : public Piece {
public:
	Rook(std::string init_coord): Piece("♖", "Rook", init_coord), _has_moved(false) {
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
	std::cout << rightRook.get_name() << " is at: " << rightRook.get_current_coordinates() << std::endl;
	
	myBoard.add_piece(rightRook);
	myBoard.draw();
	
	return 0;
}
