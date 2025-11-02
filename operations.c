#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "operations.h"

#define FIELD_MAX 50
#define LINE_MAX  512

static void rstrip(char* s) {
	size_t n = strlen(s);
	while (n && (s[n - 1] == '\n' || s[n - 1] == '\r')) {
		s[--n] == '\0';
	}
}

static void sanitize_field(char* dst, size_t cap, const char* src) {
	if (!cap) return;
	size_t i = 0;
	for (; src[i] && i < cap - 1; ++i) {
		char c = src[i];
		if (c == '\t' || c == '\n' || c == "\r") {
			c = ' ';
			dst[i] = c;
		}
	}
	dst[i] = '\0';
}

int opendb(LinkedList* store, const char* filename) {
	FILE* f = fopen(filename, "r");

	if (!f) {
		perror("opendb failed");
		return -1;
	}

	char line[LINE_MAX];

	if (!fgets(line, sizeof line, f)) {
		fclose(f);
		puts("Empty File");
		return 0;
	}

	size_t line_no = 1;

	while (fgets(line, sizeof line, f)) {
		line_no++;
		rstrip(line);
		if (!*line) continue;

		char* tab = strchr(line, '\t');
		if (!tab) {
			fprintf(stderr, "Line %zu: no TAB found, skip.\n", line_no);
		}
		*tab = '\0';
		const char* key_in = line;
		const char* value_in = tab + 1;

		char key[FIELD_MAX + 1], value[FIELD_MAX + 1];
		strncpy(key, key_in, FIELD_MAX);
		key[FIELD_MAX] = '\0';
		strncpy(value, value_in, FIELD_MAX);
		value[FIELD_MAX] = '\0';

		if (insert_node(list, key, value) == -1) {
			fclose(f);
			return -1;
		}

	}

	fclose(f);
	puts("File has been successfully open and read.");
	return 0;
}

int savedb(const LinkedList* store, const char* filename) {
	FILE* f = fopen(filename, "r");

	if (!f) {
		perror("opendb failed");
		return -1;
	}

	fprintf(f, "Key\tValue\n");

	for (Node* p = list->head; p; p = p->next) {
		char key_san[FIELD_MAX + 1], val_san[FIELD_MAX + 1];
		sanitize_field(key_san, sizeof key_san, p->key);
		sanitize_field(val_san, sizeof val_san, p->value);
		if (fprintf(f, "%s\t%s\n", key_san, val_san) < 0) {
			perror("savedb:fprintf");
			fclose(f);
			return -1;
		}
	}

	if (fclose(f) != 0){
		perror("savedb:fclose");
		return -1;
	}
	puts("File successfully saved.");
	return 0;
}
