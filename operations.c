#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "operations.h"

#define FIELD_MAX 50
#define LINE_MAX  512

/*
 * rstrip:
 * Simple helper to strip trailing '\n' and '\r' from a string read by fgets.
 * I use this to clean up lines from the file before I start parsing them.
 */
static void rstrip(char *s)
{
	size_t n = strlen(s);
	while (n && (s[n - 1] == '\n' || s[n - 1] == '\r'))
	{
		s[--n] = '\0';
	}
}

/*
 * sanitize_field:
 * This is a defensive helper to clean text fields before writing them out
 * to the TSV file.
 *
 * - Copies src into dst up to cap-1 characters.
 * - Replaces any TABs / newlines / carriage returns with a space.
 * - Guarantees dst is always NUL-terminated.
 *
 * The goal is to ensure that when I save the database, I don't accidentally
 * break the tab-separated format by having stray '\t' or '\n' inside names.
 */
static void sanitize_field(char *dst, size_t cap, const char *src)
{
    // If there's no space to write or dst is NULL, just bail out.
    if (!dst || cap == 0) return;

    // If src is NULL, treat it as empty string.
    if (!src) { dst[0] = '\0'; return; }

    size_t i = 0;
    for (; src[i] && i < cap - 1; ++i) {
        char c = src[i];
        // Replace control-like characters with a plain space
        if (c == '\t' || c == '\n' || c == '\r')
            c = ' ';
        dst[i] = c;
    }
    dst[i] = '\0';
}

/*
 * opendb:
 * - Opens the given filename as a TSV ("ID<TAB>Name<TAB>Programme<TAB>Mark").
 * - Reads and parses each line into a Student struct.
 * - Inserts each student into the linked list using insert_node().
 * - Skips malformed lines and prints an error to stderr.
 *
 * Returns:
 *   -1  on fatal error (e.g. fopen or insert_node failure)
 *    0  on success
 */
int opendb(LinkedList *store, const char *filename, int fileOpened)
{
	FILE *f = fopen(filename, "r");

	if (!f)
	{
		// If the file fails to open, I let perror show the system error.
		perror("opendb failed");
		return -1;
	}

	char line[LINE_MAX];

	// Try to read the header line (e.g. "ID\tName\tProgramme\tMark")
	if (!fgets(line, sizeof line, f))
	{
		// If even the header can't be read, treat it as an empty file.
		fclose(f);
		puts("Empty File");
		return 0;
	}

	size_t line_no = 1;
	size_t loaded  = 0;

	// Read the file line by line starting from line 2 (actual data rows)
	while (fgets(line, sizeof line, f))
	{
		line_no++;
		rstrip(line);     // strip newline characters at the end

		// Skip completely empty lines
		if (!*line)
			continue;

		// Now I split the line manually based on TABs into 4 fields: f1..f4

		char *f1 = line;               // field 1: ID
		char *t1 = strchr(f1, '\t');   // find 1st TAB
		if (!t1)
		{
			fprintf(stderr, "Line %zu: need 4 fields (no 1st TAB). Skipping.\n", line_no);
			continue;
		}
		*t1 = '\0';                    // terminate field 1
		char *f2 = t1 + 1;             // field 2: Name

		char *t2 = strchr(f2, '\t');   // find 2nd TAB
		if (!t2)
		{
			fprintf(stderr, "Line %zu: need 4 fields (no 2nd TAB). Skipping.\n", line_no);
			continue;
		}
		*t2 = '\0';                    // terminate field 2
		char *f3 = t2 + 1;             // field 3: Programme

		char *t3 = strchr(f3, '\t');   // find 3rd TAB
		if (!t3)
		{
			fprintf(stderr, "Line %zu: need 4 fields (no 3rd TAB). Skipping.\n", line_no);
			continue;
		}
		*t3 = '\0';                    // terminate field 3
		char *f4 = t3 + 1;             // field 4: Mark

		Student st;

		/* ----- Parse ID (field 1) ----- */
		char *endp = NULL;
		st.id = (int)strtol(f1, &endp, 10);
		if (endp == f1)
		{
			// If no digits were read, the ID is invalid
			fprintf(stderr, "Line %zu: bad ID. Skipping.\n", line_no);
			continue;
		}

		/* ----- Copy Name (field 2) ----- */
		strncpy(st.name, f2, MAX_NAME - 1);
		st.name[MAX_NAME - 1] = '\0';

		/* ----- Copy Programme (field 3) ----- */
		strncpy(st.programme, f3, MAX_PROGRAM - 1);
		st.programme[MAX_PROGRAM - 1] = '\0';

		/* ----- Parse Mark (field 4) ----- */
		endp = NULL;
		st.mark = (float)strtof(f4, &endp);
		if (endp == f4)
		{
			// If strtof doesn't consume anything, the mark is invalid
			fprintf(stderr, "Line %zu error Mark\n", line_no);
			continue;
		}

		// Insert the parsed student into the linked list
		if (insert_node(store, &st) == -1)
		{
			// If insert_node fails, I treat it as fatal and stop reading further
			fclose(f);
			return -1;
		}
		loaded++;
	}

	fclose(f);

	// Only print this message when the file is first opened
	if (fileOpened == 0)
	{
		printf("File has been successfully opened and read. Loaded %zu record(s).\n", loaded);
	}
	return 0;
}

