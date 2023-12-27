

#include <stdlib.h>
#include <stdio.h>
#include <map>
#include <vector>
#include <utility>
#include <string>
#include <stdarg.h>
#include "semant.h"
#include "utilities.h"


extern int semant_debug;
extern char *curr_filename;

//////////////////////////////////////////////////////////////////////
//
// Symbols
//
// For convenience, a large number of symbols are predefined here.
// These symbols include the primitive type and method names, as well
// as fixed names used by the runtime system.
//
//////////////////////////////////////////////////////////////////////
static Symbol 
    arg,
    arg2,
    Bool,
    concat,
    cool_abort,
    copy,
    Int,
    in_int,
    in_string,
    IO,
    length,
    Main,
    main_meth,
    No_class,
    No_type,
    Object,
    out_int,
    out_string,
    prim_slot,
    self,
    SELF_TYPE,
    Str,
    str_field,
    substr,
    type_name,
    val;
//
// Initializing the predefined symbols.
//
static void initialize_constants(void)
{
    arg         = idtable.add_string("arg");
    arg2        = idtable.add_string("arg2");
    Bool        = idtable.add_string("Bool");
    concat      = idtable.add_string("concat");
    cool_abort  = idtable.add_string("abort");
    copy        = idtable.add_string("copy");
    Int         = idtable.add_string("Int");
    in_int      = idtable.add_string("in_int");
    in_string   = idtable.add_string("in_string");
    IO          = idtable.add_string("IO");
    length      = idtable.add_string("length");
    Main        = idtable.add_string("Main");
    main_meth   = idtable.add_string("main");
    //   _no_class is a symbol that can't be the name of any 
    //   user-defined class.
    No_class    = idtable.add_string("_no_class");
    No_type     = idtable.add_string("_no_type");
    Object      = idtable.add_string("Object");
    out_int     = idtable.add_string("out_int");
    out_string  = idtable.add_string("out_string");
    prim_slot   = idtable.add_string("_prim_slot");
    self        = idtable.add_string("self");
    SELF_TYPE   = idtable.add_string("SELF_TYPE");
    Str         = idtable.add_string("String");
    str_field   = idtable.add_string("_str_field");
    substr      = idtable.add_string("substr");
    type_name   = idtable.add_string("type_name");
    val         = idtable.add_string("_val");
}

ClassTable::ClassTable(Classes classes)
 : semant_errors(0)
 , error_stream(cerr)
 , all_classes( append_Classes(generate_basic_classes(), classes) )
 {
    // debug output
    // std::cout << "----" << std::endl;
    // inheritance.getValue()->dump(std::cout, 1);
    // std::cout << "----" << std::endl;
 };

 Symbol ClassTable::dynamic_type(Symbol curr_type, Symbol cls) {
    return (curr_type == SELF_TYPE) ? cls : curr_type;
};

