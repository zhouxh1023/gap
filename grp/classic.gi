#############################################################################
##
#W  classic.gi                  GAP group library              Heiko Thei"sen
##
#H  @(#)$Id$
##
Revision.classic_gi :=
    "@(#)$Id$";

#############################################################################
##
#F  Sp( <d>, <q> )  . . . . . . . . . . . . . . . . . . . .  symplectic group
##
Sp := function( d, q )
    local   g,  f,  z,  o,  mat1,  mat2,  i,  size,  qi,  c;

    # the dimension must be even
    if d mod 2 = 1  then
        Error( "the dimension <d> must be even" );
    fi;
    f := GF(q);
    z := PrimitiveRoot( f );
    o := One( f );

    # if the dimension is two it is a special linear group
    if d = 2 then
        g := SL( 2, q );

    # construct the generators
    else

        # SP(4,2)
        if d = 4 and q = 2  then
            mat1 := [ [1,0,1,1], [1,0,0,1], [0,1,0,1], [1,1,1,1] ] * o;
            mat2 := [ [0,0,1,0], [1,0,0,0], [0,0,0,1], [0,1,0,0] ] * o;

        # SP(d,q)
        else
            mat1 := List( IdentityMat( d, o ), ShallowCopy );
            mat2 := List( 0 * mat1, ShallowCopy );
            for i  in [ 2 .. d/2 ]      do mat2[i][i-1]:= o;  od;
            for i  in [ d/2+1 .. d-1 ]  do mat2[i][i+1]:= o;  od;
  
            if q mod 2 = 1  then
                mat1[  1][    1] := z;
                mat1[  d][    d] := z^-1;
                mat2[  1][    1] := o;
                mat2[  1][d/2+1] := o;
                mat2[d-1][  d/2] := o;
                mat2[  d][  d/2] := -o;

            elif q <> 2  then
                mat1[    1][    1] := z;
                mat1[  d/2][  d/2] := z;
                mat1[d/2+1][d/2+1] := z^-1;
                mat1[    d][    d] := z^-1;
                mat2[    1][d/2-1] := o;
                mat2[    1][  d/2] := o;
                mat2[    1][d/2+1] := o;
                mat2[d/2+1][  d/2] := o;
                mat2[    d][  d/2] := o;

            else
                mat1[    1][  d/2] := o;
                mat1[    1][    d] := o;
                mat1[d/2+1][    d] := o;
                mat2[    1][d/2+1] := o;
                mat2[    d][  d/2] := o;
            fi;
        fi;

        # avoid to call 'Group' because this would check invertibility ...
        g := GroupByGenerators( [ mat1, mat2 ] );
        SetName( g, Concatenation("Sp(",String(d),",",String(q),")") );
        SetDimensionOfMatrixGroup( g, Length( mat1 ) );
        SetFieldOfMatrixGroup( g, f );

        # add the size
        size := 1;
        qi   := 1;
        for i in [ 1 .. d/2 ] do
            qi   := qi * q^2;
            size := size * (qi-1);
        od;
        SetSize( g, q^((d/2)^2) * size );
    fi;

    # construct the form
    c := List( 0 * One( g ), ShallowCopy );
    for i  in [ 1 .. d/2 ]  do
        c[i][d-i+1] := o;
        c[d/2+i][d/2-i+1] := -o;
    od;
    g!.form := c;
    SetFilterObj( g, IsGeneralLinearGroupWithFormRep );

    # and return
    return g;

end;

#############################################################################
##
#F  GU( <n>, <q> )  . . . . . . . . . . . . . . . . . . general unitary group
##
GU := function( n, q )
     local g, i, e, f, z, o, mat1, mat2, size, qi, eps, c;

     f:= GF( q^2 );

     # Handle the trivial case first.
     if n = 1 then
       g:= GroupByGenerators( [ [ [ PrimitiveRoot( f ) ^ (q-1) ] ] ] );
       SetName( g, Concatenation("GU(1,",String(q),")") );
       return g;
     fi;

     # Construct the generators.
     z:= PrimitiveRoot( f );
     o:= One( f );
     mat1:= List( IdentityMat( n, o ), ShallowCopy );
     mat2:= List( 0 * mat1, ShallowCopy );

     if   n = 2 then

       # We use the isomorphism of 'SU(2,q)' and 'SL(2,q)':
       # 'e' is mapped to '-e' under the Frobenius mapping.
       e:= Z(q^2) - Z(q^2)^q;
       if q = 2 then
         mat1[1][1]:= z;
         mat1[2][2]:= z;
         mat1[1][2]:= z;
         mat2[1][2]:= o;
         mat2[2][1]:= o;
       else
         mat1[1][1]:= z;
         mat1[2][2]:= z^-q;
         mat2[1][1]:= -o;
         mat2[1][2]:= e;
         mat2[2][1]:= -e^-1;
       fi;

     elif n mod 2 = 0 then
       if q mod 2 = 1 then e:= z^( (q+1)/2 ); else e:= o; fi;
       mat1[1][1]:= z;
       mat1[n][n]:= z^-q;
       for i in [ 2 .. n/2 ]     do mat2[ i ][ i-1 ]:= o; od;
       for i in [ n/2+1 .. n-1 ] do mat2[ i ][ i+1 ]:= o; od;
       mat2[ 1 ][ 1 ]:= o;
       mat2[1][n/2+1]:= e;
       mat2[n-1][n/2]:= e^-1;
       mat2[n][ n/2 ]:= -e^-1;
     else
       mat1[(n-1)/2][(n-1)/2]:= z;
       mat1[(n-1)/2+2][(n-1)/2+2]:= z^-q;
       for i in [ 1 .. (n-1)/2-1 ] do mat2[ i ][ i+1 ]:= o; od;
       for i in [ (n-1)/2+3 .. n ] do mat2[ i ][ i-1 ]:= o; od;
       mat2[(n-1)/2][  1  ]:=  -(1+z^q/z)^-1;
       mat2[(n-1)/2][(n-1)/2+1]:= -o;
       mat2[(n-1)/2][  n  ]:=  o;
       mat2[(n-1)/2+1][  1  ]:= -o;
       mat2[(n-1)/2+1][(n-1)/2+1]:= -o;
       mat2[(n-1)/2+2][  1  ]:=  o;
     fi;

     # Avoid to call 'Group' because this would check invertibility ...
     g:= GroupByGenerators( [ mat1, mat2 ] );
     SetName( g, Concatenation("GU(",String(n),",",String(q),")") );
     SetDimensionOfMatrixGroup( g, Length( mat1 ) );
     SetFieldOfMatrixGroup( g, f );

     # Add the size.
     size := q+1;
     qi   := q;
     eps  := 1;
     for i in [ 2 .. n ] do
       qi   := qi * q;
       eps  := -eps;
       size := size * (qi+eps);
     od;
     SetSize( g, q^(n*(n-1)/2) * size );

     # construct the form
     c := Reversed( One( g ) );
     g!.form := c;
     SetFilterObj( g, IsGeneralUnitaryGroupWithFormRep );
     
     # Return the group.
     return g;
