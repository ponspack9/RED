
#include "Player.hh"

using namespace std;

#define PLAYER_NAME vELEVEN

struct PLAYER_NAME : public Player {


    /**
     * Factory: returns a new instance of this class.
     * Do not modify this function.
     */
    static Player* factory () {
        return new PLAYER_NAME;
    }

    /**
     * Attributes for your player can be defined here.
     */
    int nr;                                     // number rows
	int nwc;                                    // number window colums
	vector<Dir> all_dirs;
	vector<Dir> all_dirs2;
	vector<Dir> all_dirsN;
	Dir NDEFAULT;
	Dir NDEFAULT2;
	Pos pos_nau1;
	typedef vector< vector <char> > matrix;
	matrix map_obstacles;
	bool last_alive;

    /** WORKING
     * CHECK
     */
    bool check_pos(Pos p){  	    
		CType t = cell(p).type;  
		return (t != STARSHIP and t != ASTEROID and t != MISSILE and p != pos_nau1);    
		}
	bool check_bonus(Pos p){	    CType t = cell(p).type;		return within_window(p,round()) and (t == POINT_BONUS or t == MISSILE_BONUS);	}
	bool check_missile(Pos p){      CType t = cell(p).type;     return within_window(p,round()) and t == MISSILE;}
	bool check_victim(Pos p){
        Cell c = cell(p+DEFAULT);
        if ((within_window(p+DEFAULT,round()) and c.type == STARSHIP)){
            Player_Id pid = player_of(c.sid);
            return pid != me();
        }
        return false;
    }
    bool check_slowup(Pos p){       return 	within_window(p+SLOW_UP,round())    and within_window(p+SLOW_UP,round()+1)   and check_pos(p+SLOW_UP);    }
    bool check_slowdown(Pos p){ 	return 	within_window(p+SLOW_DOWN,round())  and within_window(p+SLOW_DOWN,round()+1) and check_pos(p+SLOW_DOWN);    }
    bool check_default(Pos p){  	return 	within_window(p+DEFAULT,round())    and check_pos(p+DEFAULT);    }
    bool check_up(Pos p){       	return 	within_window(p+UP,round())         and	check_pos(p+SLOW_UP) and check_pos (p+DEFAULT) and check_pos (p+UP);   }
    bool check_down(Pos p){     	return 	within_window(p+DOWN,round())       and check_pos(p+SLOW_DOWN) and check_pos (p+DEFAULT) and	check_pos (p+DOWN);    }
    bool check_fast(Pos p){     	return	within_window(p+FAST,round())       and check_pos(p+FAST) and check_pos(p+DEFAULT);    }
    bool check_fastup(Pos p){   	return 	within_window(p+FAST_UP,round())    and check_pos(p+FAST) and check_up(p) and check_pos(p+FAST_UP);    }
    bool check_fastdown(Pos p){ 	return 	within_window(p+FAST_DOWN,round())  and check_pos(p+FAST) and check_down(p) and check_pos(p+FAST_DOWN);    }
    bool check_ndefault(Pos p){		return 	within_window(p+NDEFAULT,round())   and check_pos(p+NDEFAULT);	}
    bool check_direction(Pos p, Dir d){
        if(d == FAST)               return check_fast(p);
        else if (d == FAST_UP)      return check_fastup(p);
        else if (d == FAST_DOWN)    return check_fastdown(p);
        else if (d == DEFAULT)      return check_default(p);
        else if (d == UP)           return check_up(p);
        else if (d == DOWN)         return check_down(p);
        else if (d == SLOW_DOWN)    return check_slowdown(p);
        else if (d == SLOW_UP)      return check_slowup(p);
        else if (d == NDEFAULT){	return check_ndefault(p);	}
        return true;

    }
	Dir move_random(const Starship& s){
	    Pos p = s.pos;
	    if 	    (check_fast(p)) 	return FAST;
	    else if (check_fastup(p)) 	return FAST_UP;
	    else if (check_fastdown(p)) return FAST_DOWN;
	    else if (check_default(p)) 	return DEFAULT;
	    else if (check_up(p)) 	    return UP;
	    else if (check_down(p)) 	return DOWN;
	    else if (check_slowdown(p)) return SLOW_DOWN;
	    else if (check_slowup(p)) 	return SLOW_UP;
	    else shoot(s.sid);
	    return DEFAULT;

	}
	Dir move_random_updown(const Starship& s){
	    Pos p = s.pos;
	    if (check_fastup(p)) 	    return FAST_UP;
	    else if (check_fastdown(p)) return FAST_DOWN;
	    else if (check_up(p)) 	    return UP;
	    else if (check_down(p)) 	return DOWN;
	    else if (check_slowdown(p)) return SLOW_DOWN;
	    else if (check_slowup(p)) 	return SLOW_UP;
	    else shoot(s.sid);
	    return DEFAULT;

	}
	void restart_camefrom(){
        for (int i=0;i<nr;++i)
            for (int j=0;j<nwc;++j)
                came_from[i][j].came = {-1,-1};
	}
    void print_matrix(){
        cout << endl;
        for (int i=0;i<nr;++i){
            for(int j=0;j<nwc;++j){
                cout << "(" << first(came_from[i][j].came)
                     << "," << second(came_from[i][j].came)
                     << ")" << " ";
            }
            cout << endl;
        }
    }
    bool safe_movement(Pos p, Dir d){
        int far = 3; // DEFAULT
        if(d == SLOW_UP or d == SLOW_DOWN)       far = 2;
        else if (d == UP or d == DOWN)           far = 3;
        else if (d == FAST_UP or d == FAST_DOWN) far = 4;
        else if (d == FAST)                      far = 0;
        for (int i=1;i<=far;++i){
            Dir Far = {0,-i};
            Pos temp = p+d+Far;
            if (check_missile(temp)) {
                int x = first(temp);
                int y = second(temp);
                restart_camefrom();
                came_from[x][y].came = temp;
                return false;
                }
        }
        return true;
    }

