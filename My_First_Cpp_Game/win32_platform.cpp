#include <windows.h>
#include <string>
#include <cstdlib>
#include <math.h>
#include <ctime>

//start variables for the map and rendering

float PlayerX = 2.0f;
float PlayerY = 2.0f;
float PlayerRotation = 90.0;

float horizontalShift = 0.0f;
float verticalShift = 0.0f;

int nMapHeight = 16;
int trueWidth = 8;
int mapWidth = 3 + (2 * (trueWidth - 1));

bool LeftArrowDown = false;
bool RightArrowDown = false;

bool upArrowDown = false;
bool downArrowDown = false;

std::string map;

//end variables for the map and rendering

bool running = true;

void* buffer_memory;
int buffer_width;
int buffer_height;
int buffer_size;

BITMAPINFO buffer_bitmap_info;

#include "renderer.cpp"

void draw_walls(std::string map);
float get_radians(float angle);
void create_maze(int mapWidth);

void create_manhattan_path(int x1, int y1, int x2, int y2);

LRESULT CALLBACK window_callback(_In_ HWND hwnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam) {
	LRESULT result = 0;
	
	switch (uMsg) {
		case WM_CLOSE:

		case WM_DESTROY: {
			running = false;
		} break;

		case WM_SIZE: {
			RECT rect;
			GetClientRect(hwnd, &rect);
			buffer_width = rect.right - rect.left;
			buffer_height = rect.bottom - rect.top;

			buffer_size = buffer_width * buffer_height * sizeof(unsigned int);

			if (buffer_memory) {
				VirtualFree(buffer_memory, 0, MEM_RELEASE);
			}
			buffer_memory = VirtualAlloc(0, buffer_size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

			buffer_bitmap_info.bmiHeader.biSize = sizeof(buffer_bitmap_info.bmiHeader);
			buffer_bitmap_info.bmiHeader.biWidth = buffer_width;
			buffer_bitmap_info.bmiHeader.biHeight = buffer_height;
			buffer_bitmap_info.bmiHeader.biPlanes = 1;
			buffer_bitmap_info.bmiHeader.biBitCount = 32;
			buffer_bitmap_info.bmiHeader.biCompression = BI_RGB;

		} break;

		case WM_KEYDOWN: {
			switch (wParam) {
				
				case VK_LEFT: {
						PlayerRotation = PlayerRotation - 1.0f;
				} break;

				case VK_RIGHT: {
						PlayerRotation = PlayerRotation + 1.0f;
				} break;

				case VK_UP: {
					verticalShift = 0.08f * (float)sin(get_radians(PlayerRotation));
					horizontalShift = 0.08f * (float)cos(get_radians(PlayerRotation));
					int mapCharacterNumber = 0;
					PlayerX = PlayerX + horizontalShift;
					PlayerY = PlayerY + verticalShift;
					mapCharacterNumber = ((int)PlayerY * mapWidth) + (int)PlayerX;
					if (map[mapCharacterNumber] == '#' || map[mapCharacterNumber] == '@') {
						PlayerX = PlayerX - horizontalShift;
						PlayerY = PlayerY - verticalShift;
					}
				} break;

				case VK_DOWN: {
					verticalShift = 0.04f * (float)sin(get_radians(PlayerRotation));
					horizontalShift = 0.04f * (float)cos(get_radians(PlayerRotation));
					int mapCharacterNumber = 0;
					PlayerX = PlayerX - horizontalShift;
					PlayerY = PlayerY - verticalShift;
					mapCharacterNumber = ((int)PlayerY * mapWidth) + (int)PlayerX;
					if (map[mapCharacterNumber] == '#' || map[mapCharacterNumber] == '@') {
						PlayerX = PlayerX + horizontalShift;
						PlayerY = PlayerY + verticalShift;
					}
				} break;
				
			}
		} break;

		default: {
			result = DefWindowProc(hwnd, uMsg, wParam, lParam);
		}
	}
	return result;
}

int __stdcall WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
	// Create Window Class
	WNDCLASS window_class = {};
	window_class.style = CS_HREDRAW | CS_VREDRAW;
	window_class.lpszClassName = "Game Window Class";
	window_class.lpfnWndProc = window_callback;

	// Register Class
	RegisterClass(&window_class);

	// Create Window
	HWND window = CreateWindow(window_class.lpszClassName, "First Person Engine", WS_OVERLAPPEDWINDOW | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, 1280, 720, 0, 0, hInstance, 0);
	HDC hdc = GetDC(window);

	//create Map
	//std::string map;

	srand(time(NULL));
	create_maze(mapWidth);
	int playerMapNumber = ((int)PlayerY * mapWidth) + (int)PlayerX;
	while (map[playerMapNumber] != ' ') {
		PlayerY = (rand() % (mapWidth - 3) + 1);
		PlayerX = (rand() % (mapWidth - 3) + 1);
		playerMapNumber = ((int)PlayerY * mapWidth) + (int)PlayerX;
	}

	while (running) {
		//Input
		MSG message;
		while (PeekMessage(&message, window, 0, 0, PM_REMOVE)) {
			TranslateMessage(&message);
			DispatchMessage(&message);
		}

		//simulate
		clear_screen(0x000000);
		draw_walls(map);

			playerMapNumber = ((int)PlayerY * mapWidth) + (int)PlayerX;
			if (map[playerMapNumber] == '%') {
				return 0;
			}

		//Render
		StretchDIBits(hdc, 0, 0, buffer_width, buffer_height, 0, 0, buffer_width, buffer_height, buffer_memory, &buffer_bitmap_info, DIB_RGB_COLORS, SRCCOPY);
	}
}

