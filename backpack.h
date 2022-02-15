#ifndef BACKPACK_LIB_H
#define BACKPACK_LIB_H

#include <stdbool.h>

struct backpack_node
{
	char* name;
	unsigned char type;
	unsigned int size;
	void* data;
};
typedef struct backpack_node BP_NODE;

struct backpack
{
	BP_NODE* nodes;
	int cap;
	int count;
	bool written;
};
typedef struct backpack BACKPACK;

BACKPACK* load_backpack_file(const char* name);
BACKPACK* create_backpack();

void put_backpack_node_data(BACKPACK* bp, const char* name, int size_put, void* data, unsigned char type);
void* get_backpack_node_data(BACKPACK* bp, const char* name, int* size_put, unsigned char* type);

void write_backpack_file(BACKPACK* bp, const char* name);
void free_backpack(BACKPACK* bp);

#endif // BACKPACK_LIB_H