    struct DIRandCAME{
        Dir direction;
        Pos came;
    };
    vector < vector<DIRandCAME> > came_from;

    Pos BFS3 (Pos start, const Starship& s, int player){
        //One player go faster, other slower
        vector <Dir> all_dir;
        if (player == 1) all_dir = all_dirs;
        else all_dir = all_dirs2;

        queue<Pos> Q;
        Q.push(start);

        int x = first(start);
        int y = second(start)%nwc;
        came_from[x][y].came = start;
        came_from[x][y].direction = SLOW;

            while(not Q.empty()){
                Pos current = Q.front(); Q.pop();
                x = first(current);
                y = second(current)%nwc;

                if (check_bonus(current)){
                    while(came_from[x][y].came != start){
                        int c = x;
                        x = first(came_from[x][y].came);
                        y = second(came_from[c][y].came)%nwc;
                    }
                    return came_from[x][y].direction;
                }

                for (Dir d : all_dir){
                    if (check_direction(current,d)){
                        Pos next = current+d;
                        x = first(next);
                        y = second(next)%nwc;

                        if (first(came_from[x][y].came) == -1){
                            Q.push(next);
                            came_from[x][y].direction = d;
                            came_from[x][y].came = current;
                        }
                    }
                }
        }
        shoot(s.sid);
        return move_random(s);
    }

    Pos exterminate (Pos start, const Starship& s){

        queue<Pos> Q;
        Q.push(start);

        int x = first(start);
        int y = second(start)%nwc;
        came_from[x][y].came = start;
        came_from[x][y].direction = SLOW;

            while(not Q.empty()){
                Pos current = Q.front(); Q.pop();
                x = first(current);
                y = second(current)%nwc;

                if (check_victim(current)){
                    while(came_from[x][y].came != start){
                        int c = x;
                        x = first(came_from[x][y].came);
                        y = second(came_from[c][y].came)%nwc;
                    }
                    return came_from[x][y].direction;
                }

                for (Dir d : all_dirsN){
                    if (check_direction(current,d)){
                        Pos next = current+d;
                        x = first(next);
                        y = second(next)%nwc;

                        if (first(came_from[x][y].came) == -1){
                            Q.push(next);
                            if (d == NDEFAULT) d = SLOW;
                            came_from[x][y].direction = d;
                            came_from[x][y].came = current;
                        }
                    }
                }
        }
        restart_camefrom();
        return BFS3(start,s,0);
    }
    void scan_map(){
        CType t;
        int ships = 0;
        for (int i=0;i<nr;++i){
            for (int j=0;j<nwc;++j){
                t = cell ({i,j}).type;
                switch (t){
                    case EMPTY:         map_obstacles[i][j] = '.'; break;
                    case ASTEROID:      map_obstacles[i][j] = 'X'; break;
                    case POINT_BONUS:   map_obstacles[i][j] = 'P'; break;
                    case MISSILE_BONUS: map_obstacles[i][j] = 'M'; break;
                    case STARSHIP:      map_obstacles[i][j] = 'S'; ++ships;break;
                    case MISSILE:       map_obstacles[i][j] = '!'; break;
                    default:            map_obstacles[i][j] = '.'; break; // case STARSHIP OR MISSILE
                }
            }
        }
        last_alive = (ships == 1);

	}
    /**
     * Play method.
     *
     * This method will be invoked once per each round.
     * You have to read the board here to place your actions
     * for this round.
     *
     */
virtual void play () {

    if (round() == 0) {
      // Initialize things.
        NDEFAULT = {0,-1};
        NDEFAULT2 = {0,-2};
        nr = number_rows();
        nwc = number_window_columns();
        map_obstacles = matrix (nr, vector<char>(nwc));
        came_from = vector < vector<DIRandCAME> > (nr, vector<DIRandCAME>(nwc));
        all_dirs = {SLOW_DOWN,SLOW_UP,
                    DEFAULT,UP,DOWN,
					FAST,FAST_UP,FAST_DOWN};
        all_dirs2 = {SLOW_UP,SLOW_DOWN,
                    DEFAULT,DOWN,UP,
                    FAST,FAST_DOWN,FAST_UP};
        all_dirsN = {NDEFAULT,FAST,FAST_DOWN,FAST_UP,
                    DEFAULT,DOWN,UP,
                    SLOW_UP,SLOW_DOWN};
    }
    int player = 0;
    scan_map();
    for (Starship_Id sid = begin(me()); sid != end(me()); ++sid){
		if (sid == begin(me())) pos_nau1 = {-3,-3};
        Starship s = starship(sid);
        if (s.alive){
			
        if (round() <= 3 and check_default(s.pos)) move(s.sid,SLOW);
        restart_camefrom();
        Dir d;
        
        if (sid == begin(me()) and s.nb_miss > 0 and not last_alive) d = exterminate(s.pos,s);
        else if (number_point_bonuses() != 0) d = BFS3(s.pos,s,player);
        else d = exterminate(s.pos,s);
        if (check_victim(s.pos) or check_victim(s.pos+DEFAULT) or check_victim(s.pos+FAST)) shoot(s.sid);
        if ((check_missile(s.pos+NDEFAULT) or check_missile(s.pos+NDEFAULT2) ) and
            (d ==DEFAULT or d == FAST or d == SLOW)) d = move_random_updown(s);

        if (sid == begin(me())) pos_nau1 = s.pos + d;
        move(s.sid,d);
        ++player;
        }

    }

}



};


/**
 * Do not modify the following line.
 */
RegisterPlayer(PLAYER_NAME);

