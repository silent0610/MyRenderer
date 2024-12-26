// MyRenderer.cpp: 定义应用程序的入口点。
//

#include "MyRenderer.h"
#include "core/math.h"
using namespace std;

class matrix {
public:
	int m;
};
int main()
{
	//cout << "Hello CMake." << endl;
	Mat3x3f m = {0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f };
	std::cout<<m<<Saturate(1.2f);
	return 0;

}
