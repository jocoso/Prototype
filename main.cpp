#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <memory>
#include <sstream>
#include <limits>


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
			_board(other._board), 
			_pieces(other._pieces){}
		
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
		
		
		void move(const std::string old_coords, const std::string new_coords) {
    			std::string old_sqr = this->get_square_at(old_coords);
    			std::string new_sqr = this->get_square_at(new_coords);

    			if (old_sqr != "-" && new_sqr == "-") {
        			for (auto& [stamp, piece_ptr] : _pieces) {
        	    			if (piece_ptr->get_current_coordinates() == old_coords) {
        	        			piece_ptr->assign_new_coordinate(new_coords);

        	        			this->get_square_at(new_coords) = stamp;
        	        			this->get_square_at(old_coords) = "-";
        	        			return;
        	    			}
        			}
    			} else {
        			std::cout << "Invalid move." << std::endl;
    			}
		}
		
		std::vector<std::vector<std::string>>& get_board() {
			return _board;
		}
		
		void add_piece(std::shared_ptr<Piece> piece) {
			std::string* sqr_ptr = &this->get_square_at(piece->get_current_coordinates());
			
			if(sqr_ptr != nullptr && *sqr_ptr == "-") {
				
				*sqr_ptr = piece->stamp();
				std::cout << "Adding piece..." << std::endl;
				_pieces[piece->stamp()] = piece;
				std::cout << "Piece added. Map size: " << _pieces.size() << std::endl;
				
			} else {
				std::cout << "Cannot add piece " << piece->get_name() << " to the board." << std::endl;
			}
		} 
		
		
		
	private:
		const int _height, _width;
		std::string _cursor_coords;
		std::map<std::string, std::shared_ptr<Piece>> _pieces; // Stamp - Piece
		std::vector<std::vector<std::string>> _board;
		std::stringstream _ss;
		
};

class Chess {
public:
	Chess(const Board& board) : _board(std::make_unique<Board>(board)), _playing(true) {
	}
	~Chess() = default;
	void play() {
		std::string player_response;
		while(_playing) {
			_board->draw();
			this->get_user_input();
			
			if(_iss.str() == "quit") _playing = false;
			else this->process_input();
			
		}
	}
	void process_input() {
		std::vector<std::string> tokens;
		std::string token;
		while(_iss >> token) {
			tokens.push_back(token);
			
		}

		if(!_iss.eof()) {
			std::cerr << "Error processing input." << std::endl;
		}
		this->update_board_w_input(tokens);
	}
	void update_board_w_input(const std::vector<std::string>& tokens) {
		// Changes in board happen here.
		if(tokens.size() == 3 && tokens[0] == "move") {
			try {
				_board->move(tokens[1], tokens[2]);
			} catch (const std::exception& e) {
				std::cout << "Error Moving Piece: " << std::endl;
			}
		}
	}
	void toggle_playing() {
		_playing = !_playing;
	}
	void get_user_input() {
		std::string input;
		std::cout << ">: ";
		if(!std::getline(std::cin, input)) {
			std::cerr << "Error reading input." << std::endl;
			std::cin.clear();
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			return get_user_input();
		}
		
		_iss.clear();
		_iss.str(input);
	}
private:
	std::unique_ptr<Board> _board;
	std::istringstream _iss;
	bool _playing;
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
	
	auto rightRook = std::make_shared<Rook>("A1");
	std::cout << rightRook->get_name() << " is at: " << rightRook->get_current_coordinates() << std::endl;
	
	myBoard.add_piece(rightRook);
	
	Chess game_0(myBoard);
	game_0.play();
	
	return 0;
}
