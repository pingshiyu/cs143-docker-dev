class Main inherits IO {
    var : Type1;

    main : SELF_TYPE {
        { *) (* left is a rogue close comment symbol *)
            (let countdown : Int <- 20 in
                while countdown > 0 loop
                    {
                        cells.evolve();
                        cells.print();
                        countdown <- countdown - 1;
                    
                pool
            );  (* normal comment *)
            self; 
        } (* unclosed comment ***
    }
} 

(* this is a comment that won't end