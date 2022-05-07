
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
  Point() {this->x = 1; this->y = 1;}
};

// GLOBAL VARIABLES
// Hero starting position
Point hero_pos(20, 9);
// Board size
int board_width = 40;
int board_height = 20;
int number_of_enemies = 5;
int end_game = false; 

Point enemies_positions[5];



// void *enemy_work(void *arguments){

//     // The threat index (to update proper point in table)
//     int index = *((int *)arguments);

//     // Draw the direction (0 - left front, 1 - front , 2 - right front) 
//     int direction = rand() %3;
// };


Point get_random_enemy_position(){
    
    // Draw the side to render enemy( 0 - left, 1 - top, 2 - right, 3 - botton)
    int side = rand() % 4; 

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


// int main(void) {

//     // // Create thread intended for printing status of the threads 
//     // pthread_t simple_thread; 
//     // int simple_thread_arg = 1;
//     // //Start simple_thread
//     // int create_correct = pthread_create(&simple_thread, NULL, perform_work, &simple_thread_arg);
//     // // Zero if success
//     // assert(!create_correct);
//     // // Wait for thread 
//     // int result_code = pthread_join(simple_thread, NULL);
//     // assert(!result_code);
//     return 0;
// }


// Note: draw side, draw place at line, drow direction,(additional we can draw speed)


int main(){
   
    int btn_ascii;

    initscr();

        // screen settings
        curs_set(0);    // don't display cursor
        noecho();       // don't write at console        

        // the last two values define the left top corner of board (y, x)
        WINDOW *win = newwin(board_height, board_width ,0,60);
        refresh();

        // make border
        box(win, 0, 0);
        // game title print
        mvwprintw(win, 0, 10, "AVOID ENEMIES GAME");
        // print the hero
        mvwprintw(win, hero_pos.y, hero_pos.x, "H");
        // display
        wrefresh(win);

        




        // capture the w,s,a,d buttons upon close the game via ESC button 
        // ASCII: w -> 119, s -> 115, a -> 97, d -> 100

        while( (btn_ascii = getch()) != 27 ){

            // make operations only when correct button has been pressed
            if( (btn_ascii == 119)||(btn_ascii == 115)||(btn_ascii == 97)||(btn_ascii == 100) ){

                // erase last hero's position
                mvwprintw(win, hero_pos.y, hero_pos.x, " ");

                // calculate new one
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
                // print hero at new position
                mvwprintw(win, hero_pos.y, hero_pos.x, "H");
            }

            // variate enemies
            enemies_positions[0] = get_random_enemy_position();
            enemies_positions[1] = get_random_enemy_position();
            enemies_positions[2] = get_random_enemy_position();
            enemies_positions[3] = get_random_enemy_position();
            enemies_positions[4] = get_random_enemy_position();



            // print enemies
            for(int i = 0 ; i < number_of_enemies ; i++){
                mvwprintw(win, enemies_positions[i].y, enemies_positions[i].x, "E");
            }

            // display changes
                wrefresh(win);
        }

        // place for close threads

    endwin();
    return 0;

}