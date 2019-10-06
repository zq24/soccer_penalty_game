#include "alt_types.h"
#include "altera_avalon_pio_regs.h"
#include "sys/alt_irq.h"
#include "system.h"
#include <stdio.h>
#include <unistd.h>
#include "altera_up_avalon_video_pixel_buffer_dma.h"
/*############################################################## FUNCTION CALL ##################################################################*/
void initialPic();
void AnimPlay(int Result);

int main(void) {
/*############################################################## INITIAL SETUP ##################################################################*/
	/*
	 * Define the state for gaming GameState.
	 *   0 for START up the game and display the Title/Name
	 *   1 for first player chose direction.
	 *   2 for second player choose direction.
	 *   3 for game result calculation.
	 *   4 for Anim playing.
	 *   5 for EXIT the game.
	 *   6 for Idle, waiting for start the game.
	 */
	int GameState = 6;//----------------------------------------------------------------Default to enter Idle state
	int Round = 1; //-------------------------------------------------------------------To count the total number of round played.
	int Score_1 = 0, Score_2 = 0; //----------------------------------------------------To count the score for both players.
	int Direction_1 = 0, Direction_2 = 0; //--------------------------------------------To record the direction chosen for each player.
	/*
	 *  Define the variable to store the result from game.
	 *        Penalty taker - Goal keeper
	 *  1 for left          - left
	 *  2 for left          - center
	 *  3 for left          - right
	 *  4 for center        - left
	 *  5 for center        - center
	 *  6 for center        - right
	 *  7 for right         - left
	 *  8 for right         - center
	 *  9 for right         - right
	 */
	int GameResult = 0;


	alt_up_pixel_buffer_dma_dev* EnableDev;
	EnableDev = alt_up_pixel_buffer_dma_open_dev("/dev/Pixel_Buffer_DMA"); //-----------Enable the VGA Device
	alt_up_pixel_buffer_dma_clear_screen(EnableDev, 0); //Clean the Screen

	while (1) { //----------------------------------------------------------------------Enter the main loop.
/*############################################################## STATE 0 ##################################################################*/
		while (GameState == 0) { //-----------------------------------------------------State for START game and display the Title/Name.
			// Display the title 'Soccer Game', 'EE 3921 Final Project', 'Ziwei.C and Zhaoming.Q'
			usleep(500000); //----------------------------------------------------------Wait/sleep for 10 seconds.
			initialPic(); //------------------------------------------------------------Display the initial picture for game.
			// Reset all variables to its initial values.
			Round = 1; //---------------------------------------------------------------To count the total number of round played.
			Score_1 = 0, Score_2 = 0; //------------------------------------------------To count the score for both players.
			GameState = 1; //---------------------------------------------------------- Enter State 1, for the first player to choose direction.
		}
/*############################################################## STATE 1 ##################################################################*/
		while (GameState == 1) { //-----------------------------------------------------State for the first player to choose direction.
			// Display: 'Penalty Taker choose direction', 'Press Key 0,1,2' to choose left, center, or right'.
			int Dir_temp_1 = Direction_1;
			while (Dir_temp_1 == Direction_1) {
				Direction_1 = IORD_ALTERA_AVALON_PIO_DATA(KEY_PIO_BASE); //-------------Read the Input from key, and store the value in Direction_1.
			}
			Dir_temp_1 = Direction_1;
			while (Dir_temp_1 != 15) {
				Dir_temp_1 = IORD_ALTERA_AVALON_PIO_DATA(KEY_PIO_BASE); //--------------Read the Input from key, and store the value in Direction_1.
			}
			GameState = 2; //-----------------------------------------------------------Enter State 2, for the second player to choose Direction.
		}
/*############################################################## STATE 2 ##################################################################*/
		while (GameState == 2) { //-----------------------------------------------------State for the second player to choose direction.
			// Display: 'Goal Keeper choose direction', 'Press Key 0,1,2' to choose left, center, or right'.
			int Dir_temp_2 = Direction_2;
			while (Dir_temp_2 == Direction_2) {
				Direction_2 = IORD_ALTERA_AVALON_PIO_DATA(KEY_PIO_BASE); //-------------Read the Input from key, and store the value in Direction_2.
			}
			Dir_temp_2 = Direction_2;
			while (Dir_temp_2 != 15) {
				Dir_temp_2 = IORD_ALTERA_AVALON_PIO_DATA(KEY_PIO_BASE); //--------------Read the Input from key, and store the value in Direction_2.
			}
			GameState = 3; //-----------------------------------------------------------Enter State 3, for result computation
		}
/*############################################################## STATE 3 ##################################################################*/
		while (GameState == 3) { //-----------------------------------------------------State for compute the result for game.
			/*
			 * Compare the value for direction to decide the game result, and Store the result in GameResult.
			 */
			GameResult = 0;
			if (Direction_1 == 11 && Direction_2 == 11) {//----------------------------left - left
				GameResult = 1; //-----------------------------------------------------Save the goal
			} else if (Direction_1 == 11 && Direction_2 == 13) { //--------------------left - center
				GameResult = 2;
			} else if (Direction_1 == 11 && Direction_2 == 14) { //--------------------left - right
				GameResult = 3;
			} else if (Direction_1 == 13 && Direction_2 == 11) { //--------------------center - left
				GameResult = 4;
			} else if (Direction_1 == 13 && Direction_2 == 13) { //--------------------center - center
				GameResult = 5; //-----------------------------------------------------Save the goal
			} else if (Direction_1 == 13 && Direction_2 == 14) { //--------------------center - right
				GameResult = 6;
			} else if (Direction_1 == 14 && Direction_2 == 11) { //--------------------right - left
				GameResult = 7;
			} else if (Direction_1 == 14 && Direction_2 == 13) { //--------------------right  - center
				GameResult = 8;
			} else if (Direction_1 == 14 && Direction_2 == 14) { //--------------------right - right
				GameResult = 9; //-----------------------------------------------------Save the goal
			}
			/*
			 * Play the Anim Base on the value for GameResult.
			 * when two values are equal, the goal keeper save the goal,
			 * else the penalty take the score.
			 */
			AnimPlay(GameResult);
			usleep(2000000);
			GameState = 4; //----------------------------------------------------------Go to the state for playing Anim.
		}
/*############################################################## STATE 4 ##################################################################*/
		while (GameState == 4) { //----------------------------------------------------State for Anim playing.
			if (GameResult == 1 || GameResult == 5 || GameResult == 9) { //------------Store the score in either Score1 and Score2
				Score_2++; //----------------------------------------------------------The keeper take score
			} else {
				Score_1++; //----------------------------------------------------------The taker take score
			}
			// Update the Score on screen
			/* If Round value larger or equal than 5, enter the EXIT Game state.
			 * If Round value smaller then 5, then Enter the State for the first player to choose direction.
			 */
			if (Round < 5) {
				GameState = 1;
				Round = Round + 1; //-------------------------------------------------Update the Round value
			} else if (Round >= 5) {
				GameState = 5;
				Round = 1;
			}
		}
/*############################################################## STATE 5 ##################################################################*/
		while (GameState == 5) { //---------------------------------------------------State for Exit the game and reset all data.
			int WhoWin; //------------------------------------------------------------------0 for keeper win, 1 for taker win.
			if (Score_1 > Score_2) { //-----------------------------------------------Compare the value for Score_1 and Score_2, the larger one wins. Store value in WhoWin.
				WhoWin = 1;
			} else if (Score_1 < Score_2) {
				WhoWin = 0;
			}
			// Display the message 'XXX WIN!!!', 'Game END!!!' base on the value of WhoWin.
			alt_up_pixel_buffer_dma_clear_screen(EnableDev, 0); //-------------------Clean the Screen
			GameState = 6;
		}
		while (GameState == 6) { //--------------------------------------------------State for waiting. Wither keep waiting or start the exit game.
			// Display 'Press KeyX to start new game'
			int Restart = 0;
			int Restart_tmp = Restart; //------------------------------------------------Read the value for KeyX
			while (Restart_tmp == Restart) {
				Restart = IORD_ALTERA_AVALON_PIO_DATA(KEY_PIO_BASE);
			}
			if (Restart == 7) {
				GameState = 0; //----------------------------------------------------Enter State 0 for start new game, or stay in Idle.
			} else {
				GameState = 6;
			}
		}
	}
}
/*############################################################## FUNCTIONS ##################################################################*/
void initialPic(void) {
	alt_up_pixel_buffer_dma_dev* EnableDev;
	EnableDev = alt_up_pixel_buffer_dma_open_dev("/dev/Pixel_Buffer_DMA"); //Enable the VGA Device
	alt_up_pixel_buffer_dma_draw_hline(EnableDev, 120, 200, 50, 0XFFFF, 0);
	alt_up_pixel_buffer_dma_draw_vline(EnableDev, 120, 50, 70, 0XFFFF, 0);
	alt_up_pixel_buffer_dma_draw_vline(EnableDev, 200, 50, 70, 0XFFFF, 0);
	alt_up_pixel_buffer_dma_draw_rectangle(EnableDev, 155, 57, 165, 67, 0XFFFF,0); // keeper
	alt_up_pixel_buffer_dma_draw_rectangle(EnableDev, 155, 175, 165, 185,0XFFFF, 0); // taker
	alt_up_pixel_buffer_dma_draw_rectangle(EnableDev, 159, 170, 161, 172,0XFFFF, 0); // Ball
}

