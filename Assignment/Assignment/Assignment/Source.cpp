#include <Windows.h>
#include <gl/GL.h>
#include <gl/GLU.h>
#include <cmath>
#include <windowsx.h>
#include <iostream>
#include <cstdio> // For printf
#include "util.h"
#include <vector>

#pragma comment (lib, "OpenGL32.lib")
#pragma comment (lib, "GLU32.lib")

#define WINDOW_TITLE "OpenGL Window"


// mouse movement
float lastX = 0.0f, lastY = 0.0f;
float zoomLevel = 2000;


float cameraMoveSpeed = 0.1f;
Vector3 cam = Vector3(0.01, 0.5, 2);
Vector3 lookAt = Vector3(0, 0, 0);



struct ColorSet {
	Vector3 primary;
	Vector3 light;
	Vector3 dark;

	ColorSet(Vector3 primary, Vector3 light, Vector3 dark)
		: primary(primary), light(light), dark(dark) {}
};

std::vector<ColorSet> colorSets = {
	ColorSet(Vector3(178, 160, 130), Vector3(192, 172, 149), Vector3(118, 99, 70)), // Original
	ColorSet(Vector3(243, 174, 189), Vector3(249, 200, 222), Vector3(222, 134, 160)), // Pink
	ColorSet(Vector3(100, 149, 237), Vector3(135, 206, 250), Vector3(70, 130, 180)), // Blue
	ColorSet(Vector3(45, 45, 45), Vector3(75, 75, 75), Vector3(25, 25, 25)) // dark gray
};

std::vector<GLuint> textures;

int currentSetIndex = 0;
int textureIndex = 0;

// Body colors
Vector3 BODY_COLOR = colorSets[0].primary;
Vector3 BODY_COLOR_LIGHT = colorSets[0].light;
Vector3 BODY_COLOR_DARK = colorSets[0].dark;

Vector3 WHITE_COLOR = Vector3(255, 255, 255);

Vector3 GREY_COLOR = Vector3(180, 180, 180);
Vector3 SPINE_COLOR = Vector3(200, 200, 200);

Vector3 BLACK_COLOR_LIGHT = Vector3(80, 80, 80);
Vector3 BLACK_COLOR = Vector3(70, 70, 70);
Vector3 BLACK_COLOR_DARK = Vector3(60, 60, 60);


Vector3 q1Rotation = Vector3();


GLUquadricObj* var = NULL;



float light1Pos[] = { 0, 400, 0 };
float lightDif[] = { 1, 1, 1 };

float light0Pos[] = { 0, 0, 400 };
float lightAmb[] = { 1, 1, 1 };
float light2Pos[] = { 400, 0, 0 };
float light3Pos[] = { 0, 0, -400 };
float light4Pos[] = { -400, 0,  0 };


float light1moveSpeed = 2.0f;

float no_mat[] = { 0.0f, 0.0f, 0.0f, 1.0f };
float mat_ambient[] = { 0.7f, 0.7f, 0.7f, 1.0f };
float mat_ambient_color[] = { 0.0f, 0.0f, 1.0f, 1.0f };
float mat_diffuse[] = { 0.0f, 1.0f, 0.0f, 1.0f };
float mat_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };

//animation
Vector3 finger1 = Vector3(-80, 0, 0);
Vector3 finger2 = Vector3(-80, 0, 0);
Vector3 finger3 = Vector3(-80, 0, 0);
Vector3 finger4 = Vector3(-80, 0, 0);
Vector3 finger5 = Vector3(-80, 0, 0);
Vector3 leftPalm = Vector3();
Vector3 rightPalm = Vector3();
Vector3 leftForearm = Vector3(-20, 0, 0);   // x positive move backward, y forearm rotation
Vector3 rightForearm = Vector3(-20, 0, 0);
Vector3 leftUpperarm = Vector3(0, -10, 10);  // x positive move backward, y positive shoulder roll backwards, z positive open outwards
Vector3 rightUpperarm = Vector3(0, 10, -10);  // x positive move backward, y negative shoulder roll backwards, z negative open outwards
Vector3 leftFoot = Vector3();
Vector3 rightFoot = Vector3();
Vector3 leftCalf = Vector3(10, 0, 0); // positive x go back
Vector3 rightCalf = Vector3(10, 0, 0);
Vector3 leftThigh = Vector3(-10, 15, 0); //positive x go back, positive y roll backwards, positive z open outwards;
Vector3 rightThigh = Vector3(-10, 15, 0);
Vector3 body = Vector3();

Vector3 bulletTracking = Vector3();
Vector3 bulletTrackingBody = Vector3();

bool shooting = false;
float bulletLimit = -2000; // Initialize to a value outside the visible range
float lastBulletPosition = bulletLimit; // Initialize to a value outside the visible range
bool findLast = false;

bool shootingBody = false;
float bulletLimitBody = -2000; // Initialize to a value outside the visible range
float lastBulletPositionBody = bulletLimitBody; // Initialize to a value outside the visible range
bool findLastBody = false;

float gunPosition = 10;
float barrellPosition = 65;

bool bodyGunOut = false;
bool bodyGunOut2 = false;
bool bodyGunIn = false;
bool bodyGunIn2 = false;

Vector3 spikeBall = Vector3();
bool spikeBallSpin = false;
bool spikeBallSpin2 = false;
bool spikeBallSpin3 = false;
float spikeBallSpinSpeed = 0.0f;

bool launchSpike = false;
bool launchSpike2 = false;

float actionSpeed = 1;

bool reset = false;
bool openFingers = false;
bool closeFingers = false;
bool punchLeft = false;
bool punchRight = false;
bool peace = false;
bool peacePart2 = false;
bool surprise = false;
bool surprisePart2 = false;
bool thumbsUp = false;
bool fightStance = false;
bool kick = false;
bool kick2 = false;
bool kick3 = false;
bool taunt = false;
bool taunt2 = false;
bool taunt3 = false;
bool taunt4 = false;
bool taunt5 = false;


bool MoveTowards(float& current, float target, float speed) {
	if (current < target) {
		current += speed;
		if (current > target) current = target; // Clamp to target
	}
	else if (current > target) {
		current -= speed;
		if (current < target) current = target; // Clamp to target
	}

	return current == target;
}

bool MoveAllFingers(float target) {
	bool completed = true;

	completed &= MoveTowards(finger1.x, target, actionSpeed);
	completed &= MoveTowards(finger2.x, target, actionSpeed);
	completed &= MoveTowards(finger3.x, target, actionSpeed);
	completed &= MoveTowards(finger4.x, target, actionSpeed);
	completed &= MoveTowards(finger5.x, target, actionSpeed);
	completed &= MoveTowards(finger1.z, 0, actionSpeed);
	completed &= MoveTowards(finger2.z, 0, actionSpeed);
	completed &= MoveTowards(finger3.z, 0, actionSpeed);
	completed &= MoveTowards(finger4.z, 0, actionSpeed);
	completed &= MoveTowards(finger5.z, 0, actionSpeed);

	return completed;
}


bool FingerTaunt(float target) {
	bool completed = true;

	completed &= MoveTowards(finger2.x, target, actionSpeed);
	completed &= MoveTowards(finger3.x, target, actionSpeed);
	completed &= MoveTowards(finger4.x, target, actionSpeed);
	completed &= MoveTowards(finger5.x, target, actionSpeed);

	return completed;
}

bool Taunt(float tauntSpeed) {
	bool completed = true;

	if (!taunt2 && !taunt3 && !taunt4 && !taunt5) {

		completed &= MoveTowards(leftForearm.x, -20, tauntSpeed);
		completed &= MoveTowards(leftForearm.y, 90, tauntSpeed);
		completed &= MoveTowards(leftUpperarm.x, -70, tauntSpeed);
		completed &= MoveTowards(leftUpperarm.y, 0, tauntSpeed);
		completed &= MoveTowards(leftUpperarm.z, 10, tauntSpeed);
		completed &= MoveAllFingers(-10);

		taunt2 = completed;
	}

	if (taunt2) {
		completed &= FingerTaunt(-45);

		taunt3 = completed;
		taunt2 = !completed;
	}

	if (taunt3) {
		completed &= MoveAllFingers(-10);

		taunt4 = completed;
		taunt3 = !completed;
	}

	if (taunt4) {
		completed &= FingerTaunt(-45);

		taunt5 = completed;
		taunt4 = !completed;
	}

	if (taunt5) {
		completed &= FingerTaunt(-20);

		taunt5 = !completed;
	}

	return completed;
}

bool Kick(float punchSpeed) {
	bool completed = true;

	if (!kick2 && !kick3) {

		completed &= MoveTowards(leftThigh.x, -140, punchSpeed * 1.5);
		completed &= MoveTowards(leftThigh.y, 10, punchSpeed / 2);
		completed &= MoveTowards(leftCalf.x, 80, punchSpeed*2);

		kick2 = completed;
	}

	if (kick2) {

		completed &= MoveTowards(leftThigh.x, -90, punchSpeed*2);
		completed &= MoveTowards(leftCalf.x, 10, punchSpeed*3);

		kick3 = completed;
		kick2 = !completed;
	}
	
	if (kick3) {
		completed &= MoveTowards(leftThigh.x, -10, punchSpeed);
		completed &= MoveTowards(leftCalf.x, 10, punchSpeed);
		completed &= MoveTowards(leftThigh.y, 15, punchSpeed);

		kick3 = !completed;
	}

	return completed;
}

bool PunchLeft(float punchSpeed) {
	bool completed = true;

	completed &= MoveTowards(leftForearm.x, 0, punchSpeed);
	completed &= MoveTowards(leftForearm.y, -90, punchSpeed);
	completed &= MoveTowards(leftUpperarm.x, -90, punchSpeed);
	completed &= MoveTowards(leftUpperarm.y, 0, punchSpeed);
	completed &= MoveTowards(leftUpperarm.z, 0, punchSpeed);
	completed &= MoveAllFingers(-80);
	return completed;
}

bool PunchRight(float punchSpeed) {
	bool completed = true;
	completed &= MoveTowards(rightForearm.x, 0, punchSpeed);
	completed &= MoveTowards(rightUpperarm.x, -90, punchSpeed);
	completed &= MoveTowards(rightUpperarm.y, 0, punchSpeed);
	completed &= MoveTowards(rightUpperarm.z, -10, punchSpeed);
	
	return completed;

}

bool Peace(float animationSpeed) {
	bool completed = true;

	if (!peacePart2) {
		completed &= MoveTowards(leftForearm.x, -90, animationSpeed);
		completed &= MoveTowards(leftForearm.y, -90, animationSpeed);
		completed &= MoveTowards(leftUpperarm.x, -80, animationSpeed);
		completed &= MoveTowards(leftUpperarm.y, 0, animationSpeed);
		completed &= MoveTowards(leftUpperarm.z, 0, animationSpeed);
		MoveAllFingers(-80);

		peacePart2 = completed;
	}

	if (peacePart2) {
		completed &= MoveTowards(finger2.x, 0, animationSpeed * 2);
		completed &= MoveTowards(finger3.x, 0, animationSpeed * 2);
		completed &= MoveTowards(finger2.z, 4, animationSpeed * 2);
		completed &= MoveTowards(finger3.z, -4, animationSpeed * 2);
	}

	return completed;
}

bool Surprise(float animationSpeed) {
	bool completed = true;

	if (!surprisePart2) {
		completed &= MoveTowards(leftForearm.x, -90, animationSpeed);
		completed &= MoveTowards(leftForearm.y, 90, animationSpeed);
		completed &= MoveTowards(leftUpperarm.x, -80, animationSpeed);
		completed &= MoveTowards(leftUpperarm.y, 0, animationSpeed);
		completed &= MoveTowards(leftUpperarm.z, 0, animationSpeed);
		completed &= MoveAllFingers(-80);

		surprisePart2 = completed;
	}

	if (surprisePart2) {
		completed &= MoveTowards(finger3.x, 0, animationSpeed * 2);
	}

	return completed;
}

bool LaunchSpike() {
	bool completed = true;

	if (!launchSpike2) {
		spikeBall.y += 15;
		completed = (spikeBall.y > 2000);

		launchSpike2 = completed;
	}

	if (launchSpike2) {
		spikeBall.y -= 20;

		completed = (spikeBall.y <= 0);

		spikeBallSpin3 = completed;
		launchSpike2 = !completed;
	}

	return completed;
}

bool BodyGunRetract(float gunSpeed) {
	bool completed = true;

	if (!bodyGunIn2) {
		completed &= MoveTowards(barrellPosition, 145, gunSpeed);

		bodyGunIn2 = completed;
	}

	if (bodyGunIn2) {
		completed &= MoveTowards(gunPosition, 10, gunSpeed);
		completed &= MoveTowards(barrellPosition, 65, gunSpeed);

		bodyGunIn2 = !completed;
	}

	return completed;
}

bool BodyGun(float gunSpeed) {
	bool completed = true;

	if (!bodyGunOut2) {
		completed &= MoveTowards(gunPosition, 90, gunSpeed);
		completed &= MoveTowards(barrellPosition, 145, gunSpeed);

		bodyGunOut2 = completed;
	}

	if (bodyGunOut2) {
		completed &= MoveTowards(barrellPosition, 170, gunSpeed);

		bodyGunOut2 = !completed;
	}

	return completed;
}

bool SpinSpike() {
	bool completed = true;
	if (!spikeBallSpin2 && !spikeBallSpin3) {
		

		rightPalm.y += spikeBallSpinSpeed;

		rightPalm.y = fmod(rightPalm.y, 360);
		completed &= MoveTowards(spikeBallSpinSpeed, 4, 0.02f);
		printf("spinning %f %d\n", spikeBallSpinSpeed, completed);
		spikeBallSpin2 = completed;
	}

	if (spikeBallSpin2) {
		rightPalm.y += spikeBallSpinSpeed;

		rightPalm.y = fmod(rightPalm.y, 360);

		completed = spikeBallSpin3;

		spikeBallSpin2 = !completed;
	}

	if (spikeBallSpin3) {
		rightPalm.y += spikeBallSpinSpeed;

		rightPalm.y = fmod(rightPalm.y, 360);
		completed &= MoveTowards(spikeBallSpinSpeed, 0, 0.02f);

		spikeBallSpin3 = !completed;
	}

	return completed;
}

bool ThumbsUp(float animationSpeed) {
	bool completed = true;
	completed &= MoveTowards(leftForearm.x, -90, animationSpeed);
	completed &= MoveTowards(leftForearm.y, 0, animationSpeed);
	completed &= MoveTowards(leftUpperarm.x, -10, animationSpeed);
	completed &= MoveTowards(leftUpperarm.y, 0, animationSpeed);
	completed &= MoveTowards(leftUpperarm.z, 0, animationSpeed);
	completed &= MoveTowards(finger2.x, -80, animationSpeed);
	completed &= MoveTowards(finger3.x, -80, animationSpeed);
	completed &= MoveTowards(finger4.x, -80, animationSpeed);
	completed &= MoveTowards(finger5.x, -80, animationSpeed);

	if (completed) {
		completed &= MoveTowards(finger1.x, 0, animationSpeed * 2);
	}

	return completed;
}

