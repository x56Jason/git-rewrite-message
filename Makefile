all: git-message

git-message: git-message.c
	gcc -o git-message git-message.c

install: all
	mkdir -p ~/bin/
	mkdir -p ~/.config/git-message/
	install git-message ~/bin/
	install git-message-rewrite.pl ~/bin/
	install baseline.template ~/.config/git-message/
	install openeuler.template ~/.config/git-message/
