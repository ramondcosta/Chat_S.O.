all: main

%: %.c
	gcc $< -o $@

