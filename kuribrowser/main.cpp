#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_opengl3.h"
#include "imgui/backends/imgui_impl_glfw.h"
#include "imgui/imfilebrowser.h"
#include "rapidjson/filereadstream.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cstdio>
#define GL_SILENCE_DEPRECATION
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
#endif
#include <GLFW/glfw3.h> // Will drag system OpenGL headers

#include "rapidjson/document.h"
#include "rapidjson/istreamwrapper.h"
#ifndef RAPIDJSON_DEFAULT_ALLOCATOR
#define RAPIDJSON_DEFAULT_ALLOCATOR ::RAPIDJSON_NAMESPACE::MemoryPoolAllocator< ::RAPIDJSON_NAMESPACE::CrtAllocator >
#endif
#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions.lib")
#endif
// setup error callback
static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}


ImGui::FileBrowser InitializeFileMenu()
{
    // init file browser
    ImGui::FileBrowser fileDialog = ImGui::FileBrowser(ImGuiFileBrowserFlags_SelectDirectory);
    fileDialog.SetTitle("Add a new project");
    return fileDialog;
}

void FileSelected()
{
    
}


static void ShowFileMenu(ImGui::FileBrowser uiFileBrowser)
{

}

int get_current_selection_index(bool *selections)
{
    for (auto current_index = 0; current_index < sizeof(selections); current_index++)
        if (selections[current_index])
            return current_index;
    return -1;
}


void main(int, char**)
{
    // set error callback
    glfwSetErrorCallback(glfw_error_callback);
    // initialize glfw
    if (!glfwInit())
        return;
    


    
    // setup renderer and window
    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    
    GLFWwindow* window = glfwCreateWindow(1280, 720, "Main Window", NULL, NULL);
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
    bool bShowMainWindow = true;

    // make window maximised
    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
    ImGui::SetNextWindowViewport(viewport->ID);

    // init file browser
    auto file_browser = InitializeFileMenu();



    // FUCK IT WE'RE USING TXT
    std::ifstream file("filePaths.txt", std::ios::in);
    std::vector<std::string> lines;
    std::string line;
    auto line_count = 0;
    while (std::getline(file, line))
    {
        lines.push_back(line);
        line_count++;
    }
    std::cout << "file line count: " << line_count;

    //
    // std::ofstream newFile("newFile2.txt");
    // std::cout << "Among\n Us";
    // init local projects

    // JSON PARSING
    // std::ifstream file("filePaths.json");
    // std::ostringstream tmp;
    // tmp << file.rdbuf();
    // std::string s = tmp.str();
    //
    // rapidjson::Document d;
    // d.Parse<0>(s.c_str());
    //
    // std::cout << d["Test"].GetType();


    //FILE* fp = fopen("filePaths.json", "rb");
    //char read_buffer[65536];
    //rapidjson::FileReadStream is(fp, read_buffer, sizeof(read_buffer));
    //rapidjson::Document d;
    //d.ParseStream(is);
    //if (d.HasParseError())
    //{
    //    std::cout << d.GetParseError();
    //    
    //}
    //fclose(fp);
    // std::ifstream file_stream("filePaths.json");
    // rapidjson::IStreamWrapper isw(file_stream);
    // rapidjson::Document d;
    // d.ParseStream(file_stream);
    // if (d.HasParseError())
    // {
    //     std::cout << d.GetParseError();
    // }
    // std::cout << d["Test"].GetString();
    // for (const auto& point : d["ProjectPaths"].GetArray())
    // {
    //     std::cout << "hello";
    // }
    
    // std::cout << std::string_view(doc["ProjectPaths"]) << std::endl;
    // for (simdjson::ondemand::object path : foundPaths)
    // {
    //     std::cout << "File Path: " << simdjson::to_json_string(path) << std::endl;
    // }
    
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
            
            // create main window 
            ImGui::Begin("Main Window");
            if (ImGui::BeginTable("MainTable", 3, ImGuiTableFlags_Borders))
            {

                static bool selected[4] = {};
                ImGui::TableSetupColumn("Title");
                ImGui::TableSetupColumn("Path");
                ImGui::TableSetupColumn("Type");
                ImGui::TableHeadersRow();
                for (int row = 0; row < line_count; row++)
                {
                    std::string& element = lines[row];
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    if (ImGui::Selectable((std::to_string(row).c_str()), &selected[row], ImGuiSelectableFlags_SpanAllColumns))
                    {
                        for (auto &field : selected)
                            field = false;
                        
                        selected[row] = true;
                    }

                    // ImGui::Text("Path", row);
                    ImGui::TableNextColumn();
                    ImGui::Text(element.c_str());
                    ImGui::TableNextColumn();
                    ImGui::Text("Sus");
                }

                ImGui::EndTable();
                if (ImGui::Button("Open"))
                    std::cout << lines[get_current_selection_index(selected)] << std::endl;
            }
            
            // auto selected = file_browser.GetSelected().string();
            // char arr[800];
            // strcpy(arr, selected.c_str());
            // ImGui::Text(arr);
            //
            ImGui::End();
            file_browser.Display();

            // if(file_browser.HasSelected())
            // {
            //     selected = file_browser.GetSelected().string();
            // }
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
