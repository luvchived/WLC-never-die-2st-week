EXE = WLCneverdie
SOURCES = main.cpp imgui.cpp imgui_draw.cpp imgui_tables.cpp imgui_widgets.cpp imgui_impl_glfw.cpp imgui_impl_opengl3.cpp
CXX = c++
CXXFLAGS = -std=c++11 -I. -I/opt/homebrew/include
LIBS = -L/opt/homebrew/lib -lglfw -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo

all:
	$(CXX) $(CXXFLAGS) $(LIBS) $(SOURCES) -o $(EXE)
	@echo "빌드 완료! ./$(EXE) 를 실행하세요."

clean:
	rm -f $(EXE)