bool FightStance(float animationSpeed) {
	bool completed = true;
	completed &= MoveTowards(leftForearm.x, -110, animationSpeed);
	completed &= MoveTowards(leftForearm.y, 0, animationSpeed);
	completed &= MoveTowards(leftUpperarm.x, -15, animationSpeed);
	completed &= MoveTowards(leftUpperarm.y, -20, animationSpeed);
	completed &= MoveTowards(leftUpperarm.z, 20, animationSpeed);

	completed &= MoveTowards(rightForearm.x, -110, animationSpeed);
	completed &= MoveTowards(rightUpperarm.x, -15, animationSpeed);
	completed &= MoveTowards(rightUpperarm.y,20, animationSpeed);
	completed &= MoveTowards(rightUpperarm.z, -20, animationSpeed);
	completed &= MoveAllFingers(-80);

	return completed;
}


bool Default() {
	bool completed = true;

	completed &= MoveTowards(finger1.x, -80, actionSpeed);
	completed &= MoveTowards(finger1.y, 0, actionSpeed);
	completed &= MoveTowards(finger1.z, 0, actionSpeed);

	completed &= MoveTowards(finger2.x, -80, actionSpeed);
	completed &= MoveTowards(finger2.y, 0, actionSpeed);
	completed &= MoveTowards(finger2.z, 0, actionSpeed);

	completed &= MoveTowards(finger3.x, -80, actionSpeed);
	completed &= MoveTowards(finger3.y, 0, actionSpeed);
	completed &= MoveTowards(finger3.z, 0, actionSpeed);

	completed &= MoveTowards(finger4.x, -80, actionSpeed);
	completed &= MoveTowards(finger4.y, 0, actionSpeed);
	completed &= MoveTowards(finger4.z, 0, actionSpeed);

	completed &= MoveTowards(finger5.x, -80, actionSpeed);
	completed &= MoveTowards(finger5.y, 0, actionSpeed);
	completed &= MoveTowards(finger5.z, 0, actionSpeed);

	completed &= MoveTowards(leftPalm.x, 0, actionSpeed);
	completed &= MoveTowards(leftPalm.y, 0, actionSpeed);
	completed &= MoveTowards(leftPalm.z, 0, actionSpeed);

	completed &= MoveTowards(rightPalm.x, 0, actionSpeed);
	//completed &= MoveTowards(rightPalm.y, 0, actionSpeed);
	completed &= MoveTowards(rightPalm.z, 0, actionSpeed);

	completed &= MoveTowards(leftForearm.x, -20, actionSpeed);
	completed &= MoveTowards(leftForearm.y, 0, actionSpeed);
	completed &= MoveTowards(leftForearm.z, 0, actionSpeed);

	completed &= MoveTowards(rightForearm.x, -20, actionSpeed);
	completed &= MoveTowards(rightForearm.y, 0, actionSpeed);
	completed &= MoveTowards(rightForearm.z, 0, actionSpeed);

	completed &= MoveTowards(leftUpperarm.x, 0, actionSpeed);
	completed &= MoveTowards(leftUpperarm.y, -10, actionSpeed);
	completed &= MoveTowards(leftUpperarm.z, 10, actionSpeed);

	completed &= MoveTowards(rightUpperarm.x, 0, actionSpeed);
	completed &= MoveTowards(rightUpperarm.y, 10, actionSpeed);
	completed &= MoveTowards(rightUpperarm.z, -10, actionSpeed);

	completed &= MoveTowards(leftFoot.x, 0, actionSpeed);
	completed &= MoveTowards(leftFoot.y, 0, actionSpeed);
	completed &= MoveTowards(leftFoot.z, 0, actionSpeed);

	completed &= MoveTowards(rightFoot.x, 0, actionSpeed);
	completed &= MoveTowards(rightFoot.y, 0, actionSpeed);
	completed &= MoveTowards(rightFoot.z, 0, actionSpeed);

	completed &= MoveTowards(leftCalf.x, 10, actionSpeed);
	completed &= MoveTowards(leftCalf.y, 0, actionSpeed);
	completed &= MoveTowards(leftCalf.z, 0, actionSpeed);

	completed &= MoveTowards(rightCalf.x, 10, actionSpeed);
	completed &= MoveTowards(rightCalf.y, 0, actionSpeed);
	completed &= MoveTowards(rightCalf.z, 0, actionSpeed);

	completed &= MoveTowards(leftThigh.x, -10, actionSpeed);
	completed &= MoveTowards(leftThigh.y, 15, actionSpeed);
	completed &= MoveTowards(leftThigh.z, 0, actionSpeed);

	completed &= MoveTowards(rightThigh.x, -10, actionSpeed);
	completed &= MoveTowards(rightThigh.y, 15, actionSpeed);
	completed &= MoveTowards(rightThigh.z, 0, actionSpeed);

	completed &= MoveTowards(body.x, 0, actionSpeed);
	completed &= MoveTowards(body.y, 0, actionSpeed);
	completed &= MoveTowards(body.z, 0, actionSpeed);

	return completed;
}


void Animate() {

	if (reset) {
		spikeBallSpin3 = true;
		reset = !Default();
		shooting = false;
		shootingBody = false;
		bodyGunIn = true;
	}

	if (shooting) {
		bulletTracking.y += 5;
	}
	

	if (shootingBody) {
		bulletTrackingBody.y += 5;
	}


	if (spikeBallSpin) {
		spikeBallSpin = !SpinSpike();
	}

	if (!spikeBallSpin) {
		spikeBallSpin2 = false;
		spikeBallSpin3 = false;
	}

	if (launchSpike) {
		spikeBallSpin = true;
		launchSpike = !LaunchSpike();
	}

	if (!launchSpike) {
		launchSpike2 = false;
	}

	if (openFingers) {
		openFingers = !MoveAllFingers(-5);
	}

	if (closeFingers) {
		closeFingers = !MoveAllFingers(-80);
	}

	if (punchLeft) {
		punchLeft = !PunchLeft(1);
	}

	if (punchRight) {
		punchRight = !PunchRight(1);
	}

	if (peace) {
		peace = !Peace(1);
	}
	if (!peace) {
		peacePart2 = false;
	}

	if (thumbsUp) {
		thumbsUp = !ThumbsUp(1);
	}

	if (fightStance) {
		fightStance = !FightStance(1);
	}

	if (surprise) {
		surprise = !Surprise(1);
	}

	if (!surprise) {
		surprisePart2 = false;
	}

	if (taunt) {
		taunt = !Taunt(1);
	}

	if (!taunt) {
		taunt2 = false;
		taunt3 = false;
		taunt4 = false;
		taunt5 = false;
	}

	if (kick) {
		kick = !Kick(1);
	}

	if (!kick) {
		kick2 = false;
		kick3 = false;
	}

	if (bodyGunOut) {
		bodyGunOut = !BodyGun(1);
	}

	if (!bodyGunOut) {
		bodyGunOut2 = false;
	}

	if (bodyGunIn) {
		bodyGunIn = !BodyGunRetract(1);
	}

	if (!bodyGunIn) {
		bodyGunIn2 = false;
	}
}


bool testAction = false;
void InitialiseModel() {
	//Kick(1);
}


LRESULT WINAPI WindowProcedure(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_MOUSEMOVE:
		switch (wParam) {
		case MK_LBUTTON:
			int xPos = GET_X_LPARAM(lParam);
			int yPos = GET_Y_LPARAM(lParam);
			if (cam.z > 0) q1Rotation.y += xPos - lastX;
			else q1Rotation.y += xPos - lastX;

			if (cam.z > 0)q1Rotation.x += yPos - lastY;
			else q1Rotation.x -= yPos - lastY;
			lastX = xPos;
			lastY = yPos;
			break;
		}
		break;
	case WM_LBUTTONDOWN:
		lastX = GET_X_LPARAM(lParam);
		lastY = GET_Y_LPARAM(lParam);
		break;
	case WM_MOUSEWHEEL:
		zoomLevel += GET_WHEEL_DELTA_WPARAM(wParam);
		break;

	case WM_KEYDOWN:
		switch (wParam) {

		case VK_UP:
			light1Pos[1] += light1moveSpeed;
			lookAt.y += cameraMoveSpeed;
			break;
		case VK_DOWN:
			light1Pos[1] -= light1moveSpeed;
			lookAt.y -= cameraMoveSpeed;
			break;
		case VK_LEFT:
			light1Pos[0] -= light1moveSpeed;
			if (cam.z > 0)
				lookAt.x -= cameraMoveSpeed;
			else
				lookAt.x += cameraMoveSpeed;
			break;
		case VK_RIGHT:
			light1Pos[0] += light1moveSpeed;
			if (cam.z > 0)
				lookAt.x += cameraMoveSpeed;
			else
				lookAt.x -= cameraMoveSpeed;
			break;

		case 'Q':
			break;
		case 'E':
			break;
		case 'R':
			break;
		case 'W':
			cam.y += cameraMoveSpeed;
			break;
		case 'S':
			cam.y -= cameraMoveSpeed;
			break;
		case 'A':
			if (cam.z > 0)
				cam.x -= cameraMoveSpeed;
			else
				cam.x += cameraMoveSpeed;
			break;
		case 'D':
			if (cam.z > 0)
				cam.x += cameraMoveSpeed;
			else
				cam.x -= cameraMoveSpeed;
			break;

		default:
			break;
		}


		break;

	case WM_KEYUP:
		switch (wParam) {
		case 'C':
			if (spikeBallSpin) {
				spikeBallSpin3 = true;
			}
			else {
				spikeBallSpin = true;
			}

			break;

		case 'N':
			//open fist
			openFingers = !openFingers;
			break;

		case 'M':
			//close fist
			closeFingers = !closeFingers;
			break;

		case 0x31:
			punchRight = !punchRight;
			break;
		case 0x32:
			taunt = true;
			break;

		case 0x33:
			launchSpike = !launchSpike;
			break;

		case 0x34:
			fightStance = !fightStance;
			break;

		case 0x35:
			punchLeft = !punchLeft;
			break;
		case 0x36:
			//surprise = !surprise;
			kick = !kick;
			break;
		case 0x37:
			currentSetIndex = (currentSetIndex + 1) % colorSets.size(); // Cycle through color sets

			BODY_COLOR = colorSets[currentSetIndex].primary;
			BODY_COLOR_LIGHT = colorSets[currentSetIndex].light;
			BODY_COLOR_DARK = colorSets[currentSetIndex].dark;
			break;
		case 0x38:
			textureIndex = (textureIndex + 1) % textures.size(); // Cycle through color sets

			selectedTexture = textures[textureIndex];
			break;

		case 'T':
			peace = !peace;
			break;

		case 'Y':
			thumbsUp = !thumbsUp;
			break;
		case 'U':
			bodyGunOut = !bodyGunOut;
			break;
		case 'I':
			bodyGunIn = !bodyGunIn;
			break;

		case VK_SPACE:
			reset = !reset;
			break;

		case 'O':
			shooting = true;
			findLast = true;
			bulletTracking.y = 0;
			lastBulletPosition = -3000;
			break;
		case 'P':
			shooting = false;
			break;
		case 'K':
			shootingBody = true;
			findLastBody = true;
			bulletTrackingBody.y = 0;
			lastBulletPositionBody = -3000;
			break;
		case 'L':
			shootingBody = false;
			break;
		case 'Z':
			drawStyle = FILL;
			mode = QUAD;
			selectedTexture = textures[textureIndex];
			break;
		case 'X':
			drawStyle = LINE;
			mode = LINELOOP;
			selectedTexture = bronze3;
			break;


		default:
			break;
		}

	default:
		break;
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}
//--------------------------------------------------------------------

