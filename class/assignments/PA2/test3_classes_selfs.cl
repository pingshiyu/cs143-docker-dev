class main inherits IO {
    var : Type1;

    main : SELF_TYPE {
        {
            (let countdown : Int <- 20 in
                while countdown > 0 loop
                    {
                        cells.evolve();
                        cells.print();
                        countdown <- countdown - 1; -- some comment here
                    
                pool
            );  (* end let countdown *)
            self;
        }
    }
} 