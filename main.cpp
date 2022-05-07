
#include <pthread.h>
#include <iostream>
#include <assert.h>
#include <unistd.h>
#include <ncurses.h>


using namespace std;


struct Point {
  int x;
  int y;
  Point(int a, int b) { this->x = a; this->y = b; }
  // default
  Point() {this->x = -1; this->y = -1;}
};

// GLOBAL VARIABLES
// Hero starting position
Point hero_pos(20, 9);
Point enemies_positions[5];

// Board size
int board_width = 40;
int board_height = 20;
int number_of_enemies = 5;
int end_game = false; 


// ----------------------------------------------------------------supporting-functions------------------------------------------------------------------

Point get_random_enemy_position(int side){

    // Draw specific position on the side 
    int side_position;

    if((side == 0) || (side == 2)){
        side_position = rand() % (board_height - 2) + 1;

        if(side == 0){
            return Point(1, side_position);
        }
        else{
            return Point(board_width -2 , side_position);
        }
    }
    else{
        side_position = rand() % (board_width - 2) + 1;

        if(side == 1){
            return Point(side_position, 1);
        }
        else{
            return Point(side_position, board_height -2);
        }
    }
};

bool is_the_wall(Point point){
    // check that the poin belongs to any side 
    if( (point.x == 0) || (point.x == (board_width - 1)) || (point.y == 0) || (point.y == (board_height - 1)) ){
        return true;
    }
    return false;
};

Point next_point(Point point, int direction, int start_side){
    // Sides: 0 - left, 1 - top, 2 - right, 3 - botton
    // Directions: 0 - left front, 1 - front , 2 - right front 

    switch(start_side){

        case 0:

            switch(direction){
                case 0:
                    return Point(point.x + 1, point.y - 1);
                    break;
                case 1:
                    return Point(point.x + 1, point.y);
                    break;
                case 2:
                    return Point(point.x + 1, point.y + 1);
                    break;
            }
            break;

        case 1:

            switch(direction){
                case 0:
                    return Point(point.x + 1, point.y + 1);
                    break;
                case 1:
                    return Point(point.x , point.y + 1);
                    break;
                case 2:
                    return Point(point.x - 1, point.y + 1);
                    break;
            }
            break;

        case 2:

            switch(direction){
                case 0:
                    return Point(point.x - 1, point.y + 1);
                    break;
                case 1:
                    return Point(point.x - 1, point.y);
                    break;
                case 2:
                    return Point(point.x - 1, point.y - 1);
                    break;
            }
            break;

        case 3:

            switch(direction){
                case 0:
                    return Point(point.x - 1, point.y - 1);
                    break;
                case 1:
                    return Point(point.x, point.y - 1);
                    break;
                case 2:
                    return Point(point.x + 1, point.y - 1);
                    break;
            }
            break;
    }
    return Point(-1, -1);
}


// -----------------------------------------------------------functions-for-threads-----------------------------------------------------------------


void *enemy_work(void *arguments){

    // the threat index (to update proper point in table)
    int index = *((int *)arguments);

    while(true){

        // draw the direction (0 - left front, 1 - front , 2 - right front) 
        int direction = rand() %3;

        // draw the side to render enemy( 0 - left, 1 - top, 2 - right, 3 - botton)
        int side = rand() % 4; 

        // update table with positions of enemies (starting position)
        enemies_positions[index] = get_random_enemy_position(side);

        while(true){
            usleep(100000);
            // calculate next position 
            Point next_position = next_point(enemies_positions[index], direction, side);
            if(is_the_wall(next_position)){
                enemies_positions[index] = Point(-1,-1);
                break;
            }
            else{
                enemies_positions[index] = next_position;
            }
        }
        // time to return on the board 
        usleep(1000000);
    }

    return NULL;
};


void *window_handle(void *arguments){

    WINDOW *window = (WINDOW*)arguments;

    while(true){

        // We have to remember which points were printed out becouse we'll cover them sign (" ") after some time
        Point enemies_positions_to_disp[5];
        Point hero_position_to_disp = hero_pos;

        // print enemies
        for(int i = 0 ; i < number_of_enemies ; i++){
            // store position of printing enemy
            enemies_positions_to_disp[i] = enemies_positions[i];
            // print enemy
            mvwprintw(window, enemies_positions_to_disp[i].y, enemies_positions_to_disp[i].x, "E");
        }
        // print hero
        mvwprintw(window, hero_position_to_disp.y, hero_position_to_disp.x, "H");
        // refresh window to see result
        wrefresh(window);
        // wait some time to see result (Note: without this also works but I think we have bigger resource consumption)
        usleep(1000);
        
        // covering printed out signs via " "
        for(int i = 0 ; i < number_of_enemies ; i++){
            mvwprintw(window, enemies_positions_to_disp[i].y, enemies_positions_to_disp[i].x, " ");
        }
        mvwprintw(window, hero_position_to_disp.y, hero_position_to_disp.x, " ");
    }

    return NULL;
}



// --------------------------------------------------------------MAIN----------------------------------------------------------------------

int main(){
   
    initscr();

        // screen settings
        curs_set(0);    // don't display cursor
        noecho();       // don't write at console        

        // the last two values define the left top corner of board (y, x)
        WINDOW *win = newwin(board_height, board_width, 0, 60);

        // make border
        box(win, 0, 0);
        // game title print
        mvwprintw(win, 0, 10, "AVOID ENEMIES GAME");

        // Create enemies threads and start them
        pthread_t enemies_threads[number_of_enemies];
        int enemies_threads_args[number_of_enemies];
        int result_code;
        
        for (int i = 0; i < number_of_enemies; i++) {
            enemies_threads_args[i] = i;
            result_code = pthread_create(&enemies_threads[i], NULL, enemy_work, &enemies_threads_args[i]);
            assert(!result_code);
        }

        // Create next thread to handle printing out signs at board
        pthread_t window_handler;
        result_code = pthread_create(&window_handler, NULL, window_handle, win);
        assert(!result_code);
        
        int btn_ascii;
        // capture the w,s,a,d buttons upon close the game via ESC button 
        // ASCII: w -> 119, s -> 115, a -> 97, d -> 100
        while( (btn_ascii = getch()) != 27 ){

            // make operations only when correct button has been pressed
            if( (btn_ascii == 119)||(btn_ascii == 115)||(btn_ascii == 97)||(btn_ascii == 100) ){

                switch(btn_ascii){
                    
                    case 97:
                        hero_pos.x--;
                        if( hero_pos.x < 1 ) hero_pos.x = 1;
                        break;

                    case 100:
                        hero_pos.x++;
                        if( hero_pos.x > board_width -2 ) hero_pos.x = board_width -2;
                        break;

                    case 119:
                        hero_pos.y--;
                        if( hero_pos.y < 1 ) hero_pos.y = 1;
                        break;

                    case 115:
                        hero_pos.y++;
                        if( hero_pos.y > board_height -2 ) hero_pos.y = board_height -2;
                        break;
                }
            }
        }

    endwin();
    return 0;

}