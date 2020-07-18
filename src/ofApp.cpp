/*

	ofxWinMenu basic example - ofApp.cpp

	Example of using ofxWinMenu addon to create a menu for a Microsoft Windows application.

	Copyright (C) 2016-2017 Lynn Jarvis.

	https://github.com/leadedge

	http://www.spout.zeal.co

	=========================================================================
	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU Lesser General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.
	=========================================================================

	03.11.16 - minor comment cleanup
	21.02.17 - rebuild for OF 0.9.8

*/
#include "ofApp.h"
#include <iostream>
using namespace std;
//--------------------------------------------------------------
void ofApp::setup() {

	ofSetWindowTitle("Maze Example"); // Set the app name on the title bar
	ofSetFrameRate(15);
	ofBackground(255, 255, 255);
	// Get the window size for image loading
	windowWidth = ofGetWidth();
	windowHeight = ofGetHeight();
	isdfs = false;
	isOpen = 0;
	// Centre on the screen
	ofSetWindowPosition((ofGetScreenWidth() - windowWidth) / 2, (ofGetScreenHeight() - windowHeight) / 2);

	// Load a font rather than the default
	myFont.loadFont("verdana.ttf", 12, true, true);

	// Load an image for the example
	//myImage.loadImage("lighthouse.jpg");

	// Window handle used for topmost function
	hWnd = WindowFromDC(wglGetCurrentDC());

	// Disable escape key exit so we can exit fullscreen with Escape (see keyPressed)
	ofSetEscapeQuitsApp(false);

	//
	// Create a menu using ofxWinMenu
	//

	// A new menu object with a pointer to this class
	menu = new ofxWinMenu(this, hWnd);

	// Register an ofApp function that is called when a menu item is selected.
	// The function can be called anything but must exist. 
	// See the example "appMenuFunction".
	menu->CreateMenuFunction(&ofApp::appMenuFunction);

	// Create a window menu
	HMENU hMenu = menu->CreateWindowMenu();

	//
	// Create a "File" popup menu
	//
	HMENU hPopup = menu->AddPopupMenu(hMenu, "File");

	//
	// Add popup items to the File menu
	//

	// Open an maze file
	menu->AddPopupItem(hPopup, "Open", false, false); // Not checked and not auto-checked

	// Final File popup menu item is "Exit" - add a separator before it
	menu->AddPopupSeparator(hPopup);
	menu->AddPopupItem(hPopup, "Exit", false, false);

	//
	// View popup menu
	//
	hPopup = menu->AddPopupMenu(hMenu, "View");

	bShowInfo = true;  // screen info display on
	menu->AddPopupItem(hPopup, "Show DFS", false, false); // Checked
	bTopmost = false; // app is topmost
	menu->AddPopupItem(hPopup, "Show BFS", false, false); // Not checked (default)
	bFullscreen = false; // not fullscreen yet
	menu->AddPopupItem(hPopup, "Full screen", false, false); // Not checked and not auto-check

	//
	// Help popup menu
	//
	hPopup = menu->AddPopupMenu(hMenu, "Help");
	menu->AddPopupItem(hPopup, "About", false, false); // No auto check

	// Set the menu to the window
	menu->SetWindowMenu();

} // end Setup


//
// Menu function
//
// This function is called by ofxWinMenu when an item is selected.
// The the title and state can be checked for required action.
// 
void ofApp::appMenuFunction(string title, bool bChecked) {

	ofFileDialogResult result;
	string filePath;
	size_t pos;

	//
	// File menu
	//
	if (title == "Open") {
		readFile();
		draw();
	}
	if (title == "Exit") {
		ofExit(); // Quit the application
	}

	//
	// Window menu
	//
	if (title == "Show DFS") {
		//bShowInfo = bChecked;  // Flag is used elsewhere in Draw()
		if (isOpen)
		{
			isbfs = false;
			DFS();
			bShowInfo = bChecked;
			isdfs = true;
		}
		else
			cout << "you must open file first" << endl;

	}

	if (title == "Show BFS") {
		//doTopmost(bChecked); // Use the checked value directly
		if (isOpen) {
			isdfs = false;
			BFS();
			bShowInfo = bChecked;
			isbfs = true;
		}
		else
			cout << "you must open file first" << endl;
	}

	if (title == "Full screen") {
		bFullscreen = !bFullscreen; // Not auto-checked and also used in the keyPressed function
		doFullScreen(bFullscreen); // But als take action immediately
	}

	//
	// Help menu
	//
	if (title == "About") {
		ofSystemAlertDialog("ofxWinMenu\nbasic example\n\nhttp://spout.zeal.co");
	}

} // end appMenuFunction


