/** \file fl_variable_token.c 
 * \brief this file cover the functions used to parse
 * input string and create list of tokens to build 
 * variables and rules
 */
#include "fl.h"
#include <stdio.h>
#include <string.h>
void release_token(t_fuzzvariable_token *o)
{
	free(o->value);
	free(o);
}

// utilisée par la fonction flrule_tokenize
// peut preter a confusion parce qu'on retourne un objet t_fuzzvariable_token 
// pour creer les regles, 
//
// et qu'il y a une definition 
// de fonction qui se nomme  create_token_var qui effectue la meme tache avec 
// quelques nuances
// 
//
/*! \brief voir methode #flrule_tokenize du fichier fl_rule_token.c 
 *
 */
t_fuzzvariable_token *create_token(char type, char *value)
{
	t_fuzzvariable_token *obj;
	char b[2];
	
	zero(b);
	*b = *value;
	obj = malloc(sizeof(*obj));
	memset(obj, 0, sizeof(*obj));
	obj->type = type;
	obj->value = strdup(b);
	return obj;
}
/*! \brief extract from token list the characteres 
 * used to define values
 *
 * @param[in] h Pointer on #t_listhead object containing token
 *
 */
void reduce_chars_token(t_listhead *h)
{
	t_fuzzvariable_token *obj;
	t_fuzzvariable_token *obj2;
	int i = 0;
	int j;
	char buff[64];
	char *ptr;

	zero(buff);
	ptr = buff;
	while ( i < (int) h->length)
	{
		obj = at_list(h, i);
		if (obj && obj->type == TYPE_CHAR)
		{
			j = i+1;
			if (j >= (int) h->length)
				break;
			*ptr = *(obj->value);
			ptr++;
			obj2 = at_list(h, j); 
			while (j < (int)h->length && obj2->type == TYPE_CHAR)
			{
				*ptr = *(obj2->value);
				ptr++;
				j++;
				obj2 = at_list(h, j); 
			}	
			j--;
			if (obj->value)
				free(obj->value);
			obj->value = strdup(buff);
			zero(buff);
			ptr = buff;
			if (i != j)
			{
				splice_list(h, i+1, (j-i));
			}
		}
		i++;
	}
}
// fonction a revoir
// b est un tableau de 2 caracteres 
// apres quoi on produit une allocation memoire en copiant ce tableau via strdup
t_fuzzvariable_token *create_token_var(char type, char *value)
{
	t_fuzzvariable_token *obj;
	char b[2];
	
	obj = malloc(sizeof(*obj));
	memset(obj, 0, sizeof(*obj));
	obj->type = type;
	if (type != TYPE_CHAR)
	{
		zero(b);
		*b = *value;
		obj->value = strdup(b);
	}
	else
	{
		obj->value = value;
	}
	return obj;
}
/*! \brief test if charactere is a #TOKEN_SPACE, a #TOKEN_SHARP or #TOKEN_COMMA
 *
 * @param[in] c the input charactere to test
 * @return 1 if it match token otherwise 0
 */
static int test_is_char(char c)
{
	static const char array_token [3]= {TOKEN_SPACE, TOKEN_SHARP, TOKEN_COMMA };
	int i = 0;
	while (i < 3)
	{
		if (c == array_token[i]) return 1;
		i++;
	}
	return 0;
}
/*! \brief parse input string , tokenize the datas and push them into list
 * @param[in] str the input string representing the datas
 * @param[in] h Pointer on head of the list #t_listhead to store tokens
 */
static void parse_string(char *str, t_listhead *h)
{
	char *buffer;
	int i = 0;
	int j = 0;
	int k = 0;
	int m;
	char *ptr;
	char *word;
	t_fuzzvariable_token *obj;

	buffer = strdup(str);
	i = strlen(buffer);
	ptr = buffer;
	// reduce multiple space to one space
	while (j < i)
	{
		if (buffer[j] == TOKEN_SPACE)
		{
			if (j + 1 < i && buffer[j + 1] == TOKEN_SPACE)
			{
				k = j + 1;
				while (k < i && buffer[k] == TOKEN_SPACE)
					k++;
				// [ .........]$
				// [ ..j...k...]
				m = (k - j) - 1;
				memcpy(buffer + j + 1, ptr + k, i - k);
				*(ptr + i - m) = 0;
			}
		}
		j++;
	}
	// remove space if begin with space
	i = strlen(buffer);
	if (*buffer == TOKEN_SPACE)
	{
		memcpy(buffer , buffer + 1, i);
		*(buffer + i - 1) = 0;
		i--;
	}
	ptr = buffer;
	j = 0;
	while (j < i)
	{
		obj = 0;
		// extract string
		if (!test_is_char(buffer[j]))
		{
			k = j + 1;
			while (k < i && !test_is_char(buffer[k]))
				k++;
			// [ .........]$
			// [ ..j...k...]
			m = (k - j);
			word = malloc(m + 1);
			memset(word, 0, m +1);
			memcpy(word, buffer + j, m);
			obj = create_token_var(TYPE_CHAR, word);
			j = k -1;	
		}
		else
		{
			m = buffer[j] == TOKEN_SPACE ? 1 : 0;
			if (m == 1)
			{
				obj = create_token_var(TYPE_SPACE, buffer + j);
			}
			m = buffer[j] == TOKEN_SHARP ? 1 : 0;
			if (m == 1)
			{
				obj = create_token_var(TYPE_LOGICAL, buffer + j);
			}
				m = buffer[j] == TOKEN_COMMA ? 1 : 0;
			if (m == 1)
			{
				obj = create_token_var(TYPE_COMMA, buffer + j);
			}
		}
		if (obj != 0)
			push_list(h, obj); 
		j++;
	}
	free(buffer);
}

/*! \brief parse input string , tokenize the datas and push them into list
 * @param[in] str the input string representing the datas
 * @param[in] h Pointer on head of the list #t_listhead to store tokens
 */
void flvariable_tokenize(char *str, t_listhead *h)
{
	t_fuzzvariable_token *obj;
	t_fuzzvariable_token *p;
	t_fuzzvariable_token *n;
	t_list *c;
	t_list *cc;
	int i = -1;

	parse_string(str, h);
	// REDUCE multiple space to one space
	// // exemple : "123 # TR" => "123#TR"
	c = h->head;
	while ( i < (int) h->length)
	{
		i++;
		n = 0;
		p = 0;
		obj = 0;
		if (c)
		{
			cc = c->next;
			obj = (t_fuzzvariable_token*) c->data;
			if (c->next)
				n = (t_fuzzvariable_token*) c->next->data;
			if (c->prev)
				p =(t_fuzzvariable_token*) c->prev->data;
			if (obj && obj->type == TYPE_SPACE)
			{
				if ((n && 
							(n->type == TYPE_SPACE || n->type == TYPE_LOGICAL || n->type == TYPE_COMMA))
						|| (p && (p->type == TYPE_LOGICAL || p->type == TYPE_COMMA))
				   )
				{
					splice_list(h, i, 1);
					i--;
				}
			}	

			c = cc;
		}
	}
	//print_list_fuzzvariable_token(h);
}

/*! \brief print list of tokens
 */
void print_list_fuzzvariable_token(t_listhead *h)
{
	printf("length: %zu\n",h->length);
	t_list *c;
	int count = 0;
	t_fuzzvariable_token *o;
	c = h->head;
	while (c)
	{
		o = (t_fuzzvariable_token*)c->data;
		printf("[%d] = [type:%d , value:'%s']\n",count, o->type, (o->value));
		count++;
		c = c->next;
	}
}