Classes ClassTable::generate_basic_classes() {
    /*
    Returns the basic classes. With element 0 being "Object".
    */

    // The tree package uses these globals to annotate the classes built below.
   // curr_lineno  = 0;
    Symbol filename = stringtable.add_string("<basic class>");

    Classes all = nil_Classes();
    
    // The following demonstrates how to create dummy parse trees to
    // refer to basic Cool classes.  There's no need for method
    // bodies -- these are already built into the runtime system.
    
    // IMPORTANT: The results of the following expressions are
    // stored in local variables.  You will want to do something
    // with those variables at the end of this method to make this
    // code meaningful.

    // 
    // The Object class has no parent class. Its methods are
    //        abort() : Object    aborts the program
    //        type_name() : Str   returns a string representation of class name
    //        copy() : SELF_TYPE  returns a copy of the object
    //
    // There is no need for method bodies in the basic classes---these
    // are already built in to the runtime system.

    Class_ Object_class =
	class_(Object, 
	       No_class,
	       append_Features(
			       append_Features(
					       single_Features(method(cool_abort, nil_Formals(), Object, no_expr())),
					       single_Features(method(type_name, nil_Formals(), Str, no_expr()))),
			       single_Features(method(copy, nil_Formals(), SELF_TYPE, no_expr()))),
	       filename);
    all = append_Classes(all, single_Classes(Object_class));

    // 
    // The IO class inherits from Object. Its methods are
    //        out_string(Str) : SELF_TYPE       writes a string to the output
    //        out_int(Int) : SELF_TYPE            "    an int    "  "     "
    //        in_string() : Str                 reads a string from the input
    //        in_int() : Int                      "   an int     "  "     "
    //
    Class_ IO_class = 
	class_(IO, 
	       Object,
	       append_Features(
			       append_Features(
					       append_Features(
							       single_Features(method(out_string, single_Formals(formal(arg, Str)),
										      SELF_TYPE, no_expr())),
							       single_Features(method(out_int, single_Formals(formal(arg, Int)),
										      SELF_TYPE, no_expr()))),
					       single_Features(method(in_string, nil_Formals(), Str, no_expr()))),
			       single_Features(method(in_int, nil_Formals(), Int, no_expr()))),
	       filename);
    all = append_Classes(all, single_Classes(IO_class));

    //
    // The Int class has no methods and only a single attribute, the
    // "val" for the integer. 
    //
    Class_ Int_class =
	class_(Int, 
	       Object,
	       single_Features(attr(val, prim_slot, no_expr())),
	       filename);
    all = append_Classes(all, single_Classes(Int_class));

    //
    // Bool also has only the "val" slot.
    //
    Class_ Bool_class =
	class_(Bool, Object, single_Features(attr(val, prim_slot, no_expr())),filename);
    all = append_Classes(all, single_Classes(Bool_class)); 

    //
    // The class Str has a number of slots and operations:
    //       val                                  the length of the string
    //       str_field                            the string itself
    //       length() : Int                       returns length of the string
    //       concat(arg: Str) : Str               performs string concatenation
    //       substr(arg: Int, arg2: Int): Str     substring selection
    //       
    Class_ Str_class =
	class_(Str, 
	       Object,
	       append_Features(
			       append_Features(
					       append_Features(
							       append_Features(
									       single_Features(attr(val, Int, no_expr())),
									       single_Features(attr(str_field, prim_slot, no_expr()))),
							       single_Features(method(length, nil_Formals(), Int, no_expr()))),
					       single_Features(method(concat, 
								      single_Formals(formal(arg, Str)),
								      Str, 
								      no_expr()))),
			       single_Features(method(substr, 
						      append_Formals(single_Formals(formal(arg, Int)), 
								     single_Formals(formal(arg2, Int))),
						      Str, 
						      no_expr()))),
	       filename);
    all = append_Classes(all, single_Classes(Str_class));

    return all;
}

////////////////////////////////////////////////////////////////////
//
// semant_error is an overloaded function for reporting errors
// during semantic analysis.  There are three versions:
//
//    ostream& ClassTable::semant_error()                
//
//    ostream& ClassTable::semant_error(Class_ c)
//       print line number and filename for `c'
//
//    ostream& ClassTable::semant_error(Symbol filename, tree_node *t)  
//       print a line number and filename
//
///////////////////////////////////////////////////////////////////

ostream& ClassTable::semant_error(Class_ c)
{                                                             
    return semant_error_(c->get_filename(),c);
}    

ostream& ClassTable::semant_error_(Symbol filename, tree_node *t)
{
    error_stream << filename << ":" << t->get_line_number() << ": ";
    return semant_error();
}

ostream& ClassTable::semant_error(Symbol class_name, tree_node *t)
{
    error_stream << getFileName(class_name) << ":" << class_name << ":" << t->get_line_number() << ": ";
    return semant_error();
}

ostream& ClassTable::semant_error_e(Symbol class_name, Expression t)
{   
    t->set_type(Object);
    return semant_error(class_name, t);
}