//--------------------------------------------------------------
void ofApp::update() {
		
}


//--------------------------------------------------------------
void ofApp::draw() {

	char str[256];
	//ofBackground(0, 0, 0, 0);
	ofSetColor(100);
	ofSetLineWidth(5);
	int i, j;

	// TO DO : DRAW MAZE; 
	// 저장된 자료구조를 이용해 미로를 그린다.
	// add code here
	ofNoFill();
	ofDrawRectangle(0, 0, WIDTH * 100, HEIGHT * 100);
	int match_flag = 0;
	Node* temp;
	for (i = 0; i < HEIGHT; i++) {
		for (j = 0; j < WIDTH; j++) {
			if (i < HEIGHT - 1) {
				if (input[(i + 1) * 2][(j * 2) + 1] == '-') {
					ofDrawLine(100 * j, 100 * (i + 1), 100 * (j + 1), 100 * (i + 1)); //가로벽 (수직막아)
				}
			}
			if (j < WIDTH - 1) {
				if (input[(i * 2) + 1][(j + 1) * 2] == '|') {
					ofDrawLine(100 * (j + 1), 100 * i, 100 * (j + 1), 100 * (i + 1)); // 세로벽 (수평막아)
				}
			}
		}
	}
	

	if (isdfs)
	{	
		
		ofSetColor(200);
		ofSetLineWidth(5);
		if (isOpen) {
			
			dfsdraw();
		}
		else
			cout << "You must open file first" << endl;
	}
	if (isbfs) {

		if (isOpen) {
			
			bfsdraw();
		}
		else
			cout << "you must open file first" << endl;
	}
	if (bShowInfo) {
		// Show keyboard duplicates of menu functions
		sprintf(str, " comsil project");
		myFont.drawString(str, 15, ofGetHeight() - 20);
	}

} // end Draw


void ofApp::doFullScreen(bool bFull)
{
	// Enter full screen
	if (bFull) {
		// Remove the menu but don't destroy it
		menu->RemoveWindowMenu();
		// hide the cursor
		ofHideCursor();
		// Set full screen
		ofSetFullscreen(true);
	}
	else {
		// return from full screen
		ofSetFullscreen(false);
		// Restore the menu
		menu->SetWindowMenu();
		// Restore the window size allowing for the menu
		ofSetWindowShape(windowWidth, windowHeight + GetSystemMetrics(SM_CYMENU));
		// Centre on the screen
		ofSetWindowPosition((ofGetScreenWidth() - ofGetWidth()) / 2, (ofGetScreenHeight() - ofGetHeight()) / 2);
		// Show the cursor again
		ofShowCursor();
		// Restore topmost state
		if (bTopmost) doTopmost(true);
	}

} // end doFullScreen


