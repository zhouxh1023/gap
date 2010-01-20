#############################################################################
##
#W  z_pi.gd               GAP4 Package `ResClasses'               Stefan Kohl
##
#H  @(#)$Id: z_pi.gd,v 1.7 2007/02/05 14:28:16 stefan Exp $
##
##  This file contains the declaration part for the semilocalizations Z_(pi)
##  of the ring of integers.
##
Revision.z_pi_gd :=
  "@(#)$Id: z_pi.gd,v 1.7 2007/02/05 14:28:16 stefan Exp $";

#############################################################################
##
#O  Z_piCons( <pi> ) . . . . . . . semilocalization Z_(pi) for prime set <pi>
#F  Z_pi( <pi> )
#P  IsZ_pi( <R> )
##
DeclareConstructor( "Z_piCons", [ IsRing, IsList ] );
DeclareGlobalFunction( "Z_pi" );
DeclareProperty( "IsZ_pi", IsEuclideanRing );

#############################################################################
##
#F  NoninvertiblePrimes( <R> ) . . the set of non-inv. primes in the ring <R>
##
DeclareAttribute( "NoninvertiblePrimes", IsRing );

#############################################################################
##
#E  z_pi.gd . . . . . . . . . . . . . . . . . . . . . . . . . . . . ends here