bool initPixelFormat(HDC hdc)
{
	PIXELFORMATDESCRIPTOR pfd;
	ZeroMemory(&pfd, sizeof(PIXELFORMATDESCRIPTOR));

	pfd.cAlphaBits = 8;
	pfd.cColorBits = 32;
	pfd.cDepthBits = 24;
	pfd.cStencilBits = 0;

	pfd.dwFlags = PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW;

	pfd.iLayerType = PFD_MAIN_PLANE;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;

	// choose pixel format returns the number most similar pixel format available
	int n = ChoosePixelFormat(hdc, &pfd);

	// set pixel format returns whether it sucessfully set the pixel format
	if (SetPixelFormat(hdc, n, &pfd))
	{
		return true;
	}
	else
	{
		return false;
	}
}
//--------------------------------------------------------------------
void helmatTop() {
	glBegin(mode);
	UtilColor(BODY_COLOR_DARK);

	glTexCoord2f(0, 1);
	glVertex3f(-0.20, 0.58, 0.0);
	glTexCoord2f(0, 0);
	glVertex3f(-0.30, 0.58, 0.1);
	glTexCoord2f(1, 0);
	glVertex3f(0.30, 0.58, 0.1);
	glTexCoord2f(1, 1);
	glVertex3f(0.20, 0.58, 0.0);

	glTexCoord2f(0, 1);
	glVertex3f(-0.30, 0.58, 0.1);
	glTexCoord2f(0, 0);
	glVertex3f(-0.33, 0.48, 0.30);
	glTexCoord2f(1, 0);
	glVertex3f(0.33, 0.48, 0.30);
	glTexCoord2f(1, 1);
	glVertex3f(0.30, 0.58, 0.1);

	glTexCoord2f(0, 1);
	glVertex3f(-0.33, 0.48, 0.30);
	glTexCoord2f(0, 0);
	glVertex3f(-0.15, 0.48, 0.45);
	glTexCoord2f(1, 0);
	glVertex3f(0.15, 0.48, 0.45);
	glTexCoord2f(1, 1);
	glVertex3f(0.33, 0.48, 0.30);


	glEnd();

	//back helmat
	glBegin(mode);
	//left
	glTexCoord2f(0, 1);
	glVertex3f(-0.33, 0.39, 0.20);
	glTexCoord2f(0, 0);
	glVertex3f(-0.33, 0.33, 0.2);
	glTexCoord2f(1, 0);
	glVertex3f(-0.31, 0.33, 0.30);
	glTexCoord2f(1, 1);
	glVertex3f(-0.33, 0.48, 0.30);

	glTexCoord2f(1, 0);
	glVertex3f(-0.15, 0.33, 0.45);
	glTexCoord2f(1, 1);
	glVertex3f(-0.31, 0.33, 0.30);
	glTexCoord2f(0, 1);
	glVertex3f(-0.33, 0.48, 0.30);
	glTexCoord2f(0, 0);
	glVertex3f(-0.15, 0.48, 0.45);

	glTexCoord2f(1, 0);
	glVertex3f(-0.15, 0.20, 0.45);
	glTexCoord2f(1, 1);
	glVertex3f(-0.25, 0.20, 0.30);
	glTexCoord2f(0, 1);
	glVertex3f(-0.31, 0.33, 0.30);
	glTexCoord2f(0, 0);
	glVertex3f(-0.15, 0.33, 0.45);


	glTexCoord2f(1, 0);
	glVertex3f(-0.15, 0.20, 0.45);
	glTexCoord2f(1, 1);
	glVertex3f(-0.25, 0.20, 0.30);
	glTexCoord2f(0, 1);
	glVertex3f(-0.30, 0.07, 0.30);
	glTexCoord2f(0, 0);
	glVertex3f(-0.15, 0.07, 0.45);



	//right
	glTexCoord2f(1, 0);
	glVertex3f(0.31, 0.33, 0.30);
	glTexCoord2f(1, 1);
	glVertex3f(0.33, 0.33, 0.2);
	glTexCoord2f(0, 1);
	glVertex3f(0.33, 0.39, 0.20);
	glTexCoord2f(0, 0);
	glVertex3f(0.33, 0.48, 0.30);

	glTexCoord2f(1, 0);
	glVertex3f(0.15, 0.33, 0.45);
	glTexCoord2f(1, 1);
	glVertex3f(0.31, 0.33, 0.30);
	glTexCoord2f(0, 1);
	glVertex3f(0.33, 0.48, 0.30);
	glTexCoord2f(0, 0);
	glVertex3f(0.15, 0.48, 0.45);

	glTexCoord2f(1, 0);
	glVertex3f(0.15, 0.20, 0.45);
	glTexCoord2f(1, 1);
	glVertex3f(0.25, 0.20, 0.30);
	glTexCoord2f(0, 1);
	glVertex3f(0.31, 0.33, 0.30);
	glTexCoord2f(0, 0);
	glVertex3f(0.15, 0.33, 0.45);

	glTexCoord2f(1, 0);
	glVertex3f(0.15, 0.20, 0.45);
	glTexCoord2f(1, 1);
	glVertex3f(0.25, 0.20, 0.30);
	glTexCoord2f(0, 1);
	glVertex3f(0.30, 0.07, 0.30);
	glTexCoord2f(0, 0);
	glVertex3f(0.15, 0.07, 0.45);

	//middle 
	glTexCoord2f(1, 0);
	glVertex3f(0.15, 0.07, 0.45);
	glTexCoord2f(1, 1);
	glVertex3f(-0.15, 0.07, 0.45);
	glTexCoord2f(0, 1);
	glVertex3f(-0.15, 0.48, 0.45);
	glTexCoord2f(0, 0);
	glVertex3f(0.15, 0.48, 0.45);

	glEnd();
	glBegin(GL_POLYGON);

	// left
	glTexCoord2f(0.0, 1.0); glVertex3f(-0.31, 0.33, 0.30); // Top-left
	glTexCoord2f(0.0, 0.8); glVertex3f(-0.33, 0.33, 0.2);  // Top-right
	glTexCoord2f(0.0, 0.6); glVertex3f(-0.33, 0.30, 0.2);  // Mid-right
	glTexCoord2f(0.2, 0.5); glVertex3f(-0.30, 0.20, 0.2);  // Bottom-right
	glTexCoord2f(0.1, 0.2); glVertex3f(-0.35, 0.07, 0.2);  // Bottom-left
	glTexCoord2f(0.2, 0.0); glVertex3f(-0.30, 0.07, 0.30); // Mid-left
	glTexCoord2f(0.5, 0.5); glVertex3f(-0.25, 0.20, 0.30); // Center-left
	glEnd();



	glBegin(GL_POLYGON);

	// right
	glTexCoord2f(1.0, 1.0); glVertex3f(0.31, 0.33, 0.30); // Top-right
	glTexCoord2f(1.0, 0.8); glVertex3f(0.33, 0.33, 0.2);  // Top-left
	glTexCoord2f(1.0, 0.6); glVertex3f(0.33, 0.30, 0.2);  // Mid-left
	glTexCoord2f(0.8, 0.5); glVertex3f(0.30, 0.20, 0.2);  // Bottom-left
	glTexCoord2f(0.9, 0.2); glVertex3f(0.35, 0.07, 0.2);  // Bottom-right
	glTexCoord2f(0.8, 0.0); glVertex3f(0.30, 0.07, 0.30); // Mid-right
	glTexCoord2f(0.5, 0.5); glVertex3f(0.25, 0.20, 0.30); // Center-right

	glEnd();



}
void helmat() {
	glBegin(mode);
	//left
	UtilColor(BODY_COLOR_DARK);

	glTexCoord2f(1, 0);
	glVertex3f(-0.20, 0.42, 0.1);
	glTexCoord2f(1, 1);
	glVertex3f(-0.16, 0.38, 0.0);
	glTexCoord2f(0, 1);
	glVertex3f(-0.08, 0.35, -0.2);
	glTexCoord2f(0, 0);
	glVertex3f(-0.08, 0.42, -0.2);

	glTexCoord2f(1, 0);
	glVertex3f(-0.20, 0.42, 0.1);
	glTexCoord2f(1, 1);
	glVertex3f(-0.08, 0.42, -0.2);
	glTexCoord2f(0, 1);
	glVertex3f(-0.08, 0.48, -0.15);
	glTexCoord2f(0, 0);
	glVertex3f(-0.20, 0.48, 0.1);

	glTexCoord2f(1, 0);
	glVertex3f(-0.30, 0.42, 0.15);
	glTexCoord2f(1, 1);
	glVertex3f(-0.20, 0.42, 0.1);
	glTexCoord2f(0, 1);
	glVertex3f(-0.20, 0.48, 0.1);
	glTexCoord2f(0, 0);
	glVertex3f(-0.30, 0.48, 0.1);

	glTexCoord2f(1, 0);
	glVertex3f(-0.33, 0.39, 0.20);
	glTexCoord2f(1, 1);
	glVertex3f(-0.30, 0.42, 0.15);
	glTexCoord2f(0, 1);
	glVertex3f(-0.30, 0.48, 0.1);
	glTexCoord2f(0, 0);
	glVertex3f(-0.33, 0.48, 0.30);

	glTexCoord2f(1, 0);
	glVertex3f(-0.20, 0.48, 0.1);
	glTexCoord2f(1, 1);
	glVertex3f(-0.08, 0.48, -0.15);
	glTexCoord2f(0, 1);
	glVertex3f(-0.06, 0.58, -0.10);
	glTexCoord2f(0, 0);
	glVertex3f(-0.08, 0.58, -0.10);

	glTexCoord2f(1, 0);
	glVertex3f(-0.30, 0.48, 0.1);
	glTexCoord2f(1, 1);
	glVertex3f(-0.20, 0.48, 0.1);
	glTexCoord2f(0, 1);
	glVertex3f(-0.20, 0.58, 0.0);
	glTexCoord2f(0, 0);
	glVertex3f(-0.30, 0.58, 0.1);


	//right
	glTexCoord2f(1, 0);
	glVertex3f(0.20, 0.42, 0.1);
	glTexCoord2f(1, 1);
	glVertex3f(0.16, 0.38, 0.0);
	glTexCoord2f(0, 1);
	glVertex3f(0.08, 0.35, -0.2);
	glTexCoord2f(0, 0);
	glVertex3f(0.08, 0.42, -0.2);

	glTexCoord2f(1, 0);
	glVertex3f(0.20, 0.42, 0.1);
	glTexCoord2f(1, 1);
	glVertex3f(0.08, 0.42, -0.2);
	glTexCoord2f(0, 1);
	glVertex3f(0.08, 0.48, -0.15);
	glTexCoord2f(0, 0);
	glVertex3f(0.20, 0.48, 0.1);

	glTexCoord2f(1, 0);
	glVertex3f(0.30, 0.42, 0.15);
	glTexCoord2f(1, 1);
	glVertex3f(0.20, 0.42, 0.1);
	glTexCoord2f(0, 1);
	glVertex3f(0.20, 0.48, 0.1);
	glTexCoord2f(0, 0);
	glVertex3f(0.30, 0.48, 0.1);

	glTexCoord2f(1, 0);
	glVertex3f(0.33, 0.39, 0.20);
	glTexCoord2f(1, 1);
	glVertex3f(0.30, 0.42, 0.15);
	glTexCoord2f(0, 1);
	glVertex3f(0.30, 0.48, 0.1);
	glTexCoord2f(0, 0);
	glVertex3f(0.33, 0.48, 0.30);

	glTexCoord2f(1, 0);
	glVertex3f(0.20, 0.48, 0.1);
	glTexCoord2f(1, 1);
	glVertex3f(0.08, 0.48, -0.15);
	glTexCoord2f(0, 1);
	glVertex3f(0.06, 0.58, -0.10);
	glTexCoord2f(0, 0);
	glVertex3f(0.08, 0.58, -0.10);

	glTexCoord2f(1, 0);
	glVertex3f(0.30, 0.48, 0.1);
	glTexCoord2f(1, 1);
	glVertex3f(0.20, 0.48, 0.1);
	glTexCoord2f(0, 1);
	glVertex3f(0.20, 0.58, 0.0);
	glTexCoord2f(0, 0);
	glVertex3f(0.30, 0.58, 0.1);



	//middle
	glTexCoord2f(1, 0);
	glVertex3f(-0.08, 0.35, -0.2);
	glTexCoord2f(1, 1);
	glVertex3f(0.08, 0.35, -0.2);
	glTexCoord2f(0, 1);
	glVertex3f(0.08, 0.42, -0.2);
	glTexCoord2f(0, 0);
	glVertex3f(-0.08, 0.42, -0.2);

	//
	glTexCoord2f(1, 0);
	glVertex3f(-0.08, 0.42, -0.2);
	glTexCoord2f(1, 1);
	glVertex3f(0.08, 0.42, -0.2);
	glTexCoord2f(0, 1);
	glVertex3f(0.08, 0.48, -0.15);
	glTexCoord2f(0, 0);
	glVertex3f(-0.08, 0.48, -0.15);

	//
	glTexCoord2f(1, 0);
	glVertex3f(-0.08, 0.48, -0.15);
	glTexCoord2f(1, 1);
	glVertex3f(0.08, 0.48, -0.15);
	glTexCoord2f(0, 1);
	glVertex3f(0.06, 0.58, -0.10);
	glTexCoord2f(0, 0);
	glVertex3f(-0.06, 0.58, -0.10);

	//middle top
	glTexCoord2f(1, 0);
	glVertex3f(-0.08, 0.58, -0.10);
	glTexCoord2f(1, 1);
	glVertex3f(0.08, 0.58, -0.10);
	glTexCoord2f(0, 1);
	glVertex3f(0.20, 0.58, 0.0);
	glTexCoord2f(0, 0);
	glVertex3f(-0.20, 0.58, 0.0);

	glEnd();

	glBegin(GL_TRIANGLES);

	// left triangles
	glTexCoord2f(0.0, 0.0); glVertex3f(-0.20, 0.48, 0.1);   // First vertex (left)
	glTexCoord2f(0.0, 1.0); glVertex3f(-0.08, 0.58, -0.10); // Second vertex (top-left)
	glTexCoord2f(0.2, 1.0); glVertex3f(-0.20, 0.58, 0.0);   // Third vertex (top-right)

	glTexCoord2f(0.2, 0.0); glVertex3f(-0.33, 0.48, 0.30);  // First vertex (left)
	glTexCoord2f(0.2, 0.0); glVertex3f(-0.30, 0.48, 0.1);   // Second vertex (bottom-left)
	glTexCoord2f(0.4, 1.0); glVertex3f(-0.30, 0.58, 0.1);   // Third vertex (top-right)

	// right triangles
	glTexCoord2f(0.6, 0.0); glVertex3f(0.20, 0.48, 0.1);    // First vertex (right)
	glTexCoord2f(0.8, 1.0); glVertex3f(0.08, 0.58, -0.10);  // Second vertex (top-left)
	glTexCoord2f(0.6, 1.0); glVertex3f(0.20, 0.58, 0.0);    // Third vertex (top-right)

	glTexCoord2f(0.8, 0.0); glVertex3f(0.33, 0.48, 0.30);   // First vertex (right)
	glTexCoord2f(0.6, 0.0); glVertex3f(0.30, 0.48, 0.1);    // Second vertex (bottom-left)
	glTexCoord2f(0.6, 1.0); glVertex3f(0.30, 0.58, 0.1);    // Third vertex (top-right)

	glEnd();

}
void eyeDetail() {
	glBegin(GL_TRIANGLE_FAN);

	glTexCoord2f(0.5, 0.5); glVertex3f(-0.18, 0.25, -0.1); // v0 (center of the fan)
	glTexCoord2f(0.6, 0.4); glVertex3f(-0.05, 0.25, -0.15);
	glTexCoord2f(0.7, 0.5); glVertex3f(-0.03, 0.30, -0.15);
	glTexCoord2f(0.8, 0.6); glVertex3f(-0.08, 0.35, -0.2);
	glTexCoord2f(0.7, 0.8); glVertex3f(-0.16, 0.38, 0.0);
	glTexCoord2f(0.6, 0.9); glVertex3f(-0.20, 0.42, 0.1);
	glTexCoord2f(0.4, 0.9); glVertex3f(-0.30, 0.42, 0.15);
	glTexCoord2f(0.3, 0.8); glVertex3f(-0.33, 0.39, 0.2);
	glTexCoord2f(0.2, 0.6); glVertex3f(-0.33, 0.33, 0.2);

	glEnd();


	//right part
	glBegin(GL_TRIANGLE_FAN);

	glTexCoord2f(0.5, 0.5); glVertex3f(0.18, 0.25, -0.1); // v0 (center of the fan)
	glTexCoord2f(0.4, 0.6); glVertex3f(0.05, 0.25, -0.15);
	glTexCoord2f(0.5, 0.7); glVertex3f(0.03, 0.30, -0.15);
	glTexCoord2f(0.6, 0.8); glVertex3f(0.08, 0.35, -0.2);
	glTexCoord2f(0.8, 0.7); glVertex3f(0.16, 0.38, 0.0);
	glTexCoord2f(0.9, 0.6); glVertex3f(0.20, 0.42, 0.1);
	glTexCoord2f(0.9, 0.4); glVertex3f(0.30, 0.42, 0.15);
	glTexCoord2f(0.8, 0.3); glVertex3f(0.33, 0.39, 0.2);
	glTexCoord2f(0.6, 0.2); glVertex3f(0.33, 0.33, 0.2);

	glEnd();


	glBegin(GL_TRIANGLES);

	// left triangle
	glTexCoord2f(0.0, 0.0); glVertex3f(-0.11, 0.18, -0.15); // First vertex (left)
	glTexCoord2f(0.0, 1.0); glVertex3f(-0.18, 0.25, -0.10); // Second vertex (top-left)
	glTexCoord2f(0.2, 1.0); glVertex3f(-0.05, 0.25, -0.15); // Third vertex (top-right)

	// right triangle
	glTexCoord2f(0.8, 0.0); glVertex3f(0.11, 0.18, -0.15);  // First vertex (right)
	glTexCoord2f(1.0, 1.0); glVertex3f(0.18, 0.25, -0.10);  // Second vertex (top-right)
	glTexCoord2f(0.8, 1.0); glVertex3f(0.05, 0.25, -0.15);  // Third vertex (top-left)

	glEnd();


}
void drawFace() {
	glBegin(mode);
	//front 
	UtilColor(BODY_COLOR_DARK);

	glTexCoord2f(1, 0);
	glVertex3f(-0.10, 0.0, -0.10);
	glTexCoord2f(1, 1);
	glVertex3f(0.10, 0.0, -0.10);
	glTexCoord2f(0, 1);
	glVertex3f(0.07, 0.10, -0.2);
	glTexCoord2f(0, 0);
	glVertex3f(-0.07, 0.10, -0.2);

	//right
	UtilColor(BODY_COLOR_DARK);

	glTexCoord2f(1, 0);
	glVertex3f(0.10, 0.0, -0.10);
	glTexCoord2f(1, 1);
	glVertex3f(0.35, 0.07, 0.2);
	glTexCoord2f(0, 1);
	glVertex3f(0.30, 0.20, 0.2);
	glTexCoord2f(0, 0);
	glVertex3f(0.07, 0.10, -0.2);

	//left
	UtilColor(BODY_COLOR_DARK);

	glTexCoord2f(1, 0);
	glVertex3f(-0.35, 0.07, 0.2);
	glTexCoord2f(1, 1);
	glVertex3f(-0.10, 0.0, -0.10);
	glTexCoord2f(0, 1);
	glVertex3f(-0.07, 0.10, -0.2);
	glTexCoord2f(0, 0);
	glVertex3f(-0.30, 0.20, 0.2);
	glEnd();

	glBegin(mode);
	//left
	UtilColor(128, 128, 128);

	glTexCoord2f(1, 0);
	glVertex3f(-0.30, 0.20, 0.2);
	glTexCoord2f(1, 1);
	glVertex3f(-0.07, 0.10, -0.2);
	glTexCoord2f(0, 1);
	glVertex3f(-0.20, 0.25, -0.1);
	glTexCoord2f(0, 0);
	glVertex3f(-0.33, 0.30, 0.2);

	//right
	glTexCoord2f(1, 0);
	glVertex3f(0.30, 0.20, 0.2);
	glTexCoord2f(1, 1);
	glVertex3f(0.07, 0.10, -0.2);
	glTexCoord2f(0, 1);
	glVertex3f(0.20, 0.25, -0.1);
	glTexCoord2f(0, 0);
	glVertex3f(0.33, 0.30, 0.2);

	glEnd();

	glBegin(GL_POLYGON);
	UtilColor(255, 255, 0);

	// Texture coordinates and vertices
	glTexCoord2f(0.5, 0.0); glVertex3f(0.03, 0.11, -0.20);  // First vertex
	glTexCoord2f(0.8, 0.3); glVertex3f(0.09, 0.17, -0.15);  // Second vertex
	glTexCoord2f(0.8, 0.6); glVertex3f(0.03, 0.23, -0.15);  // Third vertex
	glTexCoord2f(0.2, 0.6); glVertex3f(-0.03, 0.23, -0.15); // Fourth vertex
	glTexCoord2f(0.0, 0.3); glVertex3f(-0.09, 0.17, -0.15); // Fifth vertex
	glTexCoord2f(0.2, 0.0); glVertex3f(-0.03, 0.11, -0.20); // Sixth vertex

	glEnd();


	eyeDetail();

	glBegin(mode);

	//middle
	glTexCoord2f(1, 0);
	glVertex3f(-0.05, 0.25, -0.15);
	glTexCoord2f(1, 1);
	glVertex3f(0.05, 0.25, -0.15);
	glTexCoord2f(0, 1);
	glVertex3f(0.03, 0.30, -0.15);
	glTexCoord2f(0, 0);
	glVertex3f(-0.03, 0.30, -0.15);

	glPushMatrix();
	//middle black shadow
	glTexCoord2f(1, 0);
	glVertex3f(-0.03, 0.30, -0.15);
	glTexCoord2f(1, 1);
	glVertex3f(0.03, 0.30, -0.15);
	glTexCoord2f(0, 1);
	glVertex3f(0.08, 0.35, -0.2);
	glTexCoord2f(0, 0);
	glVertex3f(-0.08, 0.35, -0.2);

	helmat();
	glPopMatrix();


	glEnd();
}