void ofApp::doTopmost(bool bTop)
{
	if (bTop) {
		// get the current top window for return
		hWndForeground = GetForegroundWindow();
		// Set this window topmost
		SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		ShowWindow(hWnd, SW_SHOW);
	}
	else {
		SetWindowPos(hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		ShowWindow(hWnd, SW_SHOW);
		// Reset the window that was topmost before
		if (GetWindowLong(hWndForeground, GWL_EXSTYLE) & WS_EX_TOPMOST)
			SetWindowPos(hWndForeground, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		else
			SetWindowPos(hWndForeground, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	}
} // end doTopmost


//--------------------------------------------------------------
void ofApp::keyPressed(int key) {

	// Escape key exit has been disabled but it can be checked here
	if (key == VK_ESCAPE) {
		// Disable fullscreen set, otherwise quit the application as usual
		if (bFullscreen) {
			bFullscreen = false;
			doFullScreen(false);
		}
		else {
			ofExit();
		}
	}

	// Remove or show screen info
	if (key == ' ') {
		bShowInfo = !bShowInfo;
		// Update the menu check mark because the item state has been changed here
		menu->SetPopupItem("Show DFS", bShowInfo);
	}

	if (key == 'f') {
		bFullscreen = !bFullscreen;
		doFullScreen(bFullscreen);
		// Do not check this menu item
		// If there is no menu when you call the SetPopupItem function it will crash
	}

} // end keyPressed

//--------------------------------------------------------------
void ofApp::keyReleased(int key) {

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) {

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg) {

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo) {

}
bool ofApp::readFile()
{
	ofFileDialogResult openFileResult = ofSystemLoadDialog("Select .maz file");
	string filePath;
	size_t pos;
	// Check whether the user opened a file
	if (openFileResult.bSuccess) {
		ofLogVerbose("User selected a file");

		//We have a file, check it and process it
		string fileName = openFileResult.getName();
		//string fileName = "maze0.maz";
		printf("file name is %s\n", fileName.c_str());
		filePath = openFileResult.getPath();
		printf("Open\n");
		pos = filePath.find_last_of(".");
		if (pos != string::npos && pos != 0 && filePath.substr(pos + 1) == "maz") {

			ofFile file(fileName);

			if (!file.exists()) {
				cout << "Target file does not exists." << endl;
				return false;
			}
			else {
				if (isOpen)
					freeMemory();
				cout << "We found the target file." << endl;
				isOpen = 1;
			}

			ofBuffer buffer(file);

			if (isOpen) {
				// Input_flag is a variable for indication the type of input.
				// If input_flag is zero, then work of line input is progress.
				// If input_flag is one, then work of dot input is progress.
				int input_flag = 0;

				// Idx is a variable for index of array.
				int idx = 0;

				// Read file line by line
				int cnt = 0;
				for (ofBuffer::Line it = buffer.getLines().begin(), end = buffer.getLines().end(); it != end; ++it, cnt++);
				input_HEIGHT = cnt;
				HEIGHT = input_HEIGHT / 2;
				input = new char* [input_HEIGHT];
				cnt = 0;
				// 텍스트 파일을 그대로 input 배열에 넣기
				for (ofBuffer::Line it = buffer.getLines().begin(), end = buffer.getLines().end(); it != end; ++it, cnt++) {
					string line = *it;
					if (cnt % 2 == 0) {
						input_flag = 0;
					}
					else {
						input_flag = 1;
					}

					input_WIDTH = line.size();
					WIDTH = input_WIDTH / 2;
					input[cnt] = new char[input_WIDTH];
					strcpy(input[cnt], line.c_str());
				}

				// TO DO
				// .maz 파일을 input으로 받아서 적절히 자료구조에 넣는다

				int n = 0;
				graph = new Node * [HEIGHT];
				for (int i = 0; i < HEIGHT; i++) {
					graph[i] = new Node[WIDTH];
					for (idx = 0; idx < WIDTH; idx++) {
						graph[i][idx].vertex = n++;
						graph[i][idx].centerX = (50 + 100 * idx);
						graph[i][idx].centerY = (50 + 100 * i);
					}
				}

				for (int i = 0; i < HEIGHT; i++) {
					for (idx = 0; idx < WIDTH; idx++) {
						if (i > 0) { // 수직경로 간선
							if (input[(i) * 2][(idx * 2) + 1] == ' ') {
								graph[i][idx].adj.push_back(&graph[i - 1][idx]);
								graph[i - 1][idx].adj.push_back(&graph[i][idx]);
							}

						}
						if (idx < WIDTH - 1) { //마지막 방은 오른쪽 벽 검사할 필요 ㄴ
							if (input[i * 2 + 1][(idx + 1) * 2] == ' ') {
								graph[i][idx].adj.push_back(&graph[i][idx + 1]);
								graph[i][idx + 1].adj.push_back(&graph[i][idx]);
							}
						}
					}
				}
				return true;
			}

		}
		else {
			printf("  Needs a '.maz' extension\n");
			return false;
		}
	}
}
void ofApp::freeMemory() {

	//TO DO
	// malloc한 memory를 free해주는 함수
	for (int i = 0; i < HEIGHT; i++) {
		delete[] graph[i];
	}
	delete[] graph;
	for (int i = 0; i < input_HEIGHT; i++) {
		delete[] input[i];
	}
	delete[] input;

	free(S.top);
	free(Q.front);
	free(Q.rear);
	
}

bool ofApp::DFS()//DFS탐색을 하는 함수
{
	
	int i, j;
	//TO DO
	//DFS탐색을 하는 함수 ( 3주차)
	for (i = 0; i < HEIGHT; i++) {
		for (j = 0; j < WIDTH; j++) {
			graph[i][j].visited = 0;
		}
	}
	
	S.push(&graph[0][0]);
	graph[0][0].visited = 1;
	Node* target = &graph[HEIGHT-1][WIDTH-1];

	while (!S.isEmpty()) {
		int unv_flag = 0;
		if (S.top->item == target) return true;
		for (i = 0; i < S.top->item->adj.size(); i++) {
			if (S.top->item->adj[i]->visited == 0) {
				unv_flag = 1;
				break;
			} 
		}
		if (unv_flag) {
			Node* u = S.top->item->adj[i];
			u->visited = 1;
			S.push(u);
		}
		else {
			S.pop();
		}
	}
}
void ofApp::dfsdraw()
{
	//TO DO 
	//DFS를 수행한 결과를 그린다. (3주차 내용)
	int i, j;
	for (i = 0; i < HEIGHT; i++) {
		for (j = 0; j < WIDTH; j++) {
			graph[i][j].colored = 0;
		}
	}
	{
		ofSetColor(ofColor::white);
		ofFill();
		ofDrawRectangle(0, 0, ofGetWidth(), ofGetHeight());

		ofSetColor(100);
		ofSetLineWidth(5);
		ofNoFill();
		ofDrawRectangle(0, 0, WIDTH * 100, HEIGHT * 100);
		int match_flag = 0;
		Node* temp;
		for (i = 0; i < HEIGHT; i++) {
			for (j = 0; j < WIDTH; j++) {
				if (i < HEIGHT - 1) {
					if (input[(i + 1) * 2][(j * 2) + 1] == '-') {
						ofDrawLine(100 * j, 100 * (i + 1), 100 * (j + 1), 100 * (i + 1)); //가로벽 (수직막아)
					}
				}
				if (j < WIDTH - 1) {
					if (input[(i * 2) + 1][(j + 1) * 2] == '|') {
						ofDrawLine(100 * (j + 1), 100 * i, 100 * (j + 1), 100 * (i + 1)); // 세로벽 (수평막아)
					}
				}
			}
		}
	}// 2nd comment end
	S.push(&graph[0][0]);
	graph[0][0].colored = 1;
	Node* target = &graph[HEIGHT - 1][WIDTH - 1];
	int v_flag = 0;
	while (!S.isEmpty()) {
		v_flag = 0;
		if (S.top->item == target) break;
		for (i = 0; i < S.top->item->adj.size(); i++) {
			if (S.top->item->adj[i]->visited == 1 && S.top->item->adj[i]->colored == 0) {
				v_flag = 1;
				break; //방문한 노드를 따라가자
			}
		}
		if (v_flag) {
			Node* u = S.top->item->adj[i];
			ofSetLineWidth(7);
			ofSetColor(ofColor::blue);
			ofDrawLine(S.top->item->centerX, S.top->item->centerY, u->centerX, u->centerY);
			u->colored = 1;
			S.push(u);
		}
		else {
			S.pop();
		}
	}
	while (!S.isEmpty())
		S.pop();
	DFS();
	ofSetColor(ofColor::red);
	ofSetLineWidth(7);
	int x1, x2, y1, y2;
	while ((!S.isEmpty())) {
		x1 = S.top->item->centerX;
		y1 = S.top->item->centerY;
		S.pop();
		if (S.top == NULL) break;
		x2 = S.top->item->centerX;
		y2 = S.top->item->centerY;
		ofDrawLine(x1, y1, x2, y2);
	}
	ofSetBackgroundAuto(false);
	
}

bool ofApp::BFS() {
	//그래프의 visited 랑 colored 초기화
	int i, j;
	for (i = 0; i < HEIGHT; i++) {
		for (j = 0; j < WIDTH; j++) {
			graph[i][j].visited = 0;
		}
	}

	Q.enQ(&graph[0][0]);
	graph[0][0].visited = 1;
	Node* w, * u;
	Node* target = &graph[HEIGHT - 1][WIDTH - 1];
	while (!Q.isEmpty()) {
		 w = Q.deQ();
		 if (w == target) return true;
		for (i = 0; i < w->adj.size(); i++) {
			if(w->adj[i]->visited == 0) {
				u = w->adj[i];
				u->visited = 1;
				u->parent = w;
				Q.enQ(u);
			}
		}
	}

}

void ofApp::bfsdraw() {
	int i, j;
	int x1, x2, y1, y2;
	for (i = 0; i < HEIGHT; i++) {
		for (j = 0; j < WIDTH; j++) {
			graph[i][j].colored = 0;
		}
	}
	{
		ofSetColor(ofColor::white);
		ofFill();
		ofDrawRectangle(0, 0, ofGetWidth(), ofGetHeight());

		ofSetColor(100);
		ofSetLineWidth(5);
		ofNoFill();
		ofDrawRectangle(0, 0, WIDTH * 100, HEIGHT * 100);
		int match_flag = 0;
		Node* temp;
		for (i = 0; i < HEIGHT; i++) {
			for (j = 0; j < WIDTH; j++) {
				if (i < HEIGHT - 1) {
					if (input[(i + 1) * 2][(j * 2) + 1] == '-') {
						ofDrawLine(100 * j, 100 * (i + 1), 100 * (j + 1), 100 * (i + 1)); //가로벽 (수직막아)
					}
				}
				if (j < WIDTH - 1) {
					if (input[(i * 2) + 1][(j + 1) * 2] == '|') {
						ofDrawLine(100 * (j + 1), 100 * i, 100 * (j + 1), 100 * (i + 1)); // 세로벽 (수평막아)
					}
				}
			}
		}
	}// 2nd comment end
	ofSetColor(ofColor::blue);
	ofSetLineWidth(7);
	while (!Q.isEmpty())
		Q.deQ(); // 큐 비우기


	Node* w = &graph[0][0];
	Q.enQ(w);
	w->colored = 1;

	Node* u;
	while (!Q.isEmpty()) {
		w = Q.deQ();
		if (w != &graph[0][0]) {

			ofDrawLine(w->centerX, w->centerY, w->parent->centerX, w->parent->centerY);
		}
		for (i = 0; i < w->adj.size(); i++) {
			if (w->adj[i]->visited == 1 && w->adj[i]->colored == 0) {
				u = w->adj[i];
				u->colored = 1;
				Q.enQ(u);
			}
		}
	}
	ofSetColor(ofColor::red);
	ofSetLineWidth(7);
	ofSetBackgroundAuto(false);
	w = &graph[HEIGHT - 1][WIDTH - 1];
	while (w != &graph[0][0]) {
		x1 = w->centerX;
		y1 = w->centerY;
		x2 = w->parent->centerX;
		y2 = w->parent->centerY;
		ofDrawLine(x1, y1, x2, y2);
		w = w->parent;
	}
	
}





//-- Stack
void ofApp::stack::push(Node* item) {
	stack_node* temp = (stack_node*)malloc(sizeof(stack_node));
	temp->item = item;
	temp->link = top;
	top = temp;

}
ofApp::Node* ofApp::stack::pop() {
	stack_node* temp = top;
	Node* item;
	item = temp->item;
	top = temp->link;
	free(temp);
	return item;
}
bool ofApp::stack::isEmpty() {
	if (top == NULL) {
		return true;
	}
	else return false;
}
//-- Queue
void ofApp::queue::enQ(Node* item) {
	queue_node* temp = (queue_node*)malloc(sizeof(queue_node));
	temp->item = item;
	temp->link = NULL;
	if (front) rear->link = temp;
	else front = temp;
	rear = temp;
}
ofApp::Node* ofApp::queue::deQ() {
	queue_node* temp = front;
	Node* item;
	item = temp->item;
	front = temp->link;
	free(temp);
	return item;

}
bool ofApp::queue::isEmpty() {
	if (front == NULL) {
		return true;
	}
	else return false;
}