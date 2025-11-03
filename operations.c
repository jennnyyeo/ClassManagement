#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "operations.h"

#define FIELD_MAX 50
#define LINE_MAX 512

static void rstrip(char *s)
{
	size_t n = strlen(s);
	while (n && (s[n - 1] == '\n' || s[n - 1] == '\r'))
	{
		s[--n] = '\0';
	}
}

static void sanitize_field(char *dst, size_t cap, const char *src)
{
	if (!cap)
		return;
	size_t i = 0;
	for (; src[i] && i < cap - 1; ++i)
	{
		char c = src[i];
		if (c == '\t' || c == '\n' || c == '\r')
		{
			c = ' ';
			dst[i] = c;
		}
	}
	dst[i] = '\0';
}

int opendb(LinkedList *store, const char *filename)
{
	FILE *f = fopen(filename, "r");

	if (!f)
	{
		perror("opendb failed");
		return -1;
	}

	char line[LINE_MAX];

	if (!fgets(line, sizeof line, f))
	{
		fclose(f);
		puts("Empty File");
		return 0;
	}

	size_t line_no = 1;

	size_t loaded = 0;

	while (fgets(line, sizeof line, f))
	{
		line_no++;
		rstrip(line);
		if (!*line)
			continue;

		char *f1 = line;
		char *t1 = strchr(f1, '\t');
		if (!t1)
		{
			fprintf(stderr, "Line %zu: need 4 fields (no 1st TAB). Skipping.\n", line_no);
			continue;
		}
		*t1 = '\0';
		char *f2 = t1 + 1;

		char *t2 = strchr(f2, '\t');
		if (!t2)
		{
			fprintf(stderr, "Line %zu: need 4 fields (no 2nd TAB). Skipping.\n", line_no);
			continue;
		}
		*t2 = '\0';
		char *f3 = t2 + 1;

		char *t3 = strchr(f3, '\t');
		if (!t3)
		{
			fprintf(stderr, "Line %zu: need 4 fields (no 3rd TAB). Skipping.\n", line_no);
			continue;
		}
		*t3 = '\0';
		char *f4 = t3 + 1;

		Student st;

		// ID
		char *endp = NULL;
		st.id = (int)strtol(f1, &endp, 10);
		if (endp == f1)
		{
			fprintf(stderr, "Line %zu: bad ID. Skipping.\n", line_no);
			continue;
		}

		strncpy(st.name, f2, MAX_NAME - 1);
		st.name[MAX_NAME - 1] = '\0';

		strncpy(st.programme, f3, MAX_PROGRAM - 1);
		st.programme[MAX_PROGRAM - 1] = '\0';

		// Mark
		endp = NULL;
		st.mark = (float)strtof(f4, &endp);
		if (endp == f4)
		{
			fprintf(stderr, "Line %zu error Mark\n", line_no);
			continue;
		}



		if (insert_node(store, &st) == -1)
		{
			fclose(f);
			return -1;
		}
		loaded++;
	}

	fclose(f);
	printf("File has been successfully opened and read. Loaded %zu record(s).\n", loaded);
	return 0;
}

int savedb(const LinkedList *store, const char *filename)
{
	FILE *f = fopen(filename, "w");

	if (!f)
	{
		perror("opendb failed");
		return -1;
	}

	// Writes the header
	if (fprintf(f, "ID\tName\tProgramme\tMark\n") < 0)
	{
		perror("savedb:fprintf(header)");
		fclose(f);
		return -1;
	}

	// Rows
	for (Node *p = store->head; p; p = p->next)
	{
		char name_san[FIELD_MAX + 1], prog_san[FIELD_MAX + 1];
		sanitize_field(name_san, sizeof name_san, p->s.name);
		sanitize_field(prog_san, sizeof prog_san, p->s.programme);
		if (fprintf(f, "%s\t%s\n", name_san, prog_san) < 0)
		{
			perror("savedb:fprintf");
			fclose(f);
			return -1;
		}
	}

	if (fclose(f) != 0)
	{
		perror("savedb:fclose");
		return -1;
	}
	puts("File successfully saved.");
	return 0;
}
