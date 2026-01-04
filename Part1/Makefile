workspaceFolder = .

# Detect OS
ifeq ($(OS),Windows_NT) # Windows
    CPPFLAGS = g++ --std=c++17 -fdiagnostics-color=always -Wall -g
    CFLAGS = gcc -std=c11 -Wall -g
    LDFLAGS = 
    all: find_glm copy_res_w build
else
    UNAME_S := $(shell uname -s)
    ifeq ($(UNAME_S), Darwin) # macOS
        CPPFLAGS = clang++ -std=c++17 -fcolor-diagnostics -fansi-escape-codes -Wall -g
        CFLAGS = clang -std=c11 -Wall -g
        LDFLAGS = 
        all: find_glm copy_res_m build
    else ifeq ($(UNAME_S), Linux) # Linux
        CPPFLAGS = g++ --std=c++17 -fdiagnostics-color=always -Wall -g
        CFLAGS = gcc -std=c11 -Wall -g
        LDFLAGS = 
        all: find_glm copy_res_l build
    else
        $(error Unsupported OS: $(UNAME_S))
    endif
endif

# GLM is in local include directory
GLM_PATH := include

# Add include paths
ifneq ($(GLM_PATH),)
    CPPFLAGS += -I${workspaceFolder}/src -I${GLM_PATH}
    CFLAGS += -I${workspaceFolder}/src -I${GLM_PATH}
else
    CPPFLAGS += -I${workspaceFolder}/src
    CFLAGS += -I${workspaceFolder}/src
endif

# Source files for raytracer (only the new ones we need)
RAYTRACER_SRC = src/main.cpp \
                src/Primitive.cpp \
                src/Sphere.cpp \
                src/Plane.cpp \
                src/RayCast.cpp \
                src/HitResult.cpp \
                src/GlobalLight.cpp \
                src/ParallelLight.cpp \
                src/ConeLight.cpp \
                src/stb_image_write.cpp

RAYTRACER_OBJ = $(patsubst src/%.cpp, ${workspaceFolder}/bin/%.o, $(RAYTRACER_SRC))

# Rule to compile .o files from .cpp files
${workspaceFolder}/bin/%.o: ${workspaceFolder}/src/%.cpp | ${workspaceFolder}/bin
	$(CPPFLAGS) -c $< -o $@

# Create bin directory if it doesn't exist
${workspaceFolder}/bin:
	@mkdir -p ${workspaceFolder}/bin

# Find GLM helper target
find_glm:
ifeq ($(OS),Windows_NT)
	@echo Checking for GLM...
	@if exist "$(GLM_PATH)\glm\glm.hpp" ( \
		echo Found GLM at: $(GLM_PATH) \
	) else ( \
		echo Warning: GLM not found at $(GLM_PATH)\glm \
	)
else
	@echo Checking for GLM...
	@if [ -d "$(GLM_PATH)/glm" ]; then \
		echo Found GLM at: $(GLM_PATH); \
	else \
		echo Warning: GLM not found at $(GLM_PATH)/glm; \
	fi
endif

# Build target
build: $(RAYTRACER_OBJ) | ${workspaceFolder}/bin
	@echo "Linking raytracer..."
	$(CPPFLAGS) $(RAYTRACER_OBJ) -o ${workspaceFolder}/bin/raytracer.exe $(LDFLAGS)
	@echo "Build complete! Executable: bin/raytracer.exe"

# Copy resources (MacOS/Linux)
copy_res_m:
	@echo "Copying resources for MacOS..."
	@mkdir -p ${workspaceFolder}/bin/res && cp -rf ${workspaceFolder}/src/res/* ${workspaceFolder}/bin/res 2>/dev/null || true

copy_res_l:
	@echo "Copying resources for Linux..."
	@mkdir -p ${workspaceFolder}/bin/res && cp -rf ${workspaceFolder}/src/res/* ${workspaceFolder}/bin/res 2>/dev/null || true

# Copy resources (Windows)
copy_res_w:
	@echo "Copying resources for Windows..."
	@if not exist ${workspaceFolder}\bin\res mkdir ${workspaceFolder}\bin\res
	@xcopy /E /I /Y ${workspaceFolder}\src\res ${workspaceFolder}\bin\res >nul 2>&1 || echo Resources copied

# Clean target
clean:
	@echo Cleaning build files...
ifeq ($(OS),Windows_NT)
	@if exist ${workspaceFolder}\bin\*.o del /Q ${workspaceFolder}\bin\*.o
	@if exist ${workspaceFolder}\bin\raytracer.exe del /Q ${workspaceFolder}\bin\raytracer.exe
else
	@rm -f ${workspaceFolder}/bin/*.o
	@rm -f ${workspaceFolder}/bin/raytracer.exe
	@rm -f ${workspaceFolder}/bin/raytracer
endif

# Test target - run the raytracer
test: build
	@echo "Running raytracer..."
	@cd ${workspaceFolder}/bin && ./raytracer.exe || cd ${workspaceFolder}/bin && raytracer.exe

.PHONY: all find_glm copy_res_m copy_res_w copy_res_l build clean test

