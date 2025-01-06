#include "Motion.h"

Motion::Motion(Mat4x4f originModelMatrix, Window* window) : modelMatrix(originModelMatrix), _window(window)
{
	_time = _window->PlatformGetTime();
	MovementsSize = 0;
}
Motion::~Motion()
{
	for(int i=0;i<2;i++){
		delete Movements[i];
	}
	delete Movements;
}
Mat4x4f Motion::GetModelMatrix()
{
	float curTime = _window->PlatformGetTime();
	float deltaTime = curTime - _time;
	_time = curTime;

	modelMatrix = Movements[0]->GetMovementMatrix(deltaTime) * modelMatrix;
	return modelMatrix;

}
CircleMovement::CircleMovement(float angelSpeed, float direction):Movement(), _angelSpeed(angelSpeed), _direction(direction)
{
}
Mat4x4f CircleMovement::GetMovementMatrix(float deltaTime) const
{
	float theta =  _angelSpeed * deltaTime;
	Mat4x4f matrix = matrix_set_rotate(0.0f, 1.0f, 0.0f, theta);
	return matrix;
}
