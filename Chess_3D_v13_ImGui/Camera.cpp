#include "Camera.h"

Camera::Camera()
{
}

Camera::Camera(GLuint shaderID, SDL_Window* window, SDL_GLContext* glContext)
{
	//View Matrix (Für die Transformation in Camera-Space)
	viewPOne = glm::mat4(1.0f);
	viewPTwo = glm::mat4(1.0f);
	viewMatrixLocation = glGetUniformLocation(shaderID, "u_view");

	//Projection Matrix (Für die z-Achse)
	projection = glm::perspective((float)glm::radians(110.0f), (16.0f / 9.0f), 0.01f, 100.0f);
	projectionMatrixLocation = glGetUniformLocation(shaderID, "u_projection");

	viewPOne = glm::lookAt(camPosPOne, camFrontPOne + camPosPOne, glm::vec3(0.0f, 1.0f, 0.0f));
	viewPTwo = glm::lookAt(camPosPTwo, camFrontPTwo + camPosPTwo, glm::vec3(0.0f, 1.0f, 0.0f));


	//Setup lighting
	lightPosLocation = glGetUniformLocation(shaderID, "u_lightPos");
	lightColorLocation = glGetUniformLocation(shaderID, "u_lightColor");
	lightPowerLocation = glGetUniformLocation(shaderID, "u_lightPower");
	viewPosLocation = glGetUniformLocation(shaderID, "u_viewPos");

	glUniform3f(lightPosLocation, 0.0f, 2.0f, 4.0f); 
	glUniform4f(lightColorLocation, 1.0f, 1.0f, 1.0f, 1.0f); //White light
	glUniform1f(lightPowerLocation, 1.0f); 
	glUniform3fv(viewPosLocation, 1, &camPos[0][0]); //Does this work?


	//Load the cursor model
	Model::InitTextures(ASSET_PATH"Markers/markerTex.png", cursorTexId, GL_RGB, true);

	std::vector<VertexGUI> cursorVertices;
	glo::Load2dObj(ASSET_PATH"GUI/cursorIcon.obj", cursorVertices);

	cursorIcon = GUI(&cursorVertices[0], cursorVertices.size());

	//Load the background status
	Model::InitTextures(ASSET_PATH"GUI/gameStatusBackground.png", stateBackgroundTex, GL_RGB, true);

	std::vector<VertexGUI> gameStatusBackground;
	glo::Load2dObj(ASSET_PATH"GUI/gameStatusBackground.obj", gameStatusBackground);

	stateBackground = GUI(&gameStatusBackground[0], gameStatusBackground.size());

	//Load the logo
	Model::InitTextures(ASSET_PATH"GUI/logo.png", logoTex, GL_RGBA, true);

	std::vector<VertexGUI> logoVG;
	glo::Load2dObj(ASSET_PATH"GUI/logo.obj", logoVG);

	logo = GUI(&logoVG[0], logoVG.size());

	std::vector<VertexGUI> currentPlayerVertices;
	glo::Load2dObj(ASSET_PATH"GUI/currentPlayer.obj", currentPlayerVertices);

	currentPlayer = GUI(&currentPlayerVertices[0], currentPlayerVertices.size());

	std::vector<VertexGUI> gameStateVertices;
	glo::Load2dObj(ASSET_PATH"GUI/GameState/gameState.obj", gameStateVertices);
	
	gameStatus = GUI(&gameStateVertices[0], gameStateVertices.size());


	//Setup ImGui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();

	// Setup Platform/Renderer bindings
	ImGui_ImplSDL2_InitForOpenGL(window, glContext);
	ImGui_ImplOpenGL3_Init();
}

Camera::~Camera()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();
}