void DrawKneeCover() {
	Vector3 kneeCapColor = BODY_COLOR_LIGHT;
	glPushMatrix();
	UtilTranslate(0, 0, 60);
	UtilCuboidScale(kneeCapColor, 80, 35, 1, 0.5, 0.5);
	//left piece
	glPushMatrix();
	UtilTranslate(-35, 0, 0);
	glRotatef(-20, 0, 1, 0);
	UtilTranslate(-10, 0, 0);
	UtilCuboidScale(kneeCapColor, 90, 10, 1, 0.5, 0.5);
	glRotatef(-30, 0, 1, 0);
	UtilTranslate(-18, 0, 0);
	UtilCuboidScale(kneeCapColor, 100, 10, 1, 0.5, 0.5);
	glRotatef(-30, 0, 1, 0);
	UtilTranslate(-28, 0, 0);
	UtilCuboidScale(kneeCapColor, 110, 20, 1, 0.5, 0.5);
	glPopMatrix();

	//right piece
	glPushMatrix();
	glScalef(-1, 1, 1);
	UtilTranslate(-35, 0, 0);
	glRotatef(-20, 0, 1, 0);
	UtilTranslate(-10, 0, 0);
	UtilCuboidScale(kneeCapColor, 100, 10, 1, 0.5, 0.5);
	glRotatef(-30, 0, 1, 0);
	UtilTranslate(-18, 0, 0);
	UtilCuboidScale(kneeCapColor, 110, 10, 1, 0.5, 0.5);
	glRotatef(-30, 0, 1, 0);
	UtilTranslate(-28, 0, 0);
	UtilCuboidScale(kneeCapColor, 120, 20, 1, 0.5, 0.5);
	glPopMatrix();
	glPopMatrix();

	//middlepice
	glPushMatrix();
	glScalef(1, 1, -1);
	UtilTranslate(0, 0, 60);
	glPushMatrix();
	UtilTranslate(0, 20, 0);
	UtilCuboidScale(kneeCapColor, 40, 35, 1, 0.5, 0.5);
	glPopMatrix();
	//left piece
	glPushMatrix();
	UtilTranslate(-35, 0, 0);
	glRotatef(-20, 0, 1, 0);
	UtilTranslate(-10, 0, 0);
	UtilCuboidScale(kneeCapColor, 100, 10, 1, 0.5, 0.5);
	glRotatef(-30, 0, 1, 0);
	UtilTranslate(-18, 0, 0);
	UtilCuboidScale(kneeCapColor, 110, 10, 1, 0.5, 0.5);
	glRotatef(-30, 0, 1, 0);
	UtilTranslate(-28, 0, 0);
	UtilCuboidScale(kneeCapColor, 120, 20, 1, 0.5, 0.5);
	glPopMatrix();

	//right piece
	glPushMatrix();
	glScalef(-1, 1, 1);
	UtilTranslate(-35, 0, 0);
	glRotatef(-20, 0, 1, 0);
	UtilTranslate(-10, 0, 0);
	UtilCuboidScale(kneeCapColor, 100, 10, 1, 0.5, 0.5);
	glRotatef(-30, 0, 1, 0);
	UtilTranslate(-18, 0, 0);
	UtilCuboidScale(kneeCapColor, 110, 10, 1, 0.5, 0.5);
	glRotatef(-30, 0, 1, 0);
	UtilTranslate(-28, 0, 0);
	UtilCuboidScale(kneeCapColor, 120, 20, 1, 0.5, 0.5);
	glPopMatrix();
	glPopMatrix();
}

void DrawFinger(Vector3 rotation) {
	glPushMatrix();
	//finger part 3
	glRotatef(rotation.x, 1, 0, 0);
	glRotatef(rotation.y, 0, 1, 0);
	glRotatef(rotation.z, 0, 0, 1);
	UtilSphere(var, BODY_COLOR_DARK, 7, 10, 10);
	UtilCylinder(var, BODY_COLOR, 8, 30, 15, 3);

	//finger part 2
	UtilTranslate(0, -30, 0);
	glRotatef(rotation.x, 1, 0, 0);
	glRotatef(rotation.y, 0, 1, 0);
	glRotatef(rotation.z, 0, 0, 1);
	UtilSphere(var, BODY_COLOR_DARK, 7, 10, 10);
	UtilCylinder(var, BODY_COLOR, 8, 25, 15, 3);

	//finger part 1
	UtilTranslate(0, -25, 0);
	glRotatef(rotation.x, 1, 0, 0);
	glRotatef(rotation.y, 0, 1, 0);
	glRotatef(rotation.z, 0, 0, 1);
	UtilSphere(var, BODY_COLOR_DARK, 7, 10, 10);
	UtilCylinder(var, BODY_COLOR, 8, 30, 15, 3);

	// finger tip
	UtilTranslate(0, -30, 0);
	UtilCylinder(var, BODY_COLOR, 8, 1, 5, 15, 3);
	glPopMatrix();
}

void DrawThumb(Vector3 rotation) {
	//finger part 2
	glRotatef(rotation.x, 1, 0, 0);
	glRotatef(rotation.y, 0, 1, 0);
	glRotatef(rotation.z, 0, 0, 1);
	UtilSphere(var, BODY_COLOR_DARK, 7, 15, 15);
	UtilCylinder(var, BODY_COLOR, 8, 25, 15, 15);

	//finger part 1
	UtilTranslate(0, -25, 0);
	glRotatef(rotation.x, 1, 0, 0);
	glRotatef(rotation.y, 0, 1, 0);
	glRotatef(rotation.z, 0, 0, 1);
	UtilSphere(var, BODY_COLOR_DARK, 7, 15, 15);
	UtilCylinder(var, BODY_COLOR, 8, 20, 15, 15);

	// finger tip
	UtilTranslate(0, -20, 0);
	UtilCylinder(var, BODY_COLOR, 8, 1, 5, 15, 15);
	glPopMatrix();
}

void DrawUpperArmBodyConnector(int direction) {
	glPushMatrix();
	UtilTranslate(-30 * direction, 0, 0);

	glPushMatrix();
	UtilTranslate(0, -26, 0);
	glRotatef(70 * direction, 0, 0, 1);
	UtilCylinder(var, BLACK_COLOR_LIGHT, 20, 150, 10, 10);
	UtilTranslate(45 * direction, 0, 0);
	UtilCylinder(var, BLACK_COLOR_LIGHT, 20, 150, 10, 10);
	glPopMatrix();

	UtilSphere(var, BLACK_COLOR, 52, 15, 15);

	glPushMatrix();
	UtilTranslate(120 * direction, -40, 0);
	UtilSphere(var, BLACK_COLOR, 52, 15, 15);
	glPopMatrix();

	glPopMatrix();
}

void DrawCrotch() {
	glPushMatrix();
	glScalef(0.5, 2, 1);
	glRotatef(-180 ,1 , 0, 0);
	glRotatef(90, 0, 1, 0);
	glRotatef(-90, 1, 0, 0);
	UtilTranslate(0, -55, 0);
	Vector3 kneeCapColor = BODY_COLOR;

	glPushMatrix();
	UtilTranslate(0, 0, 60);
	UtilCuboidScale(kneeCapColor, 110, 5, 3, 0.5, 0.5);
	glPushMatrix();

	UtilTranslate(-5, 0, 0);
	glRotatef(-20, 0, 1, 0);
	UtilTranslate(-10, 0, 0);
	UtilCuboidScale(kneeCapColor, 110, 10, 3, 0.5, 0.5);
	glRotatef(-30, 0, 1, 0);
	UtilTranslate(-8, 0, 5);
	UtilCuboidScale(kneeCapColor, 110, 5, 3, 0.5, 0.5);
	glRotatef(-15, 0, 1, 0);
	UtilTranslate(-8, 0, 4);
	UtilCuboidScale(kneeCapColor, 110, 5, 3, 0.5, 0.5);
	glPopMatrix();

	//right piece
	glPushMatrix();
	glScalef(-1, 1, 1);
	UtilTranslate(-5, 0, 0);
	glRotatef(-20, 0, 1, 0);
	UtilTranslate(-10, 0, 0);
	UtilCuboidScale(kneeCapColor, 110, 10, 3, 0.5, 0.5);
	glRotatef(-30, 0, 1, 0);
	UtilTranslate(-8, 0, 5);
	UtilCuboidScale(kneeCapColor, 110, 5, 3, 0.5, 0.5);
	glRotatef(-15, 0, 1, 0);
	UtilTranslate(-8, 0, 4);
	UtilCuboidScale(kneeCapColor, 110, 5, 3, 0.5, 0.5);
	glPopMatrix();
	glPopMatrix();


	glPopMatrix();
}

void DrawShoulderPads() {
	glPushMatrix();
	glScalef(2.7, 1.7, 1.7);
	glRotatef(-25, 0, 0, 1);
	glRotatef(-90, 0, 1, 0);
	glRotatef(-90, 1, 0, 0);
	UtilTranslate(0, -55, 0);
	Vector3 kneeCapColor = BODY_COLOR;

	glPushMatrix();
	UtilTranslate(0, 0, 60);
	UtilCuboidScale(kneeCapColor, 110, 35, 3, 0.5, 0.5);
	glPushMatrix();
	UtilTranslate(-35, 0, 0);
	glRotatef(-20, 0, 1, 0);
	UtilTranslate(-10, 0, 0);
	UtilCuboidScale(kneeCapColor, 110, 10, 3, 0.5, 0.5);
	glRotatef(-30, 0, 1, 0);
	UtilTranslate(-18, 0, 5);
	UtilCuboidScale(kneeCapColor, 110, 10, 3, 0.5, 0.5);
	glRotatef(-15, 0, 1, 0);
	UtilTranslate(-28, 0, 4);
	UtilCuboidScale(kneeCapColor, 110, 20, 3, 0.5, 0.5);
	glPopMatrix();

	//right piece
	glPushMatrix();
	glScalef(-1, 1, 1);
	UtilTranslate(-35, 0, 0);
	glRotatef(-20, 0, 1, 0);
	UtilTranslate(-10, 0, 0);
	UtilCuboidScale(kneeCapColor, 110, 10, 3, 0.5, 0.5);
	glRotatef(-30, 0, 1, 0);
	UtilTranslate(-18, 0, 5);
	UtilCuboidScale(kneeCapColor, 110, 10, 3, 0.5, 0.5);
	glRotatef(-15, 0, 1, 0);
	UtilTranslate(-28, 0, 4);
	UtilCuboidScale(kneeCapColor, 110, 20, 3, 0.5, 0.5);
	glPopMatrix();
	glPopMatrix();


	glPopMatrix();
}

