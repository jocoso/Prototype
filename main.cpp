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
	uint64_t xycoord_to_bitmask(int x, int y) {
		return 1ULL << ((y*8) + x);
	}
	uint64_t strcoord_to_bitmask(const std::string& coord) {
		if(coord.length() != 2) return 0;
	
		int y = coord[0] - 'A';
		int x = coord[1] - '1';
		
		return xycoord_to_bitmask(x, y);
	}
}


class Piece {
public:
	
	Piece(char pce_stamp, std::string symbol, std::string pce_name): 
		_stamp(pce_stamp),
		_symbol(symbol), 
		_name(pce_name), 
		_current_coord(nullptr) {}
	
	void set_attribute(const std::string& attr_name, const std::string attr_logic) {
		_m_attrs[attr_name] = attr_logic;	
	}
	void assign_new_coord(std::shared_ptr<cc::Coord> coord) {
		_current_coord = std::move(coord);
	}
	std::shared_ptr<cc::Coord> get_current_coord() const {
		return _current_coord;
	}
	const std::map<std::string, std::string>& get_attributes_map() {
		return _m_attrs;
	}
	const std::string get_attribute(std::string attr_name) {
		return _m_attrs.count(attr_name) ? _m_attrs[attr_name] : "";// TODO: else it should return a local constant value instead.
	}
	
	char stamp() const {
		return _stamp;
	}
	
	std::string get_symbol() const {
		return _symbol;
	}
	
	
	std::string get_name() const {
		return _name;
	}
	
protected:
	std::map<std::string, std::string> _m_attrs;
	std::shared_ptr<cc::Coord> _current_coord;
	std::string _name;
	std::string _symbol;
	char _stamp;
private:
	
		
};




class Board {
public:	
	static constexpr uint64_t ENUMERATED = 0x1;
	Board(const int height, const int width, const uint64_t flag) : _height(height), _width(width), _num_bitboards((height * width) / 64) {
		_v_board.resize(_num_bitboards, 0ULL); // init to 0
		
		if(flag == Board::ENUMERATED) this->enumerate();
		
		for(int y = _height ; y >= 0 ; --y) {
			for(int x = 0 ; x < _width ; ++x) {
				cc::Coord coord = cc::init_coord(x, y);
				_m_pos[coord.bitmask] = std::make_shared<cc::Coord>(coord);
			}
		}
		
		
	}
	
	void add_piece(std::shared_ptr<Piece> pce_ptr, int x, int y) {
	
		auto coord = std::make_shared<cc::Coord>(cc::init_coord(x, y));
		pce_ptr->assign_new_coord(coord);
		
		_v_board[coord->bitboard_idx] |= coord->bitmask;
		_m_pces[pce_ptr->get_name()] = pce_ptr;
	}
	
	void draw() {
	
		uint64_t all_occupancies = _v_board[0];
		int x, y;
		
		if(this->needs_enumeration()) {
			std::cout << "  ";
			for(x = 1 ; x <= _width ; ++x) {
				std::cout << " " << x << ' ';
			}
			
			std::cout << '\n';
		}
		
		for(y = _height - 1 ; y >= 0 ; --y) {
			
			if(this->needs_enumeration()) std::cout << (char) (y + 'A') << " ";
			
			for(x = 0 ; x < _width ; ++x) {
				cc::Coord coord = cc::init_coord(x, y);
				_m_pos[coord.bitmask] = std::make_shared<cc::Coord>(coord);
				
				bool has_piece = (_v_board[coord.bitboard_idx] & coord.bitmask) != 0;
				
				
				if(has_piece) {
				
					for(auto& [pce_name, pce_ptr] : _m_pces) {
						if(pce_ptr->get_current_coord()->bitmask == coord.bitmask) {
							std::cout << "[" << pce_ptr->get_symbol() << "]";
							break;
						}
					}
				} else {
					std::cout << "[-]";
				}
			}
			if(this->needs_enumeration()) std::cout << " " << (char) (y + 'A') ;
			std::cout << '\n';
		}
		
		if(this->needs_enumeration()) {
			std::cout << "  ";
			for(x = 1 ; x <= _width ; ++x) {
				std::cout << " " << x << ' ';
			}
			
			std::cout << '\n';
		}
	
	}
	