//Movement
void Camera::MoveCamera(const Uint8* currentKeyStates, float deltaTime)
{
	float camSpeed = baseCamSpeed * deltaTime;

	if (currentKeyStates[SDL_SCANCODE_W])
	{
		*camPos += camSpeed * (*camFront);

		*view = glm::lookAt(*camPos, (*camPos + *camFront), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniform3fv(viewPosLocation, 1, &camPos[0][0]);
		glUniform3fv(lightPosLocation, 1, &camPos[0][0]);
	}
	else if (currentKeyStates[SDL_SCANCODE_S])
	{
		*camPos -= camSpeed * *camFront;

		*view = glm::lookAt(*camPos, (*camPos + *camFront), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniform3fv(viewPosLocation, 1, &camPos[0][0]);
		glUniform3fv(lightPosLocation, 1, &camPos[0][0]);
	}
	if (currentKeyStates[SDL_SCANCODE_A])
	{
		*camPos -= camSpeed * glm::normalize(glm::cross(*camFront, glm::vec3(0.0f, 1.0f, 0.0f)));

		*view = glm::lookAt(*camPos, (*camPos + *camFront), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniform3fv(viewPosLocation, 1, &camPos[0][0]);
		glUniform3fv(lightPosLocation, 1, &camPos[0][0]);
	}
	else if (currentKeyStates[SDL_SCANCODE_D])
	{
		*camPos += camSpeed * glm::normalize(glm::cross(*camFront, glm::vec3(0.0f, 1.0f, 0.0f)));

		*view = glm::lookAt(*camPos, (*camPos + *camFront), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniform3fv(viewPosLocation, 1, &camPos[0][0]);
		glUniform3fv(lightPosLocation, 1, &camPos[0][0]);
	}
	if (currentKeyStates[SDL_SCANCODE_SPACE])
	{
		*camPos += camSpeed * glm::vec3(0.0f, 1.0f, 0.0f);

		*view = glm::lookAt(*camPos, (*camPos + *camFront), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniform3fv(viewPosLocation, 1, &camPos[0][0]);
		glUniform3fv(lightPosLocation, 1, &camPos[0][0]);
	}
	else if (currentKeyStates[SDL_SCANCODE_LSHIFT])
	{
		*camPos -= camSpeed * glm::vec3(0.0f, 1.0f, 0.0f);

		*view = glm::lookAt(*camPos, (*camPos + *camFront), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniform3fv(viewPosLocation, 1, &camPos[0][0]);
		glUniform3fv(lightPosLocation, 1, &camPos[0][0]);
	}
}

void Camera::RotateCamera(SDL_Window* window, const int SCREEN_W, const int SCREEN_H)
{
	int mX, mY;
	SDL_GetMouseState(&mX, &mY);

	glm::vec2 vecMouse = glm::vec2(((float)mX - (float)SCREEN_W / 2.0f) / (float)SCREEN_W * 2.0f,
		(((float)SCREEN_H / 2.0f - (float)mY) / (float)SCREEN_H * 2.0f));

	//if (vecMouse.x != 0 && vecMouse.y != 0)
	//	std::cout << vecMouse.x << "\t" << vecMouse.y << "\n";

	*yaw += vecMouse.x * sensitivity;
	*pitch += vecMouse.y * sensitivity;

	if (*pitch > 89.0f) //so you cannot flip
		*pitch = 89.0f;
	if (*pitch < -89.0f)
		*pitch = -89.0f;

	glm::vec3 camDirection;
	camDirection.x = cosf(glm::radians(*yaw)) * cosf(glm::radians(*pitch));
	camDirection.y = sinf(glm::radians(*pitch));
	camDirection.z = sinf(glm::radians(*yaw)) * cosf(glm::radians(*pitch));

	//std::cout << camDirection.x << "\t" << camDirection.y << "\t" << camDirection.z << "\n";

	*camFront = glm::normalize(camDirection);


	*view = glm::lookAt(*camPos, (*camPos + *camFront), glm::vec3(0.0f, 1.0f, 0.0f));

	//std::cout << camFront->x << "\t" << camFront->y << "\t" << camFront->z << "\n";

	SDL_WarpMouseInWindow(window, SCREEN_W / 2, SCREEN_H / 2); //This invokes a mousemotion event; so I flush it
	SDL_FlushEvent(SDL_MOUSEMOTION);
}

//GUI
void Camera::DrawMouseCursor()
{
	cursorIcon.BindGUI();

	//Draw Texture
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, cursorTexId);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	cursorIcon.UnbindGUI();

}

void Camera::RenderGameStatusScreen(bool currentPlayerSelect, GameState gs, SDL_Window* window, int SW)
{
	ImGui::SetNextWindowPos({ 0.0f, 0.0f });
	ImGui::SetNextWindowSize({ (SW <= 1920) ? (float)SW : 1920.0f, fontSize + 20 }); //SW DARF NICHT HÖHER SEIN ALS 1920
	ImGui::Begin("StatusBar", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar);

	ImGui::SetWindowFontScale(1.5f);
	fontSize = ImGui::GetFontSize();

	std::string currGameStatus = ChooseGameStatusString(gs, currentPlayerSelect);
	ImGui::SameLine((ImGui::GetWindowWidth() - ImGui::CalcTextSize(currGameStatus.c_str()).x) / 2.0f, -1.0f);
	ImGui::Text(currGameStatus.c_str());
	std::string currPlayerName = (currentPlayerSelect) ? "Player One" : "Player Two";
	ImGui::SameLine(ImGui::GetWindowWidth() - ImGui::CalcTextSize(currPlayerName.c_str()).x - 10, -1.0f);
	ImGui::Text(currPlayerName.c_str());

	ImGui::End();
}

bool Camera::RenderPauseMenu(int SW, int SH, bool& pause, bool& exit, SDL_Window* window, SDL_GLContext& context)
{
	float winXSize = ((SW <= 1920) ? (float)SW : 1920.0f);
	float winYSize = ((SH <= 1080) ? (float)SH : 1080.0f);
	ImGui::SetNextWindowPos({ winXSize * 0.1f, winYSize * 0.1f });
	ImGui::SetNextWindowSize({ winXSize * 0.8f, winYSize * 0.8f }); //SW DARF NICHT HÖHER SEIN ALS 1920
	ImGui::Begin("PauseMenu", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar);

	ImGui::SetWindowFontScale(1.5f);

	if (!optionActive)
	{
		if (ImGui::Button("Resume"))
		{
			pause = false;
			SDL_ShowCursor(false);
		}
		if (ImGui::Button("Options"))
		{
			optionActive = true;
		}
		if (ImGui::Button("Exit"))
		{
			exit = true;
		}
	}
	else
	{
		if (ImGui::SliderInt("Resolution", &currResolution, 0, nResolutions - 1))
		{
			
		}
		if (currResolution == 0)
			ImGui::Text("Custom");
		else
			ImGui::Text((std::to_string(resolutions[currResolution][0]) + "x" + std::to_string(resolutions[currResolution][1])).c_str());
		if (ImGui::InputInt("Custom Resolution Width", &resolutions[0][0]))
		{

		}
		//ImGui::SameLine();
		if (ImGui::InputInt("Custom Resolution Height", &resolutions[0][1]))
		{

		}
		//ImGui::NewLine();
		if (ImGui::Button("Toggle Full Screen"))
		{
			if (isFullscreen) 
			{
				SDL_SetWindowFullscreen(window, 0);
				isFullscreen = false;
			}
			else
			{
				SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
				isFullscreen = true;
			}
		}
		if (ImGui::Button("Accept"))
		{
			/*if (isFullscreen)
			{
				//SDL_SetWindowFullscreen(window, 0);

				//SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
				SDL_SetWindowSize(window, resolutions[currResolution][0], resolutions[currResolution][1]);
				int rw, rh;
				SDL_GL_GetDrawableSize(window, &rw, &rh);
				//glViewport(0, 0, rw, rh);

			}
			else
			{
				SDL_SetWindowFullscreen(window, 0);

				SDL_SetWindowSize(window, resolutions[currResolution][0], resolutions[currResolution][1]);
				glViewport(0, 0, resolutions[currResolution][0], resolutions[currResolution][1]);
			}*/

			//SDL_GLContext tempContext = context;

			/*SDL_GL_DeleteContext(context);
			SDL_DestroyWindow(window);

			Uint32 flags = SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_FULLSCREEN; // | SDL_WINDOW_FULLSCREEN_DESKTOP

			window = SDL_CreateWindow("Chess 3D Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
				2560, 1440, flags);
			context = SDL_GL_CreateContext(window);
			//context = tempContext;
			//Reload all the textures and models or copy context
			

			ImGui::End();
			ImGui_ImplOpenGL3_Shutdown();
			ImGui_ImplSDL2_Shutdown();
			ImGui::DestroyContext();

			//wglShareLists()

			IMGUI_CHECKVERSION();
			ImGui::CreateContext();
			ImGuiIO& io = ImGui::GetIO(); (void)io;

			io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;

			// Setup Dear ImGui style
			ImGui::StyleColorsDark();

			// Setup Platform/Renderer bindings
			ImGui_ImplSDL2_InitForOpenGL(window, context);
			ImGui_ImplOpenGL3_Init();*/

			//Settings should be saved to config file

			//On every start this config file should be read. It includes resolution, fullscreen
			WriteConfigFile();

			optionActive = false;			

			return true;
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel"))
		{
			optionActive = false;
		}
	}

	ImGui::End();
	return false;
}

void Camera::SetLightPosToCam()
{
	glUniform3fv(viewPosLocation, 1, &camPos[0][0]);
	glUniform3fv(lightPosLocation, 1, &camPos[0][0]);
}

std::string Camera::ChooseGameStatusString(GameState gs, bool currPlayer)
{
	if (gs.nothing)
		return "";
	else if (gs.checkmateP1)
		return gameStates[3];
	else if (gs.checkmateP2)
		return gameStates[4];
	else if ((gs.checkP1 && !gs.checkP2) || (gs.checkP1 && currPlayer))
		return gameStates[1];
	else if ((gs.checkP2 && !gs.checkP1) || (gs.checkP2 && !currPlayer))
		return gameStates[2];
	else if (gs.getBackLostFigure)
		return gameStates[5];
	else if (gs.pause)
		return gameStates[0];
}

void Camera::WriteConfigFile()
{
	std::fstream configFile;
	configFile.open(ASSET_PATH"chess.cfg", std::ios::out);
	if (configFile.is_open())
		{
		configFile << resolutions[currResolution][0] << "\n";
		configFile << resolutions[currResolution][1] << "\n";
		configFile << isFullscreen << "\n";
		configFile.close();
	}
}
