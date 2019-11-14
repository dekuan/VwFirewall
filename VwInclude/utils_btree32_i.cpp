/*
*  utils_btree32_i.c
*
*  Author    shixuliang 2004-07-24
*  Copyright 2004-2006,shixuliang
*
*  Email:shixuliang2008@yahoo.com.cn
*  Msn:  shixl@neusoft.com
*
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "utils.h"
#include "utils_btree.h"
#include "utils_btree32_i.h"
#include "utils_bin.h"


bnode32_i *create_bnode32_i(int16 type)
{
	bnode32_i *node = NULL;
	
	if (type == BNODE32_I_ROOT) {
		node = (bnode32_i *)btree_malloc(BNODE32_I_ROOT_SIZE);
	}
	else if (type == BNODE32_I_LEAF) {
		node = (bnode32_i *)btree_malloc(BNODE32_I_LEAF_SIZE);
	}
	else {
		Assert(0);
	}
	
	node->num    = 0;
	node->pos    = 0;
	node->parent = NULL;
	
	if (type == BNODE32_I_ROOT) {
		memset((void *)&node->child, (int)NULL, sizeof(node->child));
	}
	else if (type == BNODE32_I_LEAF) {
		node->child[0] = NULL;
	}
	
	return node;
}

bnode32_i *search_btree32_i(bnode32_i *node, int32 key, bool *found, int16 *pos)
{
	ENTRY_FUNCTION("search_btree32_i", search_btree32_i);
	
	Assert(node != NULL);
	Assert(found != NULL);
	Assert(pos != NULL);
	
start:
	if (bin_search32(node->key, key, node->num - 1, pos)) {
		/* find the key in the current node */ 
		*found = true;
		return node;
	}
	
	if (node->child[0] == NULL) {
        /*
	* current node is leaf node,
	* search to leaf node no found
	* the key 
		*/
		*found = false;
		return node;
	}
	/* search child node */
	node = node->child[*pos];
	goto start;
}


static void insert_none32_i(bnode32_i *node, bnode32_i *new,
			    bnode32_i *son, void *index, int32 key, int16 pos)
{
	int16 i;
	
	Assert(node != NULL);
	Assert(pos >= 0);
	
	if (node->num > pos) {
		btree_memcpy(&(node->key[pos + 1]), &(node->key[pos]),
			(node->num - pos) * sizeof(int32));
		
		btree_memcpy(&(node->index[pos + 1]), &(node->index[pos]),
                        (node->num - pos) * sizeof(void*));
		
		if (node->child[0] != NULL) {
			/* none leaf node */
			btree_memcpy(&(node->child[pos + 2]), &(node->child[pos + 1]),
				(node->num - pos) * sizeof(bnode32_i*));
			for(i = 2; i < node->num - pos + 2; i++)
				node->child[i + pos]->pos = i + pos;
		}
	}
	
	node->key[pos] = key;
	node->index[pos] = index;
	
	if (node->child[0] != NULL) {
		son->parent = node;
		node->child[pos + 1] = son;
		son->pos = pos + 1;
	}
	node->num++;
}

static void insert_middle32_i(bnode32_i *node, bnode32_i *new,
			      bnode32_i *son, void *index, void **upindex, int32 key, int32 *upkey, int16 pos)
{
	int16 i, m = BNODE32_I_MAX/2;
	
	Assert(node != NULL);
	Assert(new != NULL);
	Assert(upkey != NULL);
	Assert(upindex != NULL);
	Assert(pos >= 0);
	
	btree_memcpy(&(new->key[0]), &(node->key[m]), m * sizeof(int32));
	btree_memcpy(&(new->index[0]), &(node->index[m]), m * sizeof(void*));
	
	if (node->child[0] != NULL) {
		
		/* none leaf node */
		
		son->parent = new;
		new->child[0] = son;
		son->pos = 0;
		
		btree_memcpy(&(new->child[1]), &(node->child[m + 1]),
			m * sizeof(bnode32_i*));
		
		for(i = 0; i < m; i++) {
			new->child[i + 1]->parent = new;
			new->child[i + 1]->pos = i + 1;
		}
	}
	
	node->num = m;
	new->num = m;
	
	*upkey = key;
	*upindex = index;
}

