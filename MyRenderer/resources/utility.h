﻿#pragma once

#include <tchar.h>   
#include <iostream>

#include <string>
#include <fstream>
#include <io.h>

#include "../core/window.h"
#pragma comment(lib, "Setupapi.lib")

using std::string;
using std::ifstream;

#pragma region 文件操作

//获取文件后缀 带'.' 例如'.txt'
inline std::string GetFileExtension(const std::string& file_name)
{
    //file_name.substr(k)：从找到的k位置开始，提取子字符串，这个子字符串就是文件的后缀
    string suffix_str = file_name.substr(file_name.find_last_of('.'));
    return suffix_str;
}
// 获取文件的名称，没有后缀和路径
inline std::string GetFileNameWithoutExtension(const std::string& file_name)
{
    const std::string::size_type i_pos = file_name.find_last_of('/') + 1; //去掉路径 找到最后一个/的位置+1,即文件名的起始位置
    std::string file_name_with_extension = file_name.substr(i_pos, file_name.length() - i_pos);//提取文件名,带后缀
    std::string suffix_str = file_name_with_extension.substr(0, file_name_with_extension.rfind("."));;//去掉后缀 从右往左rfind找到第一个'.'
    return  suffix_str;
}
// 获取文件所在的文件夹，结尾没有"/"
inline std::string GetFileFolder(const std::string& file_name)
{
    const std::string::size_type i_pos = file_name.find_last_of('/');//找到最后一个'/'的位置
    std::string file_folder = file_name.substr(0, i_pos);
    return  file_folder;
}

// 在指定文件夹中检索名字中包含file_name的文件，并返回完整路径
inline std::string GetFilePathByFileName(const std::string& file_folder, const std::string& file_name)
{
    //文件句柄    
    long long h_file;
    //文件信息    
    struct _finddata_t file_info;

    std::string file_full_path;

    std::string p;
    if ((h_file = _findfirst(p.assign(file_folder).append("/*").c_str(), &file_info)) != -1)
    {
        do
        {
            std::string temp_file_name = file_info.name;
            if (temp_file_name.find(file_name) != std::string::npos)
            {
                std::string file_full_path = (p.assign(file_folder).append("/").append(file_info.name));
                return file_full_path;
            }
        } while (_findnext(h_file, &file_info) == 0);
        _findclose(h_file);
    }

    return  file_full_path;
}

// 判断文件是否存在
inline bool CheckFileExist(const std::string& file_name)
{
    std::ifstream f(file_name.c_str());
    return f.good();
}
#pragma  endregion

#pragma  region cmgen 
//cmgen用于生成天空盒和IBL等
inline void ExecuteProcess(const std::string& command_str)
{
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));
    const LPTSTR command = _tcsdup(command_str.c_str());

    if (!CreateProcess(
        nullptr,		// No module name (use command line)
        command,		// Command line
        nullptr,		// Process handle not inheritable
        nullptr,		// Thread handle not inheritable
        FALSE,          // Set handle inheritance to FALSE
        0,              // No creation flags
        nullptr,        // Use parent's environment block
        nullptr,        // Use parent's starting directory 
        &si,            // Pointer to STARTUPINFO structure
        &pi)			// Pointer to PROCESS_INFORMATION structure
        )
    {
        std::cout << "执行命令失败：" + command_str << std::endl;
        exit(EXIT_FAILURE);
    }

    // Wait until child process exits.
    WaitForSingleObject(pi.hProcess, INFINITE);

    // Close process and thread handles. 
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}


/// @brief 调用cmgen生成天空盒的贴图,包括irradiance和specular和lut
/// @param skybox_path 
inline void GenerateCubeMap(std::string skybox_path)
{

    std::string cmgen_path = "../tools/cmgen.exe";
    std::string output_path = GetFileFolder(skybox_path);
    std::string skybox_file_name = GetFileNameWithoutExtension(skybox_path);

    //三条命令
    std::string irradiance_command = cmgen_path + " --format=hdr --ibl-irradiance=" + output_path + " " + skybox_path;
    std::string specular_command = cmgen_path + " --format=hdr --size=512 --ibl-ld=" + output_path + " " + skybox_path;
    std::string lut_command = cmgen_path + " --ibl-dfg=" + output_path + "/" + skybox_file_name + "/brdf_lut.hdr";

    ExecuteProcess(irradiance_command);
    ExecuteProcess(specular_command);
    ExecuteProcess(lut_command);

}
#pragma  endregion
