#pragma once 

#include <../core/math.h>
#include <../core/window.h>
#include <vector>

class Movement
{
public:
	Movement() = default;
	~Movement() = default;
	virtual Mat4x4f GetMovementMatrix(float deltaTime) const = 0 ;
};

class CircleMovement : public Movement
{
public: 
	CircleMovement(float angelSpeed, float direction);
	Mat4x4f GetMovementMatrix(float deltaTime)const override;
private:
	float _angelSpeed;
	float _direction;
};
class Motion
{
public:
	Motion(Mat4x4f originModelMatrix, Window* window);
	~Motion();
	Mat4x4f GetModelMatrix();
	Movement * Movements[2];
	int MovementsSize;
	Mat4x4f modelMatrix;
private :
	
	Window* _window;
	float _time;
	
};

