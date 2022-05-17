
#include <pthread.h>
#include <iostream>
#include <assert.h>
#include <unistd.h>
#include <ncurses.h>


using namespace std;

// -----------Structures---------

struct Point {
  int x;
  int y;
  Point(int a, int b) { this->x = a; this->y = b; }
  // default
  Point() {this->x = -1; this->y = -1;}
};

// -------GLOBAL VARIABLES-------

const int number_of_enemies = 20;
int end_game = false; 

Point hero_pos(20, 9);                          // Hero starting position
Point enemies_positions[number_of_enemies];     // Array to store positions of enemies

int board_width = 40;                           // Board size
int board_height = 20;
int scores = 0;

// ----------------------------------------------------------------supporting-functions------------------------------------------------------------------

void restart_global_data(){

    hero_pos = Point(20, 9);
    for(int i = 0 ; i< number_of_enemies ; i++){
        enemies_positions[i] = Point();
    }
    end_game = false;
    scores = 0;
}



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
    // Check that the point belongs to any side 
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

void print_end_game(WINDOW* window){


    // Print blur at hero position
    mvwprintw(window, hero_pos.y + 1, hero_pos.x + 1, " ");
    mvwprintw(window, hero_pos.y - 1, hero_pos.x - 1, " ");
    mvwprintw(window, hero_pos.y + 1, hero_pos.x - 1, " ");
    mvwprintw(window, hero_pos.y - 1, hero_pos.x + 1, " ");
    mvwprintw(window, hero_pos.y + 1, hero_pos.x, "#");
    mvwprintw(window, hero_pos.y - 1, hero_pos.x, "#");
    mvwprintw(window, hero_pos.y, hero_pos.x - 1, "#");
    mvwprintw(window, hero_pos.y, hero_pos.x + 1, "#");
    mvwprintw(window, hero_pos.y, hero_pos.x, "#");

    // Game over in center
    mvwprintw(window, 8, 9,  "  ------------------  ");
    mvwprintw(window, 9, 9,  "  -- GAME OVER!!! --  ");
    mvwprintw(window, 10, 9, "  ------------------  ");
    wrefresh(window);

    // Restart global data (prepare for possible game restart)
    restart_global_data();
    usleep(500000);

    // Print finish or restart message
    wclear(window);
    box(window, 0, 0);
    mvwprintw(window, 0, 10, " AVOID ENEMIES GAME ");
    mvwprintw(window, 8, 8, " Press SPACE to Restart! ");
    mvwprintw(window, 9, 9, " Press ESC to Finish! ");
    wrefresh(window);

}

// -----------------------------------------------------------functions-for-threads-----------------------------------------------------------------


void *enemy_work(void *arguments){

    // The threat index (to update proper point in table)
    int index = *((int *)arguments);

    while(true){

        // Draw the direction (0 - left front, 1 - front , 2 - right front) 
        int direction = rand() %3;

        // Draw the side to render enemy( 0 - left, 1 - top, 2 - right, 3 - botton)
        int side = rand() % 4; 

        // Update table with positions of enemies (starting position)
        enemies_positions[index] = get_random_enemy_position(side);

        while(true){

            // Time to next move
            usleep(100000);

            // Calculate next position 
            Point next_position = next_point(enemies_positions[index], direction, side);
            if(is_the_wall(next_position)){
                enemies_positions[index] = Point(-1,-1);
                break;
            }
            else{
                enemies_positions[index] = next_position;
            }
        }

        // Time to return on the board 
        usleep(1000000);
    }

    return NULL;
};


