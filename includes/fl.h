/** \file fl.h */
#ifndef FL_H
#define FL_H

#include "common.h"
#include "fl_token.h"
#include "expr.h"

/*! \typedef t_fuzzvariable_token
 * \brief structure to parse string and construct the tokens associated 
 */
typedef struct s_fuzzvariable_token
{
	char type;	/**< the type of token it does represent */
	char *value;	/**< the string representation of value */
}t_fuzzvariable_token;
/*! \typedef t_fuzzvariable_set
 * \brief this structure represent a subset defined by user (S,G,TR..)
 */
typedef struct s_fuzzvariable_set t_fuzzvariable_set;
/*! \typedef t_fuzzvariable_obj
 * \brief structure of an in or out variable
 */
typedef struct s_fuzzvariable_obj t_fuzzvariable_obj;

struct s_fuzzvariable_set
{
	t_expr_container *tsk;	/**< the pointer onto object representing TSK/math expression to evalute */
	char *tsk_string;	/**< the string representation of the expression*/
	t_fuzzvariable_obj *parent;	/**< pointer on variable definition */

	char *name;		/**< name of this subset */
	double range[4];	/**< points defining the curve */
	char *wave;		/**< string representation of subset type*/
	double value;
	double min;		/**< the most lower value of point representing the curve*/
	double max;		/**< the most greatest value of point representing the curve*/
	double center;		/**< the value representing the center of area (COG-center of gravity) */
	double (*fuzzyfy)(double x, t_fuzzvariable_set *set); /**< pointer on function to call when fuzzyfy'ing value */
};

struct s_fuzzvariable_obj
{
	char *type;		/**< string representation of in or out variable */
	char *name;		/**< the name of the variable choosed by user */
	t_listhead *subsets;	/**< pointer on list of subset defined #t_fuzzvariable_set */
	double mult;		/**< result of inference multiplication */
	double sum;		/**< sum of inference */
	double ref_value;	/**< reference value , used internally to keep track of data */
	int 	subject_to_tsk; /**< set to 1 if the preset rely on TSK expression otherwise 0 */
};
/*! \typedef t_fuzzrule_obj
 * \brief structure of a rules 
 */
typedef struct s_fuzzrule_obj t_fuzzrule_obj;

struct s_fuzzrule_obj{
	char type;		/**< the type it does represent */
	char op;		/**< operator to evaluate expression */
	double membership;	/**< degree of membership triggering */
	char *name;		/**< string representation of the rule */
	char subset[64];	/**< string representation of subset associated */
	t_fuzzrule_obj *subnodes[2];	/**< pointer onto child nodes */
};
/*! \typedef t_fuzzylogic
 * \brief
 */
typedef struct s_fuzzylogic t_fuzzylogic;
struct s_fuzzylogic {
	t_listhead *vars;	/**< list of all variables (in and out) */
	t_listhead *vars_names;	/**< list of the names of all variables */
	t_listhead *vars_output;/**< list of out variables */
	t_listhead *rules;	/**< list of rules */
	int	err_parsing;	/**< return code to signal when error occured ( it does not keep track point of failure - should be implemented in future ) */
};
// fl_fuzzylogic.c
// for iter_tree
/*! \typedef t_fn
 * \brief function pointer 
 * 	@param[in] el fuzzyrule element 
 * 	@param[in] flvars list of fuzzylogic variables defined
 * 	@return pointer onto variable subset
 */
typedef t_fuzzvariable_set *(*t_fn)(t_fuzzrule_obj *el, t_listhead *flvars);

void 			fl_get_values(t_fuzzylogic *fl);
t_fuzzvariable_obj 	*fl_get_var_by_name(t_fuzzylogic *fl, char *name);
void 			fl_fuzzyfy_var(t_fuzzylogic *fl, t_fuzzvariable_obj *v, double x);
void 			fl_fuzzyfy_var_by_name(t_fuzzylogic *fl, char *str, double x);
int 			fl_rules_add(t_fuzzylogic *fl ,char *str);
t_fuzzvariable_obj 	*fl_variable_add(t_fuzzylogic *fl ,char *str);
t_fuzzylogic 		*fl_engine_create(void);
void 			fl_engine_release(t_fuzzylogic **e);

