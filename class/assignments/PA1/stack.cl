(*
 *  CS164 Fall 94
 *
 *  Programming Assignment 1
 *    Implementation of a simple stack machine.
 *
 *  Skeleton file
 *)

(* function to interpret the stack *)
class Evaluate inherits IO {
   process_input(item : StackItem, stack : Stack) : Bool {
      (let i_type : String <- item.getType() in 
         if i_type = "x" then
            false
         else if i_type = "e" then {
            self.evaluate_top(stack);
            true; -- continue anyways even if evaluation doesnt change stack
         }
         else if i_type = "d" then {
            stack.print(self);
            true;
         }
         else {
            stack.push(item);
            true;
         }
         fi fi fi
      )
   };

   evaluate_top(stack : Stack) : Bool {
      (let s: StackItem <- stack.pop() in
         (let s_type : String <- s.getType() in
            if s_type = "+" then
               (new AddStackCommand).evaluate_top(stack)
            else if s_type = "s" then
               (new SwapStackCommand).evaluate_top(stack)
            else {
               -- cannot evaluate, return stack to original state
               if not s.getType() = "empty" then stack.push(s) else 0 fi;
               false;
            }
            fi fi
         )
      )
   };
};

class StackCommand {
   (* evaluate the stack, assuming this item is on top 
    * output a boolean to indicate whether to continue *)
   evaluate_top(stack : Stack) : Bool { false };
};

class SwapStackCommand inherits StackCommand {
   evaluate_top(stack: Stack) : Bool {
      (let s1 : StackItem <- stack.pop() in 
         (let s2 : StackItem <- stack.pop() in {
            stack.push(s1);
            stack.push(s2);
            true;
         })
      )
   };
};

class AddStackCommand inherits StackCommand { 
   evaluate_top(stack: Stack) : Bool {
      (let i1 : StackItem <- stack.pop() in 
         (let i2 : StackItem <- stack.pop() in 
            (let newItem : StackItem in {
               newItem <- (new StackItem).init_int((new A2I).i2a(i1.getValue() + i2.getValue()));
               stack.push(newItem);
               true;
            })
         )
      )
   };
};

class Stack {
   items_ : StackItemsList <- (new StackItemsList); 

   push(item : StackItem) : Object {
      items_ <- items_.cons(item)
   };

   pop() : StackItem {
      if not items_.isNil() then
         (let popped: StackItem <- items_.head() in {
            items_ <- items_.tail();
            popped;
         })
      else 
         (new StackItem).init("empty")
      fi
   };

   print(io : IO) : Object {
      if items_.isNil() then 
         io.out_string("Stack is empty!")
      else 
         case items_ of 
            xxs : Cons => xxs.print(io);
            nil : StackItemsList => 0;
         esac
      fi
   };
};

(* interface *)
class StackItem inherits BoolOp {
   type : String; (* better to use an enum *)
   (* "+", "s" or "i" (for integer) *)
   value_ : Int <- ~1; (* hidden variable for value integer *)

   init(t: String) : StackItem {{
      type <- t;
      self;
   }};

   init_int(i_as_str : String) : StackItem {{
      self.init("i");
      value_ <- (new A2I).a2i(i_as_str);
      self;
   }};

   init_from_string(v : String) : StackItem {
      if or_5(v="e", v="x", v="+", v="s", v="d") then (new StackItem).init(v)
      else (new StackItem).init_int(v) 
      fi
   };

   getValue() : Int { value_ };
   getType() : String { type };
   getString(): String {
      if not value_ = ~1 then 
         (new A2I).i2c(value_)
      else 
         type
      fi
   };
};

class StackItemsList {

   isNil() : Bool { true };

   head() : StackItem { { abort(); (new StackItem); } };

   tail()  : StackItemsList { { abort(); self; } };

   cons(i : StackItem) : StackItemsList {
      (new Cons).init(i, self)
   };

};

class Cons inherits StackItemsList {

   car : StackItem;	-- The element in this list cell

   cdr : StackItemsList;	-- The rest of the list

   isNil() : Bool { false };

   head()  : StackItem { car };

   tail()  : StackItemsList { cdr };

   init(i : StackItem, rest : StackItemsList) : StackItemsList {
      {
         car <- i;
         cdr <- rest;
         self;
      }
   };

   print(io : IO) : Object {{
      io.out_string(car.getString());
      io.out_string("\n");
      case cdr of 
         xs : Cons => xs.print(io);
         o : StackItemsList => 0; -- empty list, nothing to print
      esac;
   }};

};

class BoolOp {

  and(b1 : Bool, b2 : Bool) : Bool {
     if b1 then b2 else false fi
  };


  or(b1 : Bool, b2 : Bool) : Bool {
     if b1 then true else b2 fi
  };

  or_5(b1: Bool, b2: Bool, b3: Bool, b4: Bool, b5: Bool) : Bool {
      or(or(or(or(b1, b2), b3), b4), b5)
  };

};

class Main inherits IO {

   prompt() : String {
      {
         out_string("> ");
         in_string();
      }
   };

   main() : Object {
      -- init our stack
      (let stack : Stack <- (new Stack) in
         (let evaluator : Evaluate <- (new Evaluate) in 
            while evaluator.process_input(
               (new StackItem).init_from_string(prompt()), stack) loop {
               0;
            } pool
            -- loop and load in values (while s != 'x') loop:
            --    is value to print or evaluate?
            --       yes: print/evaluate
            --    no:
            --       create stack item;
            --       push to stack

            -- out_string("Nothing implemented\n")
         )
      )
   };

};
