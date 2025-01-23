#include "Model.h"
#include "GUI.h"
#include "GameState.h"

#define ASSET_PATH "../../Assets/"

#include <imgui.h>
#include <imgui_impl_sdl.h>
#include <imgui_impl_opengl3.h> //imgui und sdl_image funzen nicht miteinander!!!!

#include <array>

class Camera
{
public:
	glm::mat4 viewPOne, viewPTwo;
	glm::mat4* view = &viewPOne;
	int viewMatrixLocation = 0;
	glm::mat4 projection;
	int projectionMatrixLocation = 0;

	int lightPosLocation = 0;
	int lightColorLocation = 0;
	int lightPowerLocation = 0;
	int viewPosLocation = 0;

	glm::vec3 camPosPOne = glm::vec3(0.0f, 2.0f, 4.0f);
	glm::vec3 camPosPTwo = glm::vec3(8.0f, 2.0f, 4.0f);
	glm::vec3* camPos = &camPosPOne;
	glm::vec3 camFrontPOne = glm::vec3(1.0f, 0.0f, 0.0f);
	glm::vec3 camFrontPTwo = glm::vec3(-1.0f, 0.0f, 0.0f);
	glm::vec3* camFront = &camFrontPOne;

	float yawPOne = 0.0f, yawPTwo = 180.0f; //for the mouse rotation (yaw x-rotation, pitch y-rotation)
	float pitchPOne = 0.0f, pitchPTwo = 0.0f;
	float* yaw = &yawPOne;
	float* pitch = &pitchPOne;

	Camera();
	Camera(GLuint shaderID, SDL_Window* window, SDL_GLContext* glContext);
	~Camera();
	void MoveCamera(const Uint8* currentKeyStates, float deltaTime);
	void RotateCamera(SDL_Window* window, const int SCREEN_W, const int SCREEN_H);
	void DrawMouseCursor();
	void RenderGameStatusScreen(bool currentPlayerSelect, GameState gs, SDL_Window* window, int SW);
	bool RenderPauseMenu(int SW, int SH, bool& pause, bool& exit, SDL_Window* window, SDL_GLContext& context);
	void SetLightPosToCam();

private:
	const float baseCamSpeed = 2.0f;
	const float sensitivity = 60.0f;
	//Model cursorBall;
	GUI cursorIcon;
	GUI stateBackground;
	GUI logo;
	GUI currentPlayer;
	GUI gameStatus;
	GLuint cursorTexId;
	GLuint stateBackgroundTex;
	GLuint logoTex;
	std::array<std::string, 6> gameStates = {"Pause", "Player One is in Check", "Player Two is in Check", 
		"Player One is Checkmate", "Player Two is Checkmate", "Get Back one lost Figure"};
	float fontSize = 10.0f;

	bool optionActive = false;
	const int nResolutions = 5;
	int resolutions[5][2] = {
		{0, 0},
		{1280, 720},
		{1920, 1080},
		{2560, 1440},
		{3840, 2160}
	};
	int currResolution = 1;
	bool isFullscreen = false;

	std::string ChooseGameStatusString(GameState gs, bool currPlayer);
	void WriteConfigFile();
};
