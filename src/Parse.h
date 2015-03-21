#ifndef PARSE_H_
#define PARSE_H_

typedef enum operations opType;
typedef enum trigFunctions trigType;
typedef struct function function;
typedef struct function_part functionPart;
typedef struct function_part_base base;

typedef struct variable var;
typedef struct variableList varList;

typedef struct polynomial poly;
typedef struct trigonometry trig;

function* parseFunction(const char* theFunction);


#endif
