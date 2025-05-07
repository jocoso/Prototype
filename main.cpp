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
#include <exception>
#include <cassert>
#include <cctype>

class Board;
class Chess;
class Piece;

namespace cc {
	struct BoardCoord {
		int row;
		int col;
		int local_idx;
		std::string alpha;
	};
	
	int calc_local_index(const int x, const int y, const int width) {
		return (y * width) + x;
	}
	
	int alpha_to_x(char alpha) {
		return alpha - 'A';
	}
	int alpha_to_y(char alpha) {
		return '8' - alpha;
	}
	
	int x_to_alpha(int x){
		return x + 'A';
	}
	int y_to_alpha(int y) {
		return '8' - y;
	}
	
	std::string xy_to_alpha(int x, int y) {
		std::string out(2, ' ');
		out[0] = x_to_alpha(x);
		out[1] = y_to_alpha(y);
		return out;
	}
	
	BoardCoord init_boardcoord_alpha(const std::string& alpha, const int width) {
		BoardCoord c;
		
		int x = alpha_to_x(alpha[0]);
		int y = alpha_to_y(alpha[1]);
		
		c.row = x;
		c.col = y;
		c.alpha = alpha;
		c.local_idx = calc_local_index(x, y, width);
		
		return c;
	}
	BoardCoord init_boardcoord_xy(int x, int y, int width) {
		BoardCoord c;
		
		c.row = x;
		c.col = y;
		c.alpha = xy_to_alpha(x, y);
		c.local_idx = calc_local_index(x, y, width);
		
		return c;
	}

}


class Piece {
public:
	Piece(Piece&& other) noexcept :
		_attrs_map(std::move(other._attrs_map)),
		_current_loc(std::move(other._current_loc)),
		_name(std::move(other._name)),
		_symbol(other._symbol)
	{}
	
	Piece& operator=(Piece&& other) noexcept {
		if(this != &other) {
			_attrs_map = std::move(other._attrs_map);
			_current_loc = std::move(other._current_loc);
			_name = std::move(other._name);
			_symbol = other._symbol;
		}
		return *this;
	}
	
	Piece(const Piece& other)
	: _attrs_map(other._attrs_map),
	_name(other._name),
	_symbol(other._symbol) {
		if(other._current_loc) 
			_current_loc = std::make_unique<cc::BoardCoord>(*other._current_loc);
		else _current_loc = nullptr;
	}
	
	// Copy assignment (deep copy)
    Piece& operator=(const Piece& other) {
        if (this != &other) {
            _attrs_map = other._attrs_map;
            _name = other._name;
            _symbol = other._symbol;
            if (other._current_loc) {
                _current_loc = std::make_unique<cc::BoardCoord>(*other._current_loc);
            } else {
                _current_loc.reset();
            }
        }
        return *this;
    }

	
	Piece(char32_t symbol, std::string pce_name): 
		_symbol(symbol), 
		_name(pce_name), 
		_current_loc(nullptr) {}
	
	void set_attribute(const std::string& attr_name, const std::string& attr) {
		_attrs_map[attr_name] = attr;	
	}
	void assign_new_location(cc::BoardCoord& coord) {
		_current_loc = std::make_unique<cc::BoardCoord>(coord);
	}
	cc::BoardCoord &get_location() const {
		return *_current_loc.get();
	}
	const std::map<std::string, std::string>& get_all_attributes() {
		return _attrs_map;
	}
	const std::string& get_attribute(std::string attr_name) {
		static const std::string empty_str = "";
		auto it = _attrs_map.find(attr_name);
		return it != _attrs_map.end() ? it->second : empty_str;// TODO: else it should return a local constant value instead.
	}
	
	char get_symbol() const {
		return _symbol;
	}
	
	
	std::string get_name() const {
		return _name;
	}
	
protected:
	std::map<std::string, std::string> _attrs_map;
	std::unique_ptr<cc::BoardCoord> _current_loc;
	std::string _name;
	char32_t _symbol;
private:
	
		
};




class Board {
public:	
	static constexpr uint64_t ENUMERATED = 0x1;
	
	Board(const int height, const int width, const uint64_t flags) 
	: _height(height), _width(width), _flags(flags), _board_spaces_arr(width * height, '-') {}
	
