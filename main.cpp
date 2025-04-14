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

struct Coord {
	int bit_idx;
	uint64_t bitmask;
	std::string str;
	
};
Coord generate_coord(const std::string& coord) {
	Coord c;
	
	int y = coord[0] - 'A';
	int x = coord[1] - '1';
	c.bit_idx = (y * 8) + x;
	c.bitmask = 1ULL << c.bit_idx; 
	
	return c;
}


class Piece {
public:
	
	Piece(uint64_t pce_stamp, std::string pce_name, std::string pce_init_coord): _stamp(pce_stamp), _name(pce_name), _current_coord(generate_coord(pce_init_coord)) {}
	
	void set_attribute(const std::string& attr_name, const std::vector<std::string>& attrs) {
		_attributes[attr_name] = attrs;	
	}
	void assign_new_coordinate(std::string coord) {
		_current_coord = generate_coord(coord);
	}
	Coord get_coord() const {
		return _current_coord;
	}
	
	virtual bool can_move(const std::string& newCoord) = 0;
	virtual ~Piece() {}
	
	uint64_t stamp() const {
		return _stamp;
	}
	
	std::string get_name() const {
		return _name;
	}
	
protected:
	std::map<std::string, std::vector<std::string>> _attributes;
	Coord _current_coord;
	std::string _name;
	uint64_t _stamp;
	
		
};

class Board {
public:	
	Board(Piece* piece) : _height(8), _width(8), _num_bitboards((_height * _width) / 64) {
		_board.resize(_num_bitboards, 0ULL); // init to 0
		this->add_piece(piece, piece->get_coord().str);
	}
	
	void add_piece(Piece *pce, std::string new_coord) {
		Coord coord = generate_coord(new_coord);
		
		int bitboard_idx = ( _num_bitboards > 1 ) ? coord.bit_idx / 64 : 0; // If is using one bitboard defaults to idx 0.
		int bit_idx = ( _num_bitboards > 1 ) ? coord.bit_idx % 64 : coord.bit_idx; 
		
		_board[bitboard_idx] |= (1ULL << bit_idx);
		_pieces[pce->get_name()] = pce;
	}
	
	void draw() {
		
		for (int i = _height - 1; i >= 0; --i) {
		std::cout << std::endl;
			for (int j = 0; j < _width; ++j) {
				int bit_idx = (i * _width) + j;
				int bitboard_idx = bit_idx / 64;
				int local_bit_idx = bit_idx % 64;
				
				if((_board[bitboard_idx] >> local_bit_idx) & 1) {
					std::cout << "[+]";
				} else {
					std::cout << "[-]";
				}
			
			}
		}
		std::cout << std::endl;
		
	}
	
	void move(uint64_t from, uint64_t to) {
		_board[0] &= from;
		_board[0] |= to;
	}
	
	Piece *get_piece_by_name(std::string pce_name) {
		if(_pieces.find(pce_name) != _pieces.end()) // then
			return _pieces[pce_name];
		else return nullptr;
	}
private:
	int _height;
	int _width;
	std::vector<uint64_t> _board;
	int _num_bitboards;
	std::map<std::string, Piece*> _pieces;	

		
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
		if(tokens[0] == "move" && (!tokens[1].empty() && !tokens[2].empty())) {
			Piece *pce_ptr = _board->get_piece_by_name(tokens[1]);
			Coord new_coord = generate_coord(tokens[2]);
			
			if(pce_ptr != nullptr) // then
				_board->move(pce_ptr->get_coord().bitmask, new_coord.bitmask);
			else {
				std::cerr << "Piece with name " << pce_ptr->get_name() << " already exists!" << std::endl;
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
	Rook(std::string init_coord): Piece(0xFF00000000000000, "rook", init_coord) {
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
	auto rightRook = std::make_shared<Rook>("A8");
	auto myBoard = std::make_shared<Board>(rightRook.get());
	
	
	Chess game_0(myBoard);
	game_0.play();
	
	return 0;
}
