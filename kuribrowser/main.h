#pragma once

// exe paths
inline std::string vs_path = "D:\\VS2022\\Common7\\IDE\\devenv.exe";
inline std::string vs_cmd;
// sln paths
inline std::string sln_path;

// glfw initialization defs
inline const char* glsl_version;
inline GLFWwindow* window;

// imgui initialization defs
inline ImGuiViewport* viewport;
inline bool bShowMainWindow;

// table values
inline int line_count = 0;

// table selection
inline std::string element;
inline bool is_selected;
inline static int selected_item = -1;
inline std::vector<std::string> lines;

// file browser
inline ImGui::FileBrowser file_dialog;
inline ImGui::FileBrowser file_browser;
inline std::string selected_path;
// process management
inline HINSTANCE process;