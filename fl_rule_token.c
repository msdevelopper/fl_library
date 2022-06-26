/** \file fl_rule_token.c
 *
 * \brief This file contains functions definition
 * that process the creation of rules.
 * 
 * a #t_fuzzrule_obj is an object 
 * which is processed as a list and as a binary tree
 * to infere the rules.
 *
 * the first node of the tree is called 'root'
 * as a reserved word
 * we should warn user about name of variable
 */
#include "fl.h"
#include <stdio.h>
#include <string.h>

/*! \brief Create a new #t_fuzzrule_obj object
 * @param[in] name The string representation for the name of the object
 * @param[in] type The character which represent the type of the object
 */
t_fuzzrule_obj *create_rule(char *name, char type)
{
	t_fuzzrule_obj *r;

	r = malloc(sizeof(*r));
	memset(r, 0, sizeof(*r));
	r->type = type;
	r->name = name ? strdup(name) : strdup("root");
	return r;
}
/*! \brief recursive function to travel in depth of the tree 
 * and release memory allocated
 * @param[in] r Pointer on the rule object to free
 */
void release_tree(t_fuzzrule_obj *r)
{
	t_fuzzrule_obj *p;
	t_fuzzrule_obj *n;

	if (!r) return;
	p = r->subnodes[0];
	n = r->subnodes[1];
	free(r->name);
	free(r);
	release_tree(p);
	release_tree(n);
}
/*! \brief Release memory allocated for the object
 * @param[in] r Pointer on the rule object to free
 */
void release_rules(t_fuzzrule_obj *r)
{
	t_fuzzrule_obj *p;
	t_fuzzrule_obj *n;

	p = r->subnodes[0];
	n = r->subnodes[1];
	if (p)
		release_tree(p);
	if (n)
		release_tree(n);
	free(r->name);
	free(r);
}
/*! \brief Print the content of rules object
 * @param[in] o Pointer on the rule object
 * @param[in] depth A value to increment and display the depth of the tree
 */
void print_list_fuzzrule_tree_obj(t_fuzzrule_obj *o, int depth)
{
	int d = depth +1;
	if (!o) return;
	//	printf("\t[tree] = [type:%d , name:'%s' , subset: '%s', op:'%c']\n", o->type, (o->name), o->subset, o->op);
	while (--d)
		printf("\t");
	printf("[tree] = [type:%d , name:'%s' , subset: '%s', op:'%c']\n", o->type, (o->name), o->subset, o->op);
	if (o->type == TYPE_INPUT_VAR)
	{

	}
	else if (o->type == TYPE_EXP)
	{
		print_list_fuzzrule_tree_obj(o->subnodes[0], depth +1);
		print_list_fuzzrule_tree_obj(o->subnodes[1], depth +1);
	}
}
/*! \brief Print all the rules into list
 * @param[in] h Pointer on the head of the list for which we print objects
 */
void print_list_fuzzrule_obj(t_listhead *h)
{
	printf("length: %zu\n",h->length);
	t_list *c;
	t_fuzzrule_obj *o;
	int count = 0;

	c = h->head;
	while (c)
	{
		o = (t_fuzzrule_obj*)c->data;
		printf("[%d] = [type:%d , name:'%s' , subset: '%s', op:'%c' s[0]=%p s[1]=%p]\n",count, o->type, (o->name), o->subset, o->op, o->subnodes[0] , o->subnodes[1]);
		if (o->type == TYPE_EXP)
		{
			// run tree
			printf("runnning tree\n");
			print_list_fuzzrule_tree_obj(o, 0);
		}

		count++;
		c = c->next;
	}
}
/*! \brief Build the tree, assigning memory reference to object 
 * and release memory associated with tokens
 * @param[in] h Pointer on the head of list that contain tokens
 */
