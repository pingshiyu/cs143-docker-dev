
(* basic class structure *)
class A {};

Class B inherits A {
};


class Errors {
    (* error recovery: let binding run off, move onto the next line *)
    alsoWrong() : Int {
        (let position : Int
        (let num : Int <- self.num_cells() in
            {
                population_map <- temp;
                self;
            }
        ) )
    };

    (* error recovery: expression inside a {} block *)
    wrong() : SELF_TYPE {
        {
            cells <- (new CellularAutomaton).init("         X         ")
        }
    };
};

class ErrorsToo {
    (* error recovery: expression inside a {} block *)
    wrong2() : Int {
        {
            cells <- (new CellularAutomaton).init("         X         ")
        }
    };
};


(* function calls  *)
class Test {
    population_map : String;

    num_cells() : Int {
        population_map.length()
    };
    
    evolve() : SELF_TYPE {
        (let position : Int in
        (let num : Int <- self.num_cells() in
            {
                population_map <- temp;
                self;
            }
        ) )
    };
};

