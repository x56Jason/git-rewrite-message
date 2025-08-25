all: git-message

git-message: git-message.c
	gcc -o git-message git-message.c

clean:
	rm -f git-message
	rm -f *.o

install: all
	mkdir -p ~/bin/
	mkdir -p ~/.config/git-rewrite-message/
	install git-message ~/bin/
	install git-message-rewrite.pl ~/bin/
	install git-rewrite-message.sh ~/bin/
	install common.template ~/.config/git-rewrite-message/
	install baseline.template ~/.config/git-rewrite-message/
	install openeuler.template ~/.config/git-rewrite-message/
