NAME :=	libmbl.a

AR := ar
RCS := rcs
CXX := c++
CXXFLAGS :=	-g -MP -MMD -std=c++20 # -Wall -Wextra -Werror

LIB_DIR :=	lib/
INC_DIR :=	inc/
SRC_DIR :=	src/
OBJ_DIR :=	.obj/

STB_LIB_DIR :=	$(LIB_DIR)stb_image

INCLUDE_DIRS :=	-I$(INC_DIR) -I$(LIB_DIR)
SDL_CFLAGS :=	$(shell sdl2-config --cflags)
SDL_LIBS :=		$(shell sdl2-config --libs)
LFLAGS :=		$(SDL_LIBS) -lGL

SRCS :=	src/platform/Window.cpp					\
		src/platform/Input.cpp					\
		src/render/Shader.cpp					\
		src/render/FrameBuffer.cpp				\
		src/loader/mesh/obj.cpp					\
		src/loader/texture/stb.cpp				\
		src/ui/ui.cpp							\
		src/ui/elements/button.cpp				\
		src/ui/elements/toggle.cpp				\
		src/ui/elements/input.cpp				\
		src/ui/elements/text.cpp				\
		src/ui/elements/slider.cpp				\
		src/ui/elements/progress_bar.cpp		\
		src/render/renderer/AABBRenderer.cpp	\

OBJS :=	$(SRCS:%.cpp=$(OBJ_DIR)%.o)
DEPS :=	$(SRCS:%.cpp=$(OBJ_DIR)%.d)

all: $(STB_LIB_DIR) $(NAME)

$(STB_LIB_DIR): | $(LIB_DIR)
	@if ls external/stb_image | grep -q "stb_image.h"; then \
		printf ""; \
	else\
		mkdir -p $(STB_LIB_DIR); \
		curl -o $(STB_LIB_DIR)/stb_image.h https://raw.githubusercontent.com/nothings/stb/master/stb_image.h;\
	fi

$(LIB_DIR):
	mkdir -p $(LIB_DIR)

$(NAME): $(OBJS)
	$(AR) $(RCS) $(NAME) $(OBJS)

$(OBJ_DIR)%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDE_DIRS) $(SDL_CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -rf $(NAME)

re: fclean all

.PHONY: all clean fclean re

-include $(DEPS)
