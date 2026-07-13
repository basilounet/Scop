##========== SOURCES ==========##

SRC = 	$(GLAD_CPP) \
		$(IMGUI_SRCS) \
		main.cpp \
		utils.cpp \
		Scop.cpp \
		Shader.cpp \
		Skybox.cpp \
		VBO.cpp \
		EBO.cpp \
		VAO.cpp \
		stb.cpp \
		Texture.cpp \
		Camera.cpp \
		Mesh.cpp \
		Object.cpp \
		math/Vec2.cpp \
		math/Vec3.cpp \
		math/Vec4.cpp \
		math/Mat4.cpp \
		math/Spline.cpp \

##========== NAMES ==========##

NAME = scop
SRCS_DIR = src/
OBJS_DIR = obj/
INCLUDE_DIR = includes/
LIBRARIES_DIR = libs/
GLFW = $(LIBRARIES_DIR)glfw/
IMGUI = $(LIBRARIES_DIR)imgui/

GLAD_CPP = glad.cpp # if in a subdirectory, update this
IMGUI_SRCS =    ../$(IMGUI)imgui.cpp \
                ../$(IMGUI)imgui_draw.cpp \
                ../$(IMGUI)imgui_widgets.cpp \
                ../$(IMGUI)imgui_tables.cpp \
                ../$(IMGUI)imgui_demo.cpp \
                ../$(IMGUI)backends/imgui_impl_opengl3.cpp \
                ../$(IMGUI)backends/imgui_impl_glfw.cpp \
                ../$(IMGUI)misc/cpp/imgui_stdlib.cpp

##========== OBJECTS ==========##

OBJS = $(addprefix $(OBJS_DIR),$(SRC:.cpp=.o))
DEPS = $(addprefix $(OBJS_DIR), $(SRC:.cpp=.d))

##========== COLORS ==========##

