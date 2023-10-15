(* preexisting test cases *)
class A {
ana(): Int {
(let x:Int <- 1 in 2)+3
};
};

Class BB__ inherits A {
};

(* basic class structure *)
class X {};

Class Y inherits X {
};

(* function calls  *)
class TestFunctionCall {
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