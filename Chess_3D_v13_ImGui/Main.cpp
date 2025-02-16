#include <GL/glew.h>
#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <SDl2/SDL_image.h>
#include <GL/GL.h>
#include <GL/GLU.h>

#include "libs/glm/glm.hpp" //OpenGL-Mathe library für Matrix bewegung
#include "libs/glm/ext/matrix_transform.hpp"
#include "libs/glm/gtc/matrix_transform.hpp"

#pragma once
#include "Shader.h"
#include "Board.h"

/*
--TODO:
	Spiel status: Nix, Pause, Schach, Schachmatt (Gewinner), Zurückholen von Figur
	Wenn Schachmatt, dann muss ein zug passieren, der den König unschachmatt setzt.
	Spieler anzeige vergrößern.
	Pause zeigt wenn könig schach ist alle felder an
	Wenn man figur zurückholt und der könig danach schach ist, wird das aktuell erst eine runde später angezeigt!
	Wenn ein figur verschoben wird und dadurch der eigene könig schach ist. das darf nicht passieren
*/

//ADD ANTI-ALIASING

void OpenGLDebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity,
	GLsizei lenght, const GLchar* message, const void* userParam) //Bei Fehler ruft OpenGl diese Funktion auf
{
	if(severity == GL_DEBUG_SEVERITY_HIGH || severity == GL_DEBUG_SEVERITY_MEDIUM)
		std::cout << "[OpenGL Error] " << message << std::endl;
}

void ReadConfigFile(int& width, int& height, bool& isFullscreen)
{
	std::fstream configFile;
	configFile.open(ASSET_PATH"chess.cfg", std::ios::in);
	if (configFile.is_open())
	{
		std::string line = "";
		int nLines = 0;
		while (std::getline(configFile, line))
		{
			if (nLines == 0)
				width = std::stoi(line);
			else if (nLines == 1)
				height = std::stoi(line);
			else if (nLines == 2)
				isFullscreen = std::stoi(line);
			nLines++;
		}
		configFile.close();
	}
}

int main(int argc, char* args[])
{
	bool restart = false;
	bool firstStart = true;
	//Camera* cam = nullptr;
	BoardData boardDataSave;
	do
	{
		SDL_Window* window;
		int SCREEN_W = 1280;
		int SCREEN_H = 720;
		bool isFullscreen = false;
		//const int SCREEN_W = 2560;
		//const int SCREEN_H = 1440;
		SDL_Init(SDL_INIT_EVERYTHING);
		IMG_Init(IMG_INIT_PNG);


		SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8); //Farbkanal hat 8 bit; 0 bis 255
		SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32); //Bitgröße des Buffers; jeder Pixel braucht 32bit; 4 Werte pro Pixel, mit je 8bit
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1); //Double Buffer einstellen
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 16);
		//Eigentlich sind das die Standardeinstellungen, aber sicherheitshalber

#ifdef _DEBUG //NUR FÜR DEBUGING! (langsamer wie im Release)
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
#endif


		ReadConfigFile(SCREEN_W, SCREEN_H, isFullscreen);

		Uint32 flags = SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI; // | SDL_WINDOW_FULLSCREEN
		if (isFullscreen)
			flags |= SDL_WINDOW_FULLSCREEN;

		window = SDL_CreateWindow("Chess 3D Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
			SCREEN_W, SCREEN_H, flags);
		SDL_GLContext glContext = SDL_GL_CreateContext(window);

		//Glew initalisieren
		GLenum err = glewInit();
		if (err != GLEW_OK)
		{
			std::cout << "Error: " << glewGetErrorString(err) << std::endl;
			std::cin.get();
			return -1;
		}
		std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;


		//Initalisierung der Debug-Callbackfunktion
