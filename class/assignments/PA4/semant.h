#ifndef SEMANT_H_
#define SEMANT_H_

#include <assert.h>
#include <iostream>  
#include <list>
#include "cool-tree.h"
#include "stringtab.h"
#include "symtab.h"
#include "list.h"

#define TRUE 1
#define FALSE 0

class ClassTable;
typedef ClassTable *ClassTableP;

// This is a structure that may be used to contain the semantic
// information such as the inheritance graph.  You may use it or not as
// you like: it is only here to provide a container for the supplied
// methods.
class ClassTable {
private:
  int semant_errors;
  Classes generate_basic_classes();
  ostream& error_stream;

  Classes all_classes;
  std::map<Symbol, Symbol> parent; // class' name -> parent class' name
  std::map<Symbol, Class_> classes; // class' name -> class object
  std::map<Symbol, Symbol*> canonical_class; // get class' canonical type instance
public:
  ClassTable(Classes);

  bool is_subclass(Symbol, Symbol) const;

  void dispatch_on(
    Symbol on_class, Symbol function_name, Expressions actual, 
    Expression node,
    ObjectTable& ot, FuncTable& ft, Symbol cls);
  bool check_int_binary_expr(
    Expression e1, Expression e2, Expression t,
    ObjectTable& ot, FuncTable& ft, Symbol cls, ClassTable& ct
  );

  Symbol join(const std::vector<Symbol>& types) const;

  void build_inheritance_graph();
  void build_symbol_table();

  Symbol getFileName(Symbol cls) { return classes.at(cls)->get_filename(); };

  int errors() { return semant_errors; };
  ostream& semant_error();
  ostream& semant_error(Class_ c);
  ostream& semant_error(Symbol class_name, tree_node *t);
  ostream& semant_error_e(Symbol class_name, Expression);
  ostream& semant_error_expected(
    Symbol class_name, Expression t, 
    std::string expr_str_desc, Symbol expected, Symbol actual);  
  ostream& semant_error_(Symbol filename, tree_node *t);
  ostream& debug(Symbol filename, tree_node *t) { return semant_error(filename, t); };

  static Symbol dynamic_type(Symbol curr_type, Symbol cls);
};


#endif