void draw_walls(std::string map) {
	float distance_to_wall_in_direction_player_is_looking = 0.0f;

	float angle_relative_to_ray = 45.f;

	float spaceBetweenRays = 0;
	spaceBetweenRays = 90.0f / buffer_width;

	float currentRayRotation = 0;
	currentRayRotation = PlayerRotation - 45.0f;
	float currentRayRotationPositiveVersion = 0.0f;
	int mapCharacterNumber = 0;

	for (int i = 0; i < buffer_width; i++) {
		angle_relative_to_ray = angle_relative_to_ray + spaceBetweenRays;
		currentRayRotation = currentRayRotation + spaceBetweenRays;
		if (currentRayRotation < 0) {
			currentRayRotationPositiveVersion = 360 + currentRayRotation;
		}
		float verticalShift = 0.01f * (float)sin(get_radians(currentRayRotation));
		float horizontalShift = 0.01f * (float)cos(get_radians(currentRayRotation));

		float rayLocationX = PlayerX;
		float rayLocationY = PlayerY;

		mapCharacterNumber = ((int)rayLocationY * mapWidth) + (int)rayLocationX;

		float distanceTraveled = 0.0f;

		while (map[mapCharacterNumber] != '#' && map[mapCharacterNumber] != '@' && map[mapCharacterNumber] != '%') {
			rayLocationX = rayLocationX + horizontalShift;
			rayLocationY = rayLocationY + verticalShift;
			distanceTraveled = distanceTraveled + 0.01f;
			mapCharacterNumber = ((int)rayLocationY * mapWidth) + (int)rayLocationX;
		}

		distance_to_wall_in_direction_player_is_looking = distanceTraveled * sin(get_radians(angle_relative_to_ray));

		int Size = (buffer_height / distance_to_wall_in_direction_player_is_looking * 1.5f);

		if (Size > buffer_height) {
			Size = buffer_height;
		}

		float colorScaleFactor = 1 / ((float)buffer_height / ((float)Size / 1.2));
		unsigned int color = 0x000000;
		if (map[mapCharacterNumber] == '#') {
			color = color + ((int)(0x0000dd * colorScaleFactor) * 0x010000); //red
			color = color + ((int)(0x0000dd * colorScaleFactor) * 0x000100); //green
			color = color + ((int)(0x0000ff * colorScaleFactor)); //blue
			draw_sized_column(i, Size, color);
		}
		else if (map[mapCharacterNumber] == '@') {
			color = color + ((int)(0x0000aa * colorScaleFactor) * 0x010000); //red
			color = color + ((int)(0x0000aa * colorScaleFactor) * 0x000100); //green
			color = color + ((int)(0x0000ff * colorScaleFactor)); //blue
			draw_sized_column(i, Size, color);
		}
		else if (map[mapCharacterNumber] == '%') {
			color = color + ((int)(0x0000ff * colorScaleFactor) * 0x000100);
			draw_sized_column(i, Size, color);
		}
	}
}

