#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_opengl3.h"
#include "imgui/backends/imgui_impl_glfw.h"
#include "imgui/imfilebrowser.h"
#include <iostream>
#include <filesystem>
#include "Windows.h"
#include <tchar.h>
#include <shellapi.h>
#include <fstream>
#include <string>
#include <cstdio>
#include "main.h"
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
#endif
#include <GLFW/glfw3.h> // Will drag system OpenGL headers
#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions.lib")
#pragma comment(lib, "shell32")
#endif
// setup error callback
static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);  // NOLINT(cert-err33-c)
}

ImGui::FileBrowser initialize_file_menu()
{
    // init file browser
    // ReSharper disable once CppUseAuto
    file_dialog = ImGui::FileBrowser(ImGuiFileBrowserFlags_SelectDirectory);
    file_dialog.SetTitle("Add a new project");
    return file_dialog;
}

void initialize_table_values()
{
    line_count = 0;
    lines.clear();
    lines.shrink_to_fit();
    // FUCK IT WE'RE USING TXT
    std::ifstream file("filePaths.txt", std::ios::in);
    std::string line;
    while (std::getline(file, line))
    {
        lines.push_back(line);
        line_count++;
    }
    std::cout << "file line count: " << line_count;
    file.close();   
}

bool verify_if_file_path_exists(const std::filesystem::path& path)
{
    return std::filesystem::exists(path);
}

void add_path_to_file(const LPCSTR path)
{
    std::ofstream file("filePaths.txt", std::ios::app);
    file << path << std::endl;
    file.close();
    initialize_table_values();
}

void launch_in_explorer(const LPCSTR path)
{
    process = ShellExecuteA(nullptr, "explore", path, nullptr, nullptr, SW_SHOWNORMAL);
    if (reinterpret_cast<int>(process) <= 32)  // NOLINT(clang-diagnostic-pointer-to-int-cast)
        std::cout << "error" << std::endl;
    
}

void draw_table(const int line_count)
{
    if (ImGui::BeginTable("MainTable", 3, ImGuiTableFlags_Borders))
    {
        ImGui::TableSetupColumn("Title");
        ImGui::TableSetupColumn("Path");
        ImGui::TableSetupColumn("Type");
        ImGui::TableHeadersRow();
        for (int row = 0; row < line_count; row++)
        {
            is_selected = (selected_item == row);
            element = lines[row];
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            if (ImGui::Selectable((std::to_string(row).c_str()), is_selected, ImGuiSelectableFlags_SpanAllColumns))
                selected_item = row;

            // ImGui::Text("Path", row);
            ImGui::TableNextColumn();
            ImGui::Text(element.c_str());  // NOLINT(clang-diagnostic-format-security)
            ImGui::TableNextColumn();
            ImGui::Text("Sus");
        }
        ImGui::EndTable();
    }
}

void open_folder_button()
{
    if (ImGui::Button("Open Folder"))
    {
        std::cout << "\nindex: " << selected_item << std::endl;
        std::cout << "value: " << lines[selected_item] << std::endl;

        const std::string path = lines[selected_item];
        launch_in_explorer(path.c_str());
    } 
}

void open_project_button()
{
    if (ImGui::Button("Open Project"))
    {
        const std::string path = lines[selected_item];
        for (const auto& entry : std::filesystem::recursive_directory_iterator(path))
        {
            if (entry.is_regular_file() && entry.path().extension() == ".sln")
            {
                sln_path = entry.path().string();
                vs_cmd = vs_path + " \"" + sln_path + R"( " /command "File.OpenSolution")";
                std::cout << sln_path << std::endl;
                std::cout << vs_cmd << std::endl;
                process = ShellExecuteA(nullptr, "open", vs_path.c_str(), sln_path.c_str(), nullptr, SW_SHOWNORMAL);
                if (reinterpret_cast<int>(process) <= 32)
                {
                    std::cout << "error opening sln";
                }
            }   
        }
    }
}

void open_unity_button()
{
    if (ImGui::Button("Open in Unity"))
    {
        const std::string project_dir = lines[selected_item];
        const std::string arg = "-projectPath " + project_dir;
        std::cout << project_dir << std::endl;
        std::cout << arg << std::endl;
        process = ShellExecuteA(nullptr, "open", unity_path.c_str(), arg.c_str(), nullptr, SW_SHOWNORMAL);
        if (reinterpret_cast<int>(process) <= 32)
        {
            std::cout << "error opening unity project";
        }
    }
}

void open_unreal_button()
{
    if (ImGui::Button("Open in Unreal"))
    {
        
    }
}

int get_current_selection_index(std::vector<bool> selections)
{
    for (auto current_index = 0; current_index < static_cast<int>(selections.size()); current_index++)
        if (selections[current_index])
            return current_index;
    return -1;
}

void draw_menu_bar()
{
    // create menu bar
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Add New Project"))
            {
                file_browser.Open();
            }
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
}


void main(int, char**)
{
    // set error callback
    glfwSetErrorCallback(glfw_error_callback);
    // initialize glfw
    if (!glfwInit())
        return;
    


    
    // setup renderer and window
    glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    
    window = glfwCreateWindow(1280, 720, "Main Window", NULL, NULL);
    if (window == nullptr)
        return;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    // setup context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    // io.ConfigFlags |=  ImGuiConfigFlags_DockingEnable;
    // io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    // setup gui style
    ImGui::StyleColorsDark();
    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding = 0.0f;
    style.Colors[ImGuiCol_WindowBg].w = 1.0f;

    // setup renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // tell what windows to display
    bShowMainWindow = true;

    // make window maximised
    viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
    ImGui::SetNextWindowViewport(viewport->ID);

    // init file browser
    file_browser = initialize_file_menu();



    initialize_table_values();

    // MAIN PROGRAM
    while (!glfwWindowShouldClose(window))
    {
        // pass through inputs
        glfwPollEvents();
        // setup imgui
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        if (bShowMainWindow)
        {

            draw_menu_bar();
            // create main window 
            ImGui::Begin("Main Window");
            
            draw_table(line_count);
            open_folder_button();
            open_project_button();
            open_unity_button();
            open_unreal_button();
            
            ImGui::End();
            file_browser.Display();

            if (file_browser.HasSelected())
            {
                selected_path = file_browser.GetSelected().string();
                if (!verify_if_file_path_exists(selected_path))
                    return;
                add_path_to_file(selected_path.c_str());
                file_browser.ClearSelected();
            }
        }
        // OUTSIDE MAIN WINDOW

        // rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            GLFWwindow* backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }
        glfwSwapBuffers(window);

        
    }

    // cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

}