static void build_tree(t_listhead * h)
{

	t_fuzzrule_obj *e;
	t_fuzzrule_obj *p;
	t_fuzzrule_obj *n;
	t_fuzzrule_obj *node;
	int i;

	i = (int)h->length;
	while (--i > -1)
	{
		e = at_list(h,i);
		p = at_list(h,i-1);
		n = at_list(h,i+1);
		if (!p || !n) continue;
		if (e && (e->type == TYPE_INPUT_VAR || e->type == TYPE_EXP))
		{
			if (p->type == TYPE_PARENTHESIS && n->type == TYPE_PARENTHESIS)
			{
				if (*(p->name) == TOKEN_OPEN_P && *(n->name) == TOKEN_CLOSE_P)
				{
					splice_list(h, i+1, 1);
					splice_list(h, i-1, 1);
					i = (int)h->length;
				}
			}
		}
		else if (e && e->type == TYPE_WORD_OP)
		{
			if (
					(p->type == TYPE_INPUT_VAR || p->type == TYPE_EXP)
					&&
					(n->type == TYPE_INPUT_VAR || n->type == TYPE_EXP)
			   )
			{
				node = create_rule(0, TYPE_EXP);	
				node->subnodes[0] = p;
				node->subnodes[1] = n;
				node->op = *(e->name);
				release_rules(e);
				h->release = 0;
				splices_list(h, i-1, 3, node, 0);
				h->release = (void(*)(void*)) release_rules;
				i = (int)h->length;

			}
		}
	}
	//print_list_fuzzrule_obj(h);
//	free_list(&h);
}
/*! \brief Build the tree, assigning memory reference to object 
 * and release memory associated with tokens
 * @param[in] h Pointer on the head of list for wich we store the #t_fuzzrule_obj object
 * @param[in] token Pointer on the head of list where tokens are stored and processed
 */
void flrule_create(t_listhead *h , t_listhead * token)
{
	t_fuzzvariable_token *obj;
	t_fuzzrule_obj *r;
	int i ;
	int len; 
	int meet_consequent;

	meet_consequent = 0;
	len = (int)token->length;
	i = 0;
	r = 0;
	while (i < len)
	{
		obj = at_list(token, i);
		if (*(obj->value) == TOKEN_DOUBLE_COLON)
		{
			meet_consequent = 1;
		}
		else if (obj->type == TYPE_PARENTHESIS)
		{
			push_list( h, create_rule(obj->value , obj->type));
		}
		else if (obj->type == TYPE_CHAR)
		{
			if (r == 0)
			{
				r = create_rule(obj->value, obj->type);
			}
			else
			{
				if (meet_consequent == 1)
					r->type = (char)TYPE_OUTPUT_VAR;
				else
					r->type = (char) TYPE_INPUT_VAR;
					
				zero(r->subset);
				memcpy(r->subset, obj->value, strlen(obj->value));
				push_list(h, r);
				r = 0;
			}
			i++;
		}
		//else if (*obj->value != TOKEN_COMMA)
		else if (*(obj->value) == TOKEN_AND || *(obj->value) == TOKEN_OR)
		{
			push_list(h, create_rule(obj->value , TYPE_WORD_OP));

		}
		i++;
	}
	//print_list_fuzzrule_obj(h);
	build_tree(h);
	//print_list_fuzzrule_tree_obj((t_fuzzrule_obj *)h->head->data, 0);
}

/*! \brief Parse the input string and tokenize it into list
 * @param[in] str The input string
 * @param[in] h Pointer on the head of #t_listhead object were we store the datas 
 */
void flrule_tokenize(char *str, t_listhead *h)
{
	t_fuzzvariable_token *obj;
	t_list *c;
	t_list *cc;
	int test;
	char type;
	int i ;

	while (str && *str)
	{
		test = char_indexof(TOKEN_LANG_ARRAY, sizeof(TOKEN_LANG_ARRAY), *str);
		type = -1;
		if (test != -1)
			type = *str;
		else if (*str == TOKEN_SPACE)
			type = TYPE_SPACE;
		else if (*str == TOKEN_OPEN_P || *str == TOKEN_CLOSE_P)
			type = TYPE_PARENTHESIS;
		else
			type = TYPE_CHAR;
		push_list(h, create_token(type, str));
		str++;
	}
	// REDUCE multiple space to one space
	c = h->head;
	i = -1;
	while ( i < (int) h->length)
	{
		i++;
		obj = 0;
		if (c)
		{
			cc = c->next;
			obj = (t_fuzzvariable_token*) c->data;
			if (obj && obj->type == TYPE_SPACE)
			{
				splice_list(h, i, 1);
				i--;
			}	

			c = cc;
		}
	}
	// remove space if begin with space
	if ((int)h->length > 1 && ((t_fuzzvariable_token*)(h->head->data))->type == TYPE_SPACE)
		shift_list(h);
	// set multiples chars to one string
	reduce_chars_token(h);
}
