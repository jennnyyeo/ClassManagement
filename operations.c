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
    if (!dst || cap == 0) return;
    if (!src) { dst[0] = '\0'; return; }

    size_t i = 0;
    for (; src[i] && i < cap - 1; ++i) {
        char c = src[i];
        if (c == '\t' || c == '\n' || c == '\r')
            c = ' ';
        dst[i] = c;
    }
    dst[i] = '\0';
}

int opendb(LinkedList *store, const char *filename, int fileOpened)
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
	if (fileOpened == 0)
	{
		printf("File has been successfully opened and read. Loaded %zu record(s).\n", loaded);
	}
	return 0;
}

int savedb(LinkedList *store, const char *filename)
{
	FILE *f = fopen(filename, "w");

	if (!f)
	{
		fprintf(stderr, "savedb: fopen(\"%s\") failed: ", filename);
		perror("");
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
	for (Node *p = store ? store->head : NULL; p; p = p->next) // Making sure that NUL-terminated sources even if structure fields are "full"
	{
		char name_src[MAX_NAME + 1];
		char prog_src[MAX_PROGRAM + 1];
		memcpy(name_src, p->s.name, MAX_NAME);
		name_src[MAX_NAME] = '\0';
		memcpy(prog_src, p->s.programme, MAX_PROGRAM);
		prog_src[MAX_PROGRAM] = '\0';

		// Sanitize in order to remove any tabs/newlines which will break the TSV file
		char name_san[MAX_NAME + 1], prog_san[MAX_PROGRAM + 1];
		sanitize_field(name_san, sizeof name_san, name_src);
		sanitize_field(prog_san, sizeof prog_san, prog_src);

		// Writing all 4 of the
		if (fprintf(f, "%d\t%s\t%s\t%.2f\n", p->s.id, name_san, prog_san, p->s.mark) < 0)
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
	return 0;
}


int autoSave(LinkedList *list, int fileOpened)
{
	if(fileOpened)
	{
		int result = savedb(list, "autosave.txt");
		if (result == -1)
		{
			printf("Error: Autosave failed.\n");
			return -1;
		}
		printf("CMS: Autosave completed. (autosave.txt updated) \n");
		return 0;
	}
}

int recoverChanges(const char *dbFile, const char *asFile)
{
	FILE *fdb, *fas;
	int char1, char2;
	int result = 0;

	fdb = fopen(dbFile, "r");
	fas = fopen(asFile, "r");

	if (fdb == NULL || fas == NULL)
	{
		perror("Error opening files.\n");
		return -1;
	}

	do
	{
		char1 = fgetc(fdb);
		char2 = fgetc(fas);

		if (char1 != char2)
		{
			result = 1;
			break;
		}
	} while (char1 != EOF && char2 != EOF);
	
	fclose(fdb);
	fclose(fas);

	return result;
}
