# MyRenderer


本人的构建方法，使用MSVC，并在根目录下构建，会出现本人不明白的错误（多线程库）
1. 移动到MyRender/MyRender目录下
2. 在该目录下使用 Cmake 调用 GCC 进行构建
3. 复制asset文件夹，tool文件夹到 MyRender/MyRender 目录下 exe 能读取obj、纹理等

**注：模型obj和纹理贴图以及天空盒hdr需要自己下载并放置在asset目录下，以及tool文件夹中的cmgen.exe**