end;

#############################################################################
##
#F  SU( <n>, <q> )  . . . . . . . . . . . . . . . . . . special unitary group
##
SU := function( n, q )
     local g, i, e, f, z, o, mat1, mat2, size, qi, eps, c;

     f:= GF( q^2 );

     # Handle the trivial case first.
     if n = 1 then
       g:= GroupByGenerators( [ [ [ One( f ) ] ] ] );
       SetName( g, Concatenation("SL(1,",String(q),")") );
       return g;
     fi;

     # Construct the generators.
     z:= PrimitiveRoot( f );
     o:= One( f );
     if n = 3 and q = 2 then

       mat1:= [ [o,z,z], [0,o,z^2], [0,0,o] ] * o;
       mat2:= [ [z,o,o], [o,o, 0 ], [o,0,0] ] * o;

     else

       mat1:= List( IdentityMat( n, o ), ShallowCopy );
       mat2:= List( 0 * mat1, ShallowCopy );

       if   n = 2 then

         # We use the isomorphism of 'SU(2,q)' and 'SL(2,q)':
         # 'e' is mapped to '-e' under the Frobenius mapping.
         e:= Z(q^2) - Z(q^2)^q;
         if q <= 3 then
           mat1[1][2]:= e;
           mat2[1][2]:= e;
           mat2[2][1]:= -e^-1;
         else
           mat1[1][1]:= z^(q+1);
           mat1[2][2]:= z^(-q-1);
           mat2[1][1]:= -o;
           mat2[1][2]:= e;
           mat2[2][1]:= -e^-1;
         fi;

       elif n mod 2 = 0 then

         mat1[1][1]:= z;
         mat1[n][n]:= z^-q;
         mat1[2][2]:= z^-1;
         mat1[ n-1 ][ n-1 ]:= z^q;

         if q mod 2 = 1 then e:= z^( (q+1)/2 ); else e:= o; fi;
         for i in [ 2 .. n/2 ]     do mat2[ i ][ i-1 ]:= o; od;
         for i in [ n/2+1 .. n-1 ] do mat2[ i ][ i+1 ]:= o; od;
         mat2[ 1 ][ 1 ]:= o;
         mat2[1][n/2+1]:= e;
         mat2[n-1][n/2]:= e^-1;
         mat2[n][ n/2 ]:= -e^-1;

       else

         mat1[  (n-1)/2  ][  (n-1)/2  ]:= z;
         mat1[ (n-1)/2+1 ][ (n-1)/2+1 ]:= z^q/z;
         mat1[ (n-1)/2+2 ][ (n-1)/2+2 ]:= z^-q;
  
         for i in [ 1 .. (n-1)/2-1 ] do mat2[ i ][ i+1 ]:= o; od;
         for i in [ (n-1)/2+3 .. n ] do mat2[ i ][ i-1 ]:= o; od;
         mat2[(n-1)/2][    1    ]:=  -(1+z^q/z)^-1;
         mat2[(n-1)/2][(n-1)/2+1]:= -o;
         mat2[(n-1)/2][    n    ]:=  o;
         mat2[(n-1)/2+1][   1   ]:= -o;
         mat2[(n-1)/2+1][(n-1)/2+1]:= -o;
         mat2[(n-1)/2+2][  1  ]:=  o;
       fi;

     fi;

     # Avoid to call 'Group' because this would check invertibility ...
     g:= GroupByGenerators( [ mat1, mat2 ] );
     SetName( g, Concatenation("SU(",String(n),",",String(q),")") );
     SetDimensionOfMatrixGroup( g, Length( mat1 ) );
     SetFieldOfMatrixGroup( g, f );

     # Add the size.
     size := 1;
     qi   := q;
     eps  := 1;
     for i in [ 2 .. n ] do
       qi   := qi * q;
       eps  := -eps;
       size := size * (qi+eps);
     od;
     SetSize( g, q^(n*(n-1)/2) * size );

     # construct the form
     c := Reversed( One( g ) );
     g!.form := c;
     SetFilterObj( g, IsGeneralLinearGroupWithFormRep );

     # Return the group.
     return g;
end;
 
#############################################################################
##

#F  EichlerTransformation( <g>, <u>, <x> )  . .  eichler trans of <u> and <x>
##
EichlerTransformation := function( g, u, x )
    local   e,  b,  i;

    # construct matrix of eichler transformation in <e>
    e := [];

    # loop over the standard vectors
    for b  in One( g )  do
        i := b
             + (b*g!.form*x)*u
             - (b*g!.form*u)*x
             - (b*g!.form*u)*((x*g!.quadraticForm)*x)*u;
        Add( e, i );
    od;
    
    # and return
    return e;

end;


