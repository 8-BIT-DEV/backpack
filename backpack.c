#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <backpack.h>

#define BACKPACK_version 0x2202

BACKPACK* load_backpack_file(const char* name)
{	
	FILE* f = fopen(name, "rb");
	
	if(f == NULL) return NULL;
	
	char magic[12];
	magic[10] = '\0';
	
	fread(magic, strlen(".BACK.PACK"), 1, f);
	
	if(strcmp(magic, ".BACK.PACK") != 0)
	{
		printf("INVALID MAGIC NUMBER\n");
		return NULL;
	}
	
	uint16_t version;
	fread(&version, sizeof(uint16_t), 1, f);
	
	if(version != BACKPACK_version)
	{
		printf("WRONG VERSION!\n");
		return NULL;
	}
	
	BACKPACK* bp = create_backpack();
	
	int node_count = -1;
	fread(&node_count, sizeof(int), 1, f);
	bp->count = 0;
	printf("NODE COUNT: %d\n", node_count);
	for(int i = 0; i < node_count; i++)
	{
		int chunk_number;
		fread(&chunk_number, sizeof(int), 1, f);
		if(chunk_number != i) {free_backpack(bp); return NULL;}
		
		unsigned char typeh;
		int size;
		
		fread(&typeh, 1, 1, f);
		fread(&size, sizeof(int), 1, f);
		
		char nch = 'A';
		char* name = malloc(48);
		int nmax = 48;
		strcpy(name, "");
		
		while(nch != '\0')
		{
			fread(&nch, 1, 1, f);
			int tlen = strlen(name);
			name[tlen] = nch;
			name[tlen+1] = '\0';
			if(tlen >= nmax-2)
			{
				char* oname = malloc(nmax*2);
				strcpy(oname, name);
				free(name);
				name = oname;
				printf("Move max up\n");
			}
		}
		
		void* data = malloc(size);
		fread(data, size, 1, f);
		
		put_backpack_node_data(bp, name, size, data, typeh);
		
		//printf("NAME1: %p, VALUE: %p :: %d\n", bp->nodes[i].name, bp->nodes[i].data, i);
		printf("NAME1: %s\n", bp->nodes[i].name);
		//free(name);
	}
	
	fclose(f);
	
	return bp;
}

BACKPACK* create_backpack()
{
	BACKPACK* bp = malloc(sizeof(BACKPACK));
	bp->cap = 8;
	bp->count = 0;
	bp->written = false;
	bp->nodes = malloc(sizeof(BP_NODE)*8);
	return bp;
}


void put_backpack_node_data(BACKPACK* bp, const char* name, int size_put, void* data, unsigned char type)
{
	#define CURRENT_WRITE bp->count	
	
	if(bp->count + 1 >= bp->cap)
	{
		bp->cap *= 2;
		BP_NODE* nnodes = malloc(sizeof(BP_NODE)*bp->cap);
		memcpy(nnodes, bp->nodes, sizeof(BP_NODE) * (bp->cap/2));
		free(bp->nodes);
		bp->nodes = nnodes;
		printf("EXPANDED TO FIT NODE IN\n");
	}
	
	bp->nodes[CURRENT_WRITE].name = malloc(strlen(name)+1);
	strcpy(bp->nodes[CURRENT_WRITE].name, name);
	bp->nodes[CURRENT_WRITE].type = type;
	bp->nodes[CURRENT_WRITE].size = size_put;
	bp->nodes[CURRENT_WRITE].data = malloc(bp->nodes[CURRENT_WRITE].size);
	memcpy(bp->nodes[CURRENT_WRITE].data, data, bp->nodes[CURRENT_WRITE].size);
	
	bp->count++;
	
	#undef CURRENT_WRITE
}

void* get_backpack_node_data(BACKPACK* bp, const char* name, int* size_put, unsigned char* type)
{
	for(int i = 0; i < bp->count; i++)
	{
		if(strcmp(bp->nodes[i].name, name) == 0)
		{
			if(size_put != NULL) *size_put = bp->nodes[i].size;
			if(type != NULL) *type = bp->nodes[i].type;
			return bp->nodes[i].data;
		}
	}
	
	return NULL;
}


void write_backpack_file(BACKPACK* bp, const char* name)
{
	uint16_t vnum = BACKPACK_version;
	
	FILE* f = fopen(name, "wb");
	
	fwrite(".BACK.PACK", strlen(".BACK.PACK"), 1, f);
	fwrite(&vnum, sizeof(uint16_t), 1, f);
	fwrite(&bp->count, sizeof(int), 1, f);
	
	for(int i = 0; i < bp->count; i++)
	{
		fwrite(&i, sizeof(int), 1, f);
		fwrite(&bp->nodes[i].type, sizeof(unsigned char), 1, f);
		fwrite(&bp->nodes[i].size, sizeof(int), 1, f);
		fwrite(bp->nodes[i].name, strlen(bp->nodes[i].name)+1, 1, f);
		fwrite(bp->nodes[i].data, bp->nodes[i].size, 1, f);
		free(bp->nodes[i].name);
		free(bp->nodes[i].data);
	}
	
	fclose(f);
	bp->written = true;
}

void free_backpack(BACKPACK* bp)
{
	if(!bp->written)
	{
		for(int i = 0; i < bp->count; i++)
		{
			free(bp->nodes[i].name);
			free(bp->nodes[i].data);
		}
	}
	free(bp);
}
