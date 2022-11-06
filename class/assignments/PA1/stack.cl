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
         else if i_type = "e" then 
            self.evaluate_top(stack)
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
            else 
               false
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
      (let swapItem : StackItem <- stack.pop() in 
         (let s1 : StackItem <- stack.pop() in 
            (let s2 : StackItem <- stack.pop() in {
               stack.push(s1);
               stack.push(s2);
               true;
            })
         )
      )
   };
};

class AddStackCommand inherits StackCommand { 
   evaluate_top(stack: Stack) : Bool {
      (let add: StackItem <- stack.pop() in 
         (let i1 : StackItem <- stack.pop() in 
            (let i2 : StackItem <- stack.pop() in 
               (let newItem : StackItem in {
                  newItem <- (new StackItem).init_int(i1.getValue() + i2.getValue());
                  stack.push(newItem);
                  true;
               })
            )
         )
      )
   };
};

class Stack {
   items_ : StackItemsList; 

   push(item : StackItem) : Object {
      items_ <- (new Cons).init(item, items_)
   };

   pop() : StackItem {
      (let popped: StackItem <- items_.head() in {
         items_ <- items_.tail();
         popped;
      })
   };

   print(io : IO) : Object {
      case items_ of 
         xxs : Cons => xxs.print(io);
         nil : StackItemsList => 0;
      esac
   };
};

(* interface *)
class StackItem {
   type : String; (* better to use an enum *)
   (* "+", "s" or "i" (for integer) *)
   value_ : Int <- ~1; (* hidden variable for value integer *)

   init() : StackItem {{
      type <- "undefined";
      self;
   }};

   init_int(i : Int) : StackItem {{
      self.init();
      value_ <- i;
      self;
   }}; 

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
      case cdr of 
         xs : Cons => xs.print(io);
         o : StackItemsList => 0; -- empty list, nothing to print
      esac;
   }};

};

class Main inherits IO {

   prompt() : String {
      {
         out_string("\n");
         out_string("> ");
         (let x : String <- in_string() in
            if x = "e"
         );
      }
   };

   main() : Object {


      -- loop and load in values (while s != 'x') loop:
      --    is value to print or evaluate?
      --       yes: print/evaluate
      --    no:
      --       create stack item;
      --       push to stack

      out_string("Nothing implemented\n")
   };

};