void DrawUpperArmCover() {
	glPushMatrix();
	glScalef(1.2, 1.2, 1.2);

	Vector3 kneeCapColor = BODY_COLOR_LIGHT;
	glPushMatrix();
	UtilTranslate(0, 110, 0);
	UtilCuboid(kneeCapColor, 5, 60, 60);
	glPopMatrix();

	glPushMatrix();
	UtilTranslate(0, 0, 60);
	UtilCuboidScale(kneeCapColor, 110, 35, 1, 0.5, 0.5);
	//left piece
	glPushMatrix();
	UtilTranslate(-35, 0, 0);
	glRotatef(-20, 0, 1, 0);
	UtilTranslate(-10, 0, 0);
	UtilCuboidScale(kneeCapColor, 110, 10, 1, 0.5, 0.5);
	glRotatef(-30, 0, 1, 0);
	UtilTranslate(-18, 0, 0);
	UtilCuboidScale(kneeCapColor, 110, 10, 1, 0.5, 0.5);
	glRotatef(-30, 0, 1, 0);
	UtilTranslate(-28, 0, 0);
	UtilCuboidScale(kneeCapColor, 110, 20, 1, 0.5, 0.5);
	glPopMatrix();

	//right piece
	glPushMatrix();
	glScalef(-1, 1, 1);
	UtilTranslate(-35, 0, 0);
	glRotatef(-20, 0, 1, 0);
	UtilTranslate(-10, 0, 0);
	UtilCuboidScale(kneeCapColor, 110, 10, 1, 0.5, 0.5);
	glRotatef(-30, 0, 1, 0);
	UtilTranslate(-18, 0, 0);
	UtilCuboidScale(kneeCapColor, 110, 10, 1, 0.5, 0.5);
	glRotatef(-30, 0, 1, 0);
	UtilTranslate(-28, 0, 0);
	UtilCuboidScale(kneeCapColor, 110, 20, 1, 0.5, 0.5);
	glPopMatrix();
	glPopMatrix();

	//middlepice
	glPushMatrix();
	glScalef(1, 1, -1);
	UtilTranslate(0, 0, 60);
	glPushMatrix();
	UtilCuboidScale(kneeCapColor, 110, 35, 1, 0.5, 0.5);
	glPopMatrix();
	//left piece
	glPushMatrix();
	UtilTranslate(-35, 0, 0);
	glRotatef(-20, 0, 1, 0);
	UtilTranslate(-10, 0, 0);
	UtilCuboidScale(kneeCapColor, 110, 10, 1, 0.5, 0.5);
	glRotatef(-30, 0, 1, 0);
	UtilTranslate(-18, 0, 0);
	UtilCuboidScale(kneeCapColor, 110, 10, 1, 0.5, 0.5);
	glRotatef(-30, 0, 1, 0);
	UtilTranslate(-28, 0, 0);
	UtilCuboidScale(kneeCapColor, 110, 20, 1, 0.5, 0.5);
	glPopMatrix();

	//right piece
	glPushMatrix();
	glScalef(-1, 1, 1);
	UtilTranslate(-35, 0, 0);
	glRotatef(-20, 0, 1, 0);
	UtilTranslate(-10, 0, 0);
	UtilCuboidScale(kneeCapColor, 110, 10, 1, 0.5, 0.5);
	glRotatef(-30, 0, 1, 0);
	UtilTranslate(-18, 0, 0);
	UtilCuboidScale(kneeCapColor, 110, 10, 1, 0.5, 0.5);
	glRotatef(-30, 0, 1, 0);
	UtilTranslate(-28, 0, 0);
	UtilCuboidScale(kneeCapColor, 110, 20, 1, 0.5, 0.5);
	glPopMatrix();
	glPopMatrix();

	glPopMatrix();
}

void DrawOuterForeArmCover(Boolean hasGun) {
	UtilTranslate(0, 20, 0);


	for (int i = 0; i < 2; i++) {
		glPushMatrix();
		glRotatef(5 * ((i % 2 == 0) ? -1 : 1), 0, 0, 1);
		glRotatef(25 * ((i % 2 == 0) ? -1 : 1), 0, 1, 0);

		glPushMatrix();
		glRotatef(10, 1, 0, 0);
		//gun support
		Texture(blackMetal);
		glPushMatrix();
		UtilTranslate(0, 20, 0);
		glRotatef(90, 1, 0, 0);
		UtilTranslate(0, 45 + 30 * hasGun, 0);
		UtilCylinder(var, WHITE_COLOR, 5, 30 + 30 * hasGun, 10, 2);
		UtilTranslate(0, 0, -20);
		UtilCylinder(var, WHITE_COLOR, 5, 30 + 30 * hasGun, 10, 2);
		UtilTranslate(0, 0, -20);
		UtilCylinder(var, WHITE_COLOR, 5, 30 + 30 * hasGun, 10, 2);
		glPopMatrix();
		glPopMatrix();

		if (hasGun) {
			//gun
			glPushMatrix();
			glRotatef(6, 1, 0, 0);
			UtilTranslate(0, 90, 90);

			Texture(blackMetal);

			UtilCylinder(var, WHITE_COLOR, 20, 100, 10, 2);
			glPopMatrix();

			//printf("%d %d", shooting, findLast);
			// Calculate the number of bullets to display
			int numBullets = (shooting || !shooting && bulletTracking.y > 0) ? (bulletTracking.y / 80 + 1) : 0; // Ensure valid number of bullets

			for (int i = 0; i < numBullets; i++) {
				//printf("hi");
				if (numBullets > 50) numBullets = 50; // Cap the number of bullets

				// Position each bullet in its trajectory
				float bulletPosition = 30 - (i * 80) - fmod(bulletTracking.y, 80);


				// Check if the bullet is within visible range
				if (bulletPosition < bulletLimit) continue;

				// Record the last bullet position when shooting stops
				if (!shooting && lastBulletPosition == (bulletLimit - 5) && findLast) {
					findLast = false;
					//printf("last bullet %f\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n", bulletPosition);
					lastBulletPosition = bulletPosition;
				}

				Texture(selectedTexture);

				//printf("bullet position %f last position %f\n", bulletPosition, lastBulletPosition);
				// Render the bullet if rendering hasn't stopped
				if (shooting) {
					//printf("render bullet position %f last position %f\n", bulletPosition, lastBulletPosition);
					glPushMatrix();
					UtilTranslate(0, bulletPosition, 90);
					UtilCylinder(var, WHITE_COLOR, 7, 40, 10, 2);
					glPopMatrix();
				}
				else {
					if (bulletPosition <= lastBulletPosition) {
						//printf("render bullet position %f last position %f\n", bulletPosition, lastBulletPosition);
						glPushMatrix();
						UtilTranslate(0, bulletPosition, 90);
						UtilCylinder(var, WHITE_COLOR, 7, 40, 10, 2);
						glPopMatrix();
					}
				}

			}

			if (lastBulletPosition < bulletLimit) {
				lastBulletPosition = bulletLimit;
				bulletTracking.y = 0;
			}


		}

		glPopMatrix();
	}

	Texture(selectedTexture);
	//middle piece
	UtilTranslate(0, -20, 45); // middle piece distance
	glRotatef(12, 1, 0, 0);
	UtilCuboid(BODY_COLOR, 140, 35, 3);
	//left piece
	glPushMatrix();
	UtilTranslate(-35, 0, 0);
	glRotatef(-20, 0, 1, 0);
	UtilTranslate(-10, 0, 0);
	UtilCuboidScale(BODY_COLOR, 130, 10, 3, 0.5, 0.5);
	glRotatef(-30, 0, 1, 0);
	UtilTranslate(-18, 0, 0);
	UtilCuboidScale(BODY_COLOR, 110, 10, 3, 0.5, 0.5);
	glRotatef(-30, 0, 1, 0);
	UtilTranslate(-18, 0, 0);
	UtilCuboidScale(BODY_COLOR, 100, 10, 3, 0.5, 0.5);
	glPopMatrix();

	//right piece
	glPushMatrix();
	glScalef(-1, 1, 1);
	UtilTranslate(-35, 0, 0);
	glRotatef(-20, 0, 1, 0);
	UtilTranslate(-10, 0, 0);
	UtilCuboidScale(BODY_COLOR, 130, 10, 3, 0.5, 0.5);
	glRotatef(-30, 0, 1, 0);
	UtilTranslate(-18, 0, 0);
	UtilCuboidScale(BODY_COLOR, 110, 10, 3, 0.5, 0.5);
	glRotatef(-30, 0, 1, 0);
	UtilTranslate(-18, 0, 0);
	UtilCuboidScale(BODY_COLOR, 100, 10, 3, 0.5, 0.5);
	glPopMatrix();
}

void DrawInnerForeArmCover(Boolean hasGun) {
	UtilTranslate(0, 20, 0);

	glPushMatrix();
	glRotatef(10, 1, 0, 0);
	//gun support
	Texture(blackMetal);
	if (hasGun) {

		glPushMatrix();
		UtilTranslate(0, 20, 0);
		glRotatef(90, 1, 0, 0);
		UtilTranslate(0, 45 + 30 * hasGun, 0);
		UtilCylinder(var, WHITE_COLOR, 5, 30 + 30 * hasGun, 10, 2);
		UtilTranslate(0, 0, -20);
		UtilCylinder(var, WHITE_COLOR, 5, 30 + 30 * hasGun, 10, 2);
		UtilTranslate(0, 0, -20);
		UtilCylinder(var, WHITE_COLOR, 5, 30 + 30 * hasGun, 10, 2);
		glPopMatrix();
	}
	glPopMatrix();

	if (hasGun) {
		//gun
		glPushMatrix();
		glRotatef(6, 1, 0, 0);
		UtilTranslate(0, 90, 90);

		Texture(blackMetal);

		UtilCylinder(var, WHITE_COLOR, 20, 100, 10, 2);
		glPopMatrix();

		// Update bullet tracking when not shooting
		if (!shooting) {
			if (bulletTracking.y > 0) {
				//printf("bullet tracking y more than 0\n");
				lastBulletPosition -= 5;
				bulletTracking.y += 5; // Continue moving existing bullets
			}
		}

		//printf("%d %d", shooting, findLast);
		// Calculate the number of bullets to display
		int numBullets = (shooting || !shooting && bulletTracking.y > 0) ? (bulletTracking.y / 80 + 1) : 0; // Ensure valid number of bullets

		for (int i = 0; i < numBullets; i++) {
			//printf("hi");
			if (numBullets > 50) numBullets = 50; // Cap the number of bullets

			// Position each bullet in its trajectory
			float bulletPosition = 30 - (i * 80) - fmod(bulletTracking.y, 80);


			// Check if the bullet is within visible range
			if (bulletPosition < bulletLimit) continue;

			// Record the last bullet position when shooting stops
			if (!shooting && lastBulletPosition == (bulletLimit - 5) && findLast) {
				findLast = false;
				//printf("last bullet %f\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n", bulletPosition);
				lastBulletPosition = bulletPosition;
			}

			Texture(selectedTexture);

			//printf("bullet position %f last position %f\n", bulletPosition, lastBulletPosition);
			// Render the bullet if rendering hasn't stopped
			if (shooting) {
				//printf("render bullet position %f last position %f\n", bulletPosition, lastBulletPosition);
				glPushMatrix();
				UtilTranslate(0, bulletPosition, 90);
				UtilCylinder(var, WHITE_COLOR, 7, 40, 10, 2);
				glPopMatrix();
			}
			else {
				if (bulletPosition <= lastBulletPosition) {
					//printf("render bullet position %f last position %f\n", bulletPosition, lastBulletPosition);
					glPushMatrix();
					UtilTranslate(0, bulletPosition, 90);
					UtilCylinder(var, WHITE_COLOR, 7, 40, 10, 2);
					glPopMatrix();
				}
			}

		}

		if (lastBulletPosition < bulletLimit) {
			lastBulletPosition = bulletLimit;
			bulletTracking.y = 0;
		}


	}

	Texture(selectedTexture);
	//middle piece
	UtilTranslate(0, -20, 40); // middle piece distance
	glRotatef(11, 1, 0, 0);
	UtilCuboidScale(BODY_COLOR, 100, 35, 1, 0.5, 0.5);
	//left piece
	glPushMatrix();
	UtilTranslate(-35, 0, 0);
	glRotatef(-20, 0, 1, 0);
	UtilTranslate(-10, 0, 0);
	UtilCuboidScale(BODY_COLOR, 100, 10, 1, 0.5, 0.5);
	glRotatef(-30, 0, 1, 0);
	UtilTranslate(-18, 0, 0);
	UtilCuboidScale(BODY_COLOR, 120, 10, 1, 0.5, 0.5);
	glRotatef(-30, 0, 1, 0);
	UtilTranslate(-18, 0, 0);
	UtilCuboidScale(BODY_COLOR, 130, 10, 1, 0.5, 0.5);
	glPopMatrix();

	//right piece
	glPushMatrix();
	glScalef(-1, 1, 1);
	UtilTranslate(-35, 0, 0);
	glRotatef(-20, 0, 1, 0);
	UtilTranslate(-10, 0, 0);
	UtilCuboidScale(BODY_COLOR, 100, 10, 1, 0.5, 0.5);
	glRotatef(-30, 0, 1, 0);
	UtilTranslate(-18, 0, 0);
	UtilCuboidScale(BODY_COLOR, 120, 10, 1, 0.5, 0.5);
	glRotatef(-30, 0, 1, 0);
	UtilTranslate(-18, 0, 0);
	UtilCuboidScale(BODY_COLOR, 130, 10, 1, 0.5, 0.5);
	glPopMatrix();
}

