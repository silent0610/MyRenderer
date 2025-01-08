# CMakeList.txt: MyRenderer 的 CMake 项目，在此处包括源代码并定义
# 项目特定的逻辑。
#

# 将源代码添加到此项目的可执行文件。
include_directories("lib")
add_executable (MyRenderer "function/renderer.cpp" "function/renderer.h" "core/math.h"  "core/vector.h" "core/matrix.h" "resources/utility.h" "resources/texture.h" "resources/model.h" "resources/texture.cpp" "core/window.cpp" "resources/model.cpp" "function/shader.h" "function/shader.cpp" "main.cpp" "function/scene.h" "function/scene.cpp" "function/camera.h" "function/camera.cpp" "function/light.h" "function/light.cpp" "function/shadow.h" "function/Motion.cpp" "core/BS_thread_pool.hpp")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET MyRenderer PROPERTY CXX_STANDARD 20)
endif()

# TODO: 如有需要，请添加测试并安装目标。
