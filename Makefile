##========== SOURCES ==========##

SRC = 	$(GLAD_CPP) \
		main.cpp \
		Scop.cpp \
		Shader.cpp \
		VBO.cpp \
		EBO.cpp \
		VAO.cpp \


##========== NAMES ==========##

NAME = scop
SRCS_DIR = src/
OBJS_DIR = obj/
INCLUDE_DIR = includes
LIBRARIES_DIR = libs/
GLFW = $(LIBRARIES_DIR)glfw/
GLAD_CPP = glad.cpp # if in a subdirectory, update this

##========== OBJECTS ==========##

OBJS = $(addprefix $(OBJS_DIR),$(SRC:.cpp=.o))

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
MAGENTA		=		\033[0;95m
DARK_MAGENTA=		\033[35m
CYAN 		=		\033[0;96m
WHITE		=		\033[0;97m

##========== COMPILATOR ==========##

CXX = c++

##========== FLAGS ==========##

CXXFLAGS = -Wall -Wextra -Werror
LDFLAGS = $(LIBS)
LIBS = -I$(INCLUDE_DIR) -I$(GLFW)include

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

NUM_SRC = $(words $(SRC))
INDEX = 0
NUMBER_OF_ANIMATION = 16
ifndef ANIMATION_RATE
	ANIMATION_RATE = 1
endif

##========== COMPILATION ==========##

all: glfw glad $(NAME)

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
		cd libs/ && git clone https://github.com/Dav1dde/glad.git glad; \
		python -m glad --out-path=glad/build --generator=c; cd ..; \
		mv libs/glad/build/src/glad.c src/glad.cpp ; \
		mkdir -p includes/glad/ && mv libs/glad/build/include/glad/glad.h includes/glad/ ; \
		rm -rf libs/glad ; \
	fi

$(NAME) : $(OBJS)
	@echo ""
	@$(CXX) -o $(NAME) $(CXXFLAGS) $(OBJS) $(LDFLAGS) $(GLFW)build/src/libglfw3.a
	@echo "$(GREEN)-= cpp compiled =-$(BASE_COLOR)"

clean:
	@rm -rf $(OBJS_DIR)
	@rm -rf $(GLFW)/build

fclean: clean
	@rm -rf $(NAME)
	@rm -rf $(GLFW)build/src/libglfw3.a
	@rm -rf $(SRCS_DIR)$(GLAD_CPP)
	@echo "$(CYAN)Files cleaned$(BASE_COLOR)"

cleanall: fclean
	@rm -rf $(GLFW)

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
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -c $< -o $@
else
	@mkdir -p $(dir $@)
	@$(CXX) $(CXXFLAGS) $(LDFLAGS) -c $< -o $@
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
	@awk -v size=$(shell stat -c %s $<) 'BEGIN { printf "[ "; for (i=0; i<int(size/1000); i++) printf "#"; printf " ] (%d KB)\n", size/1000 }'
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


.PHONY : all glfw glad clean fclean cleanall re run