	void position_piece(std::shared_ptr<Piece> pce, int x, int y) {
		
		
		cc::BoardCoord new_idx_bc = cc::init_boardcoord_xy(x, y, _width);
		_board_spaces_arr[new_idx_bc.local_idx] = pce->get_symbol();
		pce->assign_new_location(new_idx_bc);
		_pces_map[pce->get_symbol()] = pce;
		
	}
	
	void add_piece(Piece &pce) {
		char symbol = pce.get_symbol();
		_pces_map[symbol] = std::make_shared<Piece>(pce); 
	}
	
	int get_width() {
		return _width;
	}
	
	void alpha_enumerate() {
		
		std::cout << "  ";
		for(int x = 0 ; x < _width ; x++) {
			std::cout << " " << static_cast<char>(x + 'A') << " ";
		}
			
		std::cout << '\n';
		
	}
	
	void numeric_enumerate(int y) {
		std::cout << " " << y + 1;
	}
	
	
	void draw() {
	
		bool board_enumerated = this->is_flag_active(Board::ENUMERATED);
		
		// Enumerating top rows.
		if(board_enumerated) {
			alpha_enumerate();
		}
		
		for(int y = _height - 1 ; y >= 0 ; --y) {
			
			// Enumerating top columns.
			if(board_enumerated) numeric_enumerate(y);
			
			for(int x = 0 ; x < _width ; ++x) {
				int idx = y*_width+x;
				std::cout << "[" << static_cast<char>(_board_spaces_arr[idx]) << "]";

			}
			// Enumerating bottom columns.
			if(board_enumerated) numeric_enumerate(y);
			std::cout << '\n';
			
		}
		
		// Enumerating bottom rows.
		if(board_enumerated) {
			alpha_enumerate();
		}
	
	}
	
	void move_piece(Piece* pce, cc::BoardCoord& new_loc) {
		
		int old_idx = pce->get_location().local_idx;
		int new_idx = new_loc.local_idx;
		
		if (old_idx < 0 || old_idx >= _board_spaces_arr.size()) {
        std::cerr << "Error: old_idx " << old_idx << " is out of bounds!" << std::endl;
        return;  // Exit the function early to avoid further errors
    }
    if (new_idx < 0 || new_idx >= _board_spaces_arr.size()) {
        std::cerr << "Error: new_idx " << new_idx << " is out of bounds!" << std::endl;
        return;  // Exit the function early to avoid further errors
    }
		
			_board_spaces_arr[old_idx] = '-'; // Reset old
			_board_spaces_arr[new_idx] = pce->get_symbol();   // Set new
			pce->assign_new_location(new_loc);
		
	}

	const Piece* get_piece_by_symbol(char32_t symbol) {
		auto it = _pces_map.find(symbol);
		if(it != _pces_map.end() && it->second) {
			return it->second.get();
		}
		return nullptr;
	}
	Piece* get_piece_by_name(const std::string& pce_name) {
		for(const auto& pair : _pces_map) {
			if(pair.second && pair.second->get_name() == pce_name) {
				return pair.second.get();
			}
		}
		return nullptr;
	}
	
	void set_enumerate() {
		turn_on(Board::ENUMERATED);
	}
	void clean_enumerate() {
		turn_off(Board::ENUMERATED);
	}
	
private:
	
	std::map<char32_t, std::shared_ptr<Piece>> _pces_map;
	int _height, _width;
	std::vector<char32_t> _board_spaces_arr;
	uint64_t _flags; 
	std::istringstream _iss;
	
	// XXX: Logic
	void turn_off(const uint64_t flag) {
		_flags &= ~flag;
	}
	
	void turn_on(const uint64_t flag) {
		_flags |= flag;
	}
	bool is_flag_active(const uint64_t flag) const {
		return (_flags & flag) != 0;
	}
	
};


class Chess {
public:
	
	Chess() : _board_ptr(std::move(std::make_unique<Board>(8, 8, Board::ENUMERATED))), _playing(true) {
		generate_black_pieces();
		//generate_white_pieces();
	}
	~Chess() = default;
	
	void play() {
		try {
			while(_playing) {
				_board_ptr->draw();
				get_user_input();
				process_input();
			}
		}
		catch(const std::exception& e) {
			std::cout << "Unexpected Error: '" << e.what() << "'\n";
		}
		
	}
	