ostream& ClassTable::semant_error_expected(
    Symbol class_name, Expression t, 
    std::string expr_str_desc, Symbol expected, Symbol actual) {
    return semant_error_e(class_name, t) << expr_str_desc << " must be type " << expected << ". " 
            << "Got actual type " << actual << std::endl;
}

ostream& ClassTable::semant_error()                  
{                                                 
    semant_errors++;                            
    return error_stream;
}

void class__class::update_st(ObjectTable& ot, FuncTable& ft, Symbol cls, ClassTable& ct) {
    // first add all of the class' attributes into the symbol table 
    // these are local to the class, so new scope is created
    ot.enterscope();

    // self gets current class name
    ot.addid(self, &cls);
    
    // select the attributes, compute these and add into the symbol table
    for (int i = features->first(); features->more(i); i = features->next(i)) {
        Feature feat = features->nth(i);
        if (!feat->is_attr()) continue; 
        feat->update_st(ot, ft, cls, ct); 
    }

    // call update_st on its features
    for (int i = features->first(); features->more(i); i = features->next(i)) {
        Feature feat = features->nth(i);
        if (!feat->is_method()) continue; 
        features->nth(i)->update_st(ot, ft, cls, ct);
    }

    ot.exitscope();
}

void attr_class::update_st(ObjectTable& ot, FuncTable& ft, Symbol cls, ClassTable& ct) {
    // attribute expressions can be recursive (so can refer to itself in the defining expr)
    ot.addid(name, &type_decl);

    // evaluate the defining expression
    if (init->is_no_expr()) {
        init->set_type(type_decl); // empty expr: vacuously the required type
    } else {
        init->update_st(ot, ft, cls, ct);
    }

    if (!ct.is_subclass(init->get_type(), type_decl)) {
        ct.semant_error_(cls, this) 
            << "type of defining expr is incompatible with specific type" << std::endl;
    }
}

void method_class::update_st(ObjectTable& ot, FuncTable& ft, Symbol cls, ClassTable& ct) {
    // evaluate the expression ensuring it returns the correct type
    // adding the function args added into scope
    ot.enterscope();

    for (int i = formals->first(); formals->more(i); i = formals->next(i)) {
        auto formal = formals->nth(i);
        ot.addid(formal->get_name(), formal->get_type());
    }

    // evaluate expr with function args
    Symbol body_type;
    if (expr->is_no_expr()) {
        body_type = return_type;
    } else {
        expr->update_st(ot, ft, cls, ct); 
        body_type = expr->get_type();
    }

    auto dynamic_body_type = ct.dynamic_type(body_type, cls);
    auto dynamic_return_type = ct.dynamic_type(return_type, cls);

    if (!ct.is_subclass(dynamic_body_type, dynamic_return_type)) {
        ct.semant_error(cls, this) 
            << "function " << cls << "." << name << ": "
            << "return type of " << dynamic_body_type 
            << " in function body is incompatible with defined function return type: " 
            << dynamic_return_type << std::endl;
    }

    ot.exitscope();
}

void object_class::update_st(ObjectTable& ot, FuncTable& ft, Symbol cls, ClassTable& ct) {
    // must already be in the symbol table.
    Symbol* defined_type = ot.lookup(name);
    if (defined_type) set_type(*defined_type);
    else {
        ct.semant_error_e(cls, this) << "object " << name << 
            " used before it has been defined." << std::endl;
    }
}

void int_const_class::update_st(ObjectTable& ot, FuncTable& ft, Symbol cls, ClassTable& ct) {
    set_type(Int);
}

void string_const_class::update_st(ObjectTable& ot, FuncTable& ft, Symbol cls, ClassTable& ct) {
    set_type(Str);
}

void bool_const_class::update_st(ObjectTable& ot, FuncTable& ft, Symbol cls, ClassTable& ct) {
    set_type(Bool);
}

