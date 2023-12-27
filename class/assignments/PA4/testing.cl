class C {
	a : Int;
	b : Bool;
	init(x : Int, y : Bool) : Bool {
           {
		a <- x;
        y;
           }
	};
};

(* mutually recursive functions (test function table) *)
class Mut1 {
    mut1(a : Int, b : String) : Int {
        {
            (new Mut2).mut2(a);
            b;
        }
    };
};

class Mut2 {
    mut2(a : Int) : Int {
        {
            (new Mut1).mut1(a, a);
            "hehe";
        }
    };
};

class Let1 {
    let1(a : String, b : Int) : SELF_TYPE {
        {
            let x : Int <- x in a;
            self;
        }
    };
};