#if defined(_WIN32)
#endif
#if defined(UNIX)
#else
#ifdef __cplusplus
extern "C"
{
#endif
/*! \addtogroup dll_exposed_functions
 * @{
 * \brief declaration of functions exposed in DLL 
 * to use with external program 
 */
#define DllExport __declspec(dllexport)
#define DllImport __declspec(dllimport)
	typedef void (*cb_fl_result)(const char *name, int sizename, double value);
	DllExport int fl_engine_test(char **s, int size);
	DllExport int fl_engine_init(void);
	DllExport int fl_engine_variable_add(char *str, int size);
	DllExport int fl_engine_variable_adds(char **str, int size);
	DllExport int fl_engine_rules_add(char *str, int size);
	DllExport int fl_engine_rules_adds(char **str, int size);
	DllExport int fl_engine_fuzzyfy_var_by_name(char *str, double x);
	DllExport int fl_engine_get_results(cb_fl_result cb);
	DllExport int fl_engine_free(void);



	typedef void (*cb_fl_get_var)(const char *type,const char *name, const char*subsetwave, const char *subsetname, double range[4], double data[4]);
	DllExport int fl_engine_get_vars_list(cb_fl_get_var cb);


	DllExport double fl_engine_fuzzyfy_set(char *varname, char *setname, double x);


	typedef void (*cb_fl_get_rules_membership)(int idx, char *name, char *setname, double mb);
	DllExport void fl_engine_get_rules_membership(cb_fl_get_rules_membership cb);
/*! @}*/

#ifdef __cplusplus
}
#endif
#endif


//
//
// fl_fuzzyfy_methods.c
void 	get_set_domain(t_fuzzvariable_set *s);
double centroideMethod(t_fuzzvariable_set *mf);
double triangle_fuzzyfy(double x, t_fuzzvariable_set *this);
double trapez_fuzzyfy(double x, t_fuzzvariable_set *this);
double gaussian_fuzzyfy(double x, t_fuzzvariable_set *this);
double sigmoide_fuzzyfy(double x, t_fuzzvariable_set *this);
double beginsigmoide_fuzzyfy(double x, t_fuzzvariable_set *this);
double endsigmoide_fuzzyfy(double x, t_fuzzvariable_set *this);
double pshape_fuzzyfy(double x, t_fuzzvariable_set *this);

/*! \typedef t_fuzzyfy
 * \brief function pointer
 * 	@param[in] x value to fuzzyfy
 * 	@param[in] set pointer onto subset to fuzzyfy the value
 *
 * 	@return fuzzyfied value
 */
typedef double (*t_fuzzyfy)(double x, t_fuzzvariable_set *set);

static t_fuzzyfy FUZZ_METHODS[8]={
	0, // TSK ex
	trapez_fuzzyfy,
	triangle_fuzzyfy,
	gaussian_fuzzyfy,
	sigmoide_fuzzyfy,
	beginsigmoide_fuzzyfy,
	endsigmoide_fuzzyfy,
	pshape_fuzzyfy
};

// fl_rule_token.c
void flrule_create(t_listhead *l , t_listhead * token);
void release_rules(t_fuzzrule_obj *r);

void print_list_fuzzrule_obj(t_listhead *h);

// fl_variable_token.c
void flvariable_tokenize(char *str, t_listhead *h);
t_fuzzvariable_token *create_token(char type, char *value);
void reduce_chars_token(t_listhead *h);
void release_token(t_fuzzvariable_token *o);

void print_list_fuzzvariable_token(t_listhead *h);

void flrule_tokenize(char *str, t_listhead *h);

// fl_variable.c
t_fuzzvariable_obj *flvariable_create(t_listhead *list, t_listhead *token);
void release_vars(t_fuzzvariable_obj *v);
void print_list_fuzzvariable_obj(t_listhead *h);
#endif