void block_class::update_st(ObjectTable& ot, FuncTable& ft, Symbol cls, ClassTable& ct) {
    Symbol final_type;
    for (int i = body->first(); body->more(i); i = body->next(i)) {
        Expression expr = body->nth(i);
        expr->update_st(ot, ft, cls, ct);
        final_type = expr->get_type();
    }
    set_type(final_type);
}

void assign_class::update_st(ObjectTable& ot, FuncTable& ft, Symbol cls, ClassTable& ct) {
    expr->update_st(ot, ft, cls, ct);
    Symbol expr_type = expr->get_type();
    Symbol* obj_type = ot.lookup(name);
    if (!obj_type) {
        ct.semant_error_e(cls, this) << "object " << name << " has not yet been defined.";
    } else if (!ct.is_subclass(expr->get_type(), *obj_type)) {
        ct.semant_error_e(cls, this) << "object " << name << " of type " << obj_type << 
            " is incompatible with assigned expression of type: " << expr_type << std::endl;
    } else /* obj_type not null, and expr_type < obj_type */ {
        // obj type remains unchanged
        set_type(expr_type);
    }
}

void new__class::update_st(ObjectTable& ot, FuncTable& ft, Symbol cls, ClassTable& ct) {
    set_type(ct.dynamic_type(type_name, cls));
}

void ClassTable::dispatch_on(
    Symbol on_class, Symbol function_name, Expressions actual, 
    Expression node,
    ObjectTable& ot, FuncTable& ft, Symbol cls) {
    std::vector<Symbol>* function_sig = ft.lookup({on_class, function_name});

    // verify the number of arguments match
    if (((int) function_sig->size()) != (actual->len() + 1)) {
        semant_error(cls, node) << "Number of arguments provided (" << actual->len() 
            << ") does not match required arguments of " << on_class << "." << function_name 
            << " (need " << (function_sig->size() - 1) << ")" << std::endl; 
        return; // no need to type check the rest of the subtree
    }

    // check each argument type T1, ..., Tn is a subclass of the function input
    for (int i = actual->first(); actual->more(i); i = actual->next(i)) {
        auto expr_i = actual->nth(i);
        expr_i->update_st(ot, ft, cls, *this);
        auto sig_i = function_sig->at(i);

        if (!is_subclass(expr_i->get_type(), sig_i)) {
            semant_error(cls, node) << "Argument " << i << " of " 
                << on_class << "." << function_name 
                << " has type " << expr_i->get_type() << ", incompatible with " 
                << "expected type of " << sig_i << std::endl;
        }
    }

    // output type is T0 if it is SELF_TYPE, otherwise is the function's output type
    auto fn_out_type = function_sig->at(actual->len());
    auto dynamic_out_type = (fn_out_type == SELF_TYPE) ? actual->nth(actual->first())->get_type() : fn_out_type;
    node->set_type(dynamic_out_type);
}

void dispatch_class::update_st(ObjectTable& ot, FuncTable& ft, Symbol cls, ClassTable& ct) {
    // check argument type T0 is the function class type (with dynamic type)
    expr->update_st(ot, ft, cls, ct);
    auto function_class_type = ct.dynamic_type(expr->get_type(), cls);
    ct.dispatch_on(function_class_type, name, actual, this, ot, ft, cls);
}

void static_dispatch_class::update_st(ObjectTable& ot, FuncTable& ft, Symbol cls, ClassTable& ct) {
    // check argument type T0 is the function class type (with dynamic type)
    expr->update_st(ot, ft, cls, ct);
    if (!ct.is_subclass(expr->get_type(), type_name)) {
        ct.semant_error_e(cls, this) << "type of expression must be a subclass of " 
            << type_name << ", but is actually a " << expr->get_type() << std::endl;
        return;
    }
    ct.dispatch_on(type_name, name, actual, this, ot, ft, cls);
}