#############################################################################
##
#F  Oplus45() . . . . . . . . . . . . . . . . . . . . . . . . . . . . O+_4(5)
##
Oplus45 := function()
    local   f,  id,  tau2,  tau,  phi,  delta,  eichler,  g;

    # identity matrix over <f>
    f  := GF(5);
    id := IdentityMat( 4, f );

    # construct TAU2: tau(x1-x2)
    tau2 := List( 0*id, ShallowCopy );
    tau2[1][1] := One( f );
    tau2[2][2] := One( f );
    tau2[3][4] := One( f );
    tau2[4][3] := One( f );

    # construct TAU: tau(x1+x2)
    tau := List( 0*id, ShallowCopy );
    tau[1][1] := One( f );
    tau[2][2] := One( f );
    tau[3][4] := -One( f );
    tau[4][3] := -One( f );

    # construct PHI: phi(2)
    phi := List( id, ShallowCopy );
    phi[1][1] := 2*One( f );
    phi[2][2] := 3*One( f );

    # construct DELTA: u <-> v
    delta := List( id, ShallowCopy );
    delta{[1,2]}{[1,2]} := [[0,1],[1,0]]*One( f );

    # construct eichler transformation
    eichler := [[1,0,0,0],[-1,1,-1,0],[2,0,1,0],[0,0,0,1]]*One( f );

    # construct the group without calling 'Group'
    g := [ phi*tau2, tau*eichler*delta ];
    g := GroupByGenerators( g );
    SetDimensionOfMatrixGroup( g, 4 );
    SetFieldOfMatrixGroup( g, f );

    # set the size
    SetSize( g, 28800 );

    # construct the form
    g!.form := [[0,1,0,0],[1,0,0,0],[0,0,2,0],[0,0,0,2]] * One( f );

    # and the quadratic form
    g!.quadraticForm := [[0,1,0,0],[0,0,0,0],[0,0,1,0],[0,0,0,1]] * One( f );

    # and return
    return g;

end;


#############################################################################
##
#F  Opm3( <s>, <d>, <q> ) . . . . . . . . . . . . . . . . . . . .  O+-_<d>(3)
##
##  <q> must be 3, <d> at least 6,  beta is 2
##
Opm3 := function( s, d )
    local   f,  id,  theta,  i,  theta2,  phi,  eichler,  g,  delta;

    # identity matrix over <f>
    f  := GF(3);
    id := IdentityMat( d, f );

    # construct DELTA: u <-> v, x -> x
    delta := List( id, ShallowCopy );
    delta{[1,2]}{[1,2]} := [[0,1],[1,0]]*One( f );

    # construct THETA: x2 -> ... -> xk -> x2
    theta := List( 0*id, ShallowCopy );
    theta[1][1] := One( f );
    theta[2][2] := One( f );
    theta[3][3] := One( f );
    for i  in [ 4 .. d-1 ]  do
        theta[i][i+1] := One( f );
    od;
    theta[d][4] := One( f );

    # construct THETA2: x2 -> x1 -> x3 -> x2
    theta2 := List( id, ShallowCopy );
    theta2{[3..5]}{[3..5]} := [[0,1,1],[-1,-1,1],[1,-1,1]]*One( f );
    
    # construct PHI: u -> au, v -> a^-1v, x -> x
    phi := List( id, ShallowCopy );
    phi[1][1] := 2*One( f );
    phi[2][2] := (2*One( f ))^-1;

    # construct the eichler transformation
    eichler := List( id, ShallowCopy );
    eichler[2][1] := -One( f );
    eichler[2][4] := -One( f );
    eichler[4][1] := 2*One( f );

    # construct the group without calling 'Group'
    g := [ phi*theta2, theta*eichler*delta ];
    g := GroupByGenerators( g );
    SetDimensionOfMatrixGroup( g, d );
    SetFieldOfMatrixGroup( g, f );

    # construct the form
    delta := List( 2*id, ShallowCopy );
    delta{[1,2]}{[1,2]} := [[0,1],[1,0]]*One( f );
    delta[3][3] := 2*One( f )*2;
    g!.form := delta;

    # construct quadratic form
    delta := List( id, ShallowCopy );
    delta{[1,2]}{[1,2]} := [[0,1],[0,0]]*One( f );
    delta[3][3] := One( f )*2;
    g!.quadraticForm := delta;

    # set the size
    delta  := 1;
    theta  := 1;
    theta2 := 3^2;
    for i  in [ 1 .. d/2-1 ]  do
        theta := theta * theta2;
        delta := delta * (theta-1);
    od;
    SetSize( g, 2*3^(d/2*(d/2-1))*(3^(d/2)-s)*delta );

    return g;

end;


#############################################################################
##
#F  OpmSmall( <s>, <d>, <q> ) . . . . . . . . . . . . . . . . .  O+-_<d>(<q>)
##
##  <q> must be 3 or 5, <d> at least 6,  beta is 1
##
OpmSmall := function( s, d, q )
    local   f,  id,  theta,  i,  theta2,  phi,  eichler,  g,  delta;

    # identity matrix over <f>
    f  := GF(q);
    id := IdentityMat( d, f );

    # construct DELTA: u <-> v, x -> x
    delta := List( id, ShallowCopy );
    delta{[1,2]}{[1,2]} := [[0,1],[1,0]]*One( f );

    # construct THETA: x2 -> ... -> xk -> x2
    theta := List( 0*id, ShallowCopy );
    theta[1][1] := One( f );
    theta[2][2] := One( f );
    theta[3][3] := One( f );
    for i  in [ 4 .. d-1 ]  do
        theta[i][i+1] := One( f );
    od;
    theta[d][4] := One( f );

    # construct THETA2: x2 -> x1 -> x3 -> x2
    theta2 := List( id, ShallowCopy );
    theta2{[3..5]}{[3..5]} := [[0,0,1],[1,0,0],[0,1,0]]*One( f );
    
    # construct PHI: u -> au, v -> a^-1v, x -> x
    phi := List( id, ShallowCopy );
    phi[1][1] := 2*One( f );
    phi[2][2] := (2*One( f ))^-1;

    # construct the eichler transformation
    eichler := List( id, ShallowCopy );
    eichler[2][1] := -One( f );
    eichler[2][4] := -One( f );
    eichler[4][1] := 2*One( f );

    # construct the group without calling 'Group'
    g := [ phi*theta2, theta*eichler*delta ];
    g := GroupByGenerators( g );
    SetDimensionOfMatrixGroup( g, d );
    SetFieldOfMatrixGroup( g, f );

    # construct the form
    delta := List( 2*id, ShallowCopy );
    delta{[1,2]}{[1,2]} := [[0,1],[1,0]]*One( f );
    delta[3][3] := 2*One( f );
    g!.form := delta;

    # construct quadratic form
    delta := List( id, ShallowCopy );
    delta{[1,2]}{[1,2]} := [[0,1],[0,0]]*One( f );
    delta[3][3] := One( f );
    g!.quadraticForm := delta;

    # set the size
    delta  := 1;
    theta  := 1;
    theta2 := q^2;
    for i  in [ 1 .. d/2-1 ]  do
        theta := theta * theta2;
        delta := delta * (theta-1);
    od;
    SetSize( g, 2*q^(d/2*(d/2-1))*(q^(d/2)-s)*delta );

    return g;