	void enumerate() {
		this->set_enumeration_flag();
	}
	
	void clean_enumerate() {
		this->clear_enumeration_flag();
	}
	
	void move_piece(uint64_t from, uint64_t to) {
		int from_idx = cc::init_coord(from).bitboard_idx;
		int to_idx = cc::init_coord(to).bitboard_idx;
		
		_v_board[from_idx] &= ~from;
		_v_board[to_idx] |= to;
		
		// Update the pieces.
		for(auto& [name, pce_ptr] : _m_pces) {
			if(pce_ptr->get_current_coord()->bitmask == from) {
				auto new_coord = std::make_shared<cc::Coord>(cc::init_coord(to));
				pce_ptr->assign_new_coord(new_coord);
				break;
			}
		}
		
		
	}
	
	std::shared_ptr<Piece> get_piece_by_name(std::string pce_name) {
		return _m_pces.count(pce_name) ? _m_pces[pce_name] : nullptr;
	}
private:
	int _height, _width;
	std::vector<uint64_t> _v_board;
	uint64_t _flags;
	std::map<uint64_t, std::shared_ptr<cc::Coord>> _m_pos; // Piece's positions in the map. 
	int _num_bitboards;
	std::istringstream _iss;
	std::map<std::string, std::shared_ptr<Piece>> _m_pces;	
	
	// XXX: Logic
	void move_bitmap(uint64_t from, uint64_t to) {
		_v_board[0] &= ~from;
		_v_board[0] |= to;
	}
	
	void set_enumeration_flag() {
		_flags |= ENUMERATED;
	}
	
	void clear_enumeration_flag() {
		_flags &= ~ENUMERATED;
	}
	bool needs_enumeration() const {
		return (_flags & ENUMERATED) != 0;
	}
	
	
};


class Chess {
public:
	Chess(std::shared_ptr<Board> board) : _board_ptr(std::move(board)), _playing(true) {
		
		this->generate_black_pieces();
		this->generate_white_pieces();
		
	}
	