void let_class::update_st(ObjectTable& ot, FuncTable& ft, Symbol cls, ClassTable& ct) {
    Symbol identifier_type;

    if (init->is_no_expr()) {
        // no declaration
        identifier_type = type_decl;
    } else {
        init->update_st(ot, ft, cls, ct);
        auto init_type = init->get_type();
        identifier_type = ct.dynamic_type(type_decl, cls);
        if (!ct.is_subclass(init_type, identifier_type)) {
            ct.semant_error_e(cls, this) << "variable " << identifier << " of declared type " 
                << identifier_type << " is incompatible with defining expression of type "
                << init_type << std::endl;
            return;
        }
    }

    ot.enterscope(); // new scope for the new variable
    ot.addid(identifier, &identifier_type);
    body->update_st(ot, ft, cls, ct);
    set_type(body->type); // let-expression gets the type of the body
    ot.exitscope(); 
}

void typcase_class::update_st(ObjectTable& ot, FuncTable& ft, Symbol cls, ClassTable& ct) {
    std::vector<Symbol> types;
    for (int i = cases->first(); cases->more(i); i = cases->next(i)) {
        ot.enterscope(); // scope to accomodate the current branch only
        Case c = cases->nth(i);
        c->update_st(ot, ft, cls, ct);
        types.push_back(c->get_expr_type());
        ot.exitscope();
    }
    set_type(ct.join(types));
}

void branch_class::update_st(ObjectTable& ot, FuncTable& ft, Symbol cls, ClassTable& ct) {
    ot.addid(name, &type_decl);
    expr->update_st(ot, ft, cls, ct);
}

void loop_class::update_st(ObjectTable& ot, FuncTable& ft, Symbol cls, ClassTable& ct) {
    pred->update_st(ot, ft, cls, ct);
    if (pred->get_type() != Bool) {
        ct.semant_error_expected(cls, this, "predicate in the loop", Bool, pred->get_type());
        return;
    }
    body->update_st(ot, ft, cls, ct); // trigger type checking for loop body.
    set_type(Object); // loop always gets object 
}

void cond_class::update_st(ObjectTable& ot, FuncTable& ft, Symbol cls, ClassTable& ct) {
    pred->update_st(ot, ft, cls, ct);
    if (pred->get_type() != Bool) {
        ct.semant_error_expected(cls, this, "predicate in the if expression", Bool, pred->get_type());
        return;
    }

    then_exp->update_st(ot, ft, cls, ct);
    else_exp->update_st(ot, ft, cls, ct);
    set_type(ct.join(
        {then_exp->get_type(), else_exp->get_type()}
    ));
}

void comp_class::update_st(ObjectTable& ot, FuncTable& ft, Symbol cls, ClassTable& ct) {
    e1->update_st(ot, ft, cls, ct);
    if (e1->get_type() != Bool) {
        ct.semant_error_expected(cls, this, "complement of expression", Bool, e1->get_type());
        return;
    }
    set_type(Bool);
}

void eq_class::update_st(ObjectTable& ot, FuncTable& ft, Symbol cls, ClassTable& ct) {
    e1->update_st(ot, ft, cls, ct);
    e2->update_st(ot, ft, cls, ct);
    Symbol e1t = e1->get_type();
    Symbol e2t = e2->get_type();
    if (((e1t == Bool) || (e1t == Int) | (e1t == Str)) && (e1t != e2t)) {
        ct.semant_error_e(cls, this) << "Expressions on both sides must have the same type. " 
            << "LHS expression has type " << e1t 
            << ". But RHS has type " << e2t << std::endl;
        return;
    }
    set_type(Bool);
}

bool ClassTable::check_int_binary_expr(
    Expression e1, Expression e2, Expression t,
    ObjectTable& ot, FuncTable& ft, Symbol cls, ClassTable& ct) {
    e1->update_st(ot, ft, cls, ct);
    e2->update_st(ot, ft, cls, ct);
    Symbol e1t = e1->get_type();
    Symbol e2t = e2->get_type();
    if ((e1t != Int) || (e2t != Int)) {
        ct.semant_error_e(cls, t) << "Both sides of the arithmetic operation must be type. " 
            << "Int. But got LHS :: " << e1t 
            << ", RHS :: " << e2t << std::endl;
        return false;
    }

    return true;
}

