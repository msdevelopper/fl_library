/** fl_fuzzylogic.c */

#include "fl.h"

static void	fl_release_rules(t_listhead *h)
{
	t_list *c;
	t_list *n;
	t_listhead *u;
	c = h->head;
	while (c)
	{
		n = c->next;
		u = (t_listhead *)c->data;
		u->release = release_rules;
		free_list(&u);
		free(c);
		c = n;
	}
	free(h);
}

t_fuzzylogic *fl_engine_create(void)
{
	t_fuzzylogic *fl;

	fl = malloc(sizeof(*fl));
	memset(fl, 0, sizeof(*fl));
	fl->vars = 0;
	//fl->rules = new_list(fl_release_rules);
	fl->rules = new_list(0);
	fl->vars_names = 0;
	fl->vars_output = 0;
	return fl;
}
void fl_engine_release(t_fuzzylogic **e)
{
	t_fuzzylogic *fl;

	fl = *e;
	if (fl == 0) return;

	free_list(&fl->vars_output);
	free_list(&fl->vars_names);
	free_list(&fl->vars);
	fl_release_rules(fl->rules);
	if (fl)
		free(fl);
	*e = 0;
}
t_fuzzvariable_obj *fl_variable_add(t_fuzzylogic *fl ,char *str)
{
	t_listhead *token;
	t_fuzzvariable_obj *var;

	t_list *c;
	t_list *cc;
	t_fuzzvariable_set *s;
	t_fuzzvariable_obj *o;
	t_listhead *names;
	t_listhead *varout;

	token = new_list(release_token);
	if (fl->vars == 0)
		fl->vars = new_list(release_vars);
	flvariable_tokenize(str, token);
	var = flvariable_create(fl->vars, token);
	free_list(&token);

	if (!var) 
	{
		fl->err_parsing = -1;
		return 0;
	}
	if (fl->vars_names == 0)
	{
		names = new_list(0);
		varout = new_list(0);
	}
	else
	{
		names = fl->vars_names;
		varout = fl->vars_output;
	}
	push_list(names, var->name);
	if (!strcmp(var->type, TOKEN_OUTPUT_VAR))
		push_list(varout, var);

	c = fl->vars->head;
	while (c)
	{
		o = (t_fuzzvariable_obj*)c->data;
		cc = o->subsets->head;
		while (cc)
		{
			s = (t_fuzzvariable_set*)cc->data;
			if (s->tsk_string != 0 && s->tsk == 0)
			{
				s->tsk = expr_container(s->tsk_string, names); 
				if (!s->tsk)
				{
					fl->err_parsing = -1;
					return 0;
				}
				o->subject_to_tsk = 1;
			}
			cc = cc->next;
		}
		c = c->next;
	}

	fl->vars_names = names;
	fl->vars_output = varout;
	
	return var;
}

static int does_var_and_subset_exist(t_listhead *vars, char *varname, char *subsetname)
{
	int exist;
	int hasbeenchecked;
	t_list *c;
	t_list *cc;
	t_fuzzvariable_obj *v;
	t_fuzzvariable_set *s;

	c = vars->head;
	hasbeenchecked = 0;
	while (c)
	{
		v = c->data;
		if (!strcmp(v->name, varname))
		{
			cc = v->subsets->head;
			exist = 0;
			while (cc)
			{
				s = cc->data;
				if (!strcmp(s->name, subsetname))
				{
					exist++;
				}
				cc = cc->next;
			}
			if (!exist) return 0;
			hasbeenchecked = 1;
		}
		else
		{
		}
		c = c->next;
	}
	return hasbeenchecked;
}

static int check_if_subset_exist_in_tree(t_listhead * vars, t_fuzzrule_obj *r)
{
	int exist;
	t_list *c;
	t_list *cc;
	t_fuzzvariable_obj *v;
	t_fuzzvariable_set *s;

	if (r && !r->subnodes[0] && !r->subnodes[1])
	{
		// on leaf
		return does_var_and_subset_exist(vars, r->name, r->subset);
	}
	exist = check_if_subset_exist_in_tree(vars, r->subnodes[0]);
	if (!exist) return 0;
	return check_if_subset_exist_in_tree(vars, r->subnodes[1]);
}
int fl_rules_add(t_fuzzylogic *fl ,char *str)
{
	t_listhead *token;
	t_listhead *objsrule;
	t_list *c;
	int test;
	t_fuzzrule_obj *r;

	token = new_list(0);
	token->release = release_token;
	objsrule = new_list(0);
	flrule_tokenize(str, token);
	flrule_create(objsrule, token);
	free_list(&token);

	// CHECK IF VAR EXIST!
	c = objsrule->head;
	while (c)
	{
		r = (t_fuzzrule_obj*)c->data;
		test = check_if_subset_exist_in_tree(fl->vars, r);
		if (test == 0) 
		{
			fl->err_parsing = 1;
			break;

		}
		c = c->next;
	}
	push_list(fl->rules, objsrule);
	return fl->err_parsing;
}
t_fuzzvariable_obj *fl_get_var_by_name(t_fuzzylogic *fl, char *name)
{
	t_fuzzvariable_obj *var;
	t_list *c;

	c = fl->vars->head;
	while (c)
	{
		var = (t_fuzzvariable_obj*)c->data;
		if (!strcmp(var->name, name))
		{
			return var;
		}

		c = c->next;
	}
	return 0;
}

