#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <stdio.h>

#if defined(IMGUI_IMPL_OPENGL_LOADER_GL3W)
#include <GL/gl3w.h>
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLEW)
#include <GL/glew.h>
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD)
#include <glad/glad.h>
#endif

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <GLFW/glfw3.h>


enum GameStep {
    STEP_INTRO = 0,
    STEP_NAME_INPUT,
    STEP_MENT1,
    STEP_CHOICE1,
    STEP_MENT2,
    STEP_MENT3,
    STEP_MENT4,
    STEP_MENT5,
    STEP_CHOICE2,
    STEP_MENT6,
    STEP_OUT,
    STEP_OUT2
};

bool LoadTextureFromFile(const char* filename, GLuint* out_texture, int* out_width, int* out_height) {
    int image_width = 0;
    int image_height = 0;
    unsigned char* image_data = stbi_load(filename, &image_width, &image_height, NULL, 4);
    if (image_data == NULL) return false;

    GLuint image_texture;
    glGenTextures(1, &image_texture);
    glBindTexture(GL_TEXTURE_2D, image_texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
    stbi_image_free(image_data);

    *out_texture = image_texture;
    *out_width = image_width;
    *out_height = image_height;
    return true;
}

static void glfw_error_callback(int error, const char* description) {
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

int main(int, char**) {
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit()) return 1;

#if defined(__APPLE__)
    const char* glsl_version = "#version 150";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#else
    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
#endif

    GLFWwindow* window = glfwCreateWindow(1280, 800, u8"글죽않", nullptr, nullptr);
    if (window == nullptr) return 1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

#if defined(IMGUI_IMPL_OPENGL_LOADER_GLAD)
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) return 1;
#endif

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);
    ImVector<ImWchar> ranges;
    ImFontGlyphRangesBuilder builder;
    builder.AddRanges(io.Fonts->GetGlyphRangesKorean());
    builder.AddRanges(io.Fonts->GetGlyphRangesJapanese());
    builder.BuildRanges(&ranges);

    ImFont* font_normal = io.Fonts->AddFontFromFileTTF("malgun.ttf", 22.0f, NULL, ranges.Data);
    ImFont* font_title = io.Fonts->AddFontFromFileTTF("malgun.ttf", 60.0f, NULL, ranges.Data);
    if (!font_normal) font_normal = io.Fonts->AddFontDefault();

    GLuint tex_bg_intro = 0, tex_bg_hallway = 0, tex_bg_classroom = 0, tex_bg_dongbang = 0, tex_bg_dobiisfree = 0;
    GLuint tex_char_misa = 0, tex_char_sinigami = 0, tex_char_dobi = 0;
    int my_w, my_h;

    LoadTextureFromFile("images/intro.jpeg", &tex_bg_intro, &my_w, &my_h);
    LoadTextureFromFile("images/hallway.jpeg", &tex_bg_hallway, &my_w, &my_h);
    LoadTextureFromFile("images/classroom.jpeg", &tex_bg_classroom, &my_w, &my_h);
    LoadTextureFromFile("images/dongbang.jpg", &tex_bg_dongbang, &my_w, &my_h);
    LoadTextureFromFile("images/dobiisfree.jpg", &tex_bg_dobiisfree, &my_w, &my_h);
    LoadTextureFromFile("images/misa.png", &tex_char_misa, &my_w, &my_h);
    LoadTextureFromFile("images/sinigami.png", &tex_char_sinigami, &my_w, &my_h);
    LoadTextureFromFile("images/dobi.png", &tex_char_dobi, &my_w, &my_h);

    int current_step = STEP_INTRO;
    double last_change_time = glfwGetTime();
    static char playerName[128] = "Student";

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        GLuint current_bg_texture = 0;
        if (current_step <= STEP_NAME_INPUT) current_bg_texture = tex_bg_intro;
        else if (current_step == STEP_OUT || current_step == STEP_OUT2) current_bg_texture = tex_bg_hallway;
        else if (current_step == STEP_MENT6) current_bg_texture = tex_bg_dobiisfree;
        else if (current_step >= STEP_MENT1 && current_step <= STEP_MENT2) current_bg_texture = tex_bg_classroom;
        else current_bg_texture = tex_bg_dongbang;

        if (current_bg_texture != 0) {
            ImGui::GetBackgroundDrawList()->AddImage((void*)(intptr_t)current_bg_texture, ImVec2(0, 0), io.DisplaySize);
        }

    
        if (current_step == STEP_OUT || current_step == STEP_OUT2) {
            if (tex_char_sinigami != 0) {
                ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x - 450, io.DisplaySize.y - 700));
                ImGui::Begin("Shinigami", NULL, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground);
                ImGui::Image((void*)(intptr_t)tex_char_sinigami, ImVec2(400, 600));
                ImGui::End();
            }
        } else if (current_step == STEP_CHOICE1 || current_step == STEP_MENT2 || current_step == STEP_MENT4 || current_step == STEP_CHOICE2) {
            if (tex_char_misa != 0) {
                ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x - 450, io.DisplaySize.y - 700));
                ImGui::Begin("Misa", NULL, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground);
                ImGui::Image((void*)(intptr_t)tex_char_misa, ImVec2(400, 600));
                ImGui::End();
            }
        } else if (current_step == STEP_MENT3 || current_step == STEP_MENT5) {
            if (tex_char_dobi != 0) {
                ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x - 450, io.DisplaySize.y - 700));
                ImGui::Begin("Dobi", NULL, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground);
                ImGui::Image((void*)(intptr_t)tex_char_dobi, ImVec2(400, 600));
                ImGui::End();
            }
        }


        if (current_step == STEP_NAME_INPUT) {
            ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x * 0.5f - 200, io.DisplaySize.y * 0.5f - 100));
            ImGui::SetNextWindowSize(ImVec2(400, 200));
        } else if (current_step != STEP_CHOICE2) {
            ImGui::SetNextWindowPos(ImVec2(50, io.DisplaySize.y - 250));
            ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x - 100, 200));
        }

        if (current_step != STEP_MENT6 || current_step == STEP_CHOICE2) { 
            ImGui::Begin("DialogueBox", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
            
            switch (current_step) {
                case STEP_INTRO:
                    if (font_title) ImGui::PushFont(font_title);
                    ImGui::Text(u8"<< ドキドキ文学部は死なない >>");
                    if (font_title) ImGui::PopFont();
                    ImGui::Dummy(ImVec2(0, 20));
                    ImGui::TextColored(ImVec4(1, 1, 0, 1), "Loading... %.1f sec", ImGui::GetTime() - last_change_time);
                    if (ImGui::GetTime() - last_change_time > 3.0) current_step = STEP_NAME_INPUT;
                    break;

                case STEP_NAME_INPUT:
                    ImGui::Text("Please enter your name:");
                    ImGui::InputText("##NameInput", playerName, IM_ARRAYSIZE(playerName));
                    if (ImGui::Button("Start Game", ImVec2(150, 40)) || ImGui::IsKeyPressed(ImGuiKey_Enter)) current_step = STEP_MENT1;
                    break;

                case STEP_MENT1:
                    ImGui::TextWrapped(u8"%s: 이곳이 내가 다니게 될 학교? 근데...저 미녀는 누구지?", playerName);
                    ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "\n(Press ENTER to continue)");
                    if (ImGui::IsKeyPressed(ImGuiKey_Enter)) current_step = STEP_CHOICE1;
                    break;

                case STEP_CHOICE1:
                    ImGui::TextWrapped(u8"ミサ: 안녕? 신입생이야? 혹시 우리 글쓰기 동아리에 들어오지 않을래?");
                    if (ImGui::Button(u8"A. 좋아요!")) current_step = STEP_MENT2;
                    if (ImGui::Button(u8"B. 도망친다")) { current_step = STEP_OUT; last_change_time = ImGui::GetTime(); }
                    break;

                case STEP_MENT2:
                    ImGui::TextWrapped(u8"ミサ: 잘됐다! 마침 오늘이 합평하는 날이니까 구경시켜줄게! 동방으로 따라와~");
                    ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "\n(Press ENTER to continue)");
                    if (ImGui::IsKeyPressed(ImGuiKey_Enter)) current_step = STEP_MENT3;
                    break;

                case STEP_MENT3: {
                    static double delay = 0.0;
                    static bool show_next = false;
                    if (delay == 0.0) delay = ImGui::GetTime();
                    if (!show_next) {
                        ImGui::TextWrapped(u8"%s: 음...저 교수님의 노예같이 생긴 사람은 누구지?\n\nwait.....", playerName);
                        if (ImGui::GetTime() - delay > 2.0) show_next = true;
                    } else {
                        ImGui::TextWrapped(u8"Dobi: 안녕? 신입생이니?");
                        ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "\n(Press ENTER to continue)");
                        if (ImGui::IsKeyPressed(ImGuiKey_Enter)) { current_step = STEP_MENT4; delay = 0.0; show_next = false; }
                    }
                    break;
                }

                case STEP_MENT4: {
                    static double delay = 0.0;
                    static bool show_next = false;
                    if (delay == 0.0) delay = ImGui::GetTime();
                    if (!show_next) {
                        ImGui::TextWrapped(u8"ミサ: 내가 신입부원 데려왔어. 나현아 오늘 글은 잘 써왔어?\n\nwait.....");
                        if (ImGui::GetTime() - delay > 2.0) show_next = true;
                    } else {
                        ImGui::TextWrapped(u8"ミサ: 뭐? 또 안썼다고? 너 지난주에도 바쁘다고 안왔잖아!\n       너 퇴출시켜버린다.");
                        ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "(Press ENTER to continue)");
                        if (ImGui::IsKeyPressed(ImGuiKey_Enter)) { current_step = STEP_MENT5; delay = 0.0; show_next = false; }
                    }
                    break;
                }

                case STEP_MENT5:
                    ImGui::TextWrapped(u8"Dobi: 아니야. 썼어. 내가 제출할 글은... 이걸 만드는데 쓴 코드야.\n        너는 영어, 한글만 언어라고 생각해? 나는 c언어도, JAVA, c++...언어라고 생각해.\n        기술혁명 시기에 걸맞는 글쓰기 아니겠어?");
                    ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "\n\n(Press ENTER to continue)");
                    if (ImGui::IsKeyPressed(ImGuiKey_Enter)) current_step = STEP_CHOICE2;
                    break;

                case STEP_CHOICE2:
                    ImGui::TextWrapped(u8"ミサ: 뭐라는거야... %s, 얘 퇴출시켜버릴까?", playerName);
                    if (ImGui::Button(u8"A. 봐주자")) current_step = STEP_MENT6;
                    if (ImGui::Button(u8"B. 퇴출시켜!")) { current_step = STEP_OUT2; last_change_time = ImGui::GetTime(); }
                    break;

                case STEP_MENT6:
                    if (ImGui::Button("Restart")) { current_step = STEP_INTRO; last_change_time = ImGui::GetTime(); }
                    break;

                case STEP_OUT:
                    ImGui::TextWrapped(u8"リューク: 진짜로? 다시 생각해봐.");
                    if (ImGui::Button("Restart")) { current_step = STEP_CHOICE1; last_change_time = ImGui::GetTime(); }
                    break;

                case STEP_OUT2:
                    ImGui::TextWrapped(u8"リューク: 진짜로? 다시 생각해봐.");
                    if (ImGui::Button("Restart")) { current_step = STEP_CHOICE2; last_change_time = ImGui::GetTime(); }
                    break;
            }
            ImGui::End();
        }


        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}