void plus_class::update_st(ObjectTable& ot, FuncTable& ft, Symbol cls, ClassTable& ct) {
    if (ct.check_int_binary_expr(e1, e2, this, ot, ft, cls, ct)) {
        set_type(Int);
    }
}

void sub_class::update_st(ObjectTable& ot, FuncTable& ft, Symbol cls, ClassTable& ct) {
    if (ct.check_int_binary_expr(e1, e2, this, ot, ft, cls, ct)) {
        set_type(Int);
    }
}

void mul_class::update_st(ObjectTable& ot, FuncTable& ft, Symbol cls, ClassTable& ct) {
    if (ct.check_int_binary_expr(e1, e2, this, ot, ft, cls, ct)) {
        set_type(Int);
    }
}

void divide_class::update_st(ObjectTable& ot, FuncTable& ft, Symbol cls, ClassTable& ct) {
    if (ct.check_int_binary_expr(e1, e2, this, ot, ft, cls, ct)) {
        set_type(Int);
    }
}

void lt_class::update_st(ObjectTable& ot, FuncTable& ft, Symbol cls, ClassTable& ct) {
    if (ct.check_int_binary_expr(e1, e2, this, ot, ft, cls, ct)) {
        set_type(Int);
    }
}

void leq_class::update_st(ObjectTable& ot, FuncTable& ft, Symbol cls, ClassTable& ct) {
    if (ct.check_int_binary_expr(e1, e2, this, ot, ft, cls, ct)) {
        set_type(Int);
    }
}

void neg_class::update_st(ObjectTable& ot, FuncTable& ft, Symbol cls, ClassTable& ct) {
    e1->update_st(ot, ft, cls, ct);
    if (e1->get_type() != Int) {
        ct.semant_error_expected(cls, this, "Negative of expression", Int, e1->get_type());
        return;
    }
    set_type(Int);
}

void isvoid_class::update_st(ObjectTable& ot, FuncTable& ft, Symbol cls, ClassTable& ct) {
    set_type(Bool);
}

void no_expr_class::update_st(ObjectTable& ot, FuncTable& ft, Symbol cls, ClassTable& ct) {
    set_type(No_type);
}

void class__class::populate_ft(FuncTable& ft) {
    for (int i = features->first(); features->more(i); i = features->next(i))
        features->nth(i)->populate_ft(ft, name);
}

void method_class::populate_ft(FuncTable& ft, Symbol cls) {
    std::vector<Symbol>* sig = new std::vector<Symbol>();
    for (int i = formals->first(); formals->more(i); i = formals->next(i)) {
        Formal formal = formals->nth(i);
        sig->push_back(*formal->get_type());
    }
    sig->push_back(return_type);

    ft.addid({cls, name}, sig);
}

void attr_class::populate_ft(FuncTable& ft, Symbol cls) {
    // noop
}

bool ClassTable::is_subclass(Symbol t1, Symbol t2) const {
    // t1 < t2, t1 subclass of t2
    // t2 is topmost class, t1 < t2 is always true
    // assume No_type to be min, Object to be max type.
    if (t2 == Object) return true;
    if (t1 == Object) return (t2 == Object);
    if (t1 == No_type) return true;
    if (t2 == No_type) return (t1 == No_type);
    
    Symbol curr = t1;
    do {
        if (curr == t2) return true;
        
        if (parent.find(curr) != parent.end()) {
            curr = parent.at(curr);
        } else {
            std::ostringstream oss;
            oss << "types " << t1 << " <= " << t2 << " is incomparable because " << curr << " has no parent."; 
            throw std::out_of_range(oss.str()); 
        }
    } while (curr != Object);

    // curr == Object :=> t1 belongs to a different chain than t2
    return false;
}

