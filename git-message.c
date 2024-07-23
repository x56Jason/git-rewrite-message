#define _GNU_SOURCE
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>

char comment_line_char = '#';

static const char *builtin_prefixes[] = {
	"Signed-off-by: ",
	"Reviewed-by: ",
	"Acked-by: ",
	"Tested-by: ",
	"Cc: ",
	"Fixes: ",
	"Link: ",
	"Reported-by: ",
	"Closes: ",
	"Message-ID: ",
	"Co-developed-by: ",
	"Co-authored-by: ",
	"Suggested-by: ",
	"Inspired-by",
	"Reported-and-tested-by: ",
	"(cherry picked from commit ",
	NULL
};

int starts_with(const char *str, const char *prefix)
{
	return (!strncasecmp(str, prefix, strlen(prefix)));
}

static inline int is_blank_line(const char *str)
{
	const char *s = str;
	while (*s && *s != '\n' && isspace(*s))
		s++;
	return !*s || *s == '\n';
}

static const char *next_line(const char *str)
{
	const char *nl = strchrnul(str, '\n');
	return nl + !!*nl;
}

static ssize_t last_line(const char *buf, size_t len)
{
	ssize_t i;
	if (len == 0)
		return -1;
	if (len == 1)
		return 0;
	/*
	 * Skip the last character (in addition to the null terminator),
	 * because if the last character is a newline, it is considered as part
	 * of the last line anyway.
	 */
	i = len - 2;

	for (; i >= 0; i--) {
		if (buf[i] == '\n')
			return i + 1;
	}
	return 0;
}

static ssize_t find_separator(const char *line, const char *separators)
{
	int whitespace_found = 0;
	const char *c;
	for (c = line; *c; c++) {
		if (strchr(separators, *c))
			return c - line;
		if (!whitespace_found && (isalnum(*c) || *c == '-'))
			continue;
		if (c != line && (*c == ' ' || *c == '\t')) {
			whitespace_found = 1;
			continue;
		}
		break;
	}
	return -1;
}

static size_t find_body_block_start(const char *buf, size_t len)
{
	const char *s;
	/* The first paragraph is the title and cannot be trailers */
	for (s = buf; s < buf + len; s = next_line(s)) {
		if (s[0] == comment_line_char)
			continue;
		if (is_blank_line(s)) {
			do {
				s = next_line(s);
			} while (is_blank_line(s));
			break;
		}
	}
	return s - buf;
}

static size_t find_body_block_end(const char *buf, size_t trailer_block_start)
{
	size_t l = trailer_block_start;
	size_t prev;
	do {
		prev = l;
		l = last_line(buf, l);
	} while (is_blank_line(buf+l));
	return prev;
}

static size_t find_trailer_block_start(const char *buf, size_t body_start, size_t len)
{
	int recognized_prefix = 0, trailer_lines = 0, non_trailer_lines = 0;
	int possible_continuation_lines = 0;
	int only_spaces = 1;
	const char *paragraph_start, *s;

	paragraph_start = NULL;
	for (s = buf + body_start; s < buf + len; s = next_line(s)) {
		ssize_t separator_pos;
		const char **p;

		if (s[0] == comment_line_char)
			continue;

		if (is_blank_line(s)) {
			if (only_spaces)
				continue;

			if (recognized_prefix &&
			    trailer_lines * 3 >= non_trailer_lines)
				return paragraph_start - buf;
			else if (trailer_lines && !non_trailer_lines)
				return paragraph_start - buf;

			only_spaces = 1;
			recognized_prefix = 0;
			trailer_lines = 0;
			non_trailer_lines = 0;
			possible_continuation_lines = 0;

			continue;
		}

		if (only_spaces) {
			paragraph_start = s;
			only_spaces = 0;
		}

		for (p = builtin_prefixes; *p; p++) {
			if (starts_with(s, *p)) {
				trailer_lines++;
				possible_continuation_lines = 0;
				recognized_prefix = 1;
				goto continue_outer_loop;
			}
		}

		separator_pos = find_separator(s, ":");
		if (separator_pos >= 1 && !isspace(s[0])) {
			trailer_lines++;
			possible_continuation_lines = 0;
			if (recognized_prefix)
				continue;
		} else if (isspace(s[0])) {
			possible_continuation_lines++;
		} else {
			non_trailer_lines++;
			non_trailer_lines += possible_continuation_lines;
			possible_continuation_lines = 0;
		}
continue_outer_loop:
		;
	}
	if (paragraph_start)
		return paragraph_start - buf;
	return len;
}