void AnimPlay(int Result) {
	alt_up_pixel_buffer_dma_dev* EnableDev;
	EnableDev = alt_up_pixel_buffer_dma_open_dev("/dev/Pixel_Buffer_DMA"); //Enable the VGA Device
	alt_up_pixel_buffer_dma_clear_screen(EnableDev, 0); //Clean the Screen
	alt_up_pixel_buffer_dma_draw_hline(EnableDev, 120, 200, 50, 0XFFFF, 0);
	alt_up_pixel_buffer_dma_draw_vline(EnableDev, 120, 50, 70, 0XFFFF, 0);
	alt_up_pixel_buffer_dma_draw_vline(EnableDev, 200, 50, 70, 0XFFFF, 0);
	alt_up_pixel_buffer_dma_draw_rectangle(EnableDev, 155, 57, 165, 67, 0XFFFF,0); // keeper
	alt_up_pixel_buffer_dma_draw_rectangle(EnableDev, 155, 175, 165, 185,0XFFFF, 0); // taker
	alt_up_pixel_buffer_dma_draw_rectangle(EnableDev, 159, 170, 161, 172,0XFFFF, 0); // Ball
	int y1 = 170;
	int y2 = 172;
	int x1 = 159;
	int x2 = 161;
	if (Result == 1){ //left - left
		while (y1 >= 65 && y2 >= 67) {
			alt_up_pixel_buffer_dma_draw_rectangle(EnableDev, x1, y1, x2, y2,0XFFFF, 0); // Ball
			usleep(25000);
			alt_up_pixel_buffer_dma_draw_rectangle(EnableDev, x1, y1, x2, y2,0X0000, 0); // Ball
			if (x1 >= 132 && x2 >= 135) {
				x1 = x1 - 1;x2 = x2 - 1;
			}
			y1 = y1 - 4;y2 = y2 - 4;
			if (y1 <= 80) {
				alt_up_pixel_buffer_dma_draw_rectangle(EnableDev, 155, 57, 165,67, 0X0000, 0); // keeper
				alt_up_pixel_buffer_dma_draw_rectangle(EnableDev, 125, 57, 135,67, 0XFFFF, 0); // keeper
			}
		}
		alt_up_pixel_buffer_dma_draw_rectangle(EnableDev, 133, 68, 135, 70,
				0XFFFF, 0); // Ball
	}
	if (Result == 2){ //left - center
		while (y1 >= 65 && y2 >= 67) {
			alt_up_pixel_buffer_dma_draw_rectangle(EnableDev, x1, y1, x2, y2,0XFFFF, 0); // Ball
			usleep(25000);
			alt_up_pixel_buffer_dma_draw_rectangle(EnableDev, x1, y1, x2, y2,0X0000, 0); // Ball
			if (x1 >= 132 && x2 >= 135) {
				x1 = x1 - 1;x2 = x2 - 1;
			}
			y1 = y1 - 4;y2 = y2 - 4;

		}
		alt_up_pixel_buffer_dma_draw_rectangle(EnableDev, 133, 68, 135, 70,
				0XFFFF, 0); // Ball
	}
	if (Result == 3){ //left - right
		while (y1 >= 65 && y2 >= 67) {
			alt_up_pixel_buffer_dma_draw_rectangle(EnableDev, x1, y1, x2, y2,0XFFFF, 0); // Ball
			usleep(25000);
			alt_up_pixel_buffer_dma_draw_rectangle(EnableDev, x1, y1, x2, y2,0X0000, 0); // Ball
			if (x1 >= 132 && x2 >= 135) {
				x1 = x1 - 1;x2 = x2 - 1;
			}
			y1 = y1 - 4;y2 = y2 - 4;
			if (y1 <= 80) {
				alt_up_pixel_buffer_dma_draw_rectangle(EnableDev, 155, 57, 165,67, 0X0000, 0); // keeper
				alt_up_pixel_buffer_dma_draw_rectangle(EnableDev, 195, 57, 185,67, 0XFFFF, 0); // keeper
			}
		}
		alt_up_pixel_buffer_dma_draw_rectangle(EnableDev, 133, 68, 135, 70,0XFFFF, 0); // Ball
	}
	if (Result == 4){ //center - left
		while (y1 >= 65 && y2 >= 67) {
			alt_up_pixel_buffer_dma_draw_rectangle(EnableDev, x1, y1, x2, y2,0XFFFF, 0); // Ball
			usleep(25000);
			alt_up_pixel_buffer_dma_draw_rectangle(EnableDev, x1, y1, x2, y2,0X0000, 0); // Ball
			if (x1 >= 132 && x2 >= 135) {
				x1 = x1 - 0;x2 = x2 - 0;
			}
			y1 = y1 - 4;y2 = y2 - 4;
			if (y1 <= 80) {
				alt_up_pixel_buffer_dma_draw_rectangle(EnableDev, 155, 57, 165,67, 0X0000, 0); // keeper
				alt_up_pixel_buffer_dma_draw_rectangle(EnableDev, 125, 57, 135,67, 0XFFFF, 0); // keeper
			}
		}
		alt_up_pixel_buffer_dma_draw_rectangle(EnableDev, x1, y1, x2, y2,
				0XFFFF, 0); // Ball
	}
	if (Result == 5){ //center - center
		while (y1 >= 65 && y2 >= 67) {
			alt_up_pixel_buffer_dma_draw_rectangle(EnableDev, x1, y1, x2, y2,0XFFFF, 0); // Ball
			usleep(25000);
			alt_up_pixel_buffer_dma_draw_rectangle(EnableDev, x1, y1, x2, y2,0X0000, 0); // Ball
			if (x1 >= 132 && x2 >= 135) {
				x1 = x1 - 0;x2 = x2 - 0;
			}
			y1 = y1 - 4;y2 = y2 - 4;

		}
		alt_up_pixel_buffer_dma_draw_rectangle(EnableDev, x1, y1, x2, y2,0XFFFF, 0); // Ball
	}
	if (Result == 6){ //center - right
		while (y1 >= 65 && y2 >= 67) {
			alt_up_pixel_buffer_dma_draw_rectangle(EnableDev, x1, y1, x2, y2,0XFFFF, 0); // Ball
			usleep(25000);
			alt_up_pixel_buffer_dma_draw_rectangle(EnableDev, x1, y1, x2, y2,0X0000, 0); // Ball
			if (x1 >= 132 && x2 >= 135) {
				x1 = x1 - 0;x2 = x2 - 0;
			}
			y1 = y1 - 4;y2 = y2 - 4;
			if (y1 <= 80) {
				alt_up_pixel_buffer_dma_draw_rectangle(EnableDev, 155, 57, 165,67, 0X0000, 0); // keeper
				alt_up_pixel_buffer_dma_draw_rectangle(EnableDev, 195, 57, 185,67, 0XFFFF, 0); // keeper
			}
		}
		alt_up_pixel_buffer_dma_draw_rectangle(EnableDev, x1, y1, x2, y2,
				0XFFFF, 0); // Ball
	}
	if (Result == 7){ //right -left
		while (y1 >= 65 && y2 >= 67) {
			alt_up_pixel_buffer_dma_draw_rectangle(EnableDev, x1, y1, x2, y2,0XFFFF, 0); // Ball
			usleep(25000);
			alt_up_pixel_buffer_dma_draw_rectangle(EnableDev, x1, y1, x2, y2,0X0000, 0); // Ball
			if (x1 >= 132 && x2 >= 135) {
				x1 = x1 + 1;x2 = x2 + 1;
			}
			y1 = y1 - 4;y2 = y2 - 4;
			if (y1 <= 80) {
				alt_up_pixel_buffer_dma_draw_rectangle(EnableDev, 155, 57, 165,67, 0X0000, 0); // keeper
				alt_up_pixel_buffer_dma_draw_rectangle(EnableDev, 125, 57, 135,67, 0XFFFF, 0); // keeper
			}
		}
		alt_up_pixel_buffer_dma_draw_rectangle(EnableDev, x1, y1, x2, y2,
				0XFFFF, 0); // Ball
	}
	if (Result == 8){ //right -center
		while (y1 >= 65 && y2 >= 67) {
			alt_up_pixel_buffer_dma_draw_rectangle(EnableDev, x1, y1, x2, y2,0XFFFF, 0); // Ball
			usleep(25000);
			alt_up_pixel_buffer_dma_draw_rectangle(EnableDev, x1, y1, x2, y2,0X0000, 0); // Ball
			if (x1 >= 132 && x2 >= 135) {
				x1 = x1 + 1;x2 = x2 + 1;
			}
			y1 = y1 - 4;y2 = y2 - 4;
		}
		alt_up_pixel_buffer_dma_draw_rectangle(EnableDev, x1, y1, x2, y2,0XFFFF, 0); // Ball
	}
	if (Result == 9){ //right -right
		while (y1 >= 65 && y2 >= 67) {
			alt_up_pixel_buffer_dma_draw_rectangle(EnableDev, x1, y1, x2, y2,0XFFFF, 0); // Ball
			usleep(25000);
			alt_up_pixel_buffer_dma_draw_rectangle(EnableDev, x1, y1, x2, y2,0X0000, 0); // Ball
			if (x1 >= 132 && x2 >= 135) {
				x1 = x1 + 1;x2 = x2 + 1;
			}
			y1 = y1 - 4;y2 = y2 - 4;
			if (y1 <= 80) {
				alt_up_pixel_buffer_dma_draw_rectangle(EnableDev, 155, 57, 165,67, 0X0000, 0); // keeper
				alt_up_pixel_buffer_dma_draw_rectangle(EnableDev, 195, 57, 185,67, 0XFFFF, 0); // keeper
			}
		}
		alt_up_pixel_buffer_dma_draw_rectangle(EnableDev, x1, y1, x2, y2,0XFFFF, 0); // Ball
	}
}

