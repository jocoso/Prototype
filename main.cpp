#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <memory>
#include <sstream>
#include <limits>
#include <algorithm>
#if defined(__GNUC__) || defined(__clang__)
#include <climits>
#endif

class Board;
class Chess;
class Piece;

namespace cc {
	struct Coord {
		int bit_idx;
		uint64_t bitmask;
		int x, y;
		int bitboard_idx;
		int local_bit_idx;
	};
	Coord init_coord(int x, int y) {
		Coord c;
		
		c.bit_idx = (y * 8) + x;
		c.bitmask = 1ULL << c.bit_idx; 
		c.x = x;
		c.y = y;
		c.bitboard_idx = c.bit_idx / 64;
		c.local_bit_idx = c.bit_idx % 64;
		
		return c;
	}
	Coord init_coord(int bit_idx) {
		Coord c;
		
		c.bit_idx = bit_idx;
		c.bitmask = 1ULL << c.bit_idx; 
		c.x = c.bit_idx % 8;
		c.y = c.bit_idx / 8;
		c.bitboard_idx = c.bit_idx / 64;
		c.local_bit_idx = c.bit_idx % 64;
		
		return c;
	}
	Coord init_coord(uint64_t bitmask) {
	
		Coord c;
		c.bitmask = bitmask; 
		
		#if defined(__GNUC__) || defined(__clang__)
			c.bit_idx = __builtin_ctzll(c.bitmask);
		#else
		
			c.bit_idx = 0;
			while ((c.bitmask & 1) == 0 && c.bit_idx < 64) {
				c.bitmask >>= 1;
				c.bit_idx++;
			}
		#endif
		
		c.bitboard_idx = c.bit_idx / 64;
		c.local_bit_idx = c.bit_idx % 64;
		c.x = c.bit_idx % 8;
		c.y = c.bit_idx / 8;
		
		return c;
	}
}

class Piece {
public:
	
	Piece(char pce_stamp, std::string pce_name): 
		_stamp(pce_stamp), 
		_name(pce_name), 
		_current_coord(nullptr) {}
	
	void set_attribute(const std::string& attr_name, const std::vector<std::string>& attrs) {
		_attributes[attr_name] = attrs;	
	}
	void assign_new_coord(std::shared_ptr<cc::Coord> coord) {
		_current_coord = std::move(coord);
	}
	std::shared_ptr<cc::Coord> get_current_coord() const {
		return _current_coord;
	}
	
	virtual bool can_move(const std::string& newCoord) = 0; // Later implementation - probably erase.
	virtual ~Piece() {}
	
	char stamp() const {
		return _stamp;
	}
	
	std::string get_name() const {
		return _name;
	}
	
protected:
	std::map<std::string, std::vector<std::string>> _attributes;
	std::shared_ptr<cc::Coord> _current_coord;
	std::string _name;
	char _stamp;
	
		
};

class Board {
public:	
	Board() : _height(8), _width(8), _num_bitboards((_height * _width) / 64) {
		_board.resize(_num_bitboards, 0ULL); // init to 0
		
		
		for(int y = _height ; y >= 0 ; --y) {
			for(int x = 0 ; x < _width ; ++x) {
				cc::Coord coord = cc::init_coord(x, y);
				_pos_map[coord.bitmask] = std::make_shared<cc::Coord>(coord);
			}
		}
	}
	
	void add_piece(Piece* pce_ptr) {
	
		auto coord = pce_ptr->get_current_coord();
		
		int bit_idx = coord->bit_idx;
		int bitboard_idx = bit_idx / 64;
		int local_bit_idx = bit_idx % 64;
		
		_board[bitboard_idx] |= (1ULL << local_bit_idx);
		_m_pces[pce_ptr->get_name()] = pce_ptr;
	}
	
	void draw() {
	
		std::vector<uint64_t> v_occupied_bitmasks;
		for(auto& [name, pce_ptr] : _m_pces) {
			if(auto pce_coord = pce_ptr->get_current_coord()) {
				v_occupied_bitmasks.push_back(pce_coord->bitmask);
			}
		}
		
        			//std::cout << "We are here" << std::endl;
		for(int y = _height - 1 ; y >= 0 ; --y) {
        		for(int x = 0 ; x < _width ; ++x) {
            			cc::Coord coord = cc::init_coord(x, y);
            
            			// Check if any piece occupies this square
            			bool occupied = std::find(
                			v_occupied_bitmasks.begin(),
                			v_occupied_bitmasks.end(),
                			coord.bitmask
            			) != v_occupied_bitmasks.end();
            
            			// Display logic
            			if(occupied) {
                			// Find which piece is here
                			for(auto& [name, pce_ptr] : _m_pces) {
            	        			if(pce_ptr->get_current_coord()->bitmask == coord.bitmask) {
           	         		    		std::cout << "[" << pce_ptr->stamp() << "]";
         	          	  	   		break;
        	            			}
          		      		}
          	  		} else {
          		      		std::cout << "[-]";
           	 		}
        		}
        		std::cout << "\n";
   		}
	
	}
	
	void move(uint64_t from, uint64_t to) {
		_board[0] &= from;
		_board[0] |= to;
	}
	
	Piece *get_piece_by_name(std::string pce_name) {
		if(_m_pces.find(pce_name) != _m_pces.end()) // then
			return _m_pces[pce_name];
		else return nullptr;
	}
private:
	int _height;
	int _width;
	std::vector<uint64_t> _board;
	std::map<uint64_t, std::shared_ptr<cc::Coord>> _pos_map; // Piece's positions in the map. 
	int _num_bitboards;
	std::istringstream _iss;
	std::map<std::string, Piece*> _m_pces;	

		
};

class Chess {
public:
	Chess(std::shared_ptr<Board> board) : _board(std::move(board)), _playing(true) {
	}
	~Chess() = default;
	void play() {
		std::string player_response;
		while(_playing) {
			_board->draw();
			this->get_user_input();
			this->process_input();
		}
	}
	void process_input() {
		std::vector<std::string> tokens;
		std::string token;
		while(_iss >> token) {
			tokens.push_back(token);
			if(token == "quit") _playing = false;
		}

		if(!_iss.eof()) {
			std::cerr << "Error processing input." << std::endl;
		}
		this->update_board_w_input(tokens);
	}
	void update_board_w_input(const std::vector<std::string>& tokens) {
		
	}
	void toggle_playing() {
		_playing = !_playing;
	}
	void get_user_input() {
		std::string input;
		std::cout << "\n>: ";
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
	std::shared_ptr<Board> _board;
	std::istringstream _iss;
	bool _playing;
};

class Rook : public Piece {
public:
	Rook(): Piece('r', "rook") {
		// - movement -
		// From 'this' piece's row and column,
		// Players can select any point in the x-axis or any point in the y-axis
		this->set_attribute("movement", {"this", "anypoint_x v anypoint_y"});
	}
	bool can_move(const std::string& newCoord) override {
		return true;
	}
	
private:
	
};



int main() {
	auto myBoard = std::make_shared<Board>();
	
	Chess game_0(myBoard);
	game_0.play();
	
	return 0;
}