#if 0
static size_t find_trailer_block_start_reverse(const char *buf, size_t end_of_title, size_t len)
{
	size_t l;
	int only_spaces = 1;
	int recognized_prefix = 0, trailer_lines = 0, non_trailer_lines = 0;
	/*
	 * Number of possible continuation lines encountered. This will be
	 * reset to 0 if we encounter a trailer (since those lines are to be
	 * considered continuations of that trailer), and added to
	 * non_trailer_lines if we encounter a non-trailer (since those lines
	 * are to be considered non-trailers).
	 */
	int possible_continuation_lines = 0;

	/*
	 * Get the start of the trailers by looking starting from the end for a
	 * blank line before a set of non-blank lines that (i) are all
	 * trailers, or (ii) contains at least one Git-generated trailer and
	 * consists of at least 25% trailers.
	 */
	for (l = last_line(buf, len);
	     l >= end_of_title;
	     l = last_line(buf, l)) {
		const char *bol = buf + l;
		const char **p;
		ssize_t separator_pos;

		if (bol[0] == comment_line_char) {
			non_trailer_lines += possible_continuation_lines;
			possible_continuation_lines = 0;
			continue;
		}
		if (is_blank_line(bol)) {
			if (only_spaces)
				continue;
			non_trailer_lines += possible_continuation_lines;
			if (recognized_prefix &&
			    trailer_lines * 3 >= non_trailer_lines)
				return next_line(bol) - buf;
			else if (trailer_lines && !non_trailer_lines)
				return next_line(bol) - buf;
			return len;
		}
		only_spaces = 0;

		for (p = builtin_prefixes; *p; p++) {
			if (starts_with(bol, *p)) {
				trailer_lines++;
				possible_continuation_lines = 0;
				recognized_prefix = 1;
				goto continue_outer_loop;
			}
		}

		separator_pos = find_separator(bol, ":");
		if (separator_pos >= 1 && !isspace(bol[0])) {
			trailer_lines++;
			possible_continuation_lines = 0;
			if (recognized_prefix)
				continue;
		} else if (isspace(bol[0]))
			possible_continuation_lines++;
		else {
			non_trailer_lines++;
			non_trailer_lines += possible_continuation_lines;
			possible_continuation_lines = 0;
		}
continue_outer_loop:
		;
	}

	return len;
}
#endif

int main(int argc, char *argv[])
{
	size_t trailer_block_start;
	size_t body_block_start;
	size_t body_block_end;
	struct stat  sb;
	char *buf;
	int fd;
	int ret;

	if (argc != 3) {
		printf("Usage: %s [--title|--body|--trailer] <file>\n", argv[0]);
		exit(-1);
	}

	fd = open(argv[2], O_RDONLY);
	if (fd == -1)
		exit(-2);

	if (fstat(fd, &sb) == -1)
		exit(-3);

	buf = malloc(sb.st_size + 1);
	if (buf == NULL)
		exit(-4);

	ret = read(fd, buf, sb.st_size);
	if (ret != sb.st_size)
		exit(-5);
	buf[sb.st_size] = '\0';

	body_block_start = find_body_block_start(buf, sb.st_size);
	trailer_block_start = find_trailer_block_start(buf, body_block_start, sb.st_size);
	body_block_end = find_body_block_end(buf, trailer_block_start);

	ret = EXIT_SUCCESS;
	if (strcmp(argv[1], "--trailer") == 0)
		write(1, buf + trailer_block_start, sb.st_size - trailer_block_start);
	else if (strcmp(argv[1], "--body") == 0)
		write(1, buf + body_block_start, body_block_end - body_block_start);
	else if (strcmp(argv[1], "--title") == 0)
		write(1, buf, body_block_start);
	else
		ret = EXIT_FAILURE;

	munmap(buf, sb.st_size);
	close(fd);

	return ret;
}