Symbol ClassTable::join(const std::vector<Symbol>& types) const {
    if (types.empty()) return No_type;
    
    Symbol res = types.front();
    for (const auto& t : types) {
        while (!is_subclass(t, res) && (res != Object)) {
            res = parent.at(res);
        }
        
        if (res == Object) return res;
    }
    return res;
}

void ClassTable::build_inheritance_graph() {
    // store a mapping from known objects, and their parent classes
    // also from class name into their class objects.
    for (int i = all_classes->next(all_classes->first()); all_classes->more(i); i = all_classes->next(i)) {
        Class_ cc = all_classes->nth(i);
        Symbol ccs = cc->get_name();
        classes[ccs] = cc;
        canonical_class[ccs] = cc->get_type();

        // add the child->parent relationship to the map
        Symbol pcs = cc->get_parent();
        if (parent.find(ccs) == parent.end()) {
            parent[ccs] = pcs;
        } else {
            // error: class has multiple parents
            semant_error(cc) << "has multiple parents: " << pcs << " and " << parent.at(cc->get_name());
        }
    }

    // cycle detection for each of the classes
    for (auto it = parent.begin(); it != parent.end(); ++it) {
        // for all classes, go up until you find its parent is "Object", or itself, or doesn't exist
        Symbol iInit = it->first;

        Symbol iCurr = it->first;
        Symbol iParent = it->second;
        bool parentDefined = parent.find(iParent) != parent.end();
        while ((iParent != Object) && parentDefined && (iCurr != iInit)) {
            iCurr = iParent;
            iParent = parent.at(iParent);
            parentDefined = parent.find(iParent) != parent.end();
        }


        if (iParent == Object) {
            // parent is Object, chain is good.
        } else if (!parentDefined) {
            // error: parent class is undefined
            semant_error(classes.at(iCurr)) << " parent class of " << iCurr << ": " << iParent << " doesn't exist." << std::endl;
        } else if (iCurr == iInit) {
            // error: cycle found
            semant_error(classes.at(iInit)) << " has cyclic inheritance, involving " << iCurr << std::endl;
        }  else {
            semant_error(classes.at(iInit)) << " has undefined error when computing inheritance graph" << std::endl;
        }
    }

    return;
}

void ClassTable::build_symbol_table() {
    // iterate through the AST, updating the types in the AST as we go
    // building the symbol table as we go
    // making a new scope when appropriate.
    FuncTable ft; 
    ft.enterscope();
    for (int i = all_classes->next(all_classes->first()); all_classes->more(i); i = all_classes->next(i)) {
        all_classes->nth(i)->populate_ft(ft);
    }

    check_main_fn_exist(ft);

    ObjectTable ot;
    for (int i = all_classes->next(all_classes->first()); all_classes->more(i); i = all_classes->next(i)) {
        auto current_class = all_classes->nth(i);
        current_class->update_st(ot, ft, current_class->get_name(), *this);
    }
}

void ClassTable::check_main_fn_exist(FuncTable& ft) {
    if (ft.lookup({Main, main_meth}) == NULL) {
        semant_error() << "Expected program to contain a Main class, with a Main.main function." 
            << std::endl;
    }
}

/*   This is the entry point to the semantic checker.

     Your checker should do the following two things:

     1) Check that the program is semantically correct
     2) Decorate the abstract syntax tree with type information
        by setting the `type' field in each Expression node.
        (see `tree.h')

     You are free to first do 1), make sure you catch all semantic
     errors. Part 2) can be done in a second stage, when you want
     to build mycoolc.
 */
void program_class::semant()
{
    initialize_constants();

    /* ClassTable constructor may do some semantic analysis */
    ClassTable *classtable = new ClassTable(classes);

    classtable->build_inheritance_graph();
    classtable->build_symbol_table();

    if (classtable->errors()) {
	    cerr << "Compilation halted due to static semantic errors." << endl;
	    exit(1);
    }
}