static void insert_left32_i(bnode32_i *node, bnode32_i *new,
			    bnode32_i *son, void *index, void **upindex, int32 key, int32 *upkey, int16 pos)
{
	int16 i, m = BNODE32_I_MAX/2;
	int32 ckey;
	void  *cindex;
	
	Assert(node != NULL);
	Assert(new != NULL);
	Assert(upkey != NULL);
	Assert(upindex != NULL);
	Assert(pos >= 0);
	
	btree_memcpy(&(new->key[0]), &(node->key[m]), m * sizeof(int32));
	btree_memcpy(&(new->index[0]), &(node->index[m]), m * sizeof(void*));
	if (node->child[0] != NULL) {
		btree_memcpy(&(new->child[0]), &(node->child[m]),
			(m + 1) * sizeof(bnode32_i*));
		
		for(i = 0; i < m + 1; i++) {
			new->child[i]->pos = i;
			new->child[i]->parent = new;
		}
	}
	
	ckey = node->key[m - 1];
	cindex = node->index[m - 1];
	
	if (m > pos + 1) {
		btree_memcpy(&(node->key[pos + 1]), &(node->key[pos]),
			(m - (pos + 1)) * sizeof(int32));
		btree_memcpy(&(node->index[pos + 1]), &(node->index[pos]),
                        (m - (pos + 1)) * sizeof(void*));
		
		if (node->child[0] != NULL) {
			btree_memcpy(&(node->child[pos + 2]), &(node->child[pos + 1]),
				(m - (pos + 1)) * sizeof(bnode32_i*));
			
			for(i = 0; i < m - (pos + 1); i++) {
				node->child[i + pos + 2]->pos = i + pos + 2;
			}
		}
	}
	
	node->key[pos] = key;
	node->index[pos] = index;
	
	if (node->child[0] != NULL) {
		node->child[pos + 1] = son;
		son->pos = pos + 1;
		son->parent = node;
	}
	
	node->num = m;
	new->num = m;
	
	*upkey = ckey;
	*upindex = cindex;
}



static void insert_right32_i(bnode32_i *node, bnode32_i *new,
			     bnode32_i *son, void *index, void **upindex, int32 key, int32 *upkey, int16 pos)
{
	int16 i, m = BNODE32_I_MAX/2;
	
	Assert(node != NULL);
	Assert(new != NULL);
	Assert(upkey != NULL);
	Assert(upindex != NULL);
	Assert(pos >= 0);
	
	if (pos > m + 1) {
		btree_memcpy(&(new->key[0]), &(node->key[m + 1]), (pos - (m + 1)) * sizeof(int32));
		btree_memcpy(&(new->index[0]), &(node->index[m + 1]), (pos - (m + 1)) * sizeof(void*));
		if (node->child[0] != NULL) {
			btree_memcpy(&(new->child[0]), &(node->child[m + 1]),
				(pos - (m + 1)) * sizeof(bnode32_i*));
			
			for(i = 0; i < pos - (m + 1); i++) {
				new->child[i]->pos = i;
				new->child[i]->parent = new;
			}
		}
	}
	
	new->key[pos - (m + 1)] = key;
	new->index[pos - (m + 1)] = index;
	
	if (node->child[0] != NULL) {
		new->child[pos - (m + 1)] = node->child[pos];
		new->child[pos - (m + 1)]->pos = pos - (m + 1);
		new->child[pos - (m + 1)]->parent = new;
		
		new->child[pos - m] = son;
		son->pos = pos - m;
		son->parent = new;
	}
	if (pos < node->num) {
		btree_memcpy(&(new->key[pos - m]), &(node->key[pos]),
			(node->num - pos) * sizeof(int32));
		btree_memcpy(&(new->index[pos - m]), &(node->index[pos]),
                        (node->num - pos) * sizeof(void*));
		
		if (node->child[0] != NULL) {
			btree_memcpy(&(new->child[pos - m + 1]), &(node->child[pos + 1]),
				(node->num - pos) * sizeof(bnode32_i*));
			
			for(i = 0; i < node->num - pos; i++) {
				new->child[i + pos - m + 1]->pos = i + pos - m + 1;
				new->child[i + pos - m + 1]->parent = new;
			}
		}
	}
	
	node->num = m;
	new->num = m;
	
	*upkey = node->key[m];
	*upindex = node->index[m];
}


void insert_btree32_i(bnode32_i *node, void *index, int32 key, int16 pos)
{
	bnode32_i *son = NULL;
	bnode32_i *new = NULL;
	int16       m = BNODE32_I_MAX/2;
	
	Assert(node != NULL);
	Assert(pos >= 0);
	
start: 
	/* insert the ip without divide */
	if (node->num != BNODE32_I_MAX) {
		insert_none32_i(node, new, son, index, key, pos);
		return;
	}
	
	/* insert the ip with divide */
	
	if (node->parent == NULL) {
		/* create new root node */
		node->parent = create_bnode32_i(BNODE32_I_ROOT);
		node->parent->child[0] = node;
		node->pos = 0;
	}
	
	/* create new divide node */
	if (node->child[0] == NULL) {
		new = create_bnode32_i(BNODE32_I_LEAF);
	}
	else {
		new = create_bnode32_i(BNODE32_I_ROOT);
	}
	
	/* insert middle and divide */
	if (pos == m) {
		insert_middle32_i(node, new, son, index, &index, key, &key, pos);
	}
	/* insert left and divide */
	if (pos < m) {
		insert_left32_i(node, new, son, index, &index, key, &key, pos);
	}
	/* insert right and divide */
	if (pos > m) {
		insert_right32_i(node, new, son, index, &index, key, &key, pos);
	}
	
	pos  = node->pos;
	node = node->parent;
	son  = new;
	goto start;
}