float get_radians(float angle) {
	return (angle * 0.0174533f);
}

void create_maze(int mapWidth) {

	int x1 = 0;
	int y1 = 0;
	int x2 = 0;
	int y2 = 0;

	//initialize the maze in a checker pattern
	for (int j = 0; j < mapWidth; j++) {
		if (j % 2 == 0) {
			for (int i = 0; i < mapWidth; i++) {
				if (i % 2 == 0) {
					map += "#";
				}
				else {
					map += "@";
				}
			}
		}
		else {
			for (int i = 0; i < mapWidth; i++) {
				if (i % 2 == 0) {
					map += "@";
				}
				else {
					map += "#";
				}
			}
		}
	}

	//create hallways within the maze
	for (int i = 0; i < mapWidth; i++) {
		x1 = 0;
		y1 = 0;
		x2 = 0;
		y2 = 0;

		//create two random coordinates not located on the walls
		while (x1 == 0 || x1 == mapWidth) {
			x1 = rand() % (mapWidth - 1);
		}
		while (y1 == 0 || y1 == mapWidth) {
			y1 = rand() % (mapWidth - 1);
		}
		while (x2 == 0 || x2 == mapWidth) {
			x2 = rand() % (mapWidth - 1);
		}
		while (y2 == 0 || y2 == mapWidth) {
			y2 = rand() % (mapWidth - 1);
		}
		//call manhattan
		create_manhattan_path(x1, y1, x2, y2);
	}

	int mapNumberDestination = (y2 * mapWidth) + x2;
	map[mapNumberDestination] = '%';
}

void create_manhattan_path(int x1, int y1, int x2, int y2) {
	int firstx1 = x1;
	int firsty1 = y1;
	int distance_To_x2 = x2 - x1;
	int distance_To_y2 = y2 - y1;
	int firstMapNumber = (y1 * mapWidth) + x1;
	int secondMapNumber = (y2 * mapWidth) + x2;
	map[firstMapNumber] = ' ';
	int negativeCatcher = 1;
	int y_negativeCatcher = 1;
	int v1 = 0;
	int v2 = 0;

	if (x2 < x1) {
		negativeCatcher = -1;
	}

	if (y2 < y1) {
		y_negativeCatcher = -1;
	}

	while (x1 != x2 || y1 != y2) {
		distance_To_x2 = x2 - x1;
		if (distance_To_x2 == 0) {
			v1 = 0;
		}
		else {
			v1 = abs(rand() % ((distance_To_x2 * negativeCatcher) + 1));
		}
		for (int i = 0; i < v1; i++) {
			x1 = x1 + (1 * negativeCatcher);
			int firstMapNumber = (y1 * mapWidth) + x1;
			int secondMapNumber = (y2 * mapWidth) + x2;
			map[firstMapNumber] = ' ';
		}

		distance_To_y2 = y2 - y1;
		if (distance_To_y2 == 0) {
			v2 = 0;
		}
		else {
			v2 = abs(rand() % ((distance_To_y2 * y_negativeCatcher) + 1));
		}
		for (int i = 0; i < v2; i++) {
			y1 = y1 + (1 * y_negativeCatcher);
			int firstMapNumber = (y1 * mapWidth) + x1;
			int secondMapNumber = (y2 * mapWidth) + x2;
			map[firstMapNumber] = ' ';
		}
	}
}