	~Chess() = default;
	void play() {
		std::string player_response;
		while(_playing) {
			_board_ptr->draw();
			this->get_user_input();
			this->process_input();
		}
	}
	/**
	*	Takes an updated `v_tokens` and adds logic to its content via a rudimentary system
	*	of if statements designed to guard the system's "generic" features s.a. "quit"
	*	before passing the given tokens to local function `update_board_w_input`.
	*	
	*	input:
	*		void
	*	output:
	*		 void
	*/
	void process_input() {
		std::vector<std::string> v_tokens;
		std::string token;
		while(_iss >> token) {
			v_tokens.push_back(token);
			if(token == "quit") _playing = false; // Bye!
		}
		if(!_iss.eof()) {
			std::cerr << "Error processing input." << std::endl;
		}
		this->update_board_w_input(v_tokens);
	}
	bool is_valid_move(Piece *pce_ptr, uint64_t from, uint64_t to) {
		auto from_coord = cc::init_coord(from);
		auto to_coord = cc::init_coord(to);
		
		std::string rule = pce_ptr->get_attribute("movement");
		return true;
	}
	/**
	*	Updates the board based on the user's input...
	*	for actions relevant to the game universe, actions s.a.: "move", "analyze", "run", etc...
	*	input: 
	*		tokens - A series of strings containing user input separated by space using `split`.
	*	output:
	*		void
	*/
	void update_board_w_input(const std::vector<std::string>& tokens) {
		if(tokens.empty()) return;
		
		std::string action_token = tokens[0];
		
		if(action_token == "move" && tokens.size() >= 3) {
			this->move_piece_action(tokens[1], tokens[2]);
		} else if (action_token == "make" && tokens.size() >= 2) {
			if(tokens[1] == "enumerate") {
				_board_ptr->enumerate();
			}
		} else if (action_token == "clean" && tokens.size() >= 2) {
			if(tokens[1] == "enumerate") {
				_board_ptr->clean_enumerate();
			}
		}
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
		
		this->clean_stream();
		_iss.str(input);
	}
private:
	std::shared_ptr<Board> _board_ptr;
	std::istringstream _iss;
	bool _playing;
	// 
	void generate_black_pieces() {
	
		auto rook_b_0 = std::make_shared<Piece>('r', "♖", "black_rook_0");
		auto knight_b_0 = std::make_shared<Piece>('k', "♘", "black_knight_0"); 
		auto bishop_b_0 = std::make_shared<Piece>('t', "♗", "black_bishop_0"); 
		auto queen_b = std::make_shared<Piece>('q', "♕", "black_queen"); 
		auto king_b = std::make_shared<Piece>('k', "♔", "black_king"); 
		auto bishop_b_1 = std::make_shared<Piece>('t', "♗", "black_bishop_1"); 
		auto knight_b_1 = std::make_shared<Piece>('k', "♘", "black_knight_1"); 
		auto rook_b_1 = std::make_shared<Piece>('r', "♖", "black_rook_1");
		auto peon_b_0 = std::make_shared<Piece>('p', "♙", "black_peon_0");
		auto peon_b_1 = std::make_shared<Piece>('p', "♙", "black_peon_1");
		auto peon_b_2 = std::make_shared<Piece>('p', "♙", "black_peon_2");
		auto peon_b_3 = std::make_shared<Piece>('p', "♙", "black_peon_3");
		auto peon_b_4 = std::make_shared<Piece>('p', "♙", "black_peon_4");
		auto peon_b_5 = std::make_shared<Piece>('p', "♙", "black_peon_5");
		auto peon_b_6 = std::make_shared<Piece>('p', "♙", "black_peon_6");
		auto peon_b_7 = std::make_shared<Piece>('p', "♙", "black_peon_7");
		this->generate_rook_move_logic(rook_b_0);
		_board_ptr->add_piece(rook_b_0, 0, 0);
		_board_ptr->add_piece(knight_b_0, 1, 0);
		_board_ptr->add_piece(bishop_b_0, 2, 0);
		_board_ptr->add_piece(queen_b, 3, 0);
		_board_ptr->add_piece(king_b, 4, 0);
		_board_ptr->add_piece(bishop_b_1, 5, 0);
		_board_ptr->add_piece(knight_b_1, 6, 0);
		_board_ptr->add_piece(rook_b_1, 7, 0);
		_board_ptr->add_piece(peon_b_0, 0, 1);
		_board_ptr->add_piece(peon_b_1, 1, 1);
		_board_ptr->add_piece(peon_b_2, 2, 1);
		_board_ptr->add_piece(peon_b_3, 3, 1);
		_board_ptr->add_piece(peon_b_4, 4, 1);
		_board_ptr->add_piece(peon_b_5, 5, 1);
		_board_ptr->add_piece(peon_b_6, 6, 1);
		_board_ptr->add_piece(peon_b_7, 7, 1);
		
	}
	
