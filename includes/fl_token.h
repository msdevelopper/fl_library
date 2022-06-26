/** \file fl_token.h */
#ifndef FL_TOKEN_H
#define FL_TOKEN_H
#include "common.h"
/*! \def TOKEN_ROOT 
 * \brief this token represent the root node 
 * of builded tree, it is internally used 
 * \attention **sould not** be written by user to express
 * name of variable or subset
 */
#define TOKEN_ROOT		'$'	
/*! \addtogroup separator
 * @{
 * \brief list of tokens that are used to
 * build definition of input,output variables  
 * and rules
 */
#define TOKEN_SPACE 		 ' '	/**< whitespace to separate words */
#define TOKEN_OPEN_C 		 '['	/**< not used */
#define TOKEN_CLOSE_C		 ']'	/**< not used */
#define TOKEN_OPEN_B 		 '{'	/**< start token to write subset when defining rules */
#define TOKEN_CLOSE_B		 '}'	/**< end token to write subset when defining rules */
#define TOKEN_OPEN_P 		 '('	/**< start token to embrace expression when defining rules */
#define TOKEN_CLOSE_P 		 ')'	/**< end token to embrace expression when defining rules */
#define TOKEN_DOUBLE_COLON 	 ':'	/**< separator between input and ouput expression when defining rules */
#define TOKEN_SEMI_COLON 	';'	/**< not used - have to check*/
#define TOKEN_COMMA 		 ','	/**< separator for definition of subset parameters & TSK mathematical expression */
#define TOKEN_DOT 		 '.'	/**< separator for floating number */
/*! @} */

/*!  \addtogroup logical_operator
 * @{
 * \brief available logical operators 
 */
#define TOKEN_AND 		 '&'	/**< logical AND operator */
#define TOKEN_OR 		 '|'	/**< logical OR operator */
/*! @} */

/*! \addtogroup mathematical_operator
 * @{
 * \brief mathematical operator that can be written in expression
 * 
 * note : modulus, exponent are not implemented yet
 *
 */
#define TOKEN_MINUS 		 '-'
#define TOKEN_PLUS 		 '+'
#define TOKEN_MULTIPLICATOR 	 '*'
#define TOKEN_DIVISOR 		 '/'
#define TOKEN_MODULUS 		 '%'
#define TOKEN_POWER 		 '^'
#define TOKEN_EXPONENT 		 'e'
/*! @} */


/*! \brief this token is used to separate
 * the definition between subset
 */
#define TOKEN_SHARP		 '#'
/*! \brief left and right shift
 *
 * note : note implemented yet
 */
#define TOKEN_LSHIFT		 '<'
#define TOKEN_RSHIFT		 '>'
/*! \addtogroup expr_type
 * @{
 * \brief thoses tokens represents
 * the #type of a node to internally build the tree
 * when parsing definition of in-out variables 
 */
#define TYPE_NOT_COVERED	 0		/**< when token not recognised */
#define TYPE_EXP 		 1L		/**< represent an expression */
#define TYPE_PRIMITIVE 	 	2L		/**< represent a number */
#define TYPE_PARENTHESIS 	 3L		/**< match an open or close parenthesis */
#define TYPE_MATH 	 	4L		/**< represent a math expression */
#define TYPE_CHAR		5L		/**< represent a character for variable or subset definition*/
#define TYPE_FN 		 6L 		/**< represent a function defined in #TOKEN_FUNC array */
#define TYPE_FN_PARAM_SEPARATOR		7L	/**< represent a separator used for parameter inside a function definition*/
#define TYPE_SPACE 	 8L			/**< represent white space */
#define TYPE_LOGICAL 	 9L			/**< represent logical operator */
#define TYPE_COMMA 	 10L			/**< represent a comma */
#define TYPE_WORD_OP	11L			
#define TYPE_INPUT_VAR 	12L			/**< represent an input var*/
#define TYPE_OUTPUT_VAR 13L			/**< represent an output var*/
/*! @} */


#define TOKEN_PARENTHESE 	"()"

/*! \addtogroup token_subset
 * @{
 *
 * \brief definition of in-out vars 
 * and subsets type, curves available for user
 *
 */
#define TOKEN_INPUT_VAR 		 "I"	/**< def of input var*/
#define TOKEN_OUTPUT_VAR 		 "O"	/**< def of output var*/
#define TOKEN_W_TRIANGLE 		 "TR"	/**< def of triangle shape*/
#define TOKEN_W_TRAPEZE 		 "TP"	/**< def of trapez shape*/
#define TOKEN_W_GAUSSIAN 		 "G"	/**< def of gaussian shape*/
#define TOKEN_W_SIGMOIDE 		 "S"	/**< def of sigmoide shape*/
#define TOKEN_W_ENDSIGMOIDE	 "ES"		/**< def of sigmoide type shape*/
#define TOKEN_W_BEGINSIGMOIDE	 "BS"		/**< def of sigmoide type shape*/
#define TOKEN_W_PSHAPE		 "P"		/**< def of pshape*/
#define TOKEN_W_TSK 		 "TSK"		/**< def of TakagiSugenoKang math expression*/
/*! @} */


static char *TOKEN_WAVES[8]={
	(TOKEN_W_TSK),
	(TOKEN_W_TRAPEZE),
	(TOKEN_W_TRIANGLE),
	(TOKEN_W_GAUSSIAN),
	(TOKEN_W_SIGMOIDE),
	(TOKEN_W_BEGINSIGMOIDE),
	(TOKEN_W_ENDSIGMOIDE),
	(TOKEN_W_PSHAPE)
};



static char TOKEN_LANG_ARRAY[8]={
	(TOKEN_DOUBLE_COLON),
	(TOKEN_COMMA),
	(TOKEN_OPEN_P),
	(TOKEN_CLOSE_P),
	(TOKEN_OPEN_B),
	(TOKEN_CLOSE_B),
	(TOKEN_OR),
	(TOKEN_AND)

};

static char TOKEN_EXP[10]={
	TOKEN_MINUS, TOKEN_PLUS,
	TOKEN_MULTIPLICATOR, TOKEN_DIVISOR,
	TOKEN_MODULUS, TOKEN_POWER,
	TOKEN_AND, TOKEN_OR,
	TOKEN_LSHIFT, TOKEN_RSHIFT

};
#endif