BASE_COLOR 	=		\033[0;39m
BLACK		=		\033[30m
GRAY 		=		\033[0;90m
DARK_GRAY	=		\033[37m
RED 		=		\033[0;91m
DARK_GREEN	=		\033[32m
DARK_RED	=		\033[31m
GREEN 		=		\033[0;92m
ORANGE 		=		\033[0;93m
DARK_YELLOW	=		\033[33m
BLUE 		=		\033[0;94m
DARK_BLUE	=		\033[34m
PRP		=		\033[0;95m
DARK_MAGENTA=		\033[35m
CYAN 		=		\033[0;96m
WHITE		=		\033[0;97m

##========== COMPILATOR ==========##

CXX = c++

##========== FLAGS ==========##

CXXFLAGS = -Wall -Wextra -Werror -MP -MMD -std=gnu++2b
LDFLAGS = $(LIBS)
LIBS = -I$(GLFW)include -I$(LIBRARIES_DIR) -I$(IMGUI)

##========== MODES ==========##

TIMER = 0.0
IS_PRINT = 1

ifdef DEBUG
    CXXFLAGS += -g
	LDFLAGS += -D DEBUG=42
	DEBUG_MODE = 1
endif

ifdef FAST
	J4 = -j$(nproc)
endif

ifdef FSANITIZE
	LDFLAGS += -fsanitize=address
	FSANITIZE_MODE = 1
endif

##========== ANIMATIONS ==========##

START_TIME := $(shell date +%s)
NUM_SRC = $(words $(SRC))
INDEX = 0
NUMBER_OF_ANIMATION = 16
ifndef ANIMATION_RATE
	ANIMATION_RATE = 1
endif

##========== COMPILATION ==========##

all: $(NAME)

glfw:
	@if [ ! -d "$(GLFW)" ]; then \
		echo "$(DARK_GRAY)Directory $(GLFW) does not exist. Cloning the repository...$(BASE_COLOR)"; \
        git clone https://github.com/glfw/glfw.git $(GLFW); \
	fi
	@if [ ! -d "$(GLFW)build/" ]; then \
		cmake $(GLFW) -B $(GLFW)/build; \
		make -C $(GLFW)/build --no-print-directory -j$(nproc) ;\
	fi

glad:
	@if ls $(SRCS_DIR)$(dir $(GLAD_CPP)) | grep -q "glad"; then \
  		echo "$(GREEN)glad Found, no need to pull$(BASE_COLOR)"; \
  	else \
		cd $(LIBRARIES_DIR) && git clone https://github.com/Dav1dde/glad.git glad_lib; \
		python -m glad --out-path=glad_lib/build --generator=c; cd ..; \
		mv $(LIBRARIES_DIR)/glad_lib/build/src/glad.c $(SRCS_DIR)glad.cpp ; \
		mkdir -p $(LIBRARIES_DIR)/glad/ && mv $(LIBRARIES_DIR)/glad_lib/build/include/glad/glad.h $(LIBRARIES_DIR)glad/ ; \
		rm -rf $(LIBRARIES_DIR)/glad_lib ; \
	fi

imgui:
	@if [ ! -d "$(IMGUI)" ]; then \
	    echo "$(DARK_PINK)Directory $(IMGUI) does not exist. Cloning the repository...$(BASE_COLOR)"; \
	    git clone https://github.com/ocornut/imgui.git $(IMGUI); \
	else \
	    echo "$(GREEN)ImGui Found, no need to pull$(BASE_COLOR)"; \
	fi

$(NAME) : $(OBJS)
	@echo ""
	@$(CXX) -o $(NAME) $(CXXFLAGS) $(OBJS) $(LDFLAGS) -I$(INCLUDE_DIR) $(GLFW)build/src/libglfw3.a -flto
	@echo "$(PRP)Time elapsed: $(shell expr $(shell date +%s) - $(START_TIME)) seconds$(BASE_COLOR)"
	@echo "$(GREEN)-= cpp compiled =-$(BASE_COLOR)"

clean:
	@rm -rf $(OBJS_DIR)

fclean: clean
	@rm -rf $(NAME)
	@echo "$(CYAN)Files cleaned$(BASE_COLOR)"

cleanall: fclean
	@rm -rf $(SRCS_DIR)$(GLAD_CPP)
	@rm -rf $(LIBRARIES_DIR)glad
	@rm -rf $(GLFW)
	@rm -rf $(IMGUI)

re: fclean all

run: all
	@./$(NAME)

$(OBJS_DIR)%.o : $(SRCS_DIR)%.cpp
ifeq ($(IS_PRINT),1)
	@sleep $(TIMER)
	@clear
	@echo "$(GREEN)-= Compiling cpp =-$(BASE_COLOR)"
	$(animations)
	$(loading)
	$(file_size_graph)
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -I$(INCLUDE_DIR) -c $< -o $@
else
	@mkdir -p $(dir $@)
	@$(CXX) $(CXXFLAGS) $(LDFLAGS) -I$(INCLUDE_DIR) -c $< -o $@
endif

define animations
	$(animation_$(shell expr $(INDEX) / $(ANIMATION_RATE) % $(NUMBER_OF_ANIMATION)))
endef

define loading
	@$(eval INDEX=$(shell expr $(INDEX) + 1))
	@echo "╔═══════════════════════════════════════════════════╗"
	@echo -n "║"
	$(loading_color)
	@echo -n "▓"
	@for i in $$(seq 1 $$(expr $(INDEX) \* 50 / $(NUM_SRC))); do \
		echo -n "▓"; \
	done
	@for i in $$(seq 1 $$(expr 50 - $(INDEX) \* 50 / $(NUM_SRC))); do \
		echo -n " "; \
	done
	@echo "$(BASE_COLOR)║" $(shell expr $(INDEX) \* 100 / $(NUM_SRC))%
	@echo "╚═══════════════════════════════════════════════════╝"
endef

define loading_color
	@if [ $$(expr $(INDEX) - 1) -lt $$(expr $(NUM_SRC) \* 1 / 4) ]; then \
		echo -n "$(DARK_RED)" ; \
	elif [ $$(expr $(INDEX) - 1) -lt $$(expr $(NUM_SRC) \* 2 / 4) ]; then \
		echo -n "$(RED)" ; \
	elif [ $$(expr $(INDEX) - 1) -lt $$(expr $(NUM_SRC) \* 3 / 4) ]; then \
		echo -n "$(ORANGE)" ; \
	else \
		echo -n "$(GREEN)" ; \
	fi
endef

define file_size_graph
	@awk -v size=$(shell stat -c %s $<) 'BEGIN { printf "[ "; for (i=0; i<int(size/1000); i++) printf "#"; printf " ] (%d KB)", size/1000 }'
	@echo -n " : $(DARK_GREEN)$$(cat $< | wc -l) lines$(BASE_COLOR)"
	@echo -n " : $(ORANGE)$(shell expr $(shell date +%s) - $(START_TIME))s$(BASE_COLOR)"
	@echo " : $(PRP)$(INDEX)$(BASE_COLOR) / $(PRP)$(NUM_SRC)$(BASE_COLOR)"
endef

define animation_0
	@echo -n "$(WHITE)"
	@echo "      ___           ___           ___           ___   "
	@echo "     /  /\\         /  /\\         /  /\\         /  /\\  "
	@echo "    /  /:/_       /  /:/        /  /::\\       /  /::\\ "
	@echo "   /  /:/ /\\     /  /:/        /  /:/\\:\\     /  /:/\\:\\"
	@echo "  /  /:/ /::\\   /  /:/  ___   /  /:/  \\:\\   /  /:/~/:/"
	@echo " /__/:/ /:/\\:\\ /__/:/  /  /\\ /__/:/ \\__\\:\\ /__/:/ /:/ "
	@echo " \\  \\:\\/:/~/:/ \\  \\:\\ /  /:/ \\  \\:\\ /  /:/ \\  \\:\\/:/  "
	@echo "  \\  \\::/ /:/   \\  \\:\\  /:/   \\  \\:\\  /:/   \\  \\::/   "
	@echo "   \\__\\/ /:/     \\  \\:\\/:/     \\  \\:\\/:/     \\  \\:\\   "
	@echo "     /__/:/       \\  \\::/       \\  \\::/       \\  \\:\\  "
	@echo "     \\__\\/         \\__\\/         \\__\\/         \\__\\/  "
	@echo -n "$(BASE_COLOR)"
endef

define animation_1
	@echo -n "$(WHITE)"
	@echo "      ___           ___           ___           ___   "
	@echo "     /  /\\         /  /\\         /  /\\         /  /\\  "
	@echo "    /  /::\\       /  /:/        /  /::\\       /  /::\\ "
	@echo "   /__/:/\\:\\     /  /:/        /  /:/\\:\\     /  /:/\\:\\"
	@echo "  _\\_ \\:\\ \\:\\   /  /:/  ___   /  /:/  \\:\\   /  /:/~/:/"
	@echo " /__/\\ \\:\\ \\:\\ /__/:/  /  /\\ /__/:/ \\__\\:\\ /__/:/ /:/ "
	@echo " \\  \\:\\ \\:\\_\\/ \\  \\:\\ /  /:/ \\  \\:\\ /  /:/ \\  \\:\\/:/  "
	@echo "  \\  \\:\\_\\:\\    \\  \\:\\  /:/   \\  \\:\\  /:/   \\  \\::/   "
	@echo "   \\  \\:\\/:/     \\  \\:\\/:/     \\  \\:\\/:/     \\  \\:\\   "
	@echo "    \\  \\::/       \\  \\::/       \\  \\::/       \\  \\:\\  "
	@echo "     \\__\\/         \\__\\/         \\__\\/         \\__\\/  "
	@echo -n "$(BASE_COLOR)"
endef

define animation_2
	@echo -n "$(WHITE)"
	@echo "      ___           ___           ___           ___   "
	@echo "     /\\  \\         /  /\\         /  /\\         /  /\\  "
	@echo "    /::\\  \\       /  /:/        /  /::\\       /  /::\\ "
	@echo "   /:/\\ \\  \\     /  /:/        /  /:/\\:\\     /  /:/\\:\\"
	@echo "  _\\:\\~\\ \\  \\   /  /:/  ___   /  /:/  \\:\\   /  /:/~/:/"
	@echo " /\\ \\:\\ \\ \\__\\ /__/:/  /  /\\ /__/:/ \\__\\:\\ /__/:/ /:/ "
	@echo " \\:\\ \\:\\ \\/__/ \\  \\:\\ /  /:/ \\  \\:\\ /  /:/ \\  \\:\\/:/  "
	@echo "  \\:\\ \\:\\__\\    \\  \\:\\  /:/   \\  \\:\\  /:/   \\  \\::/   "
	@echo "   \\:\\/:/  /     \\  \\:\\/:/     \\  \\:\\/:/     \\  \\:\\   "
	@echo "    \\::/  /       \\  \\::/       \\  \\::/       \\  \\:\\  "
	@echo "     \\/__/         \\__\\/         \\__\\/         \\__\\/  "
	@echo -n "$(BASE_COLOR)"
endef

define animation_3
	@echo -n "$(WHITE)"
	@echo "      ___           ___           ___           ___   "
	@echo "     /\\__\\         /  /\\         /  /\\         /  /\\  "
	@echo "    /:/ _/_       /  /:/        /  /::\\       /  /::\\ "
	@echo "   /:/ /\\  \\     /  /:/        /  /:/\\:\\     /  /:/\\:\\"
	@echo "  /:/ /::\\  \\   /  /:/  ___   /  /:/  \\:\\   /  /:/~/:/"
	@echo " /:/_/:/\\:\\__\\ /__/:/  /  /\\ /__/:/ \\__\\:\\ /__/:/ /:/ "
	@echo " \\:\\/:/ /:/  / \\  \\:\\ /  /:/ \\  \\:\\ /  /:/ \\  \\:\\/:/  "
	@echo "  \\::/ /:/  /   \\  \\:\\  /:/   \\  \\:\\  /:/   \\  \\::/   "
	@echo "   \\/_/:/  /     \\  \\:\\/:/     \\  \\:\\/:/     \\  \\:\\   "
	@echo "     /:/  /       \\  \\::/       \\  \\::/       \\  \\:\\  "
	@echo "     \\/__/         \\__\\/         \\__\\/         \\__\\/  "
	@echo -n "$(BASE_COLOR)"
endef

define animation_4
	@echo -n "$(WHITE)"
	@echo "      ___           ___           ___           ___   "
	@echo "     /  /\\         /  /\\         /  /\\         /  /\\  "
	@echo "    /  /:/_       /  /:/        /  /::\\       /  /::\\ "
	@echo "   /  /:/ /\\     /  /:/        /  /:/\\:\\     /  /:/\\:\\"
	@echo "  /  /:/ /::\\   /  /:/  ___   /  /:/  \\:\\   /  /:/~/:/"
	@echo " /__/:/ /:/\\:\\ /__/:/  /  /\\ /__/:/ \\__\\:\\ /__/:/ /:/ "
	@echo " \\  \\:\\/:/~/:/ \\  \\:\\ /  /:/ \\  \\:\\ /  /:/ \\  \\:\\/:/  "
	@echo "  \\  \\::/ /:/   \\  \\:\\  /:/   \\  \\:\\  /:/   \\  \\::/   "
	@echo "   \\__\\/ /:/     \\  \\:\\/:/     \\  \\:\\/:/     \\  \\:\\   "
	@echo "     /__/:/       \\  \\::/       \\  \\::/       \\  \\:\\  "
	@echo "     \\__\\/         \\__\\/         \\__\\/         \\__\\/  "
	@echo -n "$(BASE_COLOR)"
endef

define animation_5
	@echo -n "$(WHITE)"
	@echo "      ___           ___           ___           ___   "
	@echo "     /  /\\         /  /\\         /  /\\         /  /\\  "
	@echo "    /  /:/_       /  /::\\       /  /::\\       /  /::\\ "
	@echo "   /  /:/ /\\     /  /:/\\:\\     /  /:/\\:\\     /  /:/\\:\\"
	@echo "  /  /:/ /::\\   /  /:/  \\:\\   /  /:/  \\:\\   /  /:/~/:/"
	@echo " /__/:/ /:/\\:\\ /__/:/ \\  \\:\\ /__/:/ \\__\\:\\ /__/:/ /:/ "
	@echo " \\  \\:\\/:/~/:/ \\  \\:\\  \\__\\/ \\  \\:\\ /  /:/ \\  \\:\\/:/  "
	@echo "  \\  \\::/ /:/   \\  \\:\\        \\  \\:\\  /:/   \\  \\::/   "
	@echo "   \\__\\/ /:/     \\  \\:\\        \\  \\:\\/:/     \\  \\:\\   "
	@echo "     /__/:/       \\  \\:\\        \\  \\::/       \\  \\:\\  "
	@echo "     \\__\\/         \\__\\/         \\__\\/         \\__\\/  "
	@echo -n "$(BASE_COLOR)"
endef

define animation_6
	@echo -n "$(WHITE)"
	@echo "      ___           ___           ___           ___   "
	@echo "     /  /\\         /\\  \\         /  /\\         /  /\\  "
	@echo "    /  /:/_       /::\\  \\       /  /::\\       /  /::\\ "
	@echo "   /  /:/ /\\     /:/\\:\\  \\     /  /:/\\:\\     /  /:/\\:\\"
	@echo "  /  /:/ /::\\   /:/  \\:\\  \\   /  /:/  \\:\\   /  /:/~/:/"
	@echo " /__/:/ /:/\\:\\ /:/__/ \\:\\__\\ /__/:/ \\__\\:\\ /__/:/ /:/ "
	@echo " \\  \\:\\/:/~/:/ \\:\\  \\  \\/__/ \\  \\:\\ /  /:/ \\  \\:\\/:/  "
	@echo "  \\  \\::/ /:/   \\:\\  \\        \\  \\:\\  /:/   \\  \\::/   "
	@echo "   \\__\\/ /:/     \\:\\  \\        \\  \\:\\/:/     \\  \\:\\   "
	@echo "     /__/:/       \\:\\__\\        \\  \\::/       \\  \\:\\  "
	@echo "     \\__\\/         \\/__/         \\__\\/         \\__\\/  "
	@echo -n "$(BASE_COLOR)"
endef

define animation_7
	@echo -n "$(WHITE)"
	@echo "      ___           ___           ___           ___   "
	@echo "     /  /\\         /\\__\\         /  /\\         /  /\\  "
	@echo "    /  /:/_       /:/  /        /  /::\\       /  /::\\ "
	@echo "   /  /:/ /\\     /:/  /        /  /:/\\:\\     /  /:/\\:\\"
	@echo "  /  /:/ /::\\   /:/  /  ___   /  /:/  \\:\\   /  /:/~/:/"
	@echo " /__/:/ /:/\\:\\ /:/__/  /\\__\\ /__/:/ \\__\\:\\ /__/:/ /:/ "
	@echo " \\  \\:\\/:/~/:/ \\:\\  \\ /:/  / \\  \\:\\ /  /:/ \\  \\:\\/:/  "
	@echo "  \\  \\::/ /:/   \\:\\  /:/  /   \\  \\:\\  /:/   \\  \\::/   "
	@echo "   \\__\\/ /:/     \\:\\/:/  /     \\  \\:\\/:/     \\  \\:\\   "
	@echo "     /__/:/       \\::/  /       \\  \\::/       \\  \\:\\  "
	@echo "     \\__\\/         \\/__/         \\__\\/         \\__\\/  "
	@echo -n "$(BASE_COLOR)"
endef

define animation_8
	@echo -n "$(WHITE)"
	@echo "      ___           ___           ___           ___   "
	@echo "     /  /\\         /  /\\         /  /\\         /  /\\  "
	@echo "    /  /:/_       /  /:/        /  /::\\       /  /::\\ "
	@echo "   /  /:/ /\\     /  /:/        /  /:/\\:\\     /  /:/\\:\\"
	@echo "  /  /:/ /::\\   /  /:/  ___   /  /:/  \\:\\   /  /:/~/:/"
	@echo " /__/:/ /:/\\:\\ /__/:/  /  /\\ /__/:/ \\__\\:\\ /__/:/ /:/ "
	@echo " \\  \\:\\/:/~/:/ \\  \\:\\ /  /:/ \\  \\:\\ /  /:/ \\  \\:\\/:/  "
	@echo "  \\  \\::/ /:/   \\  \\:\\  /:/   \\  \\:\\  /:/   \\  \\::/   "
	@echo "   \\__\\/ /:/     \\  \\:\\/:/     \\  \\:\\/:/     \\  \\:\\   "
	@echo "     /__/:/       \\  \\::/       \\  \\::/       \\  \\:\\  "
	@echo "     \\__\\/         \\__\\/         \\__\\/         \\__\\/  "
	@echo -n "$(BASE_COLOR)"
endef

define animation_9
	@echo -n "$(WHITE)"
	@echo "      ___           ___           ___           ___   "
	@echo "     /  /\\         /  /\\         /  /\\         /  /\\  "
	@echo "    /  /:/_       /  /:/        /  /::\\       /  /::\\ "
	@echo "   /  /:/ /\\     /  /:/        /  /:/\\:\\     /  /:/\\:\\"
	@echo "  /  /:/ /::\\   /  /:/  ___   /  /:/  \\:\\   /  /:/~/:/"
	@echo " /__/:/ /:/\\:\\ /__/:/  /  /\\ /__/:/ \\__\\:\\ /__/:/ /:/ "
	@echo " \\  \\:\\/:/~/:/ \\  \\:\\ /  /:/ \\  \\:\\ /  /:/ \\  \\:\\/:/  "
	@echo "  \\  \\::/ /:/   \\  \\:\\  /:/   \\  \\:\\  /:/   \\  \\::/   "
	@echo "   \\__\\/ /:/     \\  \\:\\/:/     \\  \\:\\/:/     \\  \\:\\   "
	@echo "     /__/:/       \\  \\::/       \\  \\::/       \\  \\:\\  "
	@echo "     \\__\\/         \\__\\/         \\__\\/         \\__\\/  "
	@echo -n "$(BASE_COLOR)"
endef

define animation_10
	@echo -n "$(WHITE)"
	@echo "      ___           ___           ___           ___   "
	@echo "     /  /\\         /  /\\         /\\  \\         /  /\\  "
	@echo "    /  /:/_       /  /:/        /::\\  \\       /  /::\\ "
	@echo "   /  /:/ /\\     /  /:/        /:/\\:\\  \\     /  /:/\\:\\"
	@echo "  /  /:/ /::\\   /  /:/  ___   /:/  \\:\\  \\   /  /:/~/:/"
	@echo " /__/:/ /:/\\:\\ /__/:/  /  /\\ /:/__/ \\:\\__\\ /__/:/ /:/ "
	@echo " \\  \\:\\/:/~/:/ \\  \\:\\ /  /:/ \\:\\  \\ /:/  / \\  \\:\\/:/  "
	@echo "  \\  \\::/ /:/   \\  \\:\\  /:/   \\:\\  /:/  /   \\  \\::/   "
	@echo "   \\__\\/ /:/     \\  \\:\\/:/     \\:\\/:/  /     \\  \\:\\   "
	@echo "     /__/:/       \\  \\::/       \\::/  /       \\  \\:\\  "
	@echo "     \\__\\/         \\__\\/         \\/__/         \\__\\/  "
	@echo -n "$(BASE_COLOR)"
endef

define animation_11
	@echo -n "$(WHITE)"
	@echo "      ___           ___           ___           ___   "
	@echo "     /  /\\         /  /\\         /\\  \\         /  /\\  "
	@echo "    /  /:/_       /  /:/        /::\\  \\       /  /::\\ "
	@echo "   /  /:/ /\\     /  /:/        /:/\\:\\  \\     /  /:/\\:\\"
	@echo "  /  /:/ /::\\   /  /:/  ___   /:/  \\:\\  \\   /  /:/~/:/"
	@echo " /__/:/ /:/\\:\\ /__/:/  /  /\\ /:/__/ \\:\\__\\ /__/:/ /:/ "
	@echo " \\  \\:\\/:/~/:/ \\  \\:\\ /  /:/ \\:\\  \\ /:/  / \\  \\:\\/:/  "
	@echo "  \\  \\::/ /:/   \\  \\:\\  /:/   \\:\\  /:/  /   \\  \\::/   "
	@echo "   \\__\\/ /:/     \\  \\:\\/:/     \\:\\/:/  /     \\  \\:\\   "
	@echo "     /__/:/       \\  \\::/       \\::/  /       \\  \\:\\  "
	@echo "     \\__\\/         \\__\\/         \\/__/         \\__\\/  "
	@echo -n "$(BASE_COLOR)"
endef

define animation_12
	@echo -n "$(WHITE)"
	@echo "      ___           ___           ___           ___   "
	@echo "     /  /\\         /  /\\         /  /\\         /  /\\  "
	@echo "    /  /:/_       /  /:/        /  /::\\       /  /::\\ "
	@echo "   /  /:/ /\\     /  /:/        /  /:/\\:\\     /  /:/\\:\\"
	@echo "  /  /:/ /::\\   /  /:/  ___   /  /:/  \\:\\   /  /:/~/:/"
	@echo " /__/:/ /:/\\:\\ /__/:/  /  /\\ /__/:/ \\__\\:\\ /__/:/ /:/ "
	@echo " \\  \\:\\/:/~/:/ \\  \\:\\ /  /:/ \\  \\:\\ /  /:/ \\  \\:\\/:/  "
	@echo "  \\  \\::/ /:/   \\  \\:\\  /:/   \\  \\:\\  /:/   \\  \\::/   "
	@echo "   \\__\\/ /:/     \\  \\:\\/:/     \\  \\:\\/:/     \\  \\:\\   "
	@echo "     /__/:/       \\  \\::/       \\  \\::/       \\  \\:\\  "
	@echo "     \\__\\/         \\__\\/         \\__\\/         \\__\\/  "
	@echo -n "$(BASE_COLOR)"
endef

define animation_13
	@echo -n "$(WHITE)"
	@echo "      ___           ___           ___                   "
	@echo "     /  /\\         /  /\\         /\\  \\          ___     "
	@echo "    /  /:/_       /  /:/        /::\\  \\        /  /\\    "
	@echo "   /  /:/ /\\     /  /:/        /:/\\:\\  \\      /  /::\\   "
	@echo "  /  /:/ /::\\   /  /:/  ___   /:/  \\:\\  \\    /  /:/\\:\\  "
	@echo " /__/:/ /:/\\:\\ /__/:/  /  /\\ /:/__/ \\:\\__\\  /  /::\\ \\:\\ "
	@echo " \\  \\:\\/:/~/:/ \\  \\:\\ /  /:/ \\:\\  \\ /:/  / /__/:/\\:\\_\\:\\"
	@echo "  \\  \\::/ /:/   \\  \\:\\  /:/   \\:\\  /:/  /  \\__\\/  \\:\\/:/"
	@echo "   \\__\\/ /:/     \\  \\:\\/:/     \\:\\/:/  /        \\  \\::/ "
	@echo "     /__/:/       \\  \\::/       \\::/  /          \\__\\/  "
	@echo "     \\__\\/         \\__\\/         \\/__/                  "
	@echo -n "$(BASE_COLOR)"
endef

define animation_14
	@echo -n "$(WHITE)"
	@echo "      ___           ___           ___           ___     "
	@echo "     /  /\\         /  /\\         /\\  \\         /\\  \\    "
	@echo "    /  /:/_       /  /:/        /::\\  \\       /::\\  \\   "
	@echo "   /  /:/ /\\     /  /:/        /:/\\:\\  \\     /:/\\:\\  \\  "
	@echo "  /  /:/ /::\\   /  /:/  ___   /:/  \\:\\  \\   /::\\~\\:\\  \\ "
	@echo " /__/:/ /:/\\:\\ /__/:/  /  /\\ /:/__/ \\:\\__\\ /:/\\:\\ \\:\\__\\"
	@echo " \\  \\:\\/:/~/:/ \\  \\:\\ /  /:/ \\:\\  \\ /:/  / \\/__\\:\\/:/  /"
	@echo "  \\  \\::/ /:/   \\  \\:\\  /:/   \\:\\  /:/  /       \\::/  / "
	@echo "   \\__\\/ /:/     \\  \\:\\/:/     \\:\\/:/  /         \\/__/  "
	@echo "     /__/:/       \\  \\::/       \\::/  /                 "
	@echo "     \\__\\/         \\__\\/         \\/__/                  "
	@echo -n "$(BASE_COLOR)"
endef

define animation_15
	@echo -n "$(WHITE)"
	@echo "      ___           ___           ___           ___   "
	@echo "     /  /\\         /  /\\         /\\  \\         /\\  \\  "
	@echo "    /  /:/_       /  /:/        /::\\  \\       /::\\  \\ "
	@echo "   /  /:/ /\\     /  /:/        /:/\\:\\  \\     /:/\\:\\__\\"
	@echo "  /  /:/ /::\\   /  /:/  ___   /:/  \\:\\  \\   /:/ /:/  /"
	@echo " /__/:/ /:/\\:\\ /__/:/  /  /\\ /:/__/ \\:\\__\\ /:/_/:/  / "
	@echo " \\  \\:\\/:/~/:/ \\  \\:\\ /  /:/ \\:\\  \\ /:/  / \\:\\/:/  /  "
	@echo "  \\  \\::/ /:/   \\  \\:\\  /:/   \\:\\  /:/  /   \\::/__/   "
	@echo "   \\__\\/ /:/     \\  \\:\\/:/     \\:\\/:/  /     \\:\\  \\   "
	@echo "     /__/:/       \\  \\::/       \\::/  /       \\:\\__\\  "
	@echo "     \\__\\/         \\__\\/         \\/__/         \\/__/  "
	@echo -n "$(BASE_COLOR)"
endef


.PHONY : all glfw glad imgui clean fclean cleanall re run

-include $(DEPS)