#include "draw.h"


Partitioning* BP1;   // set as global variable so other functions can access easily
int weightLevel = 0;
bool firstTime = 0;  //enable print when press the button
int divideBy = 1;
float windowSize = 1000.0;
float drawRange = 900.0;
float whiteSpace = 50.0;
float labelDistance = 25.0;

int openGraphics(Partitioning* BP) {
	BP1 = BP;

	/* initialize display with WHITE background */
	cout << "Starting graphics...\n";
	init_graphics("B&B Partitioner (by Zhiyuan Gu)", WHITE);               // text on the top of the window

	/* still picture drawing allows user to zoom, etc. */
	init_world(0., 0., windowSize, windowSize);                              // Set-up drawing coordinates.
	update_message("Press PROCEED or Exit to close the window");                              // text down below

	event_loop(act_on_button_press, NULL, NULL, drawPartition);

	close_graphics();                                             // close graphics tool
	cout << "Graphics closed down.\n";
	return 0;
}

void drawPartition() {                                          // redrawing routine for still pictures.
	// start drawing
	set_draw_mode(DRAW_NORMAL);                                   // Should set this if your program does any XOR drawing in callbacks.
	clearscreen();                                                 // Should precede drawing for all drawscreens
	// block No.
	int BlockSize = BP1->getBlocks()->size();
	setcolor(BLACK);
	setfontsize(12);
	char c_buffer[33];
	sprintf(c_buffer, "Block");
	drawtext(975.0, 25.0, c_buffer, 150.);
	sprintf(c_buffer, "No.");
	drawtext(975.0, 40.0, c_buffer, 150.);
	for (unsigned int i = 0; i < BlockSize; i++) {
		int BNum = BP1->getCaredBlocks()->at(i).getNumber();
		setcolor(BLACK);
		setfontsize(18);
		char c_buffer[33];
		sprintf(c_buffer, "%d", BNum);
		drawtext(975.0, whiteSpace + (i + 0.5)*drawRange / BlockSize, c_buffer, 150.);
	}
	// depth
	setfontsize(12);
	sprintf(c_buffer, "Depth");
	drawtext(25.0, 25.0, c_buffer, 150.);
	for (unsigned int i = 0; i <= BlockSize; i++) {
		setcolor(BLACK);
		setfontsize(18);
		char c_buffer[33];
		sprintf(c_buffer, "%d", i);
		drawtext(25.0, whiteSpace + (i)*drawRange / BlockSize, c_buffer, 150.);
	}
	// draw lines(tree edges) and circles(tree nodes)
	setlinestyle(SOLID);
	setlinewidth(2);
	setcolor(BLACK);

	drawline(0, 0, windowSize, 0);
	drawline(0, 0, 0, windowSize);
	drawline(windowSize, windowSize, 0, windowSize);
	drawline(windowSize, windowSize, windowSize, 0);
	for (unsigned int i = 0; i < BP1->getPosition()->size(); i++) {
		float px = BP1->getParentPos()->at(i).first + whiteSpace;
		float py = BP1->getParentPos()->at(i).second + whiteSpace;
		float x = BP1->getPosition()->at(i).first + whiteSpace;
		float y = BP1->getPosition()->at(i).second + whiteSpace;

		int lineW = ceil((950 - y) / 200);
		float arcRad = (950 - y) / 300;
		setlinewidth(lineW);
		// edge
		setcolor(BLUE);
		drawline(px, py, x, y);
		// block
		setcolor(YELLOW);
		fillarc(x, y, 6.0, 0., 360.);

	}


}


void act_on_button_press(float x, float y) {

	/* Called whenever event_loop gets a button press in the graphics *
	 * area.  Allows the user to do whatever he/she wants with button *
	 * clicks.                                                        */

	printf("User clicked a button at coordinates (%f, %f)\n", x, y);


}