end;


#############################################################################
##
#F  OpmOdd( <s>, <d>, <q> ) . . . . . . . . . . . . . . . . . . O<s>_<d>(<q>)
##
OpmOdd := function( s, d, q )
    local   f,  w,  beta,  epsilon,  id,  eb1,  tau,  theta,  i,  phi,  
            delta,  eichler,  g;

    # <d> must be at least 4
    if d mod 2 = 1  then
        Error( "<d> must be even" );
    fi;
    if d < 4  then
        Error( "<d> must be at least 4" );
    fi;

    # beta is either 1 or a generator of the field
    f := GF(q);
    w := LogFFE( -1*2^(d-2)*One( f ), PrimitiveRoot( f ) ) mod 2 = 0;
    beta := One( f );
    if s = +1 and (d*(q-1)/4) mod 2 = 0  then
        if not w  then
            beta := PrimitiveRoot( f );
        fi;
    elif s = +1 and (d*(q-1)/4) mod 2 = 1  then
        if w  then
            beta := PrimitiveRoot( f );
        fi;
    elif s = -1 and (d*(q-1)/4) mod 2 = 1  then
        if not w  then
            beta := PrimitiveRoot( f );
        fi;
    elif s = -1 and (d*(q-1)/4) mod 2 = 0  then
        if w  then
            beta := PrimitiveRoot( f );
        fi;
    else
        Error( "<s> must be -1 or +1" );
    fi;

    # special cases
    if q = 3 and d = 4 and s = +1  then
        g := Group( [[1,0,0,0],[0,1,2,1],[2,0,2,0],[1,0,0,1]]*One( f ),
                    [[0,2,2,2],[0,1,1,2],[1,0,2,0],[1,2,2,0]]*One( f ) );
        g!.form := [[0,1,0,0],[1,0,0,0],[0,0,1,0],[0,0,0,2]]*One( f );
        g!.quadraticForm := [[0,1,0,0],[0,0,0,0],[0,0,2,0],[0,0,0,1]]*One( f );
        SetSize( g, 1152 );
        return g;
    elif q = 3 and d = 4 and s = -1  then
        g := Group( [[0,2,0,0],[2,1,0,1],[0,2,0,1],[0,0,1,0]]*One( f ),
                    [[2,0,0,0],[1,2,0,2],[1,0,0,1],[0,0,1,0]]*One( f ) );
        g!.form := [[0,1,0,0],[1,0,0,0],[0,0,2,0],[0,0,0,2]]*One( f );
        g!.quadraticForm := [[0,1,0,0],[0,0,0,0],[0,0,1,0],[0,0,0,1]]*One( f );
        SetSize( g, 1440 );
        return g;
    elif q = 5 and d = 4 and s = +1  then
        return Oplus45();
    elif ( q = 3 or q = 5 ) and 4 < d and beta = One( f )  then
        return OpmSmall( s, d, q );
    elif q = 3 and 4 < d and beta <> One( f )  then
        return Opm3( s, d );
    fi;

    # find an epsilon such that (epsilon^2*beta)^2 <> 1
    if beta = PrimitiveRoot( f )  then
        epsilon := One( f );
    else
        epsilon := PrimitiveRoot( f );
    fi;

    # identity matrix over <f>
    id := IdentityMat( d, f );

    # construct the reflection TAU_epsilon*x1+x2
    eb1 := epsilon^2*beta+1;
    tau := List( id, ShallowCopy );
    tau[3][3] := 1-2*beta*epsilon^2/eb1;
    tau[3][4] := -2*beta*epsilon/eb1;
    tau[4][3] := -2*epsilon/eb1;
    tau[4][4] := 1-2/eb1;

    # construct THETA
    theta := List( 0*id, ShallowCopy );
    theta[1][1] := One( f );
    theta[2][2] := One( f );
    theta[3][3] := One( f );
    for i  in [ 4 .. d-1 ]  do
        theta[i][i+1] := One( f );
    od;
    theta[d][4] := -One( f );
    
    # construct PHI: u -> au, v -> a^-1v, x -> x
    phi := List( id, ShallowCopy );
    phi[1][1] := PrimitiveRoot( f );
    phi[2][2] := PrimitiveRoot( f )^-1;

    # construct DELTA: u <-> v, x -> x
    delta := List( id, ShallowCopy );
    delta{[1,2]}{[1,2]} := [[0,1],[1,0]]*One( f );

    # construct the eichler transformation
    eichler := List( id, ShallowCopy );
    eichler[2][1] := -One( f );
    eichler[2][4] := -One( f );
    eichler[4][1] := 2*One( f );

    # construct the group without calling 'Group'
    g := [ phi, theta*tau*eichler*delta ];
    g := GroupByGenerators( g );
    SetDimensionOfMatrixGroup( g, d );
    SetFieldOfMatrixGroup( g, f );

    # construct the form
    delta := List( 2*id, ShallowCopy );
    delta{[1,2]}{[1,2]} := [[0,1],[1,0]]*One( f );
    delta[3][3] := 2*beta;
    g!.form := delta;

    # construct quadratic form
    delta := List( id, ShallowCopy );
    delta{[1,2]}{[1,2]} := [[0,1],[0,0]]*One( f );
    delta[3][3] := beta;
    g!.quadraticForm := delta;

    # set the size
    delta := 1;
    theta := 1;
    tau   := q^2;
    for i  in [ 1 .. d/2-1 ]  do
        theta := theta * tau;
        delta := delta * (theta-1);
    od;
    SetSize( g, 2*q^(d/2*(d/2-1))*(q^(d/2)-s)*delta );

    return g;

end;
    

#############################################################################
##
#F  Oplus2( <q> ) . . . . . . . . . . . . . . . . . . . . . . . . . O+_2(<q>)
##
Oplus2 := function( q )
    local   z,  m1,  m2,  g;

    # a field generator
    z := Z(q);

    # a matrix of order q-1
    m1 := [ [ z, 0*z ], [ 0*z, z^-1 ] ];

    # a matrix of order 2
    m2 := [ [ 0, 1 ], [ 1, 0 ] ] * z^0;

    # construct the group, set the order, and return
    g := Group( m1, m2 );
    g!.form := m2;
    g!.quadraticForm := [ [ 0, 1 ], [ 0, 0 ] ] * z^0;
    SetSize( g, 2*(q-1) );
    return g;

