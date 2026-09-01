# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: leoaguia <leoaguia@student.42porto.com>    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2026/09/01 02:30:07 by leoaguia          #+#    #+#              #
#    Updated: 2026/09/01 02:47:51 by leoaguia         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

# Makefile

NAME     := ircserv
CXX      := c++
CXXFLAGS := -Wall -Wextra -Werror -std=c++98
INCDIR   := inc
SRCDIR   := src
OBJDIR   := obj


SRCS := $(SRCDIR)/main.cpp \
        $(SRCDIR)/core/Server.cpp \
        $(SRCDIR)/core/Message.cpp

OBJS := $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(SRCS))

DEPS := $(OBJS:.o=.d)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -MMD -MP -I$(INCDIR) -c $< -o $@

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $@

-include $(DEPS)