void DrawLeftArm() {
	glPushMatrix();
	glScalef(0.5, 0.5, 0.5);

	Texture(selectedTexture);

	glPushMatrix();
	glRotatef(leftUpperarm.x/3, 1, 0, 0);
	UtilTranslate(20, 0, 0);
	DrawShoulderPads();
	UtilTranslate(-20, 20, 0);
	DrawShoulderPads();
	UtilTranslate(-20, 20, 0);
	DrawShoulderPads();
	UtilTranslate(160, -80, 0);
	glRotatef(-30, 0, 0, 1);
	glScalef(0.4, 1, 0.9);
	DrawShoulderPads();
	glPopMatrix();

	//upper arm
	glRotatef(leftUpperarm.y, 0, 1, 0);

	//upper arm to forearm connector

	DrawUpperArmBodyConnector(1);

	UtilTranslate(100, -40, 0);

	glRotatef(leftUpperarm.z, 0, 0, 1);
	//upper arm two plate connector

	glRotatef(leftUpperarm.x, 1, 0, 0);



	glPushMatrix();
	UtilTranslate(0, -160, 0);
	DrawUpperArmCover();
	glPopMatrix();

	//upper arm arm
	UtilCylinder(var, BODY_COLOR, 60, 53, 200, 15, 15);

	UtilTranslate(0, -220, 0);

	//elbow
	glPushMatrix();
	glRotatef(90, 0, 0, 1);

	glPushMatrix();
	UtilTranslate(0, 35, 0);
	UtilCylinder(var, BODY_COLOR_LIGHT, 55, 80, 15, 10);
	glPopMatrix();
	glPushMatrix();
	UtilTranslate(0, 55, 0);
	UtilCylinder(var, BLACK_COLOR_LIGHT, 55, 25, 15, 10);
	UtilTranslate(0, 5, 0);
	UtilCylinder(var, BLACK_COLOR_LIGHT, 50, 5, 15, 10);
	UtilTranslate(0, 5, 0);
	UtilCylinder(var, BLACK_COLOR_LIGHT, 45, 5, 15, 10);
	glPopMatrix();

	glPushMatrix();
	UtilTranslate(0, -45, 0);
	UtilCylinder(var, BLACK_COLOR_LIGHT, 55, 10, 15, 10);
	UtilTranslate(0, -10, 0);
	UtilCylinder(var, BLACK_COLOR, 45, 10, 15, 10);
	UtilTranslate(0, -10, 0);
	UtilCylinder(var, BLACK_COLOR, 25, 20, 15, 15, 10);
	glPopMatrix();

	glPopMatrix();

	//forearm
	glRotatef(leftForearm.x, 1, 0, 0);
	glRotatef(leftForearm.z, 0, 0, 1);

	UtilTranslate(0, -170, 0);

	glPushMatrix();
	UtilTranslate(0, 150, 0);
	UtilCylinder(var, BLACK_COLOR_LIGHT, 55, 40, 10, 10);
	glPopMatrix();

	glRotatef(leftForearm.y, 0, 1, 0);

	//forearmCover

	glPushMatrix();
	glRotatef(90, 0, 1, 0);
	DrawOuterForeArmCover(True);
	glPopMatrix();

	//forearmCover 2

	glPushMatrix();
	glRotatef(-90, 0, 1, 0);
	DrawInnerForeArmCover(True);
	glPopMatrix();


	glPushMatrix();
	UtilTranslate(0, 110, 0);
	//forearm
	Texture(selectedTexture);
	UtilCylinder(var, BLACK_COLOR_LIGHT, 51, 35, 110, 10, 10);
	glPopMatrix();
	//UtilCuboid(Vector3(100, 100, 200), 150, 35, 40, 35, 40);

	// palm

	UtilTranslate(0, -60, 0);
	glRotatef(leftPalm.x, 1, 0, 0);
	glRotatef(leftPalm.y - 90, 0, 1, 0);
	glRotatef(leftPalm.z, 0, 0, 1);
	UtilCuboid(BODY_COLOR, 60, 45, 35, 15, 15);

	// finger
	UtilTranslate(-30, 0, 0);
	DrawFinger(finger5);
	UtilTranslate(20, 0, 0);
	DrawFinger(finger4);
	UtilTranslate(20, 0, 0);
	DrawFinger(finger3);
	UtilTranslate(20, 0, 0);
	DrawFinger(finger2);

	UtilTranslate(8, 40, 0);
	glRotatef(60, 0, 0, 1);
	DrawThumb(finger1);
	glPopMatrix();
}

void DrawRightArm() {
	glPushMatrix();
	glScalef(0.5, 0.5, 0.5);
	Texture(selectedTexture);
	//shoulderPad
	glPushMatrix();
	glScalef(-1, 1, 1);
	glRotatef(rightUpperarm.x / 3, 1, 0, 0);
	UtilTranslate(20, 0, 0);
	DrawShoulderPads();
	UtilTranslate(-20, 20, 0);
	DrawShoulderPads();
	UtilTranslate(-20, 20, 0);
	DrawShoulderPads();
	UtilTranslate(160, -80, 0);
	glRotatef(-30, 0, 0, 1);
	glScalef(0.4, 1, 0.9);
	DrawShoulderPads();
	glPopMatrix();
	//upper arm
	glRotatef(rightUpperarm.y, 0, 1, 0);

	//upper arm to forearm connector
	DrawUpperArmBodyConnector(-1);


	UtilTranslate(-100, -40, 0);

	glRotatef(rightUpperarm.z, 0, 0, 1);

	glRotatef(rightUpperarm.x, 1, 0, 0);


	glPushMatrix();
	UtilTranslate(0, -160, 0);
	DrawUpperArmCover();
	glPopMatrix();
	//upper arm arm
	UtilCylinder(var, BODY_COLOR, 60, 53, 200, 15, 15);

	UtilTranslate(0, -220, 0);


	///elbow
	glPushMatrix();
	glRotatef(-90, 0, 0, 1);

	glPushMatrix();
	UtilTranslate(0, 35, 0);
	UtilCylinder(var, BODY_COLOR_LIGHT, 55, 80, 15, 15);
	glPopMatrix();
	glPushMatrix();
	UtilTranslate(0, 55, 0);
	UtilCylinder(var, BLACK_COLOR_LIGHT, 55, 25, 15, 15);
	UtilTranslate(0, 5, 0);
	UtilCylinder(var, BLACK_COLOR_LIGHT, 50, 5, 15, 15);
	UtilTranslate(0, 5, 0);
	UtilCylinder(var, BLACK_COLOR_LIGHT, 45, 5, 15, 15);
	glPopMatrix();

	glPushMatrix();
	UtilTranslate(0, -45, 0);
	UtilCylinder(var, BLACK_COLOR_LIGHT, 55, 10, 15, 15);
	UtilTranslate(0, -10, 0);
	UtilCylinder(var, BLACK_COLOR, 45, 10, 15, 15);
	UtilTranslate(0, -10, 0);
	UtilCylinder(var, BLACK_COLOR, 25, 20, 15, 15, 15);
	glPopMatrix();

	glPopMatrix();

	//forearm
	glRotatef(rightForearm.x, 1, 0, 0);
	glRotatef(rightForearm.z, 0, 0, 1);

	UtilTranslate(0, -170, 0);

	glPushMatrix();
	UtilTranslate(0, 150, 0);
	UtilCylinder(var, BLACK_COLOR_LIGHT, 55, 40, 10, 10);
	glPopMatrix();

	glRotatef(rightForearm.y, 0, 1, 0);

	//forearmCover
	glPushMatrix();
	glPushMatrix();
	glRotatef(-90, 0, 1, 0);
	DrawOuterForeArmCover(False);
	glPopMatrix();

	//forearmCover 2
	glPushMatrix();
	glRotatef(90, 0, 1, 0);
	DrawInnerForeArmCover(False);
	glPopMatrix();
	glPopMatrix();

	glPushMatrix();
	UtilTranslate(0, 110, 0);

	//forearm
	Texture(selectedTexture);
	UtilCylinder(var, BLACK_COLOR_LIGHT, 51, 35, 110, 10, 10);
	glPopMatrix();
	//UtilCuboid(Vector3(100, 100, 200), 150, 35, 40, 35, 40);

	// palm
	glBindTexture(GL_TEXTURE_2D, darkRustyMetal);
	UtilCylinder(var, WHITE_COLOR, 60, 30, 10, 10);
	UtilTranslate(0, -30, 0);
	UtilCylinder(var, WHITE_COLOR, 30, 60, 10, 10);

	//grip 
	for (int i = 0; i < 4; i++) {

		glPushMatrix();
		glRotatef(90 * i, 0, 1, 0);
		UtilTranslate(-45, -30, 0);

		glPushMatrix();// grip base
		UtilTranslate(0, -5, 0);
		UtilCuboid(WHITE_COLOR, 10, 45, 25);
		glPopMatrix();

		UtilTranslate(-45, 0, 0);

		glPushMatrix(); // joint for grip
		UtilTranslate(0, 0, 24);
		glRotatef(90, 1, 0, 0);
		UtilCylinder(var, WHITE_COLOR, 5, 48, 10, 10);
		glPopMatrix();

		glRotatef(45, 0, 0, 1); //grip part 2
		UtilTranslate(-20, 0, 0);

		glPushMatrix();
		UtilTranslate(0, -5, 0);
		UtilCuboid(WHITE_COLOR, 10, 20, 25);
		glPopMatrix();


		UtilTranslate(-20, 0, 0);

		glPushMatrix(); // joint for grip
		UtilTranslate(0, 0, 24);
		glRotatef(90, 1, 0, 0);
		UtilCylinder(var, WHITE_COLOR, 5, 48, 10, 10);
		glPopMatrix();

		glRotatef(30, 0, 0, 1); //grip part 3
		UtilTranslate(-20, -5, 0);
		UtilCuboid(WHITE_COLOR, 10, 20, 25);


		glPopMatrix();
	}

	//adjust ball position
	UtilTranslate(0, -30 - spikeBall.y, 0);

	// Calculate the number of bullets to display
	int numBullets = spikeBall.y / 40 + 1;

	for (int i = 0; i < numBullets; i++) {
		if (numBullets > 100) numBullets = 100;

		// Position each bullet in its trajectory
		float bulletPosition = 30 + (i * 40);

		// Check if the bullet is within visible range
		if (bulletPosition < -3000) continue;

		// Render the bullet
		glPushMatrix();
		UtilTranslate(0, bulletPosition, 0);
		UtilCylinder(var, WHITE_COLOR, 10, 30, 10, 2);
		UtilTranslate(0, 10, 0);
		UtilCylinder(var, WHITE_COLOR, 3, 10, 10, 2);
		glPopMatrix();
	}


	//spike ball
	glRotatef(rightPalm.x, 1, 0, 0);
	glRotatef(rightPalm.y, 0, 1, 0);
	glRotatef(rightPalm.z, 0, 0, 1);

	glBindTexture(GL_TEXTURE_2D, darkRustyMetalBright);

	UtilCylinder(var, WHITE_COLOR, 30, 100, 30, 15, 5);

	UtilTranslate(0, -30, 0);
	UtilCylinder(var, WHITE_COLOR, 100, 100, 15, 5);

	glBindTexture(GL_TEXTURE_2D, blade);
	for (int j = 20; j < 100;) {

		for (int i = 0; i < 360;) {
			glPushMatrix();
			glRotatef(i, 0, 1, 0);
			UtilTranslate(0, -j, 95);
			glRotatef(120, 1, 0, 0);
			UtilTriangle(WHITE_COLOR, 40, 20, 3);
			glPopMatrix();

			i += 60;
		}

		j += 40;
	}

	for (int j = 40; j < 100;) {
		for (int i = 30; i < 360;) {
			glPushMatrix();
			glRotatef(i, 0, 1, 0);
			UtilTranslate(0, -j, 95);
			glRotatef(120, 1, 0, 0);
			UtilTriangle(WHITE_COLOR, 40, 20, 3);
			glPopMatrix();

			i += 60;
		}

		j += 40;
	}

	UtilTranslate(0, -100, 0);
	glBindTexture(GL_TEXTURE_2D, darkRustyMetalBright);
	UtilCylinder(var, WHITE_COLOR, 100, 80, 30, 15, 5);

	glBindTexture(GL_TEXTURE_2D, blade);
	for (int i = 0; i < 360;) {
		glPushMatrix();
		glRotatef(i, 0, 1, 0);
		UtilTranslate(0, -15, 85);
		glRotatef(160, 1, 0, 0);
		UtilTriangle(WHITE_COLOR, 40, 15, 3);
		glPopMatrix();

		i += 45;
	}


	UtilTranslate(0, -30, 0);
	glBindTexture(GL_TEXTURE_2D, darkRustyMetalBright);
	UtilCylinder(var, WHITE_COLOR, 80, 50, 30, 15, 5);

	glBindTexture(GL_TEXTURE_2D, blade);
	for (int i = 0; i < 360;) {
		glPushMatrix();
		glRotatef(i, 0, 1, 0);
		UtilTranslate(0, -15, 60);
		glRotatef(70, 0, 1, 0);
		glRotatef(160, 1, 0, 0);
		UtilTriangle(WHITE_COLOR, 40, 10, 3);
		glPopMatrix();

		i += 30;
	}

	UtilTranslate(0, -30, 0);

	glBindTexture(GL_TEXTURE_2D, blade);
	for (int i = 0; i < 360;) {
		glPushMatrix();
		glRotatef(i, 0, 1, 0);
		glRotatef(i, 0, 1, 0);
		UtilTranslate(0, 0, 35);
		glRotatef(180, 1, 0, 0);
		UtilTriangle(WHITE_COLOR, 20, 10, 3);
		glPopMatrix();

		i += 90;
	}

	glPopMatrix();
}