end;


#############################################################################
##
#F  Oplus4Even( <q> ) . . . . . . . . . . . . . . . . . . . . . . . O+_4(<q>)
##
Oplus4Even := function( q )
    local   f,  id,  rho,  delta,  phi,  eichler,  g;

    # <q> must be even
    if q mod 2 = 1  then
        Error( "<q> must be even" );
    fi;
    f := GF(q);

    # identity matrix over <f>
    id := IdentityMat( 4, f );

    # construct RHO: x1 <-> y1
    rho := List( id, ShallowCopy );
    rho{[3,4]}{[3,4]} := [[0,1],[1,0]] * One( f );

    # construct DELTA: u <-> v
    delta := List( id, ShallowCopy );
    delta{[1,2]}{[1,2]} := [[0,1],[1,0]]*One( f );

    # construct PHI: u -> au, v -> a^-1v, x -> x
    phi := List( id, ShallowCopy );
    phi[1][1] := PrimitiveRoot( f );
    phi[2][2] := PrimitiveRoot( f )^-1;

    # construct eichler transformation
    eichler := [[1,0,0,0],[0,1,-1,0],[0,0,1,0],[1,0,0,1]] * One( f );

    # construct the group without calling 'Group'
    g := [ phi*rho, rho*eichler*delta ];
    g := GroupByGenerators( g );
    SetDimensionOfMatrixGroup( g, 4 );
    SetFieldOfMatrixGroup( g, f );

    # set the size
    SetSize( g, 2*q^2*(q^2-1)^2 );

    # construct the form
    g!.form := [[0,1,0,0],[1,0,0,0],[0,0,0,1],[0,0,1,0]] * One( f );

    # and the quadratic form
    g!.quadraticForm := [[0,1,0,0],[0,0,0,0],[0,0,0,1],[0,0,0,0]] * One( f );

    # and return
    return g;

end;


#############################################################################
##
#F  OplusEven( <d>, <q> ) . . . . . . . . . . . . . . . . . . . . O+_<d>(<q>)
##
OplusEven := function( d, q )
    local   f,  id,  k,  phi,  delta,  theta,  i,  delta2,  eichler,  
            rho,  g;

    # <d> and <q> must be odd
    if d mod 2 = 1  then
        Error( "<d> must be even" );
    fi;
    if d < 6  then
        Error( "<d> must be at least 6" );
    fi;
    if q mod 2 = 1  then
        Error( "<q> must be even" );
    fi;
    f := GF(q);

    # identity matrix over <f>
    id := IdentityMat( d, f );

    # V = H | H_1 | ... | H_k
    k := (d-2) / 2;

    # construct PHI: u -> au, v -> a^-1v, x -> x
    phi := List( id, ShallowCopy );
    phi[1][1] := PrimitiveRoot( f );
    phi[2][2] := PrimitiveRoot( f )^-1;

    # construct DELTA: u <-> v, x -> x
    delta := List( id, ShallowCopy );
    delta{[1,2]}{[1,2]} := [[0,1],[1,0]]*One( f );

    # construct THETA: x_2 -> x_3 -> .. -> x_k -> y_2 -> .. -> y_k -> x_2
    theta := List( 0*id, ShallowCopy );
    for i  in [ 1 .. 4 ]  do
        theta[i][i] := One( f );
    od;
    for i  in [ 2 .. k-1 ]  do
        theta[1+2*i][3+2*i] := One( f );
        theta[2+2*i][4+2*i] := One( f );
    od;
    theta[1+2*k][6] := One( f );
    theta[2+2*k][5] := One( f );

    # (k even) construct DELTA2: x_i <-> y_i, 1 <= i <= k-1
    if k mod 2 = 0  then
        delta2 := List( 0*id, ShallowCopy );
        delta2{[1,2]}{[1,2]} := [[1,0],[0,1]] * One( f );
        for i  in [ 1 .. k ]  do
            delta2[1+2*i][2+2*i] := One( f );
            delta2[2+2*i][1+2*i] := One( f );
        od;

    # (k odd) construct DELTA2: x_1 <-> y_1, x_i <-> x_i+1, y_i <-> y_i+1
    else
        delta2 := List( 0*id, ShallowCopy );
        delta2{[1,2]}{[1,2]} := [[1,0],[0,1]] * One( f );
        delta2{[3,4]}{[3,4]} := [[0,1],[1,0]] * One( f );
        for i  in [ 2, 4 .. k-1 ]  do
            delta2[1+2*i][3+2*i] := One( f );
            delta2[3+2*i][1+2*i] := One( f );
            delta2[2+2*i][4+2*i] := One( f );
            delta2[4+2*i][2+2*i] := One( f );
        od;
    fi;

    # construct eichler transformation
    eichler := List( id, ShallowCopy );
    eichler[4][6] := One( f );
    eichler[5][3] := -One( f );

    # construct RHO = THETA * EICHLER
    rho := theta*eichler;

    # construct second eichler transformation
    eichler := List( id, ShallowCopy );
    eichler[2][5] := -One( f );
    eichler[6][1] := One( f );

    # there seems to be something wrong in I/E for p=2
    if k mod 2 = 0  then
        if q = 2  then
            g := [ phi*delta2, rho, eichler, delta ];
        else
            g := [ phi*delta2, rho*eichler*delta, delta ];
        fi;
    elif q = 2  then
        g := [ phi*delta2, rho*eichler*delta, rho*delta ];
    else
        g := [ phi*delta2, rho*eichler*delta ];
    fi;

    # construct the group without calling 'Group'
    g := GroupByGenerators( g );
    SetDimensionOfMatrixGroup( g, d );
    SetFieldOfMatrixGroup( g, f );

    # construct the form
    delta := List( 0*id, ShallowCopy );
    for i  in [ 1 .. d/2 ]  do
        delta[2*i-1][2*i] := One( f );
        delta[2*i][2*i-1] := One( f );
    od;
    g!.form := delta;

    # construct quadratic form
    delta := List( 0*id, ShallowCopy );
    for i  in [ 1 .. d/2 ]  do
        delta[2*i-1][2*i] := One( f );
    od;
    g!.quadraticForm := delta;

    # set the size
    delta := 1;
    theta := 1;
    rho   := q^2;
    for i  in [ 1 .. d/2-1 ]  do
        theta := theta * rho;
        delta := delta * (theta-1);
    od;
    SetSize( g, 2*q^(d/2*(d/2-1))*(q^(d/2)-1)*delta );

    return g;