void fl_fuzzyfy_var(t_fuzzylogic *fl, t_fuzzvariable_obj *v, double x)
{
	t_list *c;
	t_fuzzvariable_set *s;

	// var ref_value is used inside expr eval
	v->ref_value = x;
	c = v->subsets->head;
	while (c)
	{
		s = (t_fuzzvariable_set*)c->data;
		if (s->tsk != 0 && s->tsk_string != 0)
		{
			s->value = expr_container_eval(s->tsk,fl->vars);
		}
		else
		{
			s->value = s->fuzzyfy(x,s);
		}
		//		s->value = s->fuzzyfy(x,s);
		c = c->next;
	}
}
void fl_fuzzyfy_var_by_name(t_fuzzylogic *fl, char *str, double x)
{
	t_fuzzvariable_obj *o;

	o = fl_get_var_by_name(fl, str);
	if (o)
	{
		fl_fuzzyfy_var(fl, o, x);
	}
}


static double iter_tree(t_fuzzrule_obj *tree, t_listhead *flvars, t_fn fn)
{
	double a; double b;
	t_fuzzvariable_set *s;

	if (!tree) return 0;
	if (tree->type == TYPE_INPUT_VAR)
	{
		s = fn(tree, flvars);
		if (s)
		{
			return s->value;
		}
	}
	else if (tree->type == TYPE_EXP)
	{
		//	printf("tree->name %s\n", tree->name);
		//	print_list_fuzzrule_tree_obj(tree, 0);
		a = iter_tree(tree->subnodes[0], flvars, fn);
		//	printf("op %c a %f <= get a\n", tree->op, a);
		b = iter_tree(tree->subnodes[1], flvars, fn);
		//	printf("op %c a %f, b %f \n", tree->op, a,b);
		double test = tree->op == TOKEN_AND ? min(a,b) : max(a,b);
		//	printf("test %f\n", test);
		return test; //tree->op == TOKEN_AND ? min(a,b) : max(a,b);
	}
	return 0;
}
static t_fuzzvariable_set *getSubsetVar(t_fuzzrule_obj *elem, t_listhead *flvars)
{
	t_fuzzvariable_set *s;
	t_fuzzvariable_obj *var;
	t_list *c;
	t_list *cc;

	c = flvars->head;
	while (c)
	{
		var = (t_fuzzvariable_obj*)c->data;
		if (!strcmp(var->name, elem->name))
		{
			cc = var->subsets->head;
			while (cc)
			{
				s = (t_fuzzvariable_set*)cc->data;
				if (!strcmp(s->name, elem->subset))
					return s;
				cc = cc->next;
			}
		}
		c = c->next;
	}
	return 0;
}
void fl_get_values(t_fuzzylogic *fl)
{
	t_fuzzvariable_obj *var;
	t_fuzzvariable_set *s;
	t_list *c;
	t_list *cc;

	t_listhead *objsrule;
	t_fuzzrule_obj *r;
	double fuzzyvalue;

	if (fl->err_parsing) return;

	// set output variable sets to 0
	c = fl->vars->head;
	while (c)
	{
		var = (t_fuzzvariable_obj*)c->data;
		var->mult = 0;
		var->sum = 0;
		if (var->subject_to_tsk)
		{
			cc = var->subsets->head;
			while (cc)
			{
				s = (t_fuzzvariable_set*)cc->data;
				//	s->value = 0;
				if (s->tsk != 0 && s->tsk_string != 0)
				{

					s->value = expr_container_eval(s->tsk,fl->vars);

				}
				else
				{

				}
				cc = cc->next;
			}
		}
		c = c->next;
	}

	c = fl->rules->head;
	while (c)
	{

		objsrule = (t_listhead*)c->data;
		r = (t_fuzzrule_obj*)((objsrule)->head->data);
		r->membership = 0;
		fuzzyvalue = iter_tree(r, fl->vars , getSubsetVar);	
		cc = objsrule->head->next;
		while (cc)
		{
			r = (t_fuzzrule_obj*)cc->data;	
			r->membership = fuzzyvalue;
			//	printf("name:%s membership setted!\n",r->name);
			cc = cc->next;
		}
		c=c->next;
	}
	c = fl->rules->head;

	while (c)
	{
		objsrule = (t_listhead*)c->data;
		cc = objsrule->head->next;
		while (cc)
		{
			r = (t_fuzzrule_obj*)cc->data;	

			s = getSubsetVar(r, fl->vars);

			var = s->parent;
			//	printf("A rule name %s, rule mb %f, var name %s , var sum %f var mult %f, setname %s setvalue %f\n",r->name, r->membership, var->name, var->sum, var->mult, s->name, s->value);
			if (s->tsk == 0)
			{
				var->mult += (r->membership * s->center); 
			}
			else
			{

				var->mult += (r->membership * s->value); 
			}
			var->sum += r->membership;
			//	printf("B rule name %s, rule mb %f, var name %s , var sum %f var mult %f, setname %s setvalue %f\n",r->name, r->membership, var->name, var->sum, var->mult, s->name, s->value);
			cc = cc->next;
		}
		c=c->next;
	}

	c = fl->vars->head;
	while (c)
	{
		var = (t_fuzzvariable_obj*)c->data;
		if (var->mult - 0.00001 < var->sum
				&& var->mult + 0.00001 > var->sum)
		{
			var->sum = var->mult;
		}
		else if (var->sum < 0.00001 )
			var->sum = 0; 
		else
			var->sum = var->mult / var->sum;
		c = c->next;
	}
}