	void process_input() {
		std::vector<std::string> tokenized_input_lst;
		std::string input;
		
		
		
		while(_iss >> input) {
	
			if(input == "quit") {
				_playing = false; // Bye!
				break;
			}
			std::transform(input.begin(), input.end(), input.begin(), [](unsigned char c) { return std::tolower(c); });
			tokenized_input_lst.push_back(input);
		}
		
		if(_iss.fail() && !_iss.eof()) {
			std::cerr << "Error processing input." << std::endl;
		}
		
		update_board_w_input(tokenized_input_lst);
	}
	
	bool is_valid_move(Piece *pce_ptr, cc::BoardCoord& from, cc::BoardCoord& to) {
		
		// Logic for later.
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
	// This should be a std library.
	std::string to_upper(std::string in) {
	    std::transform(in.begin(), in.end(), in.begin(), [](unsigned char c) {
		return std::toupper(c); // Convert each character to uppercase
	    });
	    return in;
	}
	void update_board_w_input(const std::vector<std::string>& input_lst) {
		if(input_lst.empty()) return;
		
		std::string action_token = input_lst[0];
		
		if(action_token == "move" && input_lst.size() >= 3) {
			std::string pce_name = input_lst[1];
			std::string pce_new_loc = to_upper(input_lst[2]);
			move_piece_action(pce_name, pce_new_loc);
		} 
		else if (action_token == "check" && input_lst.size() >= 2 ) {
			std::string pce_name = input_lst[1];
			//check_action(pce_name); // 
		} 
		else if (action_token == "make" && input_lst.size() >= 2) {
			if(input_lst[1] == "enumerate") {
				_board_ptr->set_enumerate();
			}
		} 
		else if (action_token == "clean" && input_lst.size() >= 2) {
			if(input_lst[1] == "enumerate") {
				_board_ptr->clean_enumerate();
			}
		} 
		else {
			std::cout << "Can't process the command." << std::endl;
		}
	}
	
	void toggle_playing() {
		_playing = !_playing;
	}
	
	/**
	Takes an input from the player and adds it to the istringstream object
	*/
	void get_user_input() {
		std::string input;
		std::cout << "\n>: ";
		if(!std::getline(std::cin, input)) {
			std::cerr << "Error reading input." << std::endl;
			std::cin.clear();
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			return get_user_input(); // Absent input, ask again.
		}
		
		clean_stream();
		_iss.str(input);
	}
private:
	std::shared_ptr<Board> _board_ptr;
	std::istringstream _iss;
	bool _playing;
	
	
	// Pieces generation... 
	void generate_black_pieces() {
	
		auto rook_b_0 = std::make_shared<Piece>(U'♖', "black_rook_0");
		
		// generate_rook_move_logic(rook_b_0); // xxx: Later implementation.
		_board_ptr->position_piece(rook_b_0, 0, 7);
		
		
	}
	
	
	
	/**
	* Clear the Isstringstream off warnings and resets its content.
	*/
	void clean_stream() {
		_iss.clear();
		_iss.str("");
	}
	
	// XXX: Beta.
	//void generate_rook_move_logic(std::shared_ptr<Piece>& pce_ptr) {
		
	//	pce_ptr->set_attribute("movement", "& all_x all_y");
		
	//}
	
	// XXX: LOGIC - BACKGROUND MYSTIC.
	
	/**
	*	Dynamically generates and combines two lists of coordinates based on two logical orders.
	*	input:
	*		coord_order_0 - A set of coordinates represented as a string.
	*		coord_order_1 - Another set of coordinates represented as a string.
	*	output:
	*		A list of BoardCoord objects.
	*/
	//std::vector<Coord>& _and(const std::string& coord_order_0, const std::string& coord_order_1) {
	//	return 
	//}
	
	// XXX: ACTIONS - THINGS TO DO.
	/**
	Transfers a Piece from one previous location to another.
	input:
		pce_name - The name of the piece to move.
		move_to	 - A coordinate point represented as a string.
	*/
	void move_piece_action(const std::string& pce_name_s, const std::string& new_coord_s) {
		auto pce_ptr = _board_ptr->get_piece_by_name(pce_name_s);
		auto new_loc_bc = cc::init_boardcoord_alpha(new_coord_s, _board_ptr->get_width());
			
		if(pce_ptr != nullptr) { // If the piece exists on the board
			
			_board_ptr->move_piece(pce_ptr, new_loc_bc);
			
		} else {	
			std::cerr << "Piece not found: " << pce_name_s << "\n";
		}
	}

	
};


int main() {
	// --> Program begins here.
	Chess game_0;
	game_0.play();
	
	return 0;
}