	void generate_white_pieces() {
		auto rook_w_0 = std::make_shared<Piece>('R', "♜", "white_rook_0");
		auto knight_w_0 = std::make_shared<Piece>('K', "♞", "white_knight_0"); 
		auto bishop_w_0 = std::make_shared<Piece>('T', "♝", "white_bishop_0"); 
		auto queen_w = std::make_shared<Piece>('Q', "♛", "white_queen"); 
		auto king_w = std::make_shared<Piece>('K', "♚", "white_king"); 
		auto bishop_w_1 = std::make_shared<Piece>('T', "♝", "white_bishop_1"); 
		auto knight_w_1 = std::make_shared<Piece>('K', "♞", "white_knight_1"); 
		auto rook_w_1 = std::make_shared<Piece>('R', "♜", "white_rook_1");
		auto peon_w_0 = std::make_shared<Piece>('P', "♟", "white_peon_0");
		auto peon_w_1 = std::make_shared<Piece>('P', "♟", "white_peon_1");
		auto peon_w_2 = std::make_shared<Piece>('P', "♟", "white_peon_2");
		auto peon_w_3 = std::make_shared<Piece>('P', "♟", "white_peon_3");
		auto peon_w_4 = std::make_shared<Piece>('P', "♟", "white_peon_4");
		auto peon_w_5 = std::make_shared<Piece>('P', "♟", "white_peon_5");
		auto peon_w_6 = std::make_shared<Piece>('P', "♟", "white_peon_6");
		auto peon_w_7 = std::make_shared<Piece>('P', "♟", "white_peon_7");
		_board_ptr->add_piece(rook_w_0, 0, 7);		
		_board_ptr->add_piece(knight_w_0, 1, 7);
		_board_ptr->add_piece(bishop_w_0, 2, 7);
		_board_ptr->add_piece(queen_w, 3, 7);
		_board_ptr->add_piece(king_w, 4, 7);
		_board_ptr->add_piece(bishop_w_1, 5, 7);
		_board_ptr->add_piece(knight_w_1, 6, 7);
		_board_ptr->add_piece(rook_w_1, 7, 7);
		_board_ptr->add_piece(peon_w_0, 0, 6);		
		_board_ptr->add_piece(peon_w_1, 1, 6);
		_board_ptr->add_piece(peon_w_2, 2, 6);
		_board_ptr->add_piece(peon_w_3, 3, 6);
		_board_ptr->add_piece(peon_w_4, 4, 6);
		_board_ptr->add_piece(peon_w_5, 5, 6);
		_board_ptr->add_piece(peon_w_6, 6, 6);
		_board_ptr->add_piece(peon_w_7, 7, 6);
		
	}
	
	void clean_stream() {
		_iss.clear();
		_iss.str("");
	}
	
	void generate_rook_move_logic(std::shared_ptr<Piece>& pce_ptr) {
		
		pce_ptr->set_attribute("movement", "& all_x all_y");
	}
	
	// XXX: LOGIC 
	
	/**
	*	Dynamically generates and combines two lists of coordinates based on two logical orders.
	*	input:
	*		coord_order_0 - A set of coordinates represented as a logical string.
	*		coord_order_y - Another set of coordinates, the same as coord_order_0.
	*	output:
	*		A list of Coord objects.
	*/
	//std::vector<Coord>& _and(const std::string& coord_order_0, const std::string& coord_order_1) {
	//	return 
	//}
	
	// XXX: ACTIONS
	void move_piece_action(const std::string& pce_name, const std::string& to) {
		std::shared_ptr<Piece> pce_ptr = _board_ptr->get_piece_by_name(pce_name);
			
		if(pce_ptr) { // If the piece exists on the board
			uint64_t target_bitmask = cc::strcoord_to_bitmask(to);
			uint64_t current_bitmask = pce_ptr->get_current_coord()->bitmask;	
			if(this->is_valid_move(pce_ptr.get(), current_bitmask, target_bitmask)) {
				_board_ptr->move_piece(current_bitmask, target_bitmask);
			} else {
				std::cerr << "Illegal move!\n";
			}
		} else {	
			std::cerr << "Piece not found: " << pce_name << "\n";
		}
	}
};


int main() {
	auto myBoard = std::make_shared<Board>(8, 8, Board::ENUMERATED);
	
	Chess game_0(myBoard);
	game_0.play();
	
	return 0;
}
