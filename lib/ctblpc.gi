#############################################################################
##
#W  ctblpc.gi                    GAP library                 Alexander Hulpke
##
#H  @(#)$Id$
##
#Y  Copyright (C) 1993, 1997
##
##  This file contains the parts of the Dixon-Schneider specific to pc groups
##
Revision.ctblpc_gi :=
    "@(#)$Id$";


#############################################################################
##
#F  PcGroupClassMatrixColumn(<D>,<mat>,<r>,<t>)  . calculate the t-th column
#F       of the r-th class matrix and store it in the appropriate column of M
##
PcGroupClassMatrixColumn := function(D,M,r,t)
  local c,gt,s,z,i,T,w,e,j,p,orb;
  if t=1 then
    M[D.inversemap[r]][t]:=D.classiz[r];
  else
    orb:=DxGaloisOrbits(D,r);
    z:=D.classreps[t];
    c:=orb.orbits[t][1];
    if c<>t then
      p:=RepresentativeOperation(orb.group,c,t);
      # was the first column of the galois class active?
      if ForAny(M,i->i[c]>0) then
	for i in D.classrange do
	  M[i^p][t]:=M[i][c];
	od;
	Info(InfoCharacterTable,2,"by GaloisImage");
	return;
      fi;
    fi;

    T:=DoubleCentralizerOrbit(D,r,t);
    Info(InfoCharacterTable,2,Length(T[1])," instead of ",D.classiz[r]);

    for i in [1..Length(T[1])] do
      T[1][i]:=T[1][i]*z;
    od;

    T[1]:=List(ClassesSolvableGroup(D.group,D.group,true,0,T[1]),
               i->Position(D.ids,i.representative));

    for i in [1..Length(T[1])] do
      s:=T[1][i];
      M[s][t]:=M[s][t]+T[2][i];
    od;

  fi;
end;


#############################################################################
##
#F  IdentificationSolvableGroup(<D>,<el>) . .  class invariants for el in G
##
IdentificationSolvableGroup := function(D,el)
  return ClassesSolvableGroup(D.group,D.group,true,0,[el])[1].representative;
end;


#############################################################################
##
#M  DxPreparation(<G>)
##
InstallMethod(DxPreparation,"pc group",true,[IsPcGroup,IsRecord],0,
function(G,D)
local i,j,enum,cl;

  if not IsDxLargeGroup(G) then
    TryNextMethod();
  fi;

  D.ClassElement:=ClassElementLargeGroup;
  D.identification:=IdentificationSolvableGroup;
  D.rationalidentification:=IdentificationGenericGroup;
  D.ClassMatrixColumn:=PcGroupClassMatrixColumn;

  cl:=D.classes;
  D.ids:=[];
  D.rids:=[];
  for i in D.classrange do
    D.ids[i]:=D.classreps[i];
    D.rids[i]:=D.rationalidentification(D,D.classreps[i]);
  od;

  return D;

end);


#############################################################################
##
#E  ctblpc.gi
##