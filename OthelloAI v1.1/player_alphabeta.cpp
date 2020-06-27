#include <cstdio>
#include <iostream>
#include <fstream>
#include <array>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#define DEPTH 2
#define INF 2147483647

using namespace std;

struct Point {
    int x, y;
	Point() : Point(0, 0) {}
	Point(int x, int y) : x(x), y(y) {}
	bool operator==(const Point& rhs) const {
		return x == rhs.x && y == rhs.y;
	}
	bool operator!=(const Point& rhs) const {
		return !operator==(rhs);
	}
	Point operator+(const Point& rhs) const {
		return Point(x + rhs.x, y + rhs.y);
	}
	Point operator-(const Point& rhs) const {
		return Point(x - rhs.x, y - rhs.y);
	}
};

int player;
int n_valid_spots;
const int SIZE = 8;
std::vector<Point> next_valid_spots;

class OthelloBoard {
public:
    enum SPOT_STATE {
        EMPTY = 0,
        BLACK = 1,
        WHITE = 2
    };
    static const int SIZE = 8;
    const std::array<Point, 8> directions{{
        Point(-1, -1), Point(-1, 0), Point(-1, 1),
        Point(0, -1), /*{0, 0}, */Point(0, 1),
        Point(1, -1), Point(1, 0), Point(1, 1)
    }};
    std::array<std::array<int, SIZE>, SIZE> board;
    std::vector<Point> next_valid_spots;
    std::array<int, 3> disc_count;
    int cur_player;
    bool done;
    int winner;
    Point next_disc;
    int heuristic;
private:
    int get_next_player(int player) const {
        return 3 - player;
    }
    bool is_spot_on_board(Point p) const {
        return 0 <= p.x && p.x < SIZE && 0 <= p.y && p.y < SIZE;
    }
    int get_disc(Point p) const {
        return board[p.x][p.y];
    }
    void set_disc(Point p, int disc) {
        board[p.x][p.y] = disc;
    }
    bool is_disc_at(Point p, int disc) const {
        if (!is_spot_on_board(p))
            return false;
        if (get_disc(p) != disc)
            return false;
        return true;
    }
    bool is_spot_valid(Point center) const {
        if (get_disc(center) != EMPTY)
            return false;
        for (Point dir: directions) {
            // Move along the direction while testing.
            Point p = center + dir;
            // Check whether there exists opponent's piece between the new line
            if (!is_disc_at(p, get_next_player(cur_player)))
                continue;
            p = p + dir;
            // Check whether the new line can be made
            while (is_spot_on_board(p) && get_disc(p) != EMPTY) {
                if (is_disc_at(p, cur_player))
                    return true;
                p = p + dir;
            }
        }
        return false;
    }
    void flip_discs(Point center) {
        for (Point dir: directions) {
            // Move along the direction while testing.
            Point p = center + dir;
            if (!is_disc_at(p, get_next_player(cur_player)))
                continue;
            std::vector<Point> discs({p});
            p = p + dir;
            while (is_spot_on_board(p) && get_disc(p) != EMPTY) {
                if (is_disc_at(p, cur_player)) {
                    for (Point s: discs) {
                        set_disc(s, cur_player);
                    }
                    disc_count[cur_player] += discs.size();
                    disc_count[get_next_player(cur_player)] -= discs.size();
                    break;
                }
                discs.push_back(p);
                p = p + dir;
            }
        }
    }
public:
    OthelloBoard() {
        reset();
    }
    OthelloBoard(const OthelloBoard &rhs) {
        for (int i = 0; i < SIZE; i++)
            for (int j = 0; j < SIZE; j++)
                board[i][j] = rhs.board[i][j];

//        next_valid_spots.clear();
//        for (auto i : rhs.next_valid_spots)
//            next_valid_spots.push_back(i);
        next_valid_spots = rhs.next_valid_spots;

        for (int i = 0; i < 3; i++)
            disc_count[i] = rhs.disc_count[i];
        cur_player = rhs.cur_player;
        done = rhs.done;
        winner = rhs.winner;
        next_disc = rhs.next_disc;
        heuristic = rhs.heuristic;
    }
    // modify
    OthelloBoard& operator=(const OthelloBoard &rhs){
        for (int i = 0; i < SIZE; i++)
            for (int j = 0; j < SIZE; j++)
                board[i][j] = rhs.board[i][j];

//        next_valid_spots.clear();
//        for (auto i : rhs.next_valid_spots)
//            next_valid_spots.push_back(i);
        next_valid_spots = rhs.next_valid_spots;

        for (int i = 0; i < 3; i++)
            disc_count[i] = rhs.disc_count[i];
        cur_player = rhs.cur_player;
        done = rhs.done;
        winner = rhs.winner;
        next_disc = rhs.next_disc;
        heuristic = rhs.heuristic;
        return *this;
    }
    bool operator<(const OthelloBoard &rhs) const{
        return heuristic < rhs.heuristic;
    }
    bool operator>(const OthelloBoard &rhs) const{
        return heuristic > rhs.heuristic;
    }
    bool operator==(const OthelloBoard &rhs) const{
        return heuristic == rhs.heuristic;
    }
    bool operator<=(const OthelloBoard &rhs) const{
        return heuristic <= rhs.heuristic;
    }
    bool operator>=(const OthelloBoard &rhs) const{
        return heuristic >= rhs.heuristic;
    }
    void set_heuristic(){
        // modify
        if(done){
            if(winner == player)
                heuristic = disc_count[player] - disc_count[get_next_player(player)] + 100;
            else
                heuristic = disc_count[player] - disc_count[get_next_player(player)] - 100;
        }
        heuristic = disc_count[cur_player] - disc_count[get_next_player(cur_player)];
    }
    //
    void reset() {
        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                board[i][j] = EMPTY;
            }
        }
        board[3][4] = board[4][3] = BLACK;
        board[3][3] = board[4][4] = WHITE;
        cur_player = BLACK;
        disc_count[EMPTY] = 8*8-4;
        disc_count[BLACK] = 2;
        disc_count[WHITE] = 2;
        next_valid_spots = get_valid_spots();
        done = false;
        winner = -1;
        next_disc = Point(-1, -1);
        set_heuristic();
    }
    std::vector<Point> get_valid_spots() const {
        std::vector<Point> valid_spots;
        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                Point p = Point(i, j);
                if (board[i][j] != EMPTY)
                    continue;
                if (is_spot_valid(p))
                    valid_spots.push_back(p);
            }
        }
        return valid_spots;
    }
    bool put_disc(Point p) {
        if(!is_spot_valid(p)) {
            winner = get_next_player(cur_player);
            done = true;
            return false;
        }
        set_disc(p, cur_player);
        disc_count[cur_player]++;
        disc_count[EMPTY]--;
        flip_discs(p);
        // Give control to the other player.
        cur_player = get_next_player(cur_player);
        next_valid_spots = get_valid_spots();
        // Check Win
        if (next_valid_spots.size() == 0) {
            cur_player = get_next_player(cur_player);
            next_valid_spots = get_valid_spots();
            if (next_valid_spots.size() == 0) {
                // Game ends
                done = true;
                int white_discs = disc_count[WHITE];
                int black_discs = disc_count[BLACK];
                if (white_discs == black_discs) winner = EMPTY;
                else if (black_discs > white_discs) winner = BLACK;
                else winner = WHITE;
            }
        }
        set_heuristic();
        return true;
    }
};

