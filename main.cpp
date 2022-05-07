
#include <pthread.h>
#include <iostream>
#include <assert.h>
#include <unistd.h>
#include <ncurses.h>


using namespace std;

// void *perform_work(void *arguments){
//     // The threat index
//     int index = *((int *)arguments);
//     char inputChar;
//     while(true){
//         inputChar = getch();
//         cout<<inputChar;
//         sleep(1);
//     }
// }


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

struct Point {
  int x;
  int y;
  Point(int a, int b) { this->x = a; this->y = b; }
  Point() {this->x = 1; this->y = 1;}
};

// GLOBAL VARIABLES
// Hero starting position
Point hero_pos(20, 9);
// Board size
int board_width = 40;
int board_height = 20;
int number_of_enemies = 5;

Point enemies_positions[5];


int main(){
   
    int btn_ascii;

    initscr();

        // screen settings
        curs_set(0);    // don't display cursor
        noecho();       // don't display clicked buttons        

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
            enemies_positions[0] = Point(1,1);
            enemies_positions[1] = Point(5,5);
            enemies_positions[2] = Point(10,10);
            enemies_positions[3] = Point(15,15);
            enemies_positions[4] = Point(5,15);



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