#ifdef _DEBUG
	//Neue Version
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(OpenGLDebugCallback, 0);
#endif

		SDL_ShowCursor(false);

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_MULTISAMPLE);
		//glEnable(GL_CULL_FACE);

		Board board = Board();
		if (!firstStart)
			board.bd = boardDataSave;
		else
			firstStart = false;

		//Shader anlegen (Verzeichnis in VS ist eine Illusion)
		Shader shader("shader.vert", "shader.frag");
		Shader shaderGUI("shaderGUI.vert", "shaderGUI.frag");

		shader.Bind();
		int textureUniformLocation = glGetUniformLocation(shader.getShaderID(), "u_texture");
		if (textureUniformLocation != -1)
			glUniform1i(textureUniformLocation, 0); //Weist einen Textur-Slot zu
		shader.Unbind();

		shaderGUI.Bind();
		int cursorTexUniformLocation = glGetUniformLocation(shaderGUI.getShaderID(), "u_texture_gui");
		if (cursorTexUniformLocation != -1)
			glUniform1i(cursorTexUniformLocation, 1); //Weist einen Textur-Slot zu
		shaderGUI.Unbind();

		Uint64 perfCounterFrequency = SDL_GetPerformanceFrequency(); //Gibt die Einheit dieser CPU-Zeit an
		Uint64 lastCounter = SDL_GetPerformanceCounter(); //Speziall Zeitmessung innerhalb der CPU; genauer
		float deltaTime = 0.0f;

		int modelMatrixLocation = glGetUniformLocation(shader.getShaderID(), "u_model");

		//delete cam;
		shader.Bind();
		Camera* cam = new Camera(shader.getShaderID(), window, &glContext);
		shader.Unbind();

		//SDL_SetHint(SDL_HINT_VIDEO_HIGHDPI_DISABLED, "1");

		float time = 0.0f;
		bool gameOver = false;
		bool pause = false;
		bool close = false;
		//enum gameState { PAUSE, CHECK1, CHECK2, CHECKMATE1, CHECKMATE2, GETBACKLOSTFIGURE, NOTHING };
		while (!close) //Main Schleife
		{
			glClearColor(0.0f, 0.0f, 0.5f, 1.0f); //0;0, 1;255
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //Löscht den Bildschirm auf die mit glClearColor eingestellte Farbe
			time += deltaTime;


			shader.Bind();
			//Draw the board onto the screen
			board.DrawFullBoard(modelMatrixLocation, cam->viewMatrixLocation, *cam->view, cam->projectionMatrixLocation, cam->projection, time);
			shader.Unbind();

			//shaderGUI.Bind();
			cam->DrawMouseCursor();


			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplSDL2_NewFrame();
			ImGui::NewFrame();

			cam->RenderGameStatusScreen(board.GetCurrentPlayer(), board.bd.gameState, window, SCREEN_W);
			//cam->GameStatusBackground();
			//shaderGUI.Unbind();

			restart = false;

			if (pause)
				restart = cam->RenderPauseMenu(SCREEN_W, SCREEN_H, pause, close, window, glContext);

			if (restart)
				break;

			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

			/*else
			{
				delete cam;
				shader.Bind();
				cam = new Camera(shader.getShaderID());
				shader.Unbind();
				board = Board();
			}*/


			//Frame anzeigen
			SDL_GL_SwapWindow(SDL_GL_GetCurrentWindow());

			SDL_Event events;
			while (SDL_PollEvent(&events))
			{
				ImGui_ImplSDL2_ProcessEvent(&events);

				if (events.type == SDL_QUIT)
				{
					close = true;
				}
				else if (events.type == SDL_KEYDOWN)
				{
					if (events.key.keysym.sym == SDLK_ESCAPE)
					{
						if (pause)
						{
							pause = false;
							board.bd.gameState = board.bd.lastGameState;
							SDL_ShowCursor(false);
						}
						else
						{
							pause = true;
							board.bd.lastGameState = board.bd.gameState;
							board.bd.gameState.SetAllToFalse();
							board.bd.gameState.pause = 1;
							SDL_ShowCursor(true);
						}
					}
					else if (board.bd.gameState.getBackLostFigure && events.key.keysym.sym == SDLK_7)
					{
						board.bd.gameState.getBackLostFigure = 0;
						board.bd.gameState.SetToNothing();
						board.SetCurrentPlayer(cam);
					}
				}
				else if (events.type == SDL_MOUSEMOTION && !pause && !gameOver)
				{
					cam->RotateCamera(window, SCREEN_W, SCREEN_H);
				}
				else if (events.type == SDL_MOUSEBUTTONDOWN && !pause && !gameOver)
				{
					using glm::vec3;
					using glm::dot;

					//camFront is v
					vec3 v = *cam->camFront;
					vec3 w = vec3(0.0f, 0.0f, 0.0f) - *cam->camPos;

					vec3 xzPlaneNormal = vec3(0.0f, 1.0f, 0.0f);

					float scalingValue = dot(w, xzPlaneNormal) / dot(v, xzPlaneNormal);

					vec3 intersection = *cam->camPos + scalingValue * v;

					//check in which board field the intersection lays
					if (events.button.button == SDL_BUTTON_LEFT)
					{
						gameOver = board.SelectFigure(intersection, cam);
					}
					if (events.button.button == SDL_BUTTON_RIGHT)
					{
						gameOver = board.MoveFigure(intersection, shaderGUI.getShaderID(), cam); //returns wether the game is over
						shader.Bind();
						cam->SetLightPosToCam();
						shader.Unbind();
					}
				}
			}

			//Check the key states directly
			if (!pause && !gameOver)
			{
				shader.Bind();
				const Uint8* currentKeyStates = SDL_GetKeyboardState(NULL);
				cam->MoveCamera(currentKeyStates, deltaTime);
				shader.Unbind();
			}


			Uint64 endCounter = SDL_GetPerformanceCounter();
			Uint64 counterElapsed = endCounter - lastCounter;
			deltaTime = (float)counterElapsed / (float)perfCounterFrequency;
			Uint32 FPS = (Uint32)((float)perfCounterFrequency / (float)counterElapsed);
			//std::cout << deltaTime << std::endl;
			lastCounter = endCounter;
		}

		boardDataSave = board.bd;
		delete cam;

		SDL_DestroyWindow(window);
		SDL_GL_DeleteContext(glContext);

		IMG_Quit();
		SDL_Quit();
	} while (restart);

	return 0;
}