OthelloBoard cur_OthelloBoard;

void read_board(std::ifstream& fin) {
    fin >> player;
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            fin >> cur_OthelloBoard.board[i][j];
        }
    }
}

void read_valid_spots(std::ifstream& fin) {
    fin >> n_valid_spots;
    int x, y;
    for (int i = 0; i < n_valid_spots; i++) {
        fin >> x >> y;
        cur_OthelloBoard.next_valid_spots.push_back({x, y});
    }
}

OthelloBoard minimax(OthelloBoard &cur_state, int depth, \
                     OthelloBoard alpha, OthelloBoard beta, bool isMax){
    // modify
    if(cur_state.done || depth == 0){
        return cur_state;
    }

    OthelloBoard val;
    if(isMax){
        printf("isMax: %d\n", depth);
        val.heuristic = -INF;
        for(auto i : cur_state.next_valid_spots){
            OthelloBoard next_state = cur_state;
            next_state.put_disc(i);
            OthelloBoard next_val = minimax(next_state, depth - 1, alpha, beta, false);
            if(val < next_val){
                val = next_val;
                cur_state.next_disc = i;
            }
            if(alpha < val)
                alpha = val;
            if(beta <= alpha)
                break;
        }
    }
    else{
        printf("isMin: %d\n", depth);
        val.heuristic = INF;
        for(auto i : cur_state.next_valid_spots){
            OthelloBoard next_state = cur_state;
            next_state.put_disc(i);
            OthelloBoard next_val = minimax(next_state, depth - 1, alpha, beta, true);
            if(val > next_val){
                val = next_val;
                cur_state.next_disc = i;
            }
            if(beta > val)
                beta = val;
            if(beta <= alpha)
                break;
        }
    }
    return val;
}
void write_valid_spot(std::ofstream& fout) {
    srand(time(NULL));

    Point p = cur_OthelloBoard.next_valid_spots[0];
    fout << p.x << " " << p.y << std::endl;
    fout.flush();


    OthelloBoard alpha, beta;
    alpha.heuristic = -INF;
    beta.heuristic = INF;
    OthelloBoard next_board = minimax(cur_OthelloBoard, DEPTH, alpha, beta, true);
    p = cur_OthelloBoard.next_disc;

    // Remember to flush the output to ensure the last action is written to file.
    fout << p.x << " " << p.y << std::endl;
    fout.flush();
}

int main(int, char** argv) {
    std::cout << "player\n";
    std::ifstream fin(argv[1]);
    std::ofstream fout(argv[2]);
    cur_OthelloBoard.next_valid_spots.clear();
    read_board(fin);
    read_valid_spots(fin);
    cur_OthelloBoard.cur_player = player;
    cur_OthelloBoard.set_heuristic();

    write_valid_spot(fout);
    fin.close();
    fout.close();
    return 0;
}