end;


#############################################################################
##
#F  Ominus2( <q> )  . . . . . . . . . . . . . . . . . . . . . . . . O-_2(<q>)
##
Ominus2 := function( q )
    local   z,  x,  t,  n,  e,  bc,  m2,  m1,  g;

    # construct the root
    z := Z(q);
    
    # find irreducible x^2+x+t
    x := Indeterminate(GF(q));
    t := z^First( [ 0 .. q-2 ], u -> Length(Factors(x^2+x+z^u)) = 1 );

    # get roots in GF(q^2)
    x := Indeterminate(GF(q^2));
    n := List( Factors(x^2+x+t), x -> -x.coefficients[1] );
    e := 4*t-1;

    # construct base change
    bc := [ [ n[1]/e, 1/e ], [ n[2], z^0 ] ];

    # matrix of order 2
    m2 := [ [ -1, 0 ], [ -1, 1 ] ] * Z(q)^0;

    # matrix of order q+1 (this will lie in F(q)^dxd)
    z  := Z(q^2)^(q-1);
    m1 := bc^-1 * [[z,0*z],[0*z,z^-1]] * bc;

    # and return the group
    g := Group( m1, m2 );
    g!.form := [ [ 2, 1 ], [ 1, 2*t ] ] * z^0;
    g!.quadraticForm := [ [ 1, 1 ], [ 0, t ] ] * z^0;
    SetSize( g, 2*(q+1) );
    return g;

end;


#############################################################################
##
#F  Ominus4Even( <q> )  . . . . . . . . . . . . . . . . . . . . . . O-_4(<q>)
##
Ominus4Even := function( q )
    local   f,  id,  rho,  delta,  phi,  x,  t,  eichler,  g;

    # <q> must be even
    if q mod 2 = 1  then
        Error( "<q> must be even" );
    fi;
    f := GF(q);

    # identity matrix over <f>
    id := IdentityMat( 4, f );

    # construct RHO: x1 <-> y1
    rho := List( id, ShallowCopy );
    rho{[3,4]}{[3,4]} := [[0,1],[1,0]] * One( f );

    # construct DELTA: u <-> v
    delta := List( id, ShallowCopy );
    delta{[1,2]}{[1,2]} := [[0,1],[1,0]]*One( f );

    # construct PHI: u -> au, v -> a^-1v, x -> x
    phi := List( id, ShallowCopy );
    phi[1][1] := PrimitiveRoot( f );
    phi[2][2] := PrimitiveRoot( f )^-1;

    # find irreducible x^2+x+t
    x := Indeterminate(f);
    t := First( [ 0 .. q-2 ], u ->
        Length(Factors(x^2+x+PrimitiveRoot( f )^u)) = 1 );

    # compute square root of <t>
    t := t/2 mod (q-1);
    t := PrimitiveRoot( f )^t;

    # construct eichler transformation
    eichler := [[1,0,0,0],[-t,1,-1,0],[0,0,1,0],[1,0,0,1]] * One( f );

    # construct the group without calling 'Group'
    g := [ phi*rho, rho*eichler*delta ];
    g := GroupByGenerators( g );
    SetDimensionOfMatrixGroup( g, 4 );
    SetFieldOfMatrixGroup( g, f );

    # set the size
    SetSize( g, 2*q^2*(q^2+1)*(q^2-1) );

    # construct the form
    g!.form := [[0,1,0,0],[1,0,0,0],[0,0,0,1],[0,0,1,0]] * One( f );

    # and the quadratic form
    g!.quadraticForm := [[0,1,0,0],[0,0,0,0],[0,0,t,1],[0,0,0,t]] * One( f );

    # and return
    return g;

end;


#############################################################################
##
#F  OminusEven( <d>, <q> )  . . . . . . . . . . . . . . . . . . . O-_<d>(<q>)
##
OminusEven := function( d, q )
    local   f,  id,  k,  phi,  delta,  theta,  i,  delta2,  eichler,  
            rho,  g,  t,  x;

    # <d> and <q> must be odd
    if d mod 2 = 1  then
        Error( "<d> must be even" );
    fi;
    if d < 6  then
        Error( "<d> must be at least 6" );
    fi;
    if q mod 2 = 1  then
        Error( "<q> must be even" );
    fi;
    f := GF(q);

    # identity matrix over <f>
    id := IdentityMat( d, f );

    # V = H | H_1 | ... | H_k
    k := (d-2) / 2;

    # construct PHI: u -> au, v -> a^-1v, x -> x
    phi := List( id, ShallowCopy );
    phi[1][1] := PrimitiveRoot( f );
    phi[2][2] := PrimitiveRoot( f )^-1;

    # construct DELTA: u <-> v, x -> x
    delta := List( id, ShallowCopy );
    delta{[1,2]}{[1,2]} := [[0,1],[1,0]]*One( f );

    # construct THETA: x_2 -> x_3 -> .. -> x_k -> y_2 -> .. -> y_k -> x_2
    theta := List( 0*id, ShallowCopy );
    for i  in [ 1 .. 4 ]  do
        theta[i][i] := One( f );
    od;
    for i  in [ 2 .. k-1 ]  do
        theta[1+2*i][3+2*i] := One( f );
        theta[2+2*i][4+2*i] := One( f );
    od;
    theta[1+2*k][6] := One( f );
    theta[2+2*k][5] := One( f );

    # (k even) construct DELTA2: x_i <-> y_i, 1 <= i <= k-1
    if k mod 2 = 0  then
        delta2 := List( 0*id, ShallowCopy );
        delta2{[1,2]}{[1,2]} := [[1,0],[0,1]] * One( f );
        for i  in [ 1 .. k ]  do
            delta2[1+2*i][2+2*i] := One( f );
            delta2[2+2*i][1+2*i] := One( f );
        od;

    # (k odd) construct DELTA2: x_1 <-> y_1, x_i <-> x_i+1, y_i <-> y_i+1
    else
        delta2 := List( 0*id, ShallowCopy );
        delta2{[1,2]}{[1,2]} := [[1,0],[0,1]] * One( f );
        delta2{[3,4]}{[3,4]} := [[0,1],[1,0]] * One( f );
        for i  in [ 2, 4 .. k-1 ]  do
            delta2[1+2*i][3+2*i] := One( f );
            delta2[3+2*i][1+2*i] := One( f );
            delta2[2+2*i][4+2*i] := One( f );
            delta2[4+2*i][2+2*i] := One( f );
        od;
    fi;

    # find irreducible x^2+x+t
    x := Indeterminate(f);
    t := First( [ 0 .. q-2 ], u ->
        Length(Factors(x^2+x+PrimitiveRoot( f )^u)) = 1 );

    # compute square root of <t>
    t := t/2 mod (q-1);
    t := PrimitiveRoot( f )^t;

    # construct eichler transformation
    eichler := List( id, ShallowCopy );
    eichler[4][6] := One( f );
    eichler[5][3] := -One( f );
    eichler[5][6] := -t;

    # construct RHO = THETA * EICHLER
    rho := theta*eichler;

    # construct second eichler transformation
    eichler := List( id, ShallowCopy );
    eichler[2][5] := -One( f );
    eichler[6][1] := One( f );

    # there seems to be something wrong in I/E for p=2
    if k mod 2 = 0  then
        if q = 2  then
            g := [ phi*delta2, rho, eichler, delta ];
        else
            g := [ phi*delta2, rho*eichler*delta, delta ];
        fi;
    elif q = 2  then
        g := [ phi*delta2, rho*eichler*delta, rho*delta ];
    else
        g := [ phi*delta2, rho*eichler*delta ];
    fi;

    # construct the group without calling 'Group'
    g := GroupByGenerators( g );
    SetDimensionOfMatrixGroup( g, d );
    SetFieldOfMatrixGroup( g, f );

    # construct the form
    delta := List( 0*id, ShallowCopy );
    for i  in [ 1 .. d/2 ]  do
        delta[2*i-1][2*i] := One( f );
        delta[2*i][2*i-1] := One( f );
    od;
    g!.form := delta;

    # construct quadratic form
    delta := List( 0*id, ShallowCopy );
    for i  in [ 1 .. d/2 ]  do
        delta[2*i-1][2*i] := One( f );
    od;
    delta[3][3] := t;
    delta[4][4] := t;
    g!.quadraticForm := delta;

    # set the size
    delta := 1;
    theta := 1;
    rho   := q^2;
    for i  in [ 1 .. d/2-1 ]  do
        theta := theta * rho;
        delta := delta * (theta-1);
    od;
    SetSize( g, 2*q^(d/2*(d/2-1))*(q^(d/2)+1)*delta );

    return g;