void *window_handle(void *arguments){

    WINDOW *window = (WINDOW*)arguments;
    string scoresAsString;

    while(true){

        // We have to remember which points were printed out becouse we'll cover them sign (" ") after some time
        Point enemies_positions_to_disp[number_of_enemies];
        Point hero_position_to_disp = hero_pos;

        // Print enemies
        for(int i = 0 ; i < number_of_enemies ; i++){
            // Store position of printing enemy
            enemies_positions_to_disp[i] = enemies_positions[i];
            // Print enemy
            mvwprintw(window, enemies_positions_to_disp[i].y, enemies_positions_to_disp[i].x, "E");
        }
        // Print hero
        mvwprintw(window, hero_position_to_disp.y, hero_position_to_disp.x, "H");
        // Print score
        scoresAsString = " Score: " + to_string(scores) + " ";
        mvwprintw(window, 19, 15, scoresAsString.c_str());
        // Refresh window to see result
        wrefresh(window);
        // Wait some time to see result (Note: this value is important and can't be to small because without this the window'll not displaying properly )
        usleep(10000);
        
        // Covering printed out signs via " "
        for(int i = 0 ; i < number_of_enemies ; i++){
            mvwprintw(window, enemies_positions_to_disp[i].y, enemies_positions_to_disp[i].x, " ");
        }
        mvwprintw(window, hero_position_to_disp.y, hero_position_to_disp.x, " ");
    }

    return NULL;
}

void *check_collidation_with_enemies(void *arguments){

    WINDOW *window = (WINDOW*)arguments;

    while(true){

        for(int i = 0 ; i < number_of_enemies ; i++){

            if( enemies_positions[i].x == hero_pos.x && enemies_positions[i].y == hero_pos.y ){
                // When collision -> end game
                end_game = true;
                return NULL;
            }
        }
    }

    return NULL;
}


void *keyboard_handle(void *arguments){

    WINDOW *window = (WINDOW*)arguments;
    int btn_ascii;
    // Capture the w,s,a,d buttons upon close the game via ESC button 
    // ASCII: w -> 119, s -> 115, a -> 97, d -> 100
    while( (btn_ascii = wgetch(window)) != 27 ){

        // Make operations only when correct button has been pressed
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

    end_game = true;
    return NULL;
}

void *count_points(void *arguments){

    while(true){
        // Adds one point after one second
        usleep(1000000);
        scores += 1;
    }
    
    return NULL;
}

// --------------------------------------------------------------MAIN----------------------------------------------------------------------

int main(){
   
    initscr();

        // Screen settings
        curs_set(0);    // Don't display cursor
        noecho();       // Don't write at console        

        // The last two values define the left top corner of board (y, x)
        WINDOW *win = newwin(board_height, board_width, 0, 60);
        int ascii_code;

        // Loop for Restart game
        do{
            // Print starting screen
            wclear(win);
            box(win, 0, 0);
            mvwprintw(win, 0, 10, " AVOID ENEMIES GAME ");
            mvwprintw(win, 9, 9, " Press ENTER to start! ");
            wrefresh(win);

            // Wating for ENTER press
            while(wgetch(win) != 10);

            // Erase string
            mvwprintw(win, 9, 10, "                       ");
            wrefresh(win);

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

            // Create next thread to handle keyboard
            pthread_t keyboard_handler;
            result_code = pthread_create(&keyboard_handler, NULL, keyboard_handle, win);
            assert(!result_code);

            // Create thread to control collidation with enemies
            pthread_t collidation_handler;
            result_code = pthread_create(&collidation_handler, NULL, check_collidation_with_enemies, win);
            assert(!result_code);

            // Create thread intended for couning points
            pthread_t points_handler;
            result_code = pthread_create(&points_handler, NULL, count_points, NULL);
            assert(!result_code);

            while(true){

                if(end_game){

                    // Cancel threads
                    for (int i = 0; i < number_of_enemies; i++) {
                        pthread_cancel(enemies_threads[i]);
                        pthread_join(enemies_threads[i], NULL);
                    }

                    pthread_cancel(window_handler);
                    pthread_join(window_handler, NULL);

                    pthread_cancel(keyboard_handler);
                    pthread_join(keyboard_handler, NULL);

                    pthread_cancel(collidation_handler);
                    pthread_join(collidation_handler, NULL);

                    pthread_cancel(points_handler);
                    pthread_join(points_handler, NULL);

                    
                    print_end_game(win);

                    // Finish when ESC or restart when SPACE
                    do{
                        ascii_code = wgetch(win);

                    }while((ascii_code != 27) && (ascii_code != 32));
                    break;
                } 
            }

        // Restart when SPACE
        }while(ascii_code == 32);

    endwin();
    return 0;

}