void DrawBody() {
	//chest
	glPushMatrix();
	glRotatef(90, 0, 0, 1);
	UtilTranslate(0, 130, 0);
	UtilCylinder(var, BODY_COLOR, 60, 260, 10, 10);
	glPopMatrix();

	glPushMatrix();
	UtilTranslate(0, -60, 0);
	UtilCuboid(BODY_COLOR, 70, 120, 50);
	UtilTranslate(0, -120, 0);

	//lower abdomen
	UtilCuboid(BLACK_COLOR, 120, 30, 98, 30, 50);
	UtilTranslate(0, -50, 0);
	UtilCuboid(BLACK_COLOR, 80, 45, 55, 35, 30);
	glPopMatrix();

	for (int i = 0; i < 3; i++) {
		glPushMatrix();
		UtilTranslate(28, -240 + (25 * i), 38 + (3 * i));
		glRotatef(10, 0, 0, 1);
		glRotatef(18, 1, 0, 0);
		UtilCuboid(BLACK_COLOR_LIGHT, 20, 28, 5);
		UtilTranslate(-3, 8, 5);
		UtilCuboid(BLACK_COLOR_DARK, 10, 20, 5);
		glPopMatrix();

		glPushMatrix();
		UtilTranslate(-28, -240 + (25 * i), 38 + (3 * i));
		glRotatef(-10, 0, 0, 1);
		glRotatef(18, 1, 0, 0);
		UtilCuboid(BLACK_COLOR_LIGHT, 20, 28, 5);
		UtilTranslate(-3, 8, 5);
		UtilCuboid(BLACK_COLOR_DARK, 10, 20, 5);
		glPopMatrix();
	}

	//chest connector
	glPushMatrix();
	UtilTranslate(0, -170, 40);
	UtilCuboid(BLACK_COLOR, 45, 80, 60, 15, 20);
	glPopMatrix();

	glPushMatrix();
	UtilTranslate(-35, -145, 55);
	glRotatef(90, 0, 0, 1);
	UtilCylinder(var, BLACK_COLOR, 15, 70, 20, 5);
	glPopMatrix();

	glPushMatrix();
	UtilTranslate(0, -170, 65);
	UtilCuboid(BODY_COLOR, 10, 35, 8);
	glPopMatrix();

	//belt
	glPushMatrix();
	UtilTranslate(65, -172, 60);
	glRotatef(52, 0, 0, 1);
	glRotatef(20, 1, 0, 0);
	UtilCuboid(BODY_COLOR, 40, 20, 2);
	glPopMatrix();

	glPushMatrix();
	UtilTranslate(-65, -172, 60);
	glRotatef(-52, 0, 0, 1);
	glRotatef(20, 1, 0, 0);
	UtilCuboidScale(BODY_COLOR, 40, 20, 2, -1, 1);
	glPopMatrix();

	glPushMatrix();
	UtilTranslate(78, -185, 50);
	glRotatef(52, 0, 0, 1);
	glRotatef(30, 1, 0, 0);
	UtilCuboid(BODY_COLOR, 20, 10, 1);
	glPopMatrix();

	glPushMatrix();
	UtilTranslate(-78, -185, 45);
	glRotatef(-52, 0, 0, 1);
	glRotatef(30, 1, 0, 0);
	UtilCuboid(BODY_COLOR, 20, 10, 1);
	glPopMatrix();

	glPushMatrix();
	UtilTranslate(78, -185, 45);
	glRotatef(52, 0, 0, 1);
	glRotatef(30, 1, 0, 0);
	UtilCuboid(BODY_COLOR, 20, 10, 1);
	glPopMatrix();

	glPushMatrix();
	UtilTranslate(-85, -180, 52);
	glRotatef(-30, 0, 0, 1);
	glRotatef(10, 1, 0, 0);
	UtilCuboid(BODY_COLOR, 25, 5, 1);
	glPopMatrix();

	glPushMatrix();
	UtilTranslate(85, -180, 52);
	glRotatef(30, 0, 0, 1);
	glRotatef(10, 1, 0, 0);
	UtilCuboid(BODY_COLOR, 25, 5, 1);
	glPopMatrix();

	//middle box
	glPushMatrix();
	UtilTranslate(0, -128, 10);
	UtilCuboid(BODY_COLOR, 8, 15, 30, 48, 52);
	glPopMatrix();

	glPushMatrix();
	UtilTranslate(-28, -120, 10);
	glRotatef(90, 0, 1, 0);
	UtilCuboid(BODY_COLOR, 40, 52, 2);
	glPopMatrix();

	glPushMatrix();
	UtilTranslate(28, -120, 10);
	glRotatef(90, 0, 1, 0);
	UtilCuboid(BODY_COLOR, 40, 52, 2);
	glPopMatrix();

	glPushMatrix();
	UtilTranslate(-25, -80, 10);
	glRotatef(90, 0, 1, 0);
	glRotatef(90, 1, 0, 0);
	UtilCuboid(BODY_COLOR, 50, 52, 2);
	glPopMatrix();

	glPushMatrix();
	UtilTranslate(0, -120, 12);
	UtilCuboid(BLACK_COLOR, 27, 10, 15, 52, 52);
	glPopMatrix();

	//gun
	Vector3 gunColor = BLACK_COLOR;
	glPushMatrix();
	UtilTranslate(-60, -120, 10);
	glRotatef(90, 0, 1, 0);
	glRotatef(90, 1, 0, 0);
	UtilCuboid(BLACK_COLOR, 30, 52, 1);
	glPopMatrix();

	glPushMatrix();
	UtilTranslate(30, -120, 10);
	glRotatef(90, 0, 1, 0);
	glRotatef(90, 1, 0, 0);
	UtilCuboid(BLACK_COLOR, 30, 52, 1);
	glPopMatrix();

	glPushMatrix();
	UtilTranslate(52, -105, gunPosition); //z 90 when shot
	UtilCuboid(gunColor, 20, 18, 50);
	UtilTranslate(-104, 0, 0);
	UtilCuboid(gunColor, 20, 18, 50);
	glPopMatrix();

	glPushMatrix();
	UtilTranslate(45, -115, gunPosition);
	UtilCuboid(gunColor, 38, 4, 50);
	UtilTranslate(-90, 0, 0);
	UtilCuboid(gunColor, 38, 4, 50);
	glPopMatrix();

	glPushMatrix();
	UtilTranslate(60, -115, gunPosition);
	UtilCuboid(gunColor, 38, 4, 50);
	UtilTranslate(-120, 0, 0);
	UtilCuboid(gunColor, 38, 4, 50);
	glPopMatrix();

	glPushMatrix(); //right
	UtilTranslate(45, -95, barrellPosition); //z 170 when shot
	glRotatef(90, 1, 0, 0);
	UtilCylinder(var, gunColor, 6, 100, 20, 5);
	glPopMatrix();

	glPushMatrix();
	UtilTranslate(60, -95, barrellPosition);
	glRotatef(90, 1, 0, 0);
	UtilCylinder(var, gunColor, 6, 100, 20, 5);
	glPopMatrix();

	glPushMatrix(); //left
	UtilTranslate(-45, -95, barrellPosition);
	glRotatef(90, 1, 0, 0);
	UtilCylinder(var, gunColor, 6, 100, 20, 5);
	glPopMatrix();

	glPushMatrix();
	UtilTranslate(-60, -95, barrellPosition);
	glRotatef(90, 1, 0, 0);
	UtilCylinder(var, gunColor, 6, 100, 20, 5);
	glPopMatrix();

	// Update bullet tracking when not shooting
	if (!shootingBody) {
		if (bulletTrackingBody.y > 0) {
			//printf("bullet tracking y more than 0\n");
			lastBulletPositionBody -= 5;
			bulletTrackingBody.y += 5; // Continue moving existing bullets
		}
	}

	//printf("%d %d", shooting, findLast);
	// Calculate the number of bullets to display
	int numBullets = (shootingBody || !shootingBody && bulletTrackingBody.y > 0) ? (bulletTrackingBody.y / 50 + 1) : 0; // Ensure valid number of bullets

	for (int i = 0; i < numBullets; i++) {
		//printf("hi");
		if (numBullets > 50) numBullets = 50; // Cap the number of bullets

		// Position each bullet in its trajectory
		float bulletPosition = 30 - (i * 50) - fmod(bulletTrackingBody.y, 50);


		// Check if the bullet is within visible range
		if (bulletPosition < bulletLimitBody) continue;

		// Record the last bullet position when shooting stops
		if (!shootingBody && lastBulletPositionBody == (bulletLimitBody - 5) && findLastBody) {
			findLastBody = false;
			//printf("last bullet %f\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n", bulletPosition);
			lastBulletPositionBody = bulletPosition;
		}

		Texture(selectedTexture);

		//printf("bullet position %f last position %f\n", bulletPosition, lastBulletPosition);
		// Render the bullet if rendering hasn't stopped
		if (shootingBody) {
			//printf("render bullet position %f last position %f\n", bulletPosition, lastBulletPosition);
			glPushMatrix();
			glRotatef(-90, 1, 0, 0);
			UtilTranslate(45, bulletPosition - 100 , -95);
			UtilCylinder(var, WHITE_COLOR, 3, 15, 10, 2);
			UtilTranslate(15, 0, 0);
			UtilCylinder(var, WHITE_COLOR, 3, 15, 10, 2);
			glPopMatrix();
			glPushMatrix();
			glScalef(-1, 1, 1);
			glRotatef(-90, 1, 0, 0);
			UtilTranslate(45, bulletPosition - 100 , -95);
			UtilCylinder(var, WHITE_COLOR, 3, 15, 10, 2);
			UtilTranslate(15, 0, 0);
			UtilCylinder(var, WHITE_COLOR, 3, 15, 10, 2);
			glPopMatrix();
		}
		else {
			if (bulletPosition <= lastBulletPositionBody) {
				//printf("render bullet position %f last position %f\n", bulletPosition, lastBulletPosition);
				glPushMatrix();
				glRotatef(-90, 1, 0, 0);
				UtilTranslate(45, bulletPosition - 100, -95);
				UtilCylinder(var, WHITE_COLOR, 3, 15, 10, 2);
				UtilTranslate(15, 0, 0);
				UtilCylinder(var, WHITE_COLOR, 3, 15, 10, 2);
				glPopMatrix();
				glPushMatrix();
				glScalef(-1, 1, 1);
				glRotatef(-90, 1, 0, 0);
				UtilTranslate(45, bulletPosition - 100, -95);
				UtilCylinder(var, WHITE_COLOR, 3, 15, 10, 2);
				UtilTranslate(15, 0, 0);
				UtilCylinder(var, WHITE_COLOR, 3, 15, 10, 2);
				glPopMatrix();
			}
		}

	}

	if (lastBulletPositionBody < bulletLimitBody) {
		lastBulletPositionBody = bulletLimitBody;
		bulletTrackingBody.y = 0;
	}

	Texture(bulletRow);
	//bullets
	for (int i = 0; i < 2; i++) {
		glPushMatrix();
		UtilTranslate(35, -5, 0);
		glRotatef(-15, 0, 0, 1);

		glPushMatrix();
		if (i != 0) {
			UtilTranslate(-65, -160, -40);
			glScalef(1, 1, -1);
		}
		else UtilTranslate(-65, -160, 40);
		glRotatef(-300, 0, 0, 1);
		glRotatef(-7, 0, 1, 0);
		glRotatef(12, 1, 0, 0);
		UtilCuboidScale(GREY_COLOR, 40, 25, 3, 0.7, 1);
		UtilTranslate(0, 0, -3);
		UtilCuboidScale(Vector3(), 40, 25, 3, 0.7, 1);
		glPopMatrix();

		glPushMatrix();
		UtilTranslate(-80, -182, 0);
		glRotatef(60, 0, 0, 1);
		UtilCuboid(GREY_COLOR, 38, 2, 2, 45, 45);
		UtilTranslate(4, 0, 0);
		UtilCuboid(Vector3(), 38, 2, 2, 45, 45);
		glPopMatrix();

		glPopMatrix();

		glPushMatrix();
		UtilTranslate(-35, -5, 0);
		glRotatef(15, 0, 0, 1);

		glPushMatrix();
		if (i != 0) {
			UtilTranslate(65, -160, -40);
			glScalef(1, 1, -1);
		}
		else UtilTranslate(65, -160, 40);
		glRotatef(300, 0, 0, 1);
		glRotatef(7, 0, 1, 0);
		glRotatef(12, 1, 0, 0);
		UtilCuboidScale(GREY_COLOR, 40, 25, 3, 0.7, 1);
		UtilTranslate(0, 0, -3);
		UtilCuboidScale(Vector3(), 40, 25, 3, 0.7, 1);
		glPopMatrix();


		glPushMatrix();
		glScalef(-1, 1, 1);
		UtilTranslate(-80, -182, 0);
		glRotatef(60, 0, 0, 1);
		UtilCuboid(GREY_COLOR, 38, 2, 2, 45, 45);
		UtilTranslate(4, 0, 0);
		UtilCuboid(Vector3(), 38, 2, 2, 45, 45);
		glPopMatrix();

		glPopMatrix();
	}


	Texture(selectedTexture);
	//body side
	for (int i = 0; i < 2; i++) {
		glPushMatrix();
		UtilTranslate(0, 0, 0);
		glRotatef(-10, 0, 0, 1);
		glScalef(0.8, 0.9, 0.9);

		glPushMatrix();
		if (i != 0) {
			UtilTranslate(-130, -125, -40);
			glScalef(1, 1, -1);
		}
		else UtilTranslate(-130, -125, 40);
		glRotatef(-60, 0, 0, 1);
		glRotatef(-7, 0, 1, 0);
		glRotatef(15, 1, 0, 0);
		UtilCuboid(BODY_COLOR, 60, 30, 3);
		glPopMatrix();

		glPushMatrix();
		UtilTranslate(-115, -155, 0);
		glRotatef(30, 0, 0, 1);
		UtilCuboid(BODY_COLOR, 65, 2, 2, 42, 35);
		glPopMatrix();

		glPushMatrix();
		UtilTranslate(-152, -98, 0);
		glRotatef(-58, 0, 0, 1);
		UtilCuboid(BODY_COLOR, 65, 2, 2, 32, 45);
		glPopMatrix();

		glPopMatrix();

		glPushMatrix();
		UtilTranslate(0, 0, 0);
		glRotatef(10, 0, 0, 1);
		glScalef(0.8, 0.9, 0.9);

		glPushMatrix();
		if (i != 0) {
			UtilTranslate(130, -125, -40);
			glScalef(1, 1, -1);
		}
		else UtilTranslate(130, -125, 40);
		glRotatef(60, 0, 0, 1);
		glRotatef(7, 0, 1, 0);
		glRotatef(15, 1, 0, 0);
		UtilCuboid(BODY_COLOR, 60, 35, 3);
		glPopMatrix();

		glPushMatrix();
		UtilTranslate(115, -155, 0);
		glRotatef(-30, 0, 0, 1);
		UtilCuboid(BODY_COLOR, 65, 2, 2, 42, 35);
		glPopMatrix();

		glPushMatrix();
		UtilTranslate(152, -98, 0);
		glRotatef(58, 0, 0, 1);
		UtilCuboid(BODY_COLOR, 65, 2, 2, 32, 45);
		glPopMatrix();

		glPopMatrix();
	}

	////chest plate
	glPushMatrix();
	UtilTranslate(0, -72, 58);
	glRotatef(8, 1, 0, 0);
	UtilCuboid(BODY_COLOR_DARK, 25, 105, 128, 2, 2);
	glPopMatrix();

	glPushMatrix();
	UtilTranslate(0, -50, 60);
	UtilCuboid(BODY_COLOR_DARK, 80, 128, 2);
	glPopMatrix();


	glPushMatrix();
	UtilTranslate(0, -102, -65);
	//glRotatef(3, 1, 0, 0);
	UtilCuboid(BODY_COLOR, 50, 75, 128, 2, 2);
	glPopMatrix();

	glPushMatrix();
	UtilTranslate(0, -50, -65);
	UtilCuboid(BODY_COLOR_LIGHT, 80, 130, 150, 2, 2);
	glPopMatrix();

	glPushMatrix();
	UtilTranslate(0, -185, -62);
	glRotatef(-3, 1, 0, 0);
	UtilCuboid(BODY_COLOR, 80, 40, 75, 2, 2);
	glPopMatrix();

	Texture(logo);
	//logo
	glPushMatrix();
	UtilTranslate(0, -45, 65);
	UtilCuboid(BODY_COLOR_LIGHT, 60, 20, 40, 2, 2, 1, 1, False);
	glPopMatrix();

	Texture(selectedTexture);
	//back spine
	glPushMatrix();
	UtilTranslate(-60, -40, -70);
	UtilCuboid(SPINE_COLOR, 50, 15, 20, 6, 2);
	UtilTranslate(120, 0, 0);
	UtilCuboid(SPINE_COLOR, 50, 15, 20, 6, 2);
	glPopMatrix();

	glPushMatrix();
	UtilTranslate(-60, -20, -72);
	UtilCuboid(SPINE_COLOR, 30, 13, 16, 3, 4);
	UtilTranslate(120, 0, 0);
	UtilCuboid(SPINE_COLOR, 30, 13, 16, 3, 4);
	glPopMatrix();

	glPushMatrix();
	UtilTranslate(-17, -30, -75);
	glRotatef(5, 1, 0, 0);
	UtilCuboid(SPINE_COLOR, 35, 7, 6, 6, 2);
	UtilTranslate(34, 0, 0);
	UtilCuboid(SPINE_COLOR, 35, 7, 6, 6, 2);
	UtilTranslate(-17, 0, 0);
	UtilCuboid(SPINE_COLOR, 20, 7, 7, 6, 2);
	glPopMatrix();

	glPushMatrix();
	UtilTranslate(0, -40, -70);
	UtilCuboid(SPINE_COLOR, 10, 25, 1);
	UtilTranslate(-20, 4, -6);
	glRotatef(90, 0, 0, 1);
	UtilCylinder(var, SPINE_COLOR, 5, 40, 10, 5);
	glPopMatrix();

	glPushMatrix();
	UtilTranslate(0, -92, -76);
	glRotatef(5, 1, 0, 0);
	UtilCuboid(SPINE_COLOR, 50, 15, 25, 4, 6);
	glPopMatrix();

	glPushMatrix();
	UtilTranslate(0, -65, -82);
	glRotatef(90, 1, 0, 0);
	UtilCircle(22, 120, SPINE_COLOR);
	UtilTranslate(0, -2, 0);
	UtilCircle(17, 120, BLACK_COLOR_DARK);
	glPopMatrix();

	glPushMatrix();
	UtilTranslate(0, -65, -85);
	UtilSphere(var, SPINE_COLOR, 10, 10, 10);
	glPopMatrix();

	glPushMatrix();
	UtilTranslate(0, -125, -70);
	glRotatef(-7, 1, 0, 0);
	UtilCuboid(SPINE_COLOR, 40, 15, 20, 3, 5);
	UtilTranslate(0, -30, 0);
	UtilCuboid(SPINE_COLOR, 40, 15, 18, 3, 5);
	UtilTranslate(0, -32, 5);
	glRotatef(-7, 1, 0, 0);
	UtilCuboid(SPINE_COLOR, 40, 10, 15, 3, 5);
	UtilTranslate(0, -25, -5);
	glRotatef(0, 0, 0, 0);
	UtilCuboid(SPINE_COLOR, 40, 10, 15, 3, 4);
	glPopMatrix();

	glPushMatrix();
	UtilTranslate(0, -122, -80);
	glRotatef(-11, 1, 0, 0);
	UtilCuboid(SPINE_COLOR, 30, 3, 7, 3, 5);
	UtilTranslate(0, -30, 0);
	UtilCuboid(SPINE_COLOR, 30, 3, 7, 3, 5);
	UtilTranslate(0, -25, 0);
	glRotatef(-3, 1, 0, 0);
	UtilCuboid(SPINE_COLOR, 25, 3, 7, 3, 5);
	glPopMatrix();

	glPushMatrix();
	UtilTranslate(0, -205, -60);
	glRotatef(-20, 1, 0, 0);
	UtilCuboid(SPINE_COLOR, 30, 6, 10, 1, 5);
	glPopMatrix();
}