end;


#############################################################################
##
#F  OzeroOdd( <d>, <q>, <b> ) . . . . . . . . . . . . . . . . . . O0_<d>(<q>)
##
##  'OzeroOdd'  construct  the orthogonal   group in  odd dimension  and  odd
##  characteristic. The discriminant of the quadratic form is -(2<b>)^(<d>-2)
##
OzeroOdd := function( d, q, b )
    local   id,  phi,  delta,  rho,  i,  eichler,  g,  s,  f,  q2,  q2i;

    # <d> and <q> must be odd
    if d mod 2 = 0  then
        Error( "<d> must be odd" );
    fi;
    if d < 3  then
        Error( "<d> must be at least 3" );
    fi;
    if q mod 2 = 0  then
        Error( "<q> must be odd" );
    fi;
    f := GF(q);

    # identity matrix over <f>
    id := IdentityMat( d, f );

    # construct PHI: u -> au, v -> a^-1v, x -> x
    phi := List( id, ShallowCopy );
    phi[1][1] := PrimitiveRoot( f );
    phi[2][2] := PrimitiveRoot( f )^-1;

    # construct DELTA: u <-> v, x -> x
    delta := List( id, ShallowCopy );
    delta{[1,2]}{[1,2]} := [[0,1],[1,0]]*One( f );

    # construct RHO: u -> u, v -> v, x_i -> x_i+1
    rho := List( 0*id, ShallowCopy );
    rho[1][1] := One( f );
    rho[2][2] := One( f );
    for i  in [ 3 .. d-1 ]  do
        rho[i][i+1] := One( f );
    od;
    rho[d][3] := One( f );

    # construct eichler transformation
    eichler := List( id, ShallowCopy );
    eichler{[1..3]}{[1..3]} := [[1,0,0],[-b,1,-1],[2*b,0,1]] * One( f );

    # construct the group without calling 'Group'
    g := [ phi, rho*eichler*delta ];
    g := GroupByGenerators( g );
    SetDimensionOfMatrixGroup( g, d );
    SetFieldOfMatrixGroup( g, f );

    # and set its size
    s   := 1;
    q2  := q^2;
    q2i := 1;
    for i  in [ 1 .. (d-1)/2 ]  do
        q2i := q2 * q2i;
        s   := s  * (q2i-1);
    od;
    SetSize( g, 2 * q^((d-1)^2/4) * s );

    # construct the form
    s := List( 2*b*id, ShallowCopy );
    s{[1,2]}{[1,2]} := [[0,1],[1,0]]*One( f );
    g!.form := s;

    # and the quadratic form
    s := List( b*id, ShallowCopy );
    s{[1,2]}{[1,2]} := [[0,1],[0,0]]*One( f );
    g!.quadraticForm := s;

    # and return
    return g;

end;