/*
 * savedb:
 * - Saves the current linked list into a TSV file.
 * - Writes a header row, then one line per student:
 *       ID<TAB>Name<TAB>Programme<TAB>Mark
 * - Uses sanitize_field() to make sure Name and Programme don't contain
 *   any tabs or newlines which might corrupt the TSV format.
 *
 * Returns:
 *   -1  on failure
 *    0  on success
 */
int savedb(LinkedList *store, const char *filename)
{
	FILE *f = fopen(filename, "w");

	if (!f)
	{
		// Include filename in the error to make debugging easier
		fprintf(stderr, "savedb: fopen(\"%s\") failed: ", filename);
		perror("");
		return -1;
	}

	// Write header row at the top of the file
	if (fprintf(f, "ID\tName\tProgramme\tMark\n") < 0)
	{
		perror("savedb:fprintf(header)");
		fclose(f);
		return -1;
	}

	// Write each student as a single line
	// (The ternary handles the case where store might be NULL)
	for (Node *p = store ? store->head : NULL; p; p = p->next)
	{
		// Make sure we treat the struct fields as NUL-terminated,
		// even if they are "full".
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

		// Actually write the four fields into one TSV line
		if (fprintf(f, "%d\t%s\t%s\t%.2f\n", p->s.id, name_san, prog_san, p->s.mark) < 0)
		{
			perror("savedb:fprintf");
			fclose(f);
			return -1;
		}
	}

	// Always check fclose to make sure the buffer actually flushed to disk
	if (fclose(f) != 0)
	{
		perror("savedb:fclose");
		return -1;
	}
	return 0;
}

/*
 * autoSave:
 * - Convenience wrapper that autosaves the current list to "autosave.txt".
 * - Only runs if a file is already opened (based on fileOpened flag).
 * - Uses savedb() internally.
 *
 * Returns:
 *   -1  if autosave failed
 *    0  if autosave succeeded
 *   (no explicit return if fileOpened == 0; I currently only autosave when open)
 */
int autoSave(LinkedList *list, int fileOpened)
{
	if (fileOpened)
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

	// If file is not opened, I silently do nothing here.
	// (Could be extended to print a message or return a specific code.)
}

/*
 * recoverChanges:
 * - Compares the contents of the main DB file (dbFile) and the autosave file
 *   (asFile) character by character.
 * - This helps me detect if the autosave version has diverged from the
 *   original file on disk.
 *
 * Returns:
 *    0  if files are identical
 *    1  if there's at least one differing character
 *   -1  if opening either file fails
 */
int recoverChanges(const char *dbFile, const char *asFile)
{
	FILE *fdb, *fas;
	int char1, char2;
	int result = 0;  // assume equal until we find a mismatch

	fdb = fopen(dbFile, "r");
	fas = fopen(asFile, "r");

	if (fdb == NULL || fas == NULL)
	{
		// If either file can't be opened, I treat that as an error
		perror("Error opening files.\n");
		return -1;
	}

	// Read both files one character at a time and compare
	do
	{
		char1 = fgetc(fdb);
		char2 = fgetc(fas);

		if (char1 != char2)
		{
			// As soon as we detect a mismatch, we mark result as different
			result = 1;
			break;
		}
	} while (char1 != EOF && char2 != EOF);
	
	fclose(fdb);
	fclose(fas);

	return result;
}