void DrawFoot(Vector3& foot) {

	glPushMatrix();
	glRotatef(90, 0, 0, 1);
	UtilTranslate(0, 15, 0);
	UtilCylinder(var, BLACK_COLOR, 20, 30, 15, 15);
	glPopMatrix();

	glRotatef(foot.x, 1, 0, 0);
	glRotatef(foot.y, 0, 1, 0);
	glRotatef(foot.z, 0, 0, 1);

	UtilTranslate(0, -50, 10);
	UtilCuboid(BODY_COLOR, 50, 25, 40);
	glPushMatrix();
	UtilTranslate(0, 0, 40);
	glRotatef(-90, 0, 1, 0);
	glRotatef(135, 0, 0, 1);
	UtilTranslate(0, -35.3, 0);
	UtilTriangle(BODY_COLOR, 35.3, 35.3, 25);
	glPopMatrix();

	glPushMatrix();
	UtilTranslate(0, 0, -40);
	glRotatef(90, 0, 1, 0);
	glRotatef(135, 0, 0, 1);
	UtilTranslate(0, -20, 0);
	UtilTriangle(BODY_COLOR, 20, 20, 10);
	glPopMatrix();


	//side pieces
	glPushMatrix();
	UtilTranslate(35, 0, 0);
	glRotatef(0, 0, 1, 0);
	glRotatef(135, 0, 0, 1);
	UtilTranslate(0, -20, 0);
	UtilTriangle(BODY_COLOR, 20, 20, 10);
	glPopMatrix();

	glPushMatrix();
	UtilTranslate(20, 0, 0);
	glRotatef(0, 0, 1, 0);
	UtilTranslate(0, 1, 0);
	UtilCuboid(BLACK_COLOR, 20, 15, 5);
	glPopMatrix();
	
	//side piece 2
	glPushMatrix();

	glScalef(-1, 1, 1);
	glPushMatrix();
	UtilTranslate(35, 0, 0);
	glRotatef(0, 0, 1, 0);
	glRotatef(135, 0, 0, 1);
	UtilTranslate(0, -20, 0);
	UtilTriangle(BODY_COLOR, 20, 20, 10);
	glPopMatrix();

	glPushMatrix();
	UtilTranslate(20, 0, 0);
	glRotatef(0, 0, 1, 0);
	UtilTranslate(0, 1, 0);
	UtilCuboid(BLACK_COLOR, 20, 15, 5);
	glPopMatrix();
	glPopMatrix();

}

void DrawLeg(Vector3& thigh, Vector3& calf, Vector3& foot) {
	// thigh
	Texture(selectedTexture);
	glRotatef(thigh.y, 0, 1, 0);

	//connector to body
	glPushMatrix();
	glRotatef(90, 0, 0, 1);
	UtilCylinder(var, BLACK_COLOR, 15, 100, 10, 10);
	glPopMatrix();

	UtilTranslate(100, 0, 0);

	glRotatef(thigh.z, 0, 0, 1);
	glRotatef(thigh.x, 1, 0, 0);

	//whole uppper leg move down
	glPushMatrix();
	UtilTranslate(0, -40, 0);
	//tall back 
	glPushMatrix();
	UtilTranslate(0, 0, 0);
	UtilCuboid(BODY_COLOR_DARK, 40, 40, 45, 40, 45);
	UtilTranslate(0, 40, 0);
	UtilCuboid(BODY_COLOR, 45, 45, 30, 45, 30);
	glPopMatrix();


	//thigh thigh
	UtilCylinder(var, BLACK_COLOR, 20, 100, 15, 5);

	glPushMatrix();
	UtilTranslate(0, -65, 0);
	UtilCuboid(BODY_COLOR, 60, 25, 40, 30, 40);
	glPopMatrix();

	glPopMatrix();
	UtilTranslate(0, -120, 0);

	//thigh back pop up

	glPushMatrix();
	UtilTranslate(0, 40, -40);
	UtilCuboid(BODY_COLOR_DARK, 80, 10, 20, 10, 20);
	glPopMatrix();

	glPushMatrix();
	UtilTranslate(-30, 0, 0);
	glRotatef(90, 0, 0, 1);
	UtilCylinder(var, BLACK_COLOR_DARK, 30, 60, 15, 1, True);
	glPopMatrix();

	//calves
	glRotatef(calf.x, 1, 0, 0);
	glRotatef(calf.y, 0, 1, 0);
	glRotatef(calf.z, 0, 0, 1);

	//knee
	glPushMatrix();
	UtilTranslate(0, -40, 0);
	glScalef(0.6, 0.6, 0.6);
	DrawKneeCover();
	glPopMatrix();
	UtilCylinder(var, BLACK_COLOR, 15, 120, 15, 5);

	glPushMatrix();
	UtilTranslate(0, -110, 0);
	UtilCuboid(BODY_COLOR, 80, 25, 25, 20, 30);
	glPopMatrix();

	//feet
	//feet to calf
	UtilTranslate(0, -120, 0);
	DrawFoot(foot);
}

void DrawBodyLegConnector() {
	glPushMatrix();
	UtilCuboid(BODY_COLOR_DARK, 60, 30, 30);

	glPushMatrix();
	UtilTranslate(0, -10, 30);
	UtilCuboid(BODY_COLOR_LIGHT, 80, 30, 60, 10, 10);
	glPopMatrix();

	glPushMatrix();
	UtilTranslate(0, 10, 0);
	UtilCuboid(BODY_COLOR_DARK, 70, 30, 55, 35, 35);
	glPopMatrix();

	glPushMatrix();
	UtilTranslate(0, 0, -30);
	UtilCuboid(BODY_COLOR_LIGHT, 55, 15, 10);
	UtilTranslate(0, 55, 0);
	UtilCuboid(BODY_COLOR_LIGHT, 25, 45, 55, 10, 10);
	glPopMatrix();

	glPushMatrix();
	UtilTranslate(0, 100, 0);
	DrawCrotch();
	glPopMatrix();

	glPopMatrix();
}

void DrawBracer() {
	// ========================
	//   head
	// ========================

	//DrawHead();
	glPushMatrix();
	UtilTranslate(0, 35, 40);
	glScalef(0.27, 0.30, 0.30);
	glRotatef(180, 0, 1, 0);
	drawFace();
	glPopMatrix();
	glPushMatrix();
	UtilTranslate(0, 35, 40);
	glScalef(0.27, 0.3, 0.3);

	glRotatef(180, 0, 1, 0);
	helmatTop();
	glPopMatrix();

	// ========================
	//   body
	// ========================
	glPushMatrix();
	glRotatef(body.x, 1, 0, 0);
	glRotatef(body.y, 0, 1, 0);
	glRotatef(body.z, 0, 0, 1);

	glPushMatrix();
	glScalef(0.9, 0.8, 1);
	DrawBody();
	glPopMatrix();


	// ========================
	//    Right Arm
	// ========================
	glPushMatrix();
	UtilTranslate(-130, 0, 0);
	DrawRightArm();
	glPopMatrix();

	// ========================
	//    Left Arm
	// ========================
	glPushMatrix();
	UtilTranslate(130, 0, 0);
	DrawLeftArm();
	glPopMatrix();


	glPopMatrix();

	// ========================
	//    Body Leg Connector
	// ========================
	UtilTranslate(0, -260, 0);
	DrawBodyLegConnector();

	// ========================
	//    Left Leg
	// ========================
	glPushMatrix();
	UtilTranslate(0, 0, 0);
	//DrawLeftLeg();
	DrawLeg(leftThigh, leftCalf, leftFoot);
	glPopMatrix();

	// ========================
	//    Right Leg
	// ========================
	glPushMatrix();
	glScalef(-1, 1, 1);
	//DrawLeftLeg();
	DrawLeg(rightThigh, rightCalf, rightFoot);
	glPopMatrix();

}

void display()
{
	if (testAction) InitialiseModel();

	Texture(selectedTexture);
	//glDisable(GL_TEXTURE_2D);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	cam.z = (zoomLevel / 800);

	glMatrixMode(GL_MODELVIEW);
	glClearColor(0.3f, 0.3f, 0.3f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();

	var = gluNewQuadric();
	gluQuadricNormals(var, GLU_SMOOTH);
	gluQuadricTexture(var, GL_TRUE);
	gluLookAt(
		cam.x, cam.y, cam.z,  // Camera position
		lookAt.x, lookAt.y, lookAt.z,  // Look-at point (center of the scene)
		0.0f, 1.0f, 0.0f   // Up vector
	);

	//lighting
	setupLighting();
	GLfloat matSpecular[] = { 0.3f, 0.3f, 0.3f, 1.0f };
	GLfloat matShininess[] = { 10 };

	glMaterialfv(GL_FRONT, GL_SPECULAR, matSpecular);
	glMaterialfv(GL_FRONT, GL_SHININESS, matShininess);

	//lightSphere
	glPushMatrix();
	UtilTranslate(light1Pos[0], light1Pos[1], light1Pos[2]);
	//UtilSphere(var, Vector3(255, 0, 0), 20, 10, 10);
	glPopMatrix();


	//center ball
	UtilSphere(var, Vector3(255, 0, 0), 10, 10, 10);


	//animation
	Animate();

	//drawing
	if (q1Rotation.x > 180) {
		q1Rotation.x -= 360;
	}
	if (q1Rotation.y > 180) {
		q1Rotation.y -= 360;
	}

	glRotatef(q1Rotation.x, 1.0f, 0.0f, 0.0f);
	glRotatef(q1Rotation.y, 0.0f, 1.0f, 0.0f);


	UtilTranslate(0, 150, 0);
	DrawBracer();

	
	//DrawRightArm();
	//DrawLeftArm();
	//DrawBodyLegConnector();
	//DrawShoulderPads();
	//DrawLeftLeg();
	//DrawFoot();

}
//--------------------------------------------------------------------

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, int nCmdShow)
{
	AllocConsole();
	FILE* fp;
	freopen_s(&fp, "CONOUT$", "w", stdout); // Redirect stdout to the console
	freopen_s(&fp, "CONOUT$", "w", stderr); // Redirect stderr to the console
	freopen_s(&fp, "CONIN$", "r", stdin);  // Redirect stdin to the console

	printf("Console initialized\n");
	WNDCLASSEX wc;
	ZeroMemory(&wc, sizeof(WNDCLASSEX));

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.hInstance = GetModuleHandle(NULL);
	wc.lpfnWndProc = WindowProcedure;
	wc.lpszClassName = WINDOW_TITLE;
	wc.style = CS_HREDRAW | CS_VREDRAW;

	if (!RegisterClassEx(&wc)) return false;

	HWND hWnd = CreateWindow(WINDOW_TITLE, WINDOW_TITLE, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, windowSize, windowSize,
		NULL, NULL, wc.hInstance, NULL);

	//--------------------------------
	//	Initialize window for OpenGL
	//--------------------------------

	HDC hdc = GetDC(hWnd);

	//	initialize pixel format for the window
	initPixelFormat(hdc);

	//	get an openGL context
	HGLRC hglrc = wglCreateContext(hdc);

	//	make context current
	if (!wglMakeCurrent(hdc, hglrc)) return false;

	//--------------------------------
	//	End initialization
	//--------------------------------

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(70.0, 1.0, 0.6, 21.0);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);
	if (projectLight)glEnable(GL_LIGHTING);

	if (projectTexture) glEnable(GL_TEXTURE_2D);
	LoadAllTextures();
	ShowWindow(hWnd, nCmdShow);

	textures = { weatheredMetal, plastic };

	selectedTexture = textures[textureIndex];


	MSG msg;
	ZeroMemory(&msg, sizeof(msg));

	while (true)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT) break;

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		//printf("camx = %f, camy = %f, camz = %f\n", cam.x, cam.y, cam.z);
		//printf("%d %d %d\n", kick, kick2, kick3	);


		display();

		SwapBuffers(hdc);
	}

	fclose(fp);
	FreeConsole();

	UnregisterClass(WINDOW_TITLE, wc.hInstance);

	return true;
}
//--------------------------------------------------------------------