#############################################################################
##
#F  O( <e>, <d>, <q> )  . . . . . . . . . . . . . . . . . . . . O<e>_<d>(<q>)
##
O := function( e, d, q )
    local   g,  i;

    # <e> must be -1, 0, +1
    if e <> -1 and e <> 0 and e <> +1  then
        Error( "sign <e> must be -1, 0, +1" );
    fi;

    # if <e> = 0  then <d> must be odd
    if e = 0 and d mod 2 = 0  then
        Error( "sign <e> = 0 but dimension <d> is even" );

    # if <e> <> 0  then <d> must be even
    elif e <> 0 and d mod 2 = 1  then
        Error( "sign <e> <> 0 but dimension <d> is odd" );
    fi;

    # construct the various orthogonal group
    if   e = 0 and q mod 2 <> 0  then
        g := OzeroOdd( d, q, 1 );
    elif e = 0  then
        # g := OzeroEven( d, q );
        Error( "<d> must be even" );

    # O+(2,q) = D_2(q-1)
    elif e = +1 and d = 2  then
        g := Oplus2(q);

    # if <d> = 4 and <q> even use 'Oplus4Even'
    elif e = +1 and d = 4 and q mod 2 = 0  then
        g := Oplus4Even(q);

    # if <q> is even use 'OplusEven'
    elif e = +1 and q mod 2 = 0  then
        g := OplusEven( d, q );

    # if <q> is odd use 'OpmOdd'
    elif e = +1 and q mod 2 = 1  then
        g := OpmOdd( +1, d, q );

    # O-(2,q) = D_2(q+1)
    elif e = -1 and d = 2  then
         g := Ominus2(q);

    # if <d> = 4 and <q> even use 'Ominus4Even'
    elif e = -1 and d = 4 and q mod 2 = 0  then
        g := Ominus4Even(q);

    # if <q> is even use 'OminusEven'
    elif e = -1 and q mod 2 = 0  then
        g := OminusEven( d, q );

    # if <q> is odd use 'OpmOdd'
    elif e = -1 and q mod 2 = 1  then
        g := OpmOdd( -1, d, q );
    fi;

    # set name
    if e = +1  then i := "+";  else i := "";  fi;
    SetName( g, Concatenation( "O(", i, String(e), ",", String(d), ",",
                                   String(q), ")" ) );

    SetFilterObj( g, IsGeneralLinearGroupWithFormRep );
    
    # and return
    return g;

end;


#############################################################################
##
#F  WallForm( <form>, <m> ) . . . . . . . . . . . . . compute the wall of <m>
##
WallForm := function( form, m )
    local   id,  w,  b,  p,  i,  x,  j;

    # first argument should really be something useful
    id := One( m );

    # compute a base for Image(id-m), use the most stupid algorithm
    w := id - m;
    b := [];
    p := [];
    for i  in [ 1 .. Length(w) ]  do
        if Length(b) = 0  then
            if w[i] <> 0*w[i]  then
                Add( b, w[i] );
                Add( p, i );
            fi;
        elif RankMat(b) <> RankMat(Concatenation(b,[w[i]]))  then
            Add( b, w[i] );
            Add( p, i );
        fi;
    od;

    # compute the form
    x := List( b, x -> [] );
    for i  in [ 1 .. Length(b) ]  do
        for j  in [ 1 .. Length(b) ]  do
            x[i][j] := id[p[i]] * form * b[j];
        od;
    od;

    # and return
    return rec( base := b, pos := p, form := x );

end;


#############################################################################
##
#F  SpinorNorm( <form>, <m> ) . . . . . . . .  compute the spinor norm of <m>
##
SpinorNorm := function( form, m )
    return DeterminantMat( WallForm(form,m).form );
end;

#############################################################################
##

#F  WreathProductOfMatrixGroup( <M>, <P> )  . . . . . . . . .  wreath product
##
WreathProductOfMatrixGroup := function( M, P )
    local   m,  d,  id,  gens,  b,  ran,  mat,  gen,  G;

    m := DimensionOfMatrixGroup( M );
    d := LargestMovedPoint( P );
    id := List( IdentityMat( m * d, FieldOfMatrixGroup( M ) ), ShallowCopy );
    gens := [  ];
    for b  in [ 1 .. d ]  do
        ran := ( b - 1 ) * m + [ 1 .. m ];
        for mat  in GeneratorsOfGroup( M )  do
            gen := DeepCopy( id );
            gen{ ran }{ ran } := mat;
            Add( gens, gen );
        od;
    od;
    for gen  in GeneratorsOfGroup( P )  do
        Add( gens, Permuted( id, gen ) );
    od;
    G := GroupByGenerators( gens );
    if HasName( M )  and  HasName( P )  then
        SetName( G, Concatenation( Name( M ), " wr ", Name( P ) ) );
    fi;
    return G;
end;

#############################################################################
##
#F  TensorWreathProductOfMatrixGroup( <M>, <P> )  . . . tensor wreath product
##
TensorWreathProductOfMatrixGroup := function( M, P )
    local   m,  n,  one,  id,  a,  gens,  b,  ran,  mat,  gen,  list,
            p,  q,  adic,  i,  G;

    m := DimensionOfMatrixGroup( M );
    one := One( FieldOfMatrixGroup( M ) );
    a := LargestMovedPoint( P );
    n := m ^ a;
    id := IdentityMat( n, one );
    gens := [  ];
    for b  in [ 1 .. a ]  do
        for mat  in GeneratorsOfGroup( M )  do
            gen := KroneckerProduct
                   ( IdentityMat( m ^ ( b - 1 ), one ), mat );
            gen := KroneckerProduct
                   ( gen, IdentityMat( m ^ ( a - b ), one ) );
            Add( gens, gen );
        od;
    od;
    for gen  in GeneratorsOfGroup( SymmetricGroup( a ) )  do
        list := [  ];
        for p  in [ 0 .. n - 1 ]  do
            adic := [  ];
            for i  in [ 0 .. a - 1 ]  do
                adic[ ( a - i ) ^ gen ] := p mod m;
                p := QuoInt( p, m );
            od;
            q := 0;
            for i  in adic  do
                q := q * m + i;
            od;
            Add( list, q );
        od;
        Add( gens, id{ list + 1 } );
    od;
    G := GroupByGenerators( gens );
    if HasName( M )  and  HasName( P )  then
        SetName( G, Concatenation( Name( M ), " twr ", Name( P ) ) );
    fi;
    return G;
end;

#############################################################################
##
#F  CentralProductOfMatrixGroups( <M>, <N> )  . . . . . . . . central product
##
CentralProductOfMatrixGroups := function( M, N )
    local   gens,  id,  mat,  G;
    
    gens := [  ];
    id := One( N );
    for mat  in GeneratorsOfGroup( M )  do
        Add( gens, KroneckerProduct( mat, id ) );
    od;
    id := One( M );
    for mat  in GeneratorsOfGroup( N )  do
        Add( gens, KroneckerProduct( id, mat ) );
    od;
    G := GroupByGenerators( gens );
    if HasName( M )  and  HasName( N )  then
        SetName( G, Concatenation( Name( M ), " o ", Name( N ) ) );
    fi;
    return G;
end;

#############################################################################
##
##  Local Variables:
##  mode:             outline-minor
##  outline-regexp:   "#[WCROAPMFVE]"
##  fill-column:      77
##  End:

#############################################################################
##
#E  classic.gi  . . . . . . . . . . . . . . . . . . . . . . . . . . ends here
