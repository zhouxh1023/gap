/*******************************************************************************
**
** A transformation is of the form: 
**
** [entries image list, image set, flat kernel]
**
** An element of the internal rep of a transformation in T_TRANS2 must be at
** most 65536 and be of UInt2. 
** 
*******************************************************************************/

// Transformations must always have degree greater than or equal to the largest
// point in the image.

#include        "trans.h"               /* transformations                 */

#define MIN(a,b)          (a<b?a:b)
#define MAX(a,b)          (a<b?b:a)

#define NEW_TRANS2(deg)       NewBag(T_TRANS2, deg*sizeof(UInt2)+2*sizeof(Obj))
#define ADDR_TRANS2(f)        ((UInt2*)(ADDR_OBJ(f)))
#define IMG_TRANS2(f)         (*((Obj*)(ADDR_TRANS2(f)+DEG_TRANS2(f))))
#define KER_TRANS2(f)         (*((Obj*)(ADDR_TRANS2(f)+DEG_TRANS2(f))+1))
#define DEG_TRANS2(f)         ((UInt)(SIZE_OBJ(f)-2*sizeof(Obj))/sizeof(UInt2))
#define RANK_TRANS2(f)        (IMG_TRANS2(f)==NULL?INIT_TRANS2(f):LEN_PLIST(IMG_TRANS2(f)))

#define NEW_TRANS4(deg)       NewBag(T_TRANS4, deg*sizeof(UInt4)+2*sizeof(Obj))
#define ADDR_TRANS4(f)        ((UInt4*)(ADDR_OBJ(f)))
#define IMG_TRANS4(f)         (*((Obj*)(ADDR_TRANS4(f)+DEG_TRANS4(f))))
#define KER_TRANS4(f)         (*((Obj*)(ADDR_TRANS4(f)+DEG_TRANS4(f))+1))
#define DEG_TRANS4(f)         ((UInt)(SIZE_OBJ(f)-2*sizeof(Obj))/sizeof(UInt4))
#define RANK_TRANS4(f)        (IMG_TRANS4(f)==NULL?INIT_TRANS4(f):LEN_PLIST(IMG_TRANS4(f)))

#define IS_TRANS(f)       (TNUM_OBJ(f)==T_TRANS2||TNUM_OBJ(f)==T_TRANS4)
#define RANK_TRANS(f)     (TNUM_OBJ(f)==T_TRANS2?RANK_TRANS2(f):RANK_TRANS4(f))
#define DEG_TRANS(f)      (TNUM_OBJ(f)==T_TRANS2?DEG_TRANS2(f):DEG_TRANS4(f))
#define IMG_TRANS(f)      (TNUM_OBJ(f)==T_TRANS2?IMG_TRANS2(f):IMG_TRANS4(f))
#define KER_TRANS(f)      (TNUM_OBJ(f)==T_TRANS2?KER_TRANS2(f):KER_TRANS4(f))

Obj FuncHAS_KER_TRANS( Obj self, Obj f ){
  if(TNUM_OBJ(f)==T_TRANS2){
    return (KER_TRANS2(f)==NULL?False:True);
  } else if (TNUM_OBJ(f)==T_TRANS4){
    return (KER_TRANS4(f)==NULL?False:True);
  }
  return Fail;
}

Obj FuncHAS_IMG_TRANS( Obj self, Obj f ){
  if(TNUM_OBJ(f)==T_TRANS2){
    return (IMG_TRANS2(f)==NULL?False:True);
  } else if (TNUM_OBJ(f)==T_TRANS4){
    return (IMG_TRANS4(f)==NULL?False:True);
  }
  return Fail;
}

/*
'TmpTrans' is the handle of a bag of type 'T_TRANS4',  which is created at
initialization time of this package.  Functions in this **  package can use
this bag for  whatever purpose they want.  They have to make sure of course
that it is large enough.

The buffer is *not* guaranteed to have any particular value, routines that
require a zero-initialization need to do this at the start.
*/

Obj TmpTrans;

/* comparison for qsort */
int cmp (const void *a, const void *b){ 
  Int aa, bb;

 aa = *((const Int *)INT_INTOBJ(a));
 bb = *((const Int *)INT_INTOBJ(b));
 return (int) (aa-bb);
}

/*******************************************************************************
** Static functions for transformations
*******************************************************************************/

static inline Int UNEQUAL_DEG_TRANS(int cond){
  if(cond)
    ErrorQuit("usage: the arguments must be transformations of equal degree,",  0L, 0L);   
  return 0L;
}

static inline void ResizeTmpTrans( UInt len ){
  if(SIZE_OBJ(TmpTrans)<len*sizeof(UInt4))
    ResizeBag(TmpTrans,len*sizeof(UInt4));
}

/* find rank, canonical trans same kernel, and img set (unsorted) */
// unchanged
static UInt INIT_TRANS2(Obj f){ 
  UInt    deg, rank, i, j;
  UInt2   *ptf;
  UInt4   *pttmp;
  Obj     img, ker;

  deg=DEG_TRANS2(f);
  ResizeTmpTrans(deg);

  img=NEW_PLIST(T_PLIST_CYC+IMMUTABLE, deg);
  ker=NEW_PLIST(T_PLIST_CYC_NSORT+IMMUTABLE, deg);
  SET_LEN_PLIST(ker, (Int) deg);

  /* renew the ptr in case of garbage collection */
  ptf=ADDR_TRANS2(f); 
  pttmp=ADDR_TRANS4(TmpTrans);
  for(i=0;i<deg;i++) pttmp[i]=0;
 
  rank=0;
  for(i=0;i<deg;i++){        
    j=ptf[i];               /* f(i) */
    if(pttmp[j]==0){ 
      pttmp[j]=++rank;         
      SET_ELM_PLIST(img, rank, INTOBJ_INT(j+1));
    }
    SET_ELM_PLIST(ker, i+1, INTOBJ_INT(pttmp[j]));
  }

  SHRINK_PLIST(img, (Int) rank);
  SET_LEN_PLIST(img, (Int) rank);
  
  IMG_TRANS2(f)=img;
  KER_TRANS2(f)=ker;
  CHANGED_BAG(f);
  return rank;
}

//unchanged
static UInt4 INIT_TRANS4(Obj f){ 
  UInt4   i, j, deg;
  UInt4   rank=0;
  UInt4   *ptf, *pttmp;
  Obj     img, ker;

  deg=DEG_TRANS4(f);
  ResizeTmpTrans(deg);
 
  img=NEW_PLIST(T_PLIST_CYC+IMMUTABLE, deg);
  ker=NEW_PLIST(T_PLIST_CYC_NSORT+IMMUTABLE, deg);
  SET_LEN_PLIST(ker, (Int) deg);

  /* renew the ptr in case of garbage collection */
  ptf=ADDR_TRANS4(f); 
  pttmp=ADDR_TRANS4(TmpTrans);
  for(i=0;i<deg;i++) pttmp[i]=0;
  
  for(i=0;i<deg;i++){        
    j=ptf[i];               /* f(i) */
    if(pttmp[j]==0){ 
      pttmp[j]=++rank;         
      SET_ELM_PLIST(img, rank, INTOBJ_INT(j+1));
    }
    SET_ELM_PLIST(ker, i+1, INTOBJ_INT(pttmp[j]));
  }

  SHRINK_PLIST(img, (Int) rank);
  SET_LEN_PLIST(img, (Int) rank);
  
  IMG_TRANS4(f)=img;
  KER_TRANS4(f)=ker;
  CHANGED_BAG(f);
  return rank;
}

//unchanged
static Obj SORT_PLIST_CYC(Obj res){
  Obj     tmp;      
  UInt    h, i, k, len;
  
  len=LEN_PLIST(res); 
  h = 1;  while ( 9*h + 4 < len )  h = 3*h + 1;
  while ( 0 < h ) {
    for ( i = h+1; i <= len; i++ ) {
      tmp = ADDR_OBJ(res)[i];  k = i;
      while ( h < k && ((Int)tmp < (Int)(ADDR_OBJ(res)[k-h])) ) {
        ADDR_OBJ(res)[k] = ADDR_OBJ(res)[k-h];
        k -= h;
      }
      ADDR_OBJ(res)[k] = tmp;
    }
    h = h / 3;
  }
  RetypeBag(res, T_PLIST_CYC_SSORT + IMMUTABLE); 
  CHANGED_BAG(res);
  return res;
}

/* id for a transformation */
//unchanged
static Obj IdTrans2( UInt2 deg ){
  Obj     f, img;
  UInt2*  ptf, i;

  f=NEW_TRANS2(deg);

  img=NEW_PLIST(T_PLIST_CYC_SSORT+IMMUTABLE, deg);
  SET_LEN_PLIST(img, (Int) deg);

  ptf=ADDR_TRANS2(f);
  for(i=0;i<deg;i++){
    *(ptf++)=i;
    SET_ELM_PLIST(img, i+1, INTOBJ_INT((Int) i+1));
  }

  IMG_TRANS2(f)=img;
  KER_TRANS2(f)=img;
  CHANGED_BAG(f);
  return f;
}

//unchanged
static Obj IdTrans4( UInt4 deg ){
  Obj     f, img;
  UInt4*  ptf, i;

  f=NEW_TRANS4(deg);

  img=NEW_PLIST(T_PLIST_CYC_SSORT+IMMUTABLE, deg);
  SET_LEN_PLIST(img, (Int) deg);

  ptf=ADDR_TRANS4(f);
  for(i=0;i<deg;i++){
    *(ptf++)=i;
    SET_ELM_PLIST(img, i+1, INTOBJ_INT((Int) i+1));
  }

  IMG_TRANS4(f)=img;
  KER_TRANS4(f)=img;
  CHANGED_BAG(f);
  return f;
}

/*******************************************************************************
** GAP functions for transformations
*******************************************************************************/

/* method for creating transformation */
Obj FuncTransformationNC( Obj self, Obj list ){ 
  UInt    i, deg;
  UInt2*  ptf2;
  UInt4*  ptf4;
  Obj     f; 
 
  deg=LEN_LIST(list);
  
  if(deg<=65536){ 
    f=NEW_TRANS2(deg);
    ptf2=ADDR_TRANS2(f);
    for(i=0;i<deg;i++) ptf2[i]=INT_INTOBJ(ELM_LIST(list, i+1))-1;
  }else{
    f=NEW_TRANS4(deg);
    ptf4=ADDR_TRANS4(f);
    for(i=0;i<deg;i++) ptf4[i]=INT_INTOBJ(ELM_LIST(list, i+1))-1;
  }
  return f; 
}

//new
Obj FuncTransformationListListNC( Obj self, Obj src, Obj ran ){ 
  UInt    deg, i, s, r;
  Obj     f;
  UInt2*  ptf2;
  UInt4*  ptf4;

  if(!IS_SMALL_LIST(src)){
    ErrorQuit("usage: <src> must be a list (not a %s)", 
        (Int)TNAM_OBJ(src), 0L);
  }
  if(!IS_SMALL_LIST(ran)){
    ErrorQuit("usage: <ran> must be a list (not a %s)", 
        (Int)TNAM_OBJ(ran), 0L);
  }
  if(LEN_LIST(src)!=LEN_LIST(ran)){
    ErrorQuit("usage: <src> and <ran> must have equal length,", 0L, 0L);
  }

  deg=0;
  for(i=LEN_LIST(src);1<=i;i--){
    s=INT_INTOBJ(ELM_LIST(src, i));
    if(s>deg) deg=s;
    r=INT_INTOBJ(ELM_LIST(ran, i));
    if(r>deg) deg=r;
  } 

  if(deg<=65536){ 
    f=NEW_TRANS2(deg);
    ptf2=ADDR_TRANS2(f);
    for(i=0;i<deg;i++) ptf2[i]=i;
    for(i=LEN_LIST(src);1<=i;i--){
      ptf2[INT_INTOBJ(ELM_LIST(src, i))-1]=INT_INTOBJ(ELM_LIST(ran, i))-1;
    }
  }else{
    f=NEW_TRANS4(deg);
    ptf4=ADDR_TRANS4(f);
    for(i=0;i<deg;i++) ptf4[i]=i;
    for(i=LEN_LIST(src);1<=i;i--){
      ptf4[INT_INTOBJ(ELM_LIST(src, i))-1]=INT_INTOBJ(ELM_LIST(ran, i))-1;
    }
  }
  return f; 
}

Obj FuncDegreeOfTransformation (Obj self, Obj f){
  if(TNUM_OBJ(f)==T_TRANS2) return INTOBJ_INT(DEG_TRANS2(f));
  if(TNUM_OBJ(f)!=T_TRANS4){
    ErrorQuit("usage: the argument should be a transformation,", 0L, 0L);
  }
  return INTOBJ_INT(DEG_TRANS4(f));
}

/* rank of transformation */
//changed
Obj FuncRANK_TRANS(Obj self, Obj f){ 
  if(TNUM_OBJ(f)==T_TRANS2) return INTOBJ_INT(RANK_TRANS2(f));
  if(TNUM_OBJ(f)!=T_TRANS4){ 
    ErrorQuit("usage: the argument should be a transformation,", 0L, 0L);
  }
  return INTOBJ_INT(RANK_TRANS4(f));
}

/* test if a transformation is the identity. */
//unchanged
Obj FuncIS_ID_TRANS(Obj self, Obj f){
  UInt2*  ptf2=ADDR_TRANS2(f);
  UInt4*  ptf4=ADDR_TRANS4(f);
  UInt    deg, i; 

  if(TNUM_OBJ(f)==T_TRANS2){
    deg=DEG_TRANS2(f);
    for(i=0;i<deg;i++){
      if(ptf2[i]!=i){
        return False;
      }
    }
  } else {
    deg=DEG_TRANS4(f);
    for(i=0;i<deg;i++){
      if(ptf4[i]!=i){
        return False;
      }
    }
  }
  return True;
}

//new
Obj FuncLARGEST_MOVED_PT_TRANS(Obj self, Obj f){
  UInt2   *ptf2;
  UInt4   *ptf4;
  UInt    i;
 
  if(!IS_TRANS(f)){
    ErrorQuit("usage: the argument should be a transformation,", 0L, 0L);
  }
  
  if(FuncIS_ID_TRANS(self, f)==True){
    return INTOBJ_INT(0);
  };

  if(TNUM_OBJ(f)==T_TRANS2){
    ptf2=ADDR_TRANS2(f);
    for(i=DEG_TRANS2(f);1<=i;i--){
      if(ptf2[i-1]!=i-1) break;
    }
  } else { 
    ptf4=ADDR_TRANS4(f);
    for(i=DEG_TRANS4(f);1<=i;i--){
      if(ptf4[i-1]!=i-1) break;
    }
  }
  return INTOBJ_INT(i);
}

// the largest point in [1..LargestMovedPoint(f)]^f
// new
Obj FuncLARGEST_IMAGE_PT (Obj self, Obj f){
  UInt2   *ptf2;
  UInt4   *ptf4;
  UInt    i, max, def;
  
  if(!IS_TRANS(f)){
    ErrorQuit("usage: the argument should be a transformation,", 0L, 0L);
  }
 
  max=0;
  if(TNUM_OBJ(f)==T_TRANS2){
    def=DEG_TRANS2(f);
    ptf2=ADDR_TRANS2(f);
    for(i=DEG_TRANS2(f);1<=i;i--){ if(ptf2[i-1]!=i-1) break; }
    for(;1<=i;i--){ 
      if(ptf2[i-1]+1>max){
        max=ptf2[i-1]+1; 
        if(max==def) break;
      }
    }
  } else {
    def=DEG_TRANS4(f);
    ptf4=ADDR_TRANS4(f);
    for(i=DEG_TRANS4(f);1<=i;i--){ if(ptf4[i-1]!=i-1) break; }
    for(;1<=i;i--){ 
      if(ptf4[i-1]+1>max){ 
        max=ptf4[i-1]+1;
        if(max==def) break;
      }
    }
  }
  return INTOBJ_INT(max);
}

// this shouldn't be applied to the identity. 
//new
Obj FuncSMALLEST_MOVED_PT_TRANS(Obj self, Obj f){
  UInt2   *ptf2;
  UInt4   *ptf4;
  UInt    i, deg;
 
  if(!IS_TRANS(f)){
    ErrorQuit("usage: the argument should be a transformation,", 0L, 0L);
  }
  
  if(TNUM_OBJ(f)==T_TRANS2){
    ptf2=ADDR_TRANS2(f);
    deg=DEG_TRANS2(f);
    for(i=1;i<=deg;i++) if(ptf2[i-1]!=i-1) break;
  } else { 
    ptf4=ADDR_TRANS4(f);
    deg=DEG_TRANS4(f);
    for(i=1;i<=deg;i++) if(ptf4[i-1]!=i-1) break;
  }
  return INTOBJ_INT(i);
}

// the smallest point in [SmallestMovedPoint..LargestMovedPoint(f)]^f
// new
Obj FuncSMALLEST_IMAGE_PT (Obj self, Obj f){
  UInt2   *ptf2;
  UInt4   *ptf4;
  UInt    i, min, deg;
  
  if(!IS_TRANS(f)){
    ErrorQuit("usage: the argument should be a transformation,", 0L, 0L);
  }
  if(TNUM_OBJ(f)==T_TRANS2){
    ptf2=ADDR_TRANS2(f);
    deg=DEG_TRANS2(f); 
    min=deg;
    for(i=0;i<deg;i++){ if(ptf2[i]!=i&&ptf2[i]<min) min=ptf2[i]; }
  } else {
    ptf4=ADDR_TRANS4(f);
    deg=DEG_TRANS4(f);
    min=deg;
    for(i=0;i<deg;i++){ if(ptf4[i]!=i&&ptf4[i]<min) min=ptf4[i]; }
  }
  return INTOBJ_INT(min+1);
}

//new 
Obj FuncNR_MOVED_PTS_TRANS(Obj self, Obj f){
  UInt    nr, i, deg;
  UInt2*  ptf2;
  UInt4*  ptf4;

  if(!IS_TRANS(f)){
    ErrorQuit("usage: the argument should be a transformation,", 0L, 0L);
  }

  nr=0;
  if(TNUM_OBJ(f)==T_TRANS2){
    ptf2=ADDR_TRANS2(f);
    deg=DEG_TRANS2(f);
    for(i=0;i<deg;i++){ if(ptf2[i]!=i) nr++; }
  } else {
    ptf4=ADDR_TRANS4(f);
    deg=DEG_TRANS4(f);
    for(i=0;i<deg;i++){ if(ptf4[i]!=i) nr++; }
  }
  return INTOBJ_INT(nr);
}


//new 
Obj FuncMOVED_PTS_TRANS(Obj self, Obj f){
  UInt    len, deg, i, k;
  Obj     out, tmp;
  UInt2   *ptf2;
  UInt4   *ptf4;

  if(!IS_TRANS(f)){
    ErrorQuit("usage: the argument should be a transformation,", 0L, 0L);
  }

  if(FuncIS_ID_TRANS(self, f)==True){
    out=NEW_PLIST(T_PLIST_EMPTY, 0);
    SET_LEN_PLIST(out, 0);
    return out;
  }

  len=0;
  if(TNUM_OBJ(f)==T_TRANS2){
    deg=DEG_TRANS2(f);
    out=NEW_PLIST(T_PLIST_CYC_SSORT, deg);
    ptf2=ADDR_TRANS2(f);
    for(i=0;i<deg;i++){
      if(ptf2[i]!=i) SET_ELM_PLIST(out, ++len, INTOBJ_INT(i+1));
    }
  } else {
    deg=DEG_TRANS4(f);
    out=NEW_PLIST(T_PLIST_CYC_SSORT, deg);
    ptf4=ADDR_TRANS4(f);
    for(i=0;i<deg;i++){
      if(ptf4[i]!=i) SET_ELM_PLIST(out, ++len, INTOBJ_INT(i+1));
    }
  }
  
  // remove duplicates
  tmp=ADDR_OBJ(out)[1];  k = 1;
  for(i=2;i<=len;i++){
    if(INT_INTOBJ(tmp)!=INT_INTOBJ(ADDR_OBJ(out)[i])) {
      k++;
      tmp = ADDR_OBJ(out)[i];
      ADDR_OBJ(out)[k] = tmp;
    }
  }

  if(k<len||len<deg){
    ResizeBag(out, (k+1)*sizeof(Obj) );
  }
  SET_LEN_PLIST(out, k);
  return out;
}

/* kernel of transformation */
//unchanged
Obj FuncFLAT_KERNEL_TRANS (Obj self, Obj f){ 

  if(TNUM_OBJ(f)==T_TRANS2){
    if(KER_TRANS2(f)==NULL) INIT_TRANS2(f);
    return KER_TRANS2(f);
  }
  if(KER_TRANS4(f)==NULL) INIT_TRANS4(f);
  return KER_TRANS4(f);
} 

/* image set of transformation */
//unchanged
Obj FuncIMAGE_SET_TRANS (Obj self, Obj f){ 
  if(TNUM_OBJ(f)==T_TRANS2){
    if(IMG_TRANS2(f)==NULL){
      INIT_TRANS2(f);
      return SORT_PLIST_CYC(IMG_TRANS2(f));
    } else if(!IS_SSORT_LIST(IMG_TRANS2(f))){
      return SORT_PLIST_CYC(IMG_TRANS2(f));
    }
    return IMG_TRANS2(f);  
  }
 if(IMG_TRANS4(f)==NULL){
    INIT_TRANS4(f);
    return SORT_PLIST_CYC(IMG_TRANS4(f));
  } else if(!IS_SSORT_LIST(IMG_TRANS4(f))){
    return SORT_PLIST_CYC(IMG_TRANS4(f));
  }
  return IMG_TRANS4(f);
} 

//the image set of <f> when applied to <1..n> 
//new
Obj FuncIMAGE_SET_TRANS_INT (Obj self, Obj f, Obj n){ 
  Obj     im, new; 
  UInt    deg, m, len, i, j, rank;
  Obj     *ptnew, *ptim;
  UInt4   *pttmp, *ptf4;
  UInt2   *ptf2;

  m=INT_INTOBJ(n);
  deg=DEG_TRANS(f);

  if(m<deg){
    ResizeTmpTrans(deg);
    pttmp=ADDR_TRANS4(TmpTrans);
    for(i=0;i<deg;i++) pttmp[i]=0;

    new=NEW_PLIST(T_PLIST_CYC+IMMUTABLE, m);
    pttmp=ADDR_TRANS4(TmpTrans);
    
    if(TNUM_OBJ(f)==T_TRANS2){
      ptf2=ADDR_TRANS2(f);
      rank=0;
      for(i=0;i<m;i++){        
        j=ptf2[i];               /* f(i) */
        if(pttmp[j]==0){ 
          pttmp[j]=++rank;         
          SET_ELM_PLIST(new, rank, INTOBJ_INT(j+1));
        }
      }
    } else {
      ptf4=ADDR_TRANS4(f);
      rank=0;
      for(i=0;i<m;i++){        
        j=ptf4[i];               /* f(i) */
        if(pttmp[j]==0){ 
          pttmp[j]=++rank;         
          SET_ELM_PLIST(new, rank, INTOBJ_INT(j+1));
        }
      }
    }
    SHRINK_PLIST(new, (Int) rank);
    SET_LEN_PLIST(new, (Int) rank);
    SORT_PLIST_CYC(new);
  } else {    
    im=FuncIMAGE_SET_TRANS(self, f);
    
    if(m==deg) return im;
    
    len=LEN_PLIST(im);
    new=NEW_PLIST(TNUM_OBJ(im), m-deg+len);
    SET_LEN_PLIST(new, m-deg+len);
    
    ptnew=ADDR_OBJ(new)+1;
    ptim=ADDR_OBJ(im)+1;

    //copy the image set 
    for(i=0;i<len;i++)      *ptnew++=*ptim++;
    //add new points
    for(i=deg+1;i<=m;i++)   *ptnew++=INTOBJ_INT(i);
  }
  return new;
} 

/* image list of transformation */
//changed
Obj FuncIMAGE_TRANS (Obj self, Obj f, Obj n ){ 
  UInt2*    ptf2;
  UInt4*    ptf4;
  UInt      i, deg, m;
  Obj       out;
  
  m=INT_INTOBJ(n);

  if(m==0){
    out=NEW_PLIST(T_PLIST_EMPTY, 0);
    SET_LEN_PLIST(out, 0);
    return out;
  }

  if(TNUM_OBJ(f)==T_TRANS2){
    out=NEW_PLIST(T_PLIST_CYC+IMMUTABLE, m);
    ptf2=ADDR_TRANS2(f);
    deg=MIN(DEG_TRANS2(f), m); 
    for(i=0;i<deg;i++){ 
      SET_ELM_PLIST(out,i+1,INTOBJ_INT(ptf2[i]+1));
    }
    for(;i<m;i++) SET_ELM_PLIST(out,i+1,INTOBJ_INT(i+1));
  }else{
    out=NEW_PLIST(T_PLIST_CYC+IMMUTABLE, m);
    ptf4=ADDR_TRANS4(f);
    deg=MIN(DEG_TRANS4(f), m);
    for(i=0;i<deg;i++){ 
      SET_ELM_PLIST(out,i+1,INTOBJ_INT(ptf4[i]+1));
    }
    for(;i<m;i++) SET_ELM_PLIST(out,i+1,INTOBJ_INT(i+1));
  }

  SET_LEN_PLIST(out,(Int) m);
  return out;
} 

/* the kernel as a partition of [1..n] */
//changed
Obj FuncKERNEL_TRANS (Obj self, Obj f, Obj n){
  Obj     ker, flat;
  UInt    i, j, deg, nr, m, rank, len, min;
  UInt4*  pttmp;
 
  deg=DEG_TRANS(f);
  rank=RANK_TRANS(f);
  flat=KER_TRANS(f);
  
  m=INT_INTOBJ(n);
  nr=(m<=deg?rank:rank+m-deg);  // the number of classes
  len=(UInt) deg/nr+1;          // average size of a class
  min=MIN(m,deg);
  
  ker=NEW_PLIST(T_PLIST_HOM_SSORT, nr);
  
  ResizeTmpTrans(nr);
  pttmp=ADDR_TRANS4(TmpTrans);
  for(i=0;i<nr;i++) pttmp[i]=0;

  nr=0;
  // read off flat kernel
  for(i=0;i<min;i++){
    /* renew the ptrs in case of garbage collection */
    j=INT_INTOBJ(ELM_PLIST(flat, i+1));
    if(pttmp[j-1]==0){
      nr++;
      SET_ELM_PLIST(ker, j, NEW_PLIST(T_PLIST_CYC_SSORT, len));
      CHANGED_BAG(ker);
      pttmp=ADDR_TRANS4(TmpTrans);
    }
    AssPlist(ELM_PLIST(ker, j), (Int) ++pttmp[j-1], INTOBJ_INT(i+1));
    pttmp=ADDR_TRANS4(TmpTrans);
  }
  
  for(i=0;i<nr;i++){
    SET_LEN_PLIST(ELM_PLIST(ker, i+1), (Int) pttmp[i]);
    SHRINK_PLIST(ELM_PLIST(ker, i+1), (Int) pttmp[i]);
    /* beware maybe SHRINK_PLIST will trigger a garbage collection */
  }

  for(i=deg;i<m;i++){//add trailing singletons if there are any
    SET_ELM_PLIST(ker, ++nr, NEW_PLIST(T_PLIST_CYC_SSORT, 1));
    SET_LEN_PLIST(ELM_PLIST(ker, nr), 1); 
    SET_ELM_PLIST(ELM_PLIST(ker, nr), 1, INTOBJ_INT(i+1));
    CHANGED_BAG(ker);
  }
  SET_LEN_PLIST(ker, (Int) nr);
  return ker;
}

//changed
Obj FuncPREIMAGES_TRANS_INT (Obj self, Obj f, Obj pt){
  UInt2   *ptf2;
  UInt4   *ptf4;
  UInt    deg, nr, i, j;
  Obj     out;

  deg=DEG_TRANS(f);

  if(INT_INTOBJ(pt)>deg){
    out=NEW_PLIST(T_PLIST_CYC, 1);
    SET_LEN_PLIST(out, 1);
    SET_ELM_PLIST(out, 1, pt);
    return out;
  }

  i=(UInt) INT_INTOBJ(pt)-1;
  out=NEW_PLIST(T_PLIST_CYC_SSORT, deg);

  /* renew the ptr in case of garbage collection */
  nr=0;
  if(TNUM_OBJ(f)==T_TRANS2){
    ptf2=ADDR_TRANS2(f);
    for(j=0;j<deg;j++) if(ptf2[j]==i) SET_ELM_PLIST(out, ++nr, INTOBJ_INT(j+1));
  }else{
    ptf4=ADDR_TRANS4(f);
    for(j=0;j<deg;j++) if(ptf4[j]==i) SET_ELM_PLIST(out, ++nr, INTOBJ_INT(j+1));
  }

  SET_LEN_PLIST(out, (Int) nr);
  SHRINK_PLIST(out, (Int) nr);
  return out;
}

/* identity transformation on <deg> points */
//unchanged
Obj FuncID_TRANS(Obj self, Obj deg){
  if(INT_INTOBJ(deg)<=65536){
    return IdTrans2((UInt2) INT_INTOBJ(deg));
  }
  return IdTrans4((UInt4) INT_INTOBJ(deg));
}

// AsTransformation for a permutation <p> and a pos int <n>. This might be
// quicker if we don't install the kernel etc, but then getting the kernel etc
// back is slower than it is from here. 
// changed
Obj FuncAS_TRANS_PERM_INT(Obj self, Obj p, Obj deg){
  UInt2   *ptp2, *ptf2;
  UInt4   *ptp4, *ptf4;
  Obj     f, img, *ptimg;
  UInt    def, dep, i, min;
  Int     n;
  
  n=INT_INTOBJ(deg);
  if(n==0) return FuncID_TRANS(self, INTOBJ_INT(0));

  //find the degree of f
  def=n;
  dep=(TNUM_OBJ(p)==T_PERM2?DEG_PERM2(p):DEG_PERM4(p));

  if(n<dep){
    min=def;
    if(TNUM_OBJ(p)==T_PERM2){
      ptp2=ADDR_PERM2(p);
      for(i=0;i<n;i++){
        if(ptp2[i]+1>def) def=ptp2[i]+1;
      }
    } else {
      dep=DEG_PERM4(p);
      ptp4=ADDR_PERM4(p);
      for(i=0;i<n;i++){
        if(ptp4[i]+1>def) def=ptp4[i]+1;
      }
    }
  } else {
    min=dep;
  }

  img=NEW_PLIST(T_PLIST_CYC_SSORT+IMMUTABLE, def);
  //create f 
  if(def<=65536){
    f=NEW_TRANS2(def);
    ptimg=ADDR_OBJ(img)+1;
    ptf2=ADDR_TRANS2(f);
    
    if(TNUM_OBJ(p)==T_PERM2){
      ptp2=ADDR_PERM2(p);
      for(i=0;i<min;i++){
        ptf2[i]=ptp2[i];
        ptimg[i]=INTOBJ_INT(i+1);
      }
    } else { //TNUM_OBJ(p)==T_PERM4
      ptp4=ADDR_PERM4(p);
      for(i=0;i<min;i++){
        ptf2[i]=ptp4[i];
        ptimg[i]=INTOBJ_INT(i+1);
      }
    }
    for(;i<def;i++){
      ptf2[i]=i;
      ptimg[i]=INTOBJ_INT(i+1);
    }
    IMG_TRANS2(f)=img;
    KER_TRANS2(f)=img;
    CHANGED_BAG(f);
  } else { //def>65536
    f=NEW_TRANS4(def);
    ptimg=ADDR_OBJ(img)+1;
    ptf4=ADDR_TRANS4(f);
    
    if(TNUM_OBJ(p)==T_PERM2){
      ptp2=ADDR_PERM2(p);
      for(i=0;i<min;i++){
        ptf4[i]=ptp2[i];
        ptimg[i]=INTOBJ_INT(i+1);
      }
    } else { //TNUM_OBJ(p)==T_PERM4
      ptp4=ADDR_PERM4(p);
      for(i=0;i<min;i++){
        ptf4[i]=ptp4[i];
        ptimg[i]=INTOBJ_INT(i+1);
      }
    }
    for(;i<def;i++){
      ptf4[i]=i;
      ptimg[i]=INTOBJ_INT(i+1);
    }
    IMG_TRANS4(f)=img;
    KER_TRANS4(f)=img;
    CHANGED_BAG(f);
  }
  
  SET_LEN_PLIST(img, def);
  return f;
}

/* AsTransformation for a permutation */
//unchanged
Obj FuncAS_TRANS_PERM(Obj self, Obj p){
  UInt2   *ptPerm2;
  UInt4   *ptPerm4;
  UInt    sup;

  //find largest moved point 
  if(TNUM_OBJ(p)==T_PERM2){
    ptPerm2=ADDR_PERM2(p);
    for(sup=DEG_PERM2(p);1<=sup;sup--) if(ptPerm2[sup-1]!=sup-1) break;
    return FuncAS_TRANS_PERM_INT(self, p, INTOBJ_INT(sup));
  } 
  ptPerm4 = ADDR_PERM4(p);
  for ( sup = DEG_PERM4(p); 1 <= sup; sup-- ) {
    if ( ptPerm4[sup-1] != sup-1 ) break;
  }
  return FuncAS_TRANS_PERM_INT(self, p, INTOBJ_INT(sup));
}

/* converts transformation into permutation of its image if possible */
//unchanged
Obj FuncAS_PERM_TRANS(Obj self, Obj f){
  UInt2   *ptf2, *ptp2;
  UInt4   *ptf4, *ptp4, *pttmp;
  UInt    deg, rank, i, j;
  Obj     p, img;

  if(TNUM_OBJ(f)==T_TRANS2){
    /* if we haven't already got the image set find it */
    /* note that the image set does not have to be sorted for this function */
    rank=RANK_TRANS2(f);
    deg=DEG_TRANS2(f);

    p=NEW_PERM2(deg);
    ResizeTmpTrans(deg); 
    
    pttmp=ADDR_TRANS4(TmpTrans);
    ptp2=ADDR_PERM2(p);
    for(i=0;i<deg;i++){ pttmp[i]=0; ptp2[i]=i; }
    
    ptf2=ADDR_TRANS2(f);
    img=IMG_TRANS2(f);
   
    for(i=0;i<rank;i++){
      j=INT_INTOBJ(ELM_PLIST(img, i+1))-1;    /* ranset(f)[i] */ 
      if(pttmp[ptf2[j]]!=0) return Fail; 
      pttmp[ptf2[j]]=1;
      ptp2[j]=ptf2[j];
    }
  }else{
    rank=RANK_TRANS4(f);
    deg=DEG_TRANS4(f);

    p=NEW_PERM4(deg);
    ResizeTmpTrans(deg);

    pttmp=ADDR_TRANS4(TmpTrans);
    ptp4=ADDR_PERM4(p);
    for(i=0;i<deg;i++){ pttmp[i]=0; ptp4[i]=i; }
    
    ptf4=ADDR_TRANS4(f);
    img=IMG_TRANS4(f);
   
    for(i=0;i<rank;i++){
      j=INT_INTOBJ(ELM_PLIST(img, i+1))-1;    /* ranset(f)[i] */ 
      if(pttmp[ptf4[j]]!=0) return Fail; 
      pttmp[ptf4[j]]=1;
      ptp4[j]=ptf4[j];
    }
  }
  return p;
}

/* if <g>=RESTRICTED_TRANS(f), then <g> acts like <f> on <list> and fixes every
 * other point */
//unchanged
Obj FuncRESTRICTED_TRANS(Obj self, Obj f, Obj list){
  UInt    deg, i, j, len;
  UInt2   *ptf2, *ptg2;
  UInt4   *ptf4, *ptg4;
  Obj     g;

  len=LEN_LIST(list);

  if(TNUM_OBJ(f)==T_TRANS2){
    deg=DEG_TRANS2(f);
    g=NEW_TRANS2(deg);
  
    ptf2=ADDR_TRANS2(f);
    ptg2=ADDR_TRANS2(g);

    /* g fixes every point */
    for(i=0;i<deg;i++) ptg2[i]=i;

    /* g acts like f on list */
    for(i=0;i<len;i++){
      j=INT_INTOBJ(ELM_LIST(list, i+1))-1;
      if(j<deg) ptg2[j]=ptf2[j];
    }
  }else{
    deg=DEG_TRANS4(f);
    g=NEW_TRANS4(deg);
  
    ptf4=ADDR_TRANS4(f);
    ptg4=ADDR_TRANS4(g);

    /* g fixes every point */
    for(i=0;i<deg;i++) ptg4[i]=i;

    /* g acts like f on list */
    for(i=0;i<len;i++){
      j=INT_INTOBJ(ELM_LIST(list, i+1))-1;
      if(j<deg) ptg4[j]=ptf4[j];
    }
  }
  return g;
}

// AsTransformation for a transformation <f> and a pos int <m>
// either restricts <f> to [1..m] or extends <f> so that <f> fixes
// [Degree(f)+1..m] depending on whether m is less than or equal Degree(f) or 
// not 

// in the first form, this is similar to TRIM_TRANS except that a new
// transformation is returned. 

//changed
Obj FuncAS_TRANS_TRANS(Obj self, Obj f, Obj m){
  UInt2   *ptf2, *ptg2;
  UInt4   *ptf4, *ptg4;
  UInt    i, n, deg, rank;
  Obj     g, ker, img, kerf, imgf;

  n=INT_INTOBJ(m);
  deg=DEG_TRANS(f);

  if(n==deg) return f;
  
  if(TNUM_OBJ(f)==T_TRANS2&&n<=65536){/* f and g are T_TRANS2 */
    g=NEW_TRANS2(n);
    
    if(IMG_TRANS2(f)==NULL||deg>n){
      ptf2=ADDR_TRANS2(f);
      ptg2=ADDR_TRANS2(g);
      deg=MIN(deg, n); 
      for(i=0;i<deg;i++){
        if(ptf2[i]>n-1){
          ErrorQuit("usage: the first argument is not a transformation when\nrestricted to [1..%d],", (Int) n, 0L);
        }
        ptg2[i]=ptf2[i];
      }
      for(;i<n;i++) ptg2[i]=i;
      return g;
    } else { /* rank is known AND deg<=n */
      rank=RANK_TRANS2(f);
      ker=NEW_PLIST(T_PLIST_CYC+IMMUTABLE, n);
      SET_LEN_PLIST(ker, (Int) n);
      img=NEW_PLIST(T_PLIST_CYC+IMMUTABLE, rank+n-deg);
      SET_LEN_PLIST(img, (Int) rank+n-deg);
      kerf=KER_TRANS2(f);
      imgf=IMG_TRANS2(f);
      
      ptf2=ADDR_TRANS2(f);
      ptg2=ADDR_TRANS2(g);
      
      for(i=0;i<rank;i++){
        ptg2[i]=ptf2[i];              /* image    */
        SET_ELM_PLIST(ker, i+1, ELM_PLIST(kerf, i+1));
        SET_ELM_PLIST(img, i+1, ELM_PLIST(imgf, i+1));
      }
      for(;i<deg;i++){
        ptg2[i]=ptf2[i];              /* image    */
        SET_ELM_PLIST(ker, i+1, ELM_PLIST(kerf, i+1));
      }
      for(;i<n;i++){
        ptg2[i]=i;                   /* image    */
        SET_ELM_PLIST(ker, i+1, INTOBJ_INT(i-deg+rank+1));
        SET_ELM_PLIST(img, i-deg+rank+1, INTOBJ_INT(i+1));
      }
      IMG_TRANS2(g)=img;
      KER_TRANS2(g)=ker;
      CHANGED_BAG(g);
    }
  }else if(TNUM_OBJ(f)==T_TRANS2) { /* n>65535>=deg and so g is T_TRANS4 */
    g=NEW_TRANS4(n);
    
    if(IMG_TRANS2(f)==NULL){
      ptf2=ADDR_TRANS2(f);
      ptg4=ADDR_TRANS4(g);
      for(i=0;i<deg;i++){
        ptg4[i]=(UInt4) ptf2[i];
      }
      for(;i<n;i++) ptg4[i]=i;
    } else { /* rank is known */
      rank=RANK_TRANS2(f);
      ker=NEW_PLIST(T_PLIST_CYC+IMMUTABLE, n);
      SET_LEN_PLIST(ker, (Int) n);
      img=NEW_PLIST(T_PLIST_CYC+IMMUTABLE, rank+n-deg);
      SET_LEN_PLIST(img, (Int) rank+n-deg);
      kerf=KER_TRANS2(f);
      imgf=IMG_TRANS2(f);
      
      ptf2=ADDR_TRANS2(f);
      ptg4=ADDR_TRANS4(g);
      
      for(i=0;i<rank;i++){
        ptg4[i]=(UInt4) ptf2[i];              /* image    */
        SET_ELM_PLIST(ker, i+1, ELM_PLIST(kerf, i+1));
        SET_ELM_PLIST(img, i+1, ELM_PLIST(imgf, i+1));
      }
      for(;i<deg;i++){
        ptg4[i]=(UInt4) ptf2[i];              /* image    */
        SET_ELM_PLIST(ker, i+1, ELM_PLIST(kerf, i+1));
      }
      for(;i<n;i++){
        ptg4[i]=i;                   /* image    */
        SET_ELM_PLIST(ker, i+1, INTOBJ_INT(i-deg+rank+1));
        SET_ELM_PLIST(img, i-deg+rank+1, INTOBJ_INT(i+1));
      }
      IMG_TRANS4(g)=img;
      KER_TRANS4(g)=ker;
      CHANGED_BAG(g);
    }
  }else if(n>65536){ /* f and g are T_TRANS4 */
    g=NEW_TRANS4(n);
    
    if(IMG_TRANS4(f)==NULL||deg>n){
      ptf4=ADDR_TRANS4(f);
      ptg4=ADDR_TRANS4(g);
      deg=MIN(deg,n); 
      for(i=0;i<deg;i++){
        if(ptf4[i]>n-1){
          ErrorQuit("usage: the first argument is not a transformation when\nrestricted to [1..%d],", (Int) n, 0L);
        }
        ptg4[i]=ptf4[i];
      }
      for(;i<n;i++) ptg4[i]=i;
    } else { /* rank is known AND n>deg */
      rank=RANK_TRANS4(f);
      ker=NEW_PLIST(T_PLIST_CYC+IMMUTABLE, n);
      SET_LEN_PLIST(ker, (Int) n);
      img=NEW_PLIST(T_PLIST_CYC+IMMUTABLE, rank+n-deg);
      SET_LEN_PLIST(img, (Int) rank+n-deg);
      kerf=KER_TRANS4(f);
      imgf=IMG_TRANS4(f);
      
      ptf4=ADDR_TRANS4(f);
      ptg4=ADDR_TRANS4(g);
      
      for(i=0;i<rank;i++){
        ptg4[i]=ptf4[i];              /* image    */
        SET_ELM_PLIST(ker, i+1, ELM_PLIST(kerf, i+1));
        SET_ELM_PLIST(img, i+1, ELM_PLIST(imgf, i+1));
      }
      for(;i<deg;i++){
        ptg4[i]=ptf4[i];              /* image    */
        SET_ELM_PLIST(ker, i+1, ELM_PLIST(kerf, i+1));
      }
      for(;i<n;i++){
        ptg4[i]=i;                   /* image    */
        SET_ELM_PLIST(ker, i+1, INTOBJ_INT(i-deg+rank+1));
        SET_ELM_PLIST(img, i-deg+rank+1, INTOBJ_INT(i+1));
      }
      IMG_TRANS4(g)=img;
      KER_TRANS4(g)=ker;
      CHANGED_BAG(g);
    }
  }else{/* f is T_TRANS4 but n<65536<=deg and so g will be T_TRANS2 */
    g=NEW_TRANS2(n);
    
    ptf4=ADDR_TRANS4(f);
    ptg2=ADDR_TRANS2(g);
    for(i=0;i<n;i++){
      if(ptf4[i]>n-1){
        ErrorQuit("usage: the first argument is not a transformation when\nrestricted to [1..%d],", (Int) n, 0L);
      }
      ptg2[i]=(UInt2) ptf4[i];
    }
  }

  return g;
}

// it is assumed that f is actually a transformation of [1..m]
//new
Obj FuncTRIM_TRANS (Obj self, Obj f, Obj m){
  UInt    deg, i;
  UInt4   *ptf;

  if(!IS_TRANS(f)){
    ErrorQuit("the argument must be a transformation,", 0L, 0L);
  }

  deg=INT_INTOBJ(m);

  if(TNUM_OBJ(f)==T_TRANS2){
    if(deg>DEG_TRANS2(f)) return f;
    ResizeBag(f, deg*sizeof(UInt2)+2*sizeof(Obj));
    IMG_TRANS2(f)=NULL;
    KER_TRANS2(f)=NULL;
  } else {
    if(deg>DEG_TRANS4(f)) return f;
    if(deg>65536UL){
      ResizeBag(f, deg*sizeof(UInt4)+2*sizeof(Obj));
      IMG_TRANS4(f)=NULL;
      KER_TRANS4(f)=NULL;
    } else {
      ptf=((UInt4*)(ADDR_OBJ(f)));
      for(i=0;i<deg;i++) ((UInt2*)ptf)[i]=(UInt2)ptf[i];
      if((*((Obj*)(ptf+deg)+1))!=NULL&&deg==DEG_TRANS4(f)){
        (*((Obj*)((UInt2*)ptf+deg)  ))=(*((Obj*)(ptf+deg)  ));
        (*((Obj*)((UInt2*)ptf+deg)+1))=(*((Obj*)(ptf+deg)+1)); 
      } else {
        (*((Obj*)((UInt2*)ptf+deg)  ))=NULL;
        (*((Obj*)((UInt2*)ptf+deg)+1))=NULL;
      }
      RetypeBag(f, T_TRANS2);
      ResizeBag(f, deg*sizeof(UInt2)+2*sizeof(Obj));
    }
  }
  CHANGED_BAG(f);
  return (Obj)0;
}

/* check if the trans or list t is injective on the list l */
//changed
Obj FuncIS_INJECTIVE_LIST_TRANS( Obj self, Obj l, Obj t){
  UInt    n, i, j;
  UInt2   *ptt2;
  UInt4   *pttmp=0L;
  UInt4   *ptt4;
  
  /* init buffer */
  n=(IS_TRANS(t)?DEG_TRANS(t):LEN_LIST(t));
  ResizeTmpTrans(n);
  pttmp=ADDR_TRANS4(TmpTrans);
  for(i=0;i<n;i++) pttmp[i]=0;

  if(TNUM_OBJ(t)==T_TRANS2){/* and LEN_LIST(l)<=deg(f)<65536 */
    ptt2=ADDR_TRANS2(t);
    for(i=LEN_LIST(l);i>=1;i--){
      j=(UInt) INT_INTOBJ(ELM_LIST(l, i));
      if(j<=n){
        if(pttmp[ptt2[j-1]]!=0) return False;
        pttmp[ptt2[j-1]]=1;
      }
    }
  } else if(TNUM_OBJ(t)==T_TRANS4){
    ptt4=ADDR_TRANS4(t);
    for(i=LEN_LIST(l);i>=1;i--){
      j=(UInt) INT_INTOBJ(ELM_LIST(l, i));
      if(j<=n) {
        if(pttmp[ptt4[j-1]]!=0) return False;
        pttmp[ptt4[j-1]]=1;
      }
    }
  }else if(n<=65536){/* t is a list */
    for(i=LEN_LIST(l);i>=1;i--){
      j=INT_INTOBJ(ELM_LIST(l, i));    
      if(j<=n){
        if(pttmp[INT_INTOBJ(ELM_LIST(t, j))-1]!=0) return False;
        pttmp[INT_INTOBJ(ELM_LIST(t, j))-1]=1;
      }
    }
  }else{ /* t is a list */
    for(i=LEN_LIST(l);i>=1;i--){
      j=INT_INTOBJ(ELM_LIST(l, i));    
      if(j<=n){
        if(pttmp[INT_INTOBJ(ELM_LIST(t, j))-1]!=0) return False;
        pttmp[INT_INTOBJ(ELM_LIST(t, j))-1]=1;
      }
    }
  }
  return True;
}

/* the perm2 of im(f) induced by f^-1*g, no checking*/
//unchanged
Obj FuncPERM_LEFT_QUO_TRANS_NC(Obj self, Obj f, Obj g)
{ UInt2   *ptf2, *ptg2, *ptp2;
  UInt4   *ptf4, *ptg4, *ptp4;
  UInt    deg, i;
  Obj     perm;

  if(TNUM_OBJ(f)==T_TRANS2&&TNUM_OBJ(g)==T_TRANS2){
    deg=DEG_TRANS2(f);
    UNEQUAL_DEG_TRANS(DEG_TRANS2(g)!=deg);
    
    perm=NEW_PERM2(deg);
    ptp2=ADDR_PERM2(perm);
    ptf2=ADDR_TRANS2(f);
    ptg2=ADDR_TRANS2(g);
  
    for(i=0;i<deg;i++) ptp2[i]=i;
    for(i=0;i<deg;i++) ptp2[ptf2[i]]=ptg2[i];
    return perm;
  } 
  
  UNEQUAL_DEG_TRANS(TNUM_OBJ(f)!=T_TRANS4||TNUM_OBJ(g)!=T_TRANS4||DEG_TRANS4(f)!=DEG_TRANS4(g));

  deg=DEG_TRANS4(f);
  perm=NEW_PERM4(deg);
  ptp4=ADDR_PERM4(perm);
  ptf4=ADDR_TRANS4(f);
  ptg4=ADDR_TRANS4(g);

  for(i=0;i<deg;i++) ptp4[i]=i;
  for(i=0;i<deg;i++) ptp4[ptf4[i]]=ptg4[i];
  return perm;
}

/* transformation from image set and flat kernel, no checking*/
//unchanged
Obj FuncTRANS_IMG_KER_NC(Obj self, Obj img, Obj ker){
  UInt    deg=LEN_LIST(ker);
  Obj     f;
  UInt2*  ptf2;
  UInt4*  ptf4;
  UInt    i;
  
  if(deg<=65536){
    f=NEW_TRANS2(deg);
    ptf2=ADDR_TRANS2(f);
    IMG_TRANS2(f)=img;
    KER_TRANS2(f)=ker;

    for(i=0;i<deg;i++){
      ptf2[i]=INT_INTOBJ(ELM_LIST(img, INT_INTOBJ(ELM_LIST(ker, i+1))))-1;
    }
  }else{
    f=NEW_TRANS4(deg);
    ptf4=ADDR_TRANS4(f);
    IMG_TRANS4(f)=img;
    KER_TRANS4(f)=ker;
        
    for(i=0;i<deg;i++){
      ptf4[i]=INT_INTOBJ(ELM_LIST(img, INT_INTOBJ(ELM_LIST(ker, i+1))))-1;
    }   
  }
  CHANGED_BAG(f);
  return f;
}

/* idempotent from image set and flat kernel, no checking.
*  Note that this is not the same as the previous function */
//unchanged
Obj FuncIDEM_IMG_KER_NC(Obj self, Obj img, Obj ker){
  UInt    deg=LEN_LIST(ker);
  UInt    rank=LEN_LIST(img);
  Obj     f;
  UInt2   *ptf2;
  UInt4   *ptf4, *pttmp;
  UInt    i, j;
  
  if(!IS_PLIST(img)) PLAIN_LIST(img);
  if(!IS_PLIST(ker)) PLAIN_LIST(ker);
    
  if(IS_MUTABLE_OBJ(img)) RetypeBag(img, TNUM_OBJ(img)+IMMUTABLE);
  if(IS_MUTABLE_OBJ(ker)) RetypeBag(ker, TNUM_OBJ(ker)+IMMUTABLE);

  ResizeTmpTrans(deg);
  pttmp=ADDR_TRANS4(TmpTrans);
  
  // setup the lookup table
  for(i=0;i<rank;i++){
    j=INT_INTOBJ(ELM_PLIST(img, i+1));
    pttmp[INT_INTOBJ(ELM_PLIST(ker, j))-1]=j-1;
  }
  if(deg<=65536){
    f=NEW_TRANS2(deg);
    ptf2=ADDR_TRANS2(f);
    pttmp=ADDR_TRANS4(TmpTrans); 

    IMG_TRANS2(f)=img;
    KER_TRANS2(f)=ker;

    for(i=0;i<deg;i++) ptf2[i]=pttmp[INT_INTOBJ(ELM_PLIST(ker, i+1))-1];
  }else{
    f=NEW_TRANS4(deg);
    ptf4=ADDR_TRANS4(f);
    pttmp=ADDR_TRANS4(TmpTrans); 
    
    IMG_TRANS4(f)=img;
    KER_TRANS4(f)=ker;

    for(i=0;i<deg;i++) ptf4[i]=pttmp[INT_INTOBJ(ELM_PLIST(ker, i+1))-1];
  }
  CHANGED_BAG(f);
  return f;
}

/* an inverse of a transformation f*g*f=f and g*f*g=g */
//unchanged
Obj FuncINV_TRANS(Obj self, Obj f){
  UInt2   *ptf2, *ptg2;
  UInt4   *ptf4, *ptg4;
  UInt    deg, i;
  Obj     g;

  if(FuncIS_ID_TRANS(self, f)==True) return f;

  if(TNUM_OBJ(f)==T_TRANS2){
    deg=DEG_TRANS2(f);
    g=NEW_TRANS2(deg);
    ptf2=ADDR_TRANS2(f);
    ptg2=ADDR_TRANS2(g);
    for(i=0;i<deg;i++) ptg2[i]=0;
    for(i=deg-1;i>0;i--) ptg2[ptf2[i]]=i;
    /* to ensure that 1 is in the image and so rank of g equals that of f*/
    ptg2[ptf2[0]]=0;
  }else{
    deg=DEG_TRANS4(f);
    g=NEW_TRANS4(deg);
    ptf4=ADDR_TRANS4(f);
    ptg4=ADDR_TRANS4(g);
    for(i=0;i<deg;i++) ptg4[i]=0;
    for(i=deg-1;i>0;i--) ptg4[ptf4[i]]=i;
    /* to ensure that 1 is in the image and so rank of g equals that of f*/
    ptg4[ptf4[0]]=0;
  }
  return g;
}

/* a transformation g such that g: i^f -> i for all i in list 
 * where it is supposed that f is injective on list */
//unchanged
Obj FuncINV_LIST_TRANS(Obj self, Obj list, Obj f){
  UInt2   *ptf2, *ptg2; 
  UInt4   *ptf4, *ptg4; 
  UInt    deg, i, j, len;
  Obj     g;

  if(TNUM_OBJ(f)==T_TRANS2){
    deg=DEG_TRANS2(f);      
    g=NEW_TRANS2(deg);
    ptf2=ADDR_TRANS2(f);
    ptg2=ADDR_TRANS2(g);
    
    i=INT_INTOBJ(ELM_LIST(list, 1))-1;
    len=LEN_LIST(list);
    for(j=0;j<deg;j++) ptg2[j]=i;
    for(j=1;j<=len;j++){
      i=INT_INTOBJ(ELM_LIST(list, j))-1;
      ptg2[ptf2[i]]=i;
    }
  }else{
    deg=DEG_TRANS4(f);      
    g=NEW_TRANS4(deg);
    ptf4=ADDR_TRANS4(f);
    ptg4=ADDR_TRANS4(g);
    
    i=INT_INTOBJ(ELM_LIST(list, 1))-1;
    len=LEN_LIST(list);
    for(j=0;j<deg;j++) ptg4[j]=i;
    for(j=1;j<=len;j++){
      i=INT_INTOBJ(ELM_LIST(list, j))-1;
      ptg4[ptf4[i]]=i;
    }
  }
  return g;
}

/* returns the permutation p conjugating image set f to image set g 
 * when ker(f)=ker(g) so that gf^-1(i)=p(i). 
 * This is the same as MappingPermListList(IMAGE_TRANS(f), IMAGE_TRANS(g)); */
//unchanged
Obj FuncTRANS_IMG_CONJ(Obj self, Obj f, Obj g){
  Obj     perm;
  UInt2   *ptp2, *ptf2, *ptg2;
  UInt4   *ptsrc, *ptdst, *ptp4, *ptf4, *ptg4;
  UInt    deg, i, j;

  deg=DEG_TRANS(f);
  ResizeTmpTrans(2*deg);

  ptsrc=ADDR_TRANS4(TmpTrans);
  ptdst=ADDR_TRANS4(TmpTrans)+deg;
  
  for(i=0;i<deg;i++){ ptsrc[i]=0; ptdst[i]=0; }
  
  if(TNUM_OBJ(f)==T_TRANS2){
    perm=NEW_PERM2(deg);

    ptsrc=ADDR_TRANS4(TmpTrans);
    ptdst=ADDR_TRANS4(TmpTrans)+deg;
    ptp2=ADDR_PERM2(perm);
    ptf2=ADDR_TRANS2(f);
    ptg2=ADDR_TRANS2(g);
    
    for(i=0;i<deg;i++){
      ptsrc[ptf2[i]]=1;
      ptdst[ptg2[i]]=1;
      ptp2[ptf2[i]]=ptg2[i];
    }
    j=0;
    for(i=0;i<deg;i++){
      if(ptsrc[i]==0){
        while(ptdst[j]!=0){ j++; } 
        ptp2[i]=j;
        j++;
      }
    }
  } else {
    perm=NEW_PERM4(deg);

    ptsrc=ADDR_TRANS4(TmpTrans);
    ptdst=ADDR_TRANS4(TmpTrans)+deg;
    ptp4=ADDR_PERM4(perm);
    ptf4=ADDR_TRANS4(f);
    ptg4=ADDR_TRANS4(g);
    
    for(i=0;i<deg;i++){
      ptsrc[ptf4[i]]=1;
      ptdst[ptg4[i]]=1;
      ptp4[ptf4[i]]=ptg4[i];
    }
    j=0;
    for(i=0;i<deg;i++){
      if(ptsrc[i]==0){
        while(ptdst[j]!=0){ j++; } 
        ptp4[i]=j;
        j++;
      }
    }
  }
  return perm;
}

/* the least m, r such that f^m+r=f^m */
//unchanged
Obj FuncINDEX_PERIOD_TRANS(Obj self, Obj f){
  UInt2   *ptf2;
  UInt4   *ptf4, *ptseen, *ptlast, *ptcurrent, *tmp; 
  UInt    deg, i, current, last, pow, len, j;
  Obj     ord, out;
  Int     s, t, gcd;
 
  deg=DEG_TRANS(f);
  ResizeTmpTrans(3*deg);
  
  ptseen=ADDR_TRANS4(TmpTrans);
  ptlast=ADDR_TRANS4(TmpTrans)+deg;
  ptcurrent=ADDR_TRANS4(TmpTrans)+2*deg;
  
  for(i=0;i<deg;i++) ptcurrent[i]=i;
  
  if(TNUM_OBJ(f)==T_TRANS2){
    ptf2=ADDR_TRANS2(f);
    last=0; current=deg; pow=0; 

    /* find least power of f which is a permutation */
    while(last!=current){
      pow++; last=current; current=0; 
      tmp=ptlast; ptlast=ptcurrent; ptcurrent=tmp;

      for(i=0;i<deg;i++){ptseen[i]=0; ptcurrent[i]=0;}
      
      for(i=0;i<last;i++){ /* loop over the last image */
        if(ptseen[ptf2[ptlast[i]]]==0){
          ptseen[ptf2[ptlast[i]]]=1;
          ptcurrent[current++]=ptf2[ptlast[i]];
        }
        /* ptcurrent holds the image set of f^pow (unsorted) */
        /* ptseen is a lookup for membership in ptcurrent */
      }
    }
    /* find the order of the perm induced by f on im_set(f^pow) */

    /* clear the buffer bag (ptlast) */
    for(i=0;i<deg;i++) ptlast[i]=0;

    /* start with order 1 */
    ord=INTOBJ_INT(1);

    /* loop over all cycles */
    for(i=0;i<deg;i++){
      /* if we haven't looked at this cycle so far */
      if(ptlast[i]==0&&ptseen[i]!=0&&ptf2[i]!=i){

        /* find the length of this cycle                           */
        len=1;
        for(j=ptf2[i];j!=i;j=ptf2[j]){
            len++;  ptlast[j]=1;
        }

        /* compute the gcd with the previously order ord           */
        /* Note that since len is single precision, ord % len is to*/
        gcd=len;  s=INT_INTOBJ(ModInt(ord,INTOBJ_INT(len)));
        while (s!= 0){
            t=s;  s=gcd%s;  gcd=t;
        }
        ord=ProdInt(ord,INTOBJ_INT(len/gcd));
      }
    }
  } else {
    ptf4=ADDR_TRANS4(f);
    last=0; current=deg; pow=0; 

    /* find least power of f which is a permutation */
    while(last!=current){
      pow++; last=current; current=0; 
      tmp=ptlast; ptlast=ptcurrent; ptcurrent=tmp;

      for(i=0;i<deg;i++){ptseen[i]=0; ptcurrent[i]=0;}
      
      for(i=0;i<last;i++){ /* loop over the last image */
        if(ptseen[ptf4[ptlast[i]]]==0){
          ptseen[ptf4[ptlast[i]]]=1;
          ptcurrent[current++]=ptf4[ptlast[i]];
        }
        /* ptcurrent holds the image set of f^pow (unsorted) */
        /* ptseen is a lookup for membership in ptcurrent */
      }
    }
    /* find the order of the perm induced by f on im_set(f^pow) */

    /* clear the buffer bag (ptlast) */
    for(i=0;i<deg;i++) ptlast[i]=0;

    /* start with order 1 */
    ord=INTOBJ_INT(1);

    /* loop over all cycles */
    for(i=0;i<deg;i++){
      /* if we haven't looked at this cycle so far */
      if(ptlast[i]==0&&ptseen[i]!=0&&ptf4[i]!=i){

        /* find the length of this cycle                           */
        len=1;
        for(j=ptf4[i];j!=i;j=ptf4[j]){
            len++;  ptlast[j]=1;
        }

        /* compute the gcd with the previously order ord           */
        /* Note that since len is single precision, ord % len is to*/
        gcd=len;  s=INT_INTOBJ(ModInt(ord,INTOBJ_INT(len)));
        while (s!= 0){
            t=s;  s=gcd%s;  gcd=t;
        }
        ord=ProdInt(ord,INTOBJ_INT(len/gcd));
      }
    }
  }
  out=NEW_PLIST(T_PLIST_CYC, 2);
  SET_LEN_PLIST(out, 2);
  SET_ELM_PLIST(out, 1, INTOBJ_INT(--pow));
  SET_ELM_PLIST(out, 2, ord);
  return out;
}

/* the least power of <f> which is an idempotent */
//unchanged
Obj FuncSMALLEST_IDEM_POW_TRANS( Obj self, Obj f ){
  Obj x, ind, per, pow;

  x=FuncINDEX_PERIOD_TRANS(self, f);
  ind=ELM_PLIST(x, 1);
  per=ELM_PLIST(x, 2);
  pow=per;
  while(LtInt(pow, ind)) pow=SumInt(pow, per);
  return pow;
}

/* the kernel obtained by multiplying f by any g with ker(g)=ker */
//unchanged
Obj FuncON_KERNEL_ANTI_ACTION(Obj self, Obj ker, Obj f){
  UInt2   *ptf2;
  UInt4   *ptf4, *pttmp;
  UInt    deg, i, j, rank;
  Obj     out;

  if(INT_INTOBJ(ELM_LIST(ker, LEN_LIST(ker)))==0){ 
    return FuncFLAT_KERNEL_TRANS(self, f);
  }

  deg=DEG_TRANS(f);
  out=NEW_PLIST(T_PLIST_CYC+IMMUTABLE, (Int) deg);
  SET_LEN_PLIST(out, (Int) deg);
  
  ResizeTmpTrans(deg);
  pttmp=ADDR_TRANS4(TmpTrans);
  for(i=0;i<deg;i++) pttmp[i]=0;
  
  rank=1;
  
  if(TNUM_OBJ(f)==T_TRANS2){
    ptf2=ADDR_TRANS2(f);
    for(i=0;i<deg;i++){
      j=INT_INTOBJ(ELM_LIST(ker, ptf2[i]+1))-1; /* f first! */
      if(pttmp[j]==0) pttmp[j]=rank++;
      SET_ELM_PLIST(out, i+1, INTOBJ_INT(pttmp[j]));
    }     
  } else {
    ptf4=ADDR_TRANS4(f);
    for(i=0;i<deg;i++){
      j=INT_INTOBJ(ELM_LIST(ker, ptf4[i]+1))-1; /* f first! */
      if(pttmp[j]==0) pttmp[j]=rank++;
      SET_ELM_PLIST(out, i+1, INTOBJ_INT(pttmp[j]));
    }     
  }
  return out; 
}  

/* if f is a transformation and g is a transformatio such that
 * g=Transformation(ker(g));, then INV_KER_TRANS(ker(g), f) returns 
 * a transformation h such that h*f*g=g and so
 * in particular, the following holds:
 * ON_KERNEL_ANTI_ACTION(ON_KERNEL_ANTI_ACTION(ker(g), f), h)=ker(g);
 * only if the rank(f*g)=rank(g), i.e. the # of classes of ker(g) equals the
 * number of classes of ker(f*g).
 * */
//unchanged
Obj FuncINV_KER_TRANS(Obj self, Obj X, Obj f){
  Obj     g;
  UInt2   *ptf2, *ptg2;
  UInt4   *pttmp, *ptf4, *ptg4;
  UInt    deg, i;
  
  deg=DEG_TRANS(f);
  ResizeTmpTrans(deg);

  if(TNUM_OBJ(f)==T_TRANS2){
    g=NEW_TRANS2(deg);
    pttmp=ADDR_TRANS4(TmpTrans);
    ptf2=ADDR_TRANS2(f);
    ptg2=ADDR_TRANS2(g);
    
    /* calculate a transversal of Y */
    for(i=0;i<deg;i++) pttmp[INT_INTOBJ(ELM_LIST(X, ptf2[i]+1))-1]=i;
    for(i=LEN_LIST(X);i>=1;i--) ptg2[i-1]=pttmp[INT_INTOBJ(ELM_LIST(X, i))-1];
  } else {
    g=NEW_TRANS4(deg);
    pttmp=ADDR_TRANS4(TmpTrans);
    ptf4=ADDR_TRANS4(f);
    ptg4=ADDR_TRANS4(g);
    
    /* calculate a transversal of Y */
    for(i=0;i<deg;i++) pttmp[INT_INTOBJ(ELM_LIST(X, ptf4[i]+1))-1]=i;
    for(i=LEN_LIST(X);i>=1;i--) ptg4[i-1]=pttmp[INT_INTOBJ(ELM_LIST(X, i))-1];
  }
  return g;
}

/* test if a transformation is an idempotent. */
//unchanged
Obj FuncIS_IDEM_TRANS(Obj self, Obj f){
  UInt2*  ptf2;
  UInt4*  ptf4;
  UInt    deg, i;
  
  if(TNUM_OBJ(f)==T_TRANS2){
    deg=DEG_TRANS2(f);
    ptf2=ADDR_TRANS2(f);
    for(i=0;i<deg;i++){
      if(ptf2[ptf2[i]]!=ptf2[i]){
        return False;
      }
    }
  } else {
    deg=DEG_TRANS4(f);
    ptf4=ADDR_TRANS4(f);
    for(i=0;i<deg;i++){
      if(ptf4[ptf4[i]]!=ptf4[i]){
        return False;
      }
    }
  }
  return True;
}


/* returns the least list <out> such that for all <i> in [1..degree(f)]
 * there exists <j> in <out> and a pos int <k> such that <j^(f^k)=i>. */
//unchanged
Obj FuncCOMPONENT_REPS_TRANS(Obj self, Obj f){
  Obj     out;
  UInt2   *ptf2; 
  UInt4   *ptf4, *ptseen, *ptlookup, *ptlens, *ptimg;
  UInt    deg, i, nr, count, m, j, k;

  deg=DEG_TRANS(f);
  
  ResizeTmpTrans(4*deg);
  out=NEW_PLIST(T_PLIST_CYC_SSORT, deg);
  
  ptseen=ADDR_TRANS4(TmpTrans);
  ptlookup=ADDR_TRANS4(TmpTrans)+deg;
  ptlens=ADDR_TRANS4(TmpTrans)+2*deg;
  ptimg=ADDR_TRANS4(TmpTrans)+3*deg;
    
  for(i=0;i<deg;i++){ ptseen[i]=0; ptlookup[i]=0; ptlens[i]=0; ptimg[i]=0; }

  if(TNUM_OBJ(f)==T_TRANS2){
    ptf2=ADDR_TRANS2(f);
    
    /* points in the image of f */
    for(i=0;i<deg;i++) ptimg[ptf2[i]]=1; 

    nr=0; m=0; count=0;

    /* components corresponding to points not in image */
    for(i=0;i<deg;i++){
      if(ptimg[i]==0&&ptseen[i]==0){
        m++;
        for(j=i;ptseen[j]==0;j=ptf2[j]){ ptseen[j]=m; count++;} 
        if(ptseen[j]==m){/* new component */
          k=nr;
          ptlookup[m-1]=nr;
          SET_ELM_PLIST(out, ++nr, NEW_PLIST(T_PLIST_CYC_SSORT, deg-count));
          CHANGED_BAG(out);
          ptlens=ADDR_TRANS4(TmpTrans)+2*deg;
        }else{ /* old component */
          k=ptlookup[ptseen[j]-1];
          ptlookup[m-1]=k;
        }
        AssPlist(ELM_PLIST(out, k+1), ++ptlens[k], INTOBJ_INT(i+1));
      }
      ptf2=ADDR_TRANS2(f);
      ptseen=ADDR_TRANS4(TmpTrans);
      ptlookup=ADDR_TRANS4(TmpTrans)+deg;
      ptlens=ADDR_TRANS4(TmpTrans)+2*deg;
      ptimg=ADDR_TRANS4(TmpTrans)+3*deg;
    }

    for(i=0;i<nr;i++){
      SHRINK_PLIST(ELM_PLIST(out, i+1), (Int) ptlens[i]);
      SET_LEN_PLIST(ELM_PLIST(out, i+1), (Int) ptlens[i]);
    }

    ptseen=ADDR_TRANS4(TmpTrans);
    ptf2=ADDR_TRANS2(f);

    /* components corresponding to cycles */
    for(i=0;i<deg;i++){
      if(ptseen[i]==0){
        for(j=ptf2[i];j!=i;j=ptf2[j]) ptseen[j]=1;
        
        SET_ELM_PLIST(out, ++nr, NEW_PLIST(T_PLIST_CYC_SSORT, 1));
        SET_LEN_PLIST(ELM_PLIST(out, nr), 1);
        SET_ELM_PLIST(ELM_PLIST(out, nr), 1, INTOBJ_INT(i+1));
        CHANGED_BAG(out);
        
        ptseen=ADDR_TRANS4(TmpTrans);
        ptf2=ADDR_TRANS2(f);
      }
    }
  } else {

    ptf4=ADDR_TRANS4(f);
    /* points in the image of f */
    for(i=0;i<deg;i++){ ptimg[ptf4[i]]=1; }

    nr=0; m=0; count=0;

    /* components corresponding to points not in image */
    for(i=0;i<deg;i++){
      if(ptimg[i]==0&&ptseen[i]==0){
        m++;
        for(j=i;ptseen[j]==0;j=ptf4[j]){ ptseen[j]=m; count++;} 
        if(ptseen[j]==m){/* new component */
          k=nr;
          ptlookup[m-1]=nr;
          SET_ELM_PLIST(out, ++nr, NEW_PLIST(T_PLIST_CYC_SSORT, deg-count));
          CHANGED_BAG(out);
          ptlens=ADDR_TRANS4(TmpTrans)+2*deg;
        }else{ /* old component */
          k=ptlookup[ptseen[j]-1];
          ptlookup[m-1]=k;
        }
        AssPlist(ELM_PLIST(out, k+1), ++ptlens[k], INTOBJ_INT(i+1));
      }
      ptf4=ADDR_TRANS4(f); 
      ptseen=ADDR_TRANS4(TmpTrans);
      ptlookup=ADDR_TRANS4(TmpTrans)+deg;
      ptlens=ADDR_TRANS4(TmpTrans)+2*deg;
      ptimg=ADDR_TRANS4(TmpTrans)+3*deg;
    }

    for(i=0;i<nr-1;i++){
      SHRINK_PLIST(ELM_PLIST(out, i+1), (Int) ptlens[i]);
      SET_LEN_PLIST(ELM_PLIST(out, i+1), (Int) ptlens[i]);
    }
    
    ptseen=ADDR_TRANS4(TmpTrans);
    ptf4=ADDR_TRANS4(f);

    /* components corresponding to cycles */
    for(i=0;i<deg;i++){
      if(ptseen[i]==0){
        for(j=ptf4[i];j!=i;j=ptf4[j]) ptseen[j]=1;
        
        SET_ELM_PLIST(out, ++nr, NEW_PLIST(T_PLIST_CYC_SSORT, 1));
        SET_LEN_PLIST(ELM_PLIST(out, nr), 1);
        SET_ELM_PLIST(ELM_PLIST(out, nr), 1, INTOBJ_INT(i+1));
        CHANGED_BAG(out);

        ptseen=ADDR_TRANS4(TmpTrans);
        ptf4=ADDR_TRANS4(f);
      }
    }
  }
 
  SHRINK_PLIST(out, (Int) nr);
  SET_LEN_PLIST(out,  (Int) nr);
  return out;
}

/* the number of components of a transformation (as a functional digraph) */
//unchanged
Obj FuncNR_COMPONENTS_TRANS(Obj self, Obj f){
  UInt    nr, m, i, j, deg;
  UInt2   *ptf2;
  UInt4   *ptseen, *ptf4;

  deg=DEG_TRANS(f);
  
  ResizeTmpTrans(deg);
  ptseen=ADDR_TRANS4(TmpTrans);
  for(i=0;i<deg;i++) ptseen[i]=0;
  
  nr=0; m=0;

  if(TNUM_OBJ(f)==T_TRANS2){
    ptf2=ADDR_TRANS2(f);
    for(i=0;i<deg;i++){
      if(ptseen[i]==0){
        m++;
        for(j=i;ptseen[j]==0;j=ptf2[j]) ptseen[j]=m; 
        if(ptseen[j]==m) nr++;
      }
    }
  }else{
    ptf4=ADDR_TRANS4(f);
    for(i=0;i<deg;i++){
      if(ptseen[i]==0){
        m++;
        for(j=i;ptseen[j]==0;j=ptf4[j]) ptseen[j]=m; 
        if(ptseen[j]==m) nr++;
      }
    }
  }
  return INTOBJ_INT(nr);
}

/* the components of a transformation (as a functional digraph) */
//unchanged
Obj FuncCOMPONENTS_TRANS(Obj self, Obj f){
  UInt    deg, i, nr, m, j;
  UInt2   *ptf2;
  UInt4   *ptseen, *ptlookup, *ptlens, *ptf4;
  Obj     out;
  
  deg=DEG_TRANS(f);
  ResizeTmpTrans(3*deg);
  ptseen=ADDR_TRANS4(TmpTrans);
  ptlookup=ADDR_TRANS4(TmpTrans)+deg;
  ptlens=ADDR_TRANS4(TmpTrans)+2*deg;
  
  for(i=0;i<deg;i++){ ptseen[i]=0; ptlookup[i]=0; ptlens[i]=0; }

  nr=0; m=0;
  
  if(TNUM_OBJ(f)==T_TRANS2){
    //find components
    ptf2=ADDR_TRANS2(f);
    for(i=0;i<deg;i++){
      if(ptseen[i]==0){
        m++;
        for(j=i;ptseen[j]==0;j=ptf2[j]){ ptseen[j]=m; } 
        if(ptseen[j]==m){
          ptlookup[m-1]=nr++;
        }else{
          ptlookup[m-1]=ptlookup[ptseen[j]-1];
        }
      }
    }
  } else {
    //find components 
    ptf4=ADDR_TRANS4(f);
    for(i=0;i<deg;i++){
      if(ptseen[i]==0){
        m++;
        for(j=i;ptseen[j]==0;j=ptf4[j]){ ptseen[j]=m; } 
        if(ptseen[j]==m){
          ptlookup[m-1]=nr++;
        }else{
          ptlookup[m-1]=ptlookup[ptseen[j]-1];
        }
      }
    }
  }
  
  out=NEW_PLIST(T_PLIST_CYC_SSORT, nr);
  SET_LEN_PLIST(out, (Int) nr);

  // install the points in out
  for(i=0;i<deg;i++){
    ptseen=ADDR_TRANS4(TmpTrans);
    ptlookup=ADDR_TRANS4(TmpTrans)+deg;
    ptlens=ADDR_TRANS4(TmpTrans)+2*deg;
    
    m=ptlookup[ptseen[i]-1];
    if(ptlens[m]==0){
      SET_ELM_PLIST(out, m+1, NEW_PLIST(T_PLIST_CYC_SSORT, deg));
      CHANGED_BAG(out);
      ptlens=ADDR_TRANS4(TmpTrans)+2*deg;
    }
    AssPlist(ELM_PLIST(out, m+1), (Int) ++ptlens[m], INTOBJ_INT(i+1));
  }
  
  ptlens=ADDR_TRANS4(TmpTrans)+2*deg;
  for(i=0;i<nr;i++){
    SHRINK_PLIST(ELM_PLIST(out, i+1), (Int) ptlens[i]);
    SET_LEN_PLIST(ELM_PLIST(out, i+1), (Int) ptlens[i]);
  }
  return out;
}

//changed
Obj FuncCOMPONENT_TRANS_INT(Obj self, Obj f, Obj pt){
  UInt    deg, cpt, len, i;
  Obj     out;
  UInt2   *ptf2;
  UInt4   *ptseen, *ptf4;
    
  deg=DEG_TRANS(f);
  cpt=INT_INTOBJ(pt)-1;
  
  if(cpt>=deg){
    out=NEW_PLIST(T_PLIST_CYC_SSORT, 1);
    SET_LEN_PLIST(out, 1);
    SET_ELM_PLIST(out, 1, pt);
    return out;
  }
  ResizeTmpTrans(deg); 
  out=NEW_PLIST(T_PLIST_CYC, deg);
  
  ptseen=ADDR_TRANS4(TmpTrans);
  for(i=0;i<deg;i++) ptseen[i]=0;
  
  len=0;
  
  //install the points
  if(TNUM_OBJ(f)==T_TRANS2){
    ptf2=ADDR_TRANS2(f);
    do{ SET_ELM_PLIST(out, ++len, INTOBJ_INT(cpt+1));
        ptseen[cpt]=1;
        cpt=ptf2[cpt];
    }while(ptseen[cpt]==0);
  } else {
    ptf4=ADDR_TRANS4(f);
    do{ SET_ELM_PLIST(out, ++len, INTOBJ_INT(cpt+1));
        ptseen[cpt]=1;
        cpt=ptf4[cpt];
    }while(ptseen[cpt]==0);
  }
  SHRINK_PLIST(out, (Int) len);
  SET_LEN_PLIST(out, (Int) len);
  return out;
}

//new
Obj FuncCYCLE_TRANS_INT(Obj self, Obj f, Obj pt){
  UInt    deg, cpt, len, i;
  Obj     out;
  UInt2   *ptf2;
  UInt4   *ptseen, *ptf4;
    
  deg=DEG_TRANS(f);
  cpt=INT_INTOBJ(pt)-1;
  
  if(cpt>=deg){
    out=NEW_PLIST(T_PLIST_CYC_SSORT, 1);
    SET_LEN_PLIST(out, 1);
    SET_ELM_PLIST(out, 1, pt);
    return out;
  }
 
  ResizeTmpTrans(deg);
  out=NEW_PLIST(T_PLIST_CYC, deg);
  
  ptseen=ADDR_TRANS4(TmpTrans);
  for(i=0;i<deg;i++) ptseen[i]=0;
  len=0;
  
  if(TNUM_OBJ(f)==T_TRANS2){
    ptf2=ADDR_TRANS2(f);
    //find component 
    do{ ptseen[cpt]=1;
        cpt=ptf2[cpt];
    }while(ptseen[cpt]==0);
    //find cycle
    i=cpt;
    do{ SET_ELM_PLIST(out, ++len, INTOBJ_INT(i+1));
        i=ptf2[i];
    }while(i!=cpt);
  } else {
    ptf4=ADDR_TRANS4(f);
    //find component 
    do{ ptseen[cpt]=1;
        cpt=ptf4[cpt];
    }while(ptseen[cpt]==0);
    //find cycle
    i=cpt;
    do{ SET_ELM_PLIST(out, ++len, INTOBJ_INT(i+1));
        i=ptf4[i];
    }while(i!=cpt);
  }
  SHRINK_PLIST (out, (Int) len);
  SET_LEN_PLIST(out, (Int) len);
  return out;
}

//new
Obj FuncCYCLES_TRANS_LIST(Obj self, Obj f, Obj list){
  UInt    deg, pt, len_list, len_out, i, j, m;
  Obj     out;
  UInt2   *ptf2;
  UInt4   *ptseen, *ptlens, *ptf4;
   
  deg=DEG_TRANS(f);
  len_list=LEN_LIST(list);
 
  ResizeTmpTrans(deg+len_list);
  out=NEW_PLIST(T_PLIST, len_list);
  
  ptseen=ADDR_TRANS4(TmpTrans);
  ptlens=ADDR_TRANS4(TmpTrans)+deg;

  for(i=0;i<deg;i++){ ptseen[i]=0; ptlens[i]=0; }
  for(;i<len_list;i++){ ptlens[i]=0; }

  len_out=0; m=0;
  
  if(TNUM_OBJ(f)==T_TRANS2){
    for(i=1;i<=len_list;i++){
      pt=INT_INTOBJ(ELM_LIST(list, i))-1;
      if(pt>=deg){
        SET_ELM_PLIST(out, ++len_out, NEW_PLIST(T_PLIST_CYC, 1));
        SET_ELM_PLIST(ELM_PLIST(out, len_out), 1, INTOBJ_INT(pt+1));
        CHANGED_BAG(out);
        ((ADDR_TRANS4(TmpTrans)+deg)[len_out])++; //ptlens[len_out]++
      } else {
        m++;
        ptseen=ADDR_TRANS4(TmpTrans);
        ptf2=ADDR_TRANS2(f);
        while(ptseen[pt]==0){ //look for pts already seen
          ptseen[pt]=m;
          pt=ptf2[pt];
        }
        if(ptseen[pt]==m){//new cycle
          j=pt;
          SET_ELM_PLIST(out, ++len_out, NEW_PLIST(T_PLIST_CYC, 32));
          CHANGED_BAG(out);
          ptlens=ADDR_TRANS4(TmpTrans)+deg;
          do{ AssPlist(ELM_PLIST(out, len_out), ++ptlens[len_out], 
               INTOBJ_INT(j+1));
              j=(ADDR_TRANS2(f))[j];
              ptlens=ADDR_TRANS4(TmpTrans)+deg;
          }while(j!=pt);
        }
      }
    }
  } else {
    for(i=1;i<=len_list;i++){
      pt=INT_INTOBJ(ELM_LIST(list, i))-1;
      if(pt>=deg){
        SET_ELM_PLIST(out, ++len_out, NEW_PLIST(T_PLIST_CYC, 1));
        SET_ELM_PLIST(ELM_PLIST(out, len_out), 1, INTOBJ_INT(pt+1));
        CHANGED_BAG(out);
        ((ADDR_TRANS4(TmpTrans)+deg)[len_out])++; //ptlens[len_out]++
      } else {
        m++;
        ptseen=ADDR_TRANS4(TmpTrans);
        ptf4=ADDR_TRANS4(f);
        while(ptseen[pt]==0){ //look for pts already seen
          ptseen[pt]=m;
          pt=ptf4[pt];
        }
        if(ptseen[pt]==m){//new cycle
          j=pt;
          SET_ELM_PLIST(out, ++len_out, NEW_PLIST(T_PLIST_CYC, 32));
          CHANGED_BAG(out);
          ptlens=ADDR_TRANS4(TmpTrans)+deg;
          do{ AssPlist(ELM_PLIST(out, len_out), ++ptlens[len_out], 
               INTOBJ_INT(j+1));
              j=(ADDR_TRANS4(f))[j];
              ptlens=ADDR_TRANS4(TmpTrans)+deg;
          }while(j!=pt);
        }
      }
    }
  }
  ptlens=ADDR_TRANS4(TmpTrans)+deg;
  for(i=1;i<=len_out;i++){
    SHRINK_PLIST (ELM_PLIST(out, i), (Int) ptlens[i]);
    SET_LEN_PLIST(ELM_PLIST(out, i), (Int) ptlens[i]);
  }
  SHRINK_PLIST (out, (Int) len_out);
  SET_LEN_PLIST(out, (Int) len_out);
  return out;
}

/* an idempotent transformation <e> with ker(e)=ker(f) */
//unchanged
Obj FuncLEFT_ONE_TRANS( Obj self, Obj f){
  Obj   ker, img;
  UInt  rank, n, i;

  if(TNUM_OBJ(f)==T_TRANS2){
    rank=RANK_TRANS2(f);
    ker=KER_TRANS2(f);
  } else {
    rank=RANK_TRANS4(f);
    ker=KER_TRANS4(f);
  }
  img=NEW_PLIST(T_PLIST_CYC, rank);
  n=0;

  for(i=0;n<rank;i++){
    if(INT_INTOBJ(ELM_PLIST(ker, i))==n+1){
      SET_ELM_PLIST(img, ++n, INTOBJ_INT(i));
    }
  }
  
  SET_LEN_PLIST(img, (Int) n);
  return FuncIDEM_IMG_KER_NC(self, img, ker);
}

/* an idempotent transformation <e> with im(e)=im(f) */
//unchanged
Obj FuncRIGHT_ONE_TRANS( Obj self, Obj f){
  Obj   ker, img;
  UInt  deg, len, i, j, n;

  if(TNUM_OBJ(f)==T_TRANS2){
    deg=DEG_TRANS2(f);
  } else {
    deg=DEG_TRANS4(f);
  }

  img=FuncIMAGE_SET_TRANS(self, f);
  ker=NEW_PLIST(T_PLIST_CYC, deg);
  SET_LEN_PLIST(ker, deg);
  len=LEN_PLIST(img);
  j=1; n=0;

  for(i=0;i<deg;i++){
    if(j<len&&i+1==INT_INTOBJ(ELM_PLIST(img, j+1))) j++;
    SET_ELM_PLIST(ker, ++n, INTOBJ_INT(j));
  }
  return FuncIDEM_IMG_KER_NC(self, img, ker);
}

/****************************************************************************/

/* GAP kernel functions */

/* one for a transformation */
Obj OneTrans2( Obj f ){
  return IdTrans2(DEG_TRANS2(f));
}

Obj OneTrans4( Obj f ){
  return IdTrans4(DEG_TRANS4(f));
}

/* equality for transformations */
//changed
Int EqTrans22 (Obj f, Obj g){
  UInt    i, def, deg;
  UInt2   *ptf, *ptg;

  ptf=ADDR_TRANS2(f);   ptg=ADDR_TRANS2(g);
  def=DEG_TRANS2(f);    deg=DEG_TRANS2(g);

  if(def<=deg){
    for(i=0;i<def;i++) if(*(ptf++)!=*(ptg++)) return 0L;
    for(;i<deg;i++)    if(*(ptg++)!=i) return 0L;
  } else {
    for(i=0;i<deg;i++) if(*(ptf++)!=*(ptg++)) return 0L;
    for(;i<def;i++)    if(*(ptf++)!=i) return 0L;
  }
  
  /* otherwise they must be equal */
  return 1L;
}

//changed
Int EqTrans24 (Obj f, Obj g){
  UInt   i, def, deg;
  UInt2  *ptf;
  UInt4  *ptg;

  ptf=ADDR_TRANS2(f);   ptg=ADDR_TRANS4(g);
  def=DEG_TRANS2(f);    deg=DEG_TRANS4(g);

  if(def<=deg){
    for(i=0;i<def;i++) if(*(ptf++)!=*(ptg++)) return 0L;
    for(;i<deg;i++)    if(*(ptg++)!=i) return 0L;
  } else {
    for(i=0;i<deg;i++) if(*(ptf++)!=*(ptg++)) return 0L;
    for(;i<def;i++)    if(*(ptf++)!=i) return 0L;
  }
  
  /* otherwise they must be equal */
  return 1L;
}

//changed
Int EqTrans42 (Obj f, Obj g){
  UInt   i, def, deg;
  UInt4  *ptf;
  UInt2  *ptg;

  ptf=ADDR_TRANS4(f);   ptg=ADDR_TRANS2(g);
  def=DEG_TRANS4(f);    deg=DEG_TRANS2(g);

  if(def<=deg){
    for(i=0;i<def;i++) if(*(ptf++)!=*(ptg++)) return 0L;
    for(;i<deg;i++)    if(*(ptg++)!=i)        return 0L;
  } else {
    for(i=0;i<deg;i++) if(*(ptf++)!=*(ptg++)) return 0L;
    for(;i<def;i++)    if(*(ptf++)!=i)        return 0L;
  }
  
  /* otherwise they must be equal */
  return 1L;
}

//changed
Int EqTrans44 (Obj f, Obj g){
  UInt   i, def, deg;
  UInt4  *ptf, *ptg;

  ptf=ADDR_TRANS4(f);   ptg=ADDR_TRANS4(g);
  def=DEG_TRANS4(f);    deg=DEG_TRANS4(g);

  if(def<=deg){
    for(i=0;i<def;i++) if(*(ptf++)!=*(ptg++)) return 0L;
    for(;i<deg;i++)    if(*(ptg++)!=i) return 0L;
  } else {
    for(i=0;i<deg;i++) if(*(ptf++)!=*(ptg++)) return 0L;
    for(;i<def;i++)    if(*(ptf++)!=i) return 0L;
  }
  
  /* otherwise they must be equal */
  return 1L;
}

/* less than for transformations */
//changed
Int LtTrans22(Obj f, Obj g){ 
  UInt   i, def, deg;
  UInt2  *ptf, *ptg;

  ptf=ADDR_TRANS2(f);   ptg=ADDR_TRANS2(g);
  def= DEG_TRANS2(f);   deg= DEG_TRANS2(g);
  
  if(def<=deg){
    for(i=0;i<def;i++){ 
      if(ptf[i]!=ptg[i]){
        if(ptf[i]<ptg[i]){ return 1L; } else { return 0L; }
      }
    }
    for(;i<deg;i++){
      if(ptg[i]!=i){ 
        if(i<ptg[i]){ return 1L; } else { return 0L; }
      }
    }
  } else {
    for(i=0;i<deg;i++){ 
      if(ptf[i]!=ptg[i]){
        if(ptf[i]<ptg[i]){ return 1L; } else { return 0L; }
      }
    }
    for(;i<def;i++){
      if(ptf[i]!=i){ 
        if(i<ptf[i]){ return 1L; } else { return 0L; }
      }
    }
  }
  return 0L;
}

//changed
Int LtTrans24(Obj f, Obj g){ 
  UInt   i, def, deg;
  UInt2  *ptf;
  UInt4  *ptg;

  ptf=ADDR_TRANS2(f);   ptg=ADDR_TRANS4(g);
  def= DEG_TRANS2(f);   deg= DEG_TRANS4(g);
  
  if(def<=deg){
    for(i=0;i<def;i++){ 
      if(ptf[i]!=ptg[i]){
        if(ptf[i]<ptg[i]){ return 1L; } else { return 0L; }
      }
    }
    for(;i<deg;i++){
      if(ptg[i]!=i){ 
        if(i<ptg[i]){ return 1L; } else { return 0L; }
      }
    }
  } else {
    for(i=0;i<deg;i++){ 
      if(ptf[i]!=ptg[i]){
        if(ptf[i]<ptg[i]){ return 1L; } else { return 0L; }
      }
    }
    for(;i<def;i++){
      if(ptf[i]!=i){ 
        if(i<ptf[i]){ return 1L; } else { return 0L; }
      }
    }
  }
  return 0L;
}

//changed
Int LtTrans42(Obj f, Obj g){ 
  UInt   i, def, deg;
  UInt4  *ptf; 
  UInt2  *ptg;

  ptf=ADDR_TRANS4(f);   ptg=ADDR_TRANS2(g);
  def= DEG_TRANS4(f);   deg= DEG_TRANS2(g);
  
  if(def<=deg){
    for(i=0;i<def;i++){ 
      if(ptf[i]!=ptg[i]){
        if(ptf[i]<ptg[i]){ return 1L; } else { return 0L; }
      }
    }
    for(;i<deg;i++){
      if(ptg[i]!=i){ 
        if(i<ptg[i]){ return 1L; } else { return 0L; }
      }
    }
  } else {
    for(i=0;i<deg;i++){ 
      if(ptf[i]!=ptg[i]){
        if(ptf[i]<ptg[i]){ return 1L; } else { return 0L; }
      }
    }
    for(;i<def;i++){
      if(ptf[i]!=i){ 
        if(i<ptf[i]){ return 1L; } else { return 0L; }
      }
    }
  }
  return 0L;
}

//changed
Int LtTrans44(Obj f, Obj g){ 
  UInt   i, def, deg;
  UInt4  *ptf, *ptg;

  ptf=ADDR_TRANS4(f);   ptg=ADDR_TRANS4(g);
  def= DEG_TRANS4(f);   deg= DEG_TRANS4(g);
  
  if(def<=deg){
    for(i=0;i<def;i++){ 
      if(ptf[i]!=ptg[i]){
        if(ptf[i]<ptg[i]){ return 1L; } else { return 0L; }
      }
    }
    for(;i<deg;i++){
      if(ptg[i]!=i){ 
        if(i<ptg[i]){ return 1L; } else { return 0L; }
      }
    }
  } else {
    for(i=0;i<deg;i++){ 
      if(ptf[i]!=ptg[i]){
        if(ptf[i]<ptg[i]){ return 1L; } else { return 0L; }
      }
    }
    for(;i<def;i++){
      if(ptf[i]!=i){ 
        if(i<ptf[i]){ return 1L; } else { return 0L; }
      }
    }
  }
  return 0L;
}

/* product of transformations */
Obj ProdTrans22(Obj f, Obj g){ 
  UInt2   *ptf, *ptg, *ptfg;
  UInt    i, def, deg, defg;
  Obj     fg;

  def =DEG_TRANS2(f);
  deg =DEG_TRANS2(g);
  defg=MAX(def,deg);
  fg=NEW_TRANS2(defg);
  
  ptfg=ADDR_TRANS2(fg);
  ptf =ADDR_TRANS2(f);
  ptg =ADDR_TRANS2(g);
  if(def<=deg){
    for(i=0;i<def;i++) *(ptfg++)=ptg[*(ptf++)];
    for(;i<deg;i++) *(ptfg++)=ptg[i];
  } else {
    for(i=0;i<def;i++) *(ptfg++)=IMAGE(ptf[i], ptg, deg);
  }
  return fg;
}

Obj ProdTrans24(Obj f, Obj g){ 
  UInt2   *ptf; 
  UInt4   *ptg, *ptfg;
  UInt    i, def, deg, defg;
  Obj     fg;

  def =DEG_TRANS2(f);
  deg =DEG_TRANS4(g);
  defg=MAX(def,deg);
  fg=NEW_TRANS4(defg);
  
  ptfg=ADDR_TRANS4(fg);
  ptf =ADDR_TRANS2(f);
  ptg =ADDR_TRANS4(g);
  if(def<=deg){
    for(i=0;i<def;i++) *ptfg++=ptg[*ptf++]; 
    for(;i<deg;i++) *ptfg++=ptg[i]; 
  } else {
    for(i=0;i<def;i++) *(ptfg++)=IMAGE(ptf[i], ptg, deg);
  }
  return fg;
}

Obj ProdTrans42(Obj f, Obj g){ 
  UInt4   *ptf, *ptfg;
  UInt2   *ptg;
  UInt    i, def, deg, defg;
  Obj     fg;

  def=DEG_TRANS4(f);
  deg=DEG_TRANS2(g);
  defg=MAX(def,deg);
  fg=NEW_TRANS4(defg);
  
  ptfg=ADDR_TRANS4(fg);
  ptf =ADDR_TRANS4(f);
  ptg =ADDR_TRANS2(g);
  if(def<=deg){
    for(i=0;i<def;i++) *(ptfg++)=ptg[*(ptf++)];
    for(;i<deg;i++) *(ptfg++)=ptg[i];
  } else {
    for(i=0;i<def;i++) *(ptfg++)=IMAGE(ptf[i], ptg, deg);
  }
  return fg;
}

Obj ProdTrans44(Obj f, Obj g){ 
  UInt4   *ptf, *ptg, *ptfg;
  UInt    i, def, deg, defg;
  Obj     fg;

  def=DEG_TRANS4(f);
  deg=DEG_TRANS4(g);
  defg=MAX(def,deg);
  fg=NEW_TRANS4(defg);
  
  ptfg=ADDR_TRANS4(fg);
  ptf =ADDR_TRANS4(f);
  ptg =ADDR_TRANS4(g);
  if(def<=deg){
    for(i=0;i<def;i++) *(ptfg++)=ptg[*(ptf++)];
    for(;i<deg;i++) *(ptfg++)=ptg[i];
  } else {
    for(i=0;i<def;i++) *(ptfg++)=IMAGE(ptf[i], ptg, deg);
  }
  return fg;
}

/* product of transformation and permutation */
Obj ProdTrans2Perm2(Obj f, Obj p){ /* p(f(x)) */
  UInt2   *ptf, *ptp, *ptfp;
  UInt    i, def, dep, defp;
  Obj     fp;

  dep =  DEG_PERM2(p);
  def = DEG_TRANS2(f);
  defp=MAX(def,dep);
  fp  = NEW_TRANS2(defp);

  ptfp=ADDR_TRANS2(fp);
  ptf =ADDR_TRANS2(f);
  ptp = ADDR_PERM2(p);
  
  if(def<=dep){
    for(i=0;i<def;i++) *(ptfp++)=ptp[*(ptf++)];
    for(;i<dep;i++) *(ptfp++)=ptp[i];
  } else {
    for(i=0;i<def;i++) *(ptfp++)=IMAGE(ptf[i], ptp, dep);
  }
  return fp;
}

Obj ProdTrans2Perm4(Obj f, Obj p){ /* p(f(x)) */
  UInt2   *ptf;
  UInt4   *ptp, *ptfp;
  UInt    i, def, dep, defp;
  Obj     fp;

  dep =  DEG_PERM4(p);
  def = DEG_TRANS2(f);
  defp= MAX(def,dep);
  fp  = NEW_TRANS4(defp);

  ptfp=ADDR_TRANS4(fp);
  ptf =ADDR_TRANS2(f);
  ptp = ADDR_PERM4(p);
  
  if(def<=dep){
    for(i=0;i<def;i++) *(ptfp++)=ptp[*(ptf++)];
    for(;i<dep;i++) *(ptfp++)=ptp[i];
  } else {
    for(i=0;i<def;i++) *(ptfp++)=IMAGE(ptf[i], ptp, dep);
  }
  return fp;
}

Obj ProdTrans4Perm2(Obj f, Obj p){ /* p(f(x)) */
  UInt4   *ptf, *ptfp;
  UInt2   *ptp;
  UInt    i, def, dep, defp;
  Obj     fp;

  dep =  DEG_PERM2(p);
  def = DEG_TRANS4(f);
  defp= MAX(def,dep);
  fp  = NEW_TRANS4(defp);

  ptfp=ADDR_TRANS4(fp);
  ptf =ADDR_TRANS4(f);
  ptp = ADDR_PERM2(p);
  
  if(def<=dep){
    for(i=0;i<def;i++) *(ptfp++)=ptp[*(ptf++)];
    for(;i<dep;i++) *(ptfp++)=ptp[i];
  } else {
    for(i=0;i<def;i++) *(ptfp++)=IMAGE(ptf[i], ptp, dep);
  }
  return fp;
}

Obj ProdTrans4Perm4(Obj f, Obj p){ /* p(f(x)) */
  UInt4   *ptf, *ptp, *ptfp;
  UInt    i, def, dep, defp;
  Obj     fp;

  dep =  DEG_PERM4(p);
  def = DEG_TRANS4(f);
  defp= MAX(def,dep);
  fp  = NEW_TRANS4(defp);
  
  ptfp=ADDR_TRANS4(fp);
  ptf =ADDR_TRANS4(f);
  ptp = ADDR_PERM4(p);
  
  if(def<=dep){
    for(i=0;i<def;i++) *(ptfp++)=ptp[*(ptf++)];
    for(;i<dep;i++) *(ptfp++)=ptp[i];
  } else {
    for(i=0;i<def;i++) *(ptfp++)=IMAGE(ptf[i], ptp, dep);
  }
  return fp;
}

/* product of permutation and transformation */
Obj ProdPerm2Trans2(Obj p, Obj f){ /* f(p(x)) */
  UInt2   *ptf, *ptp, *ptpf;
  UInt    i, def, dep, depf;
  Obj     pf;

  dep =  DEG_PERM2(p);
  def = DEG_TRANS2(f);
  depf= MAX(def,dep);
  pf  = NEW_TRANS2(depf);

  ptpf=ADDR_TRANS2(pf);
  ptf =ADDR_TRANS2(f);
  ptp = ADDR_PERM2(p);
  
  if(dep<=def){
    for(i=0;i<dep;i++) *(ptpf++)=ptf[*(ptp++)];
    for(;i<def;i++) *(ptpf++)=ptf[i];
  } else {
    for(i=0;i<dep;i++) *(ptpf++)=IMAGE(ptp[i], ptf, def);
  }
  return pf;
}

Obj ProdPerm2Trans4(Obj p, Obj f){ /* f(p(x)) */
  UInt4   *ptf, *ptpf; 
  UInt2   *ptp;
  UInt    i, def, dep, depf;
  Obj     pf;

  dep =  DEG_PERM2(p);
  def = DEG_TRANS4(f);
  depf= MAX(def,dep);
  pf  = NEW_TRANS4(depf);

  ptpf=ADDR_TRANS4(pf);
  ptf =ADDR_TRANS4(f);
  ptp = ADDR_PERM2(p);
  
  if(dep<=def){
    for(i=0;i<dep;i++) *(ptpf++)=ptf[*(ptp++)];
    for(;i<def;i++) *(ptpf++)=ptf[i];
  } else {
    for(i=0;i<dep;i++) *(ptpf++)=IMAGE(ptp[i], ptf, def);
  }
  return pf;
}

Obj ProdPerm4Trans2(Obj p, Obj f){ /* f(p(x)) */
  UInt2   *ptf;
  UInt4   *ptp, *ptpf;
  UInt    i, def, dep, depf;
  Obj     pf;

  dep =  DEG_PERM4(p);
  def = DEG_TRANS2(f);
  depf= MAX(def,dep);
  pf  = NEW_TRANS4(depf);

  ptpf=ADDR_TRANS4(pf);
  ptf =ADDR_TRANS2(f);
  ptp = ADDR_PERM4(p);
  
  if(dep<=def){
    for(i=0;i<dep;i++) *(ptpf++)=ptf[*(ptp++)];
    for(;i<def;i++) *(ptpf++)=ptf[i];
  } else {
    for(i=0;i<dep;i++) *(ptpf++)=IMAGE(ptp[i], ptf, def);
  }
  return pf;
}

Obj ProdPerm4Trans4(Obj p, Obj f){ /* f(p(x)) */
  UInt4   *ptf, *ptp, *ptpf;
  UInt    i, def, dep, depf;
  Obj     pf;

  dep =  DEG_PERM4(p);
  def = DEG_TRANS4(f);
  depf= MAX(def,dep);
  pf  = NEW_TRANS4(depf);

  ptpf=ADDR_TRANS4(pf);
  ptf =ADDR_TRANS4(f);
  ptp = ADDR_PERM4(p);
  
  if(dep<=def){
    for(i=0;i<dep;i++) *(ptpf++)=ptf[*(ptp++)];
    for(;i<def;i++) *(ptpf++)=ptf[i];
  } else {
    for(i=0;i<dep;i++) *(ptpf++)=IMAGE(ptp[i], ptf, def);
  }
  return pf;
}

/* Conjugation: p^-1*f*p */
Obj PowTrans2Perm2(Obj f, Obj p){
  UInt2   *ptf, *ptp, *ptcnj;
  UInt    i, def, dep, decnj;
  Obj     cnj;

  dep  =  DEG_PERM2(p);
  def  = DEG_TRANS2(f);
  decnj=MAX(dep,def);
  cnj  = NEW_TRANS2(decnj);

  ptcnj=ADDR_TRANS2(cnj);
  ptf  =ADDR_TRANS2(f);
  ptp  = ADDR_PERM2(p);
  
  if(def==dep){
    for(i=0;i<decnj;i++) ptcnj[ptp[i]]=ptp[ptf[i]];
  } else {
    for(i=0;i<decnj;i++){
      ptcnj[IMAGE(i, ptp, dep)]=IMAGE(IMAGE(i, ptf, def), ptp, dep);
    }
  }
  return cnj;
}

Obj PowTrans2Perm4(Obj f, Obj p){
  UInt2   *ptf;
  UInt4   *ptp, *ptcnj;
  UInt    i, def, dep, decnj;
  Obj     cnj;

  dep  =  DEG_PERM4(p);
  def  = DEG_TRANS2(f);
  decnj=MAX(dep,def);
  cnj  = NEW_TRANS4(decnj);

  ptcnj=ADDR_TRANS4(cnj);
  ptf  =ADDR_TRANS2(f);
  ptp  = ADDR_PERM4(p);
  
  if(def==dep){
    for(i=0;i<decnj;i++) ptcnj[ptp[i]]=ptp[ptf[i]];
  } else {
    for(i=0;i<decnj;i++){
      ptcnj[IMAGE(i, ptp, dep)]=IMAGE(IMAGE(i, ptf, def), ptp, dep);
    }
  }
  return cnj;
}

Obj PowTrans4Perm2(Obj f, Obj p){
  UInt2   *ptp;
  UInt4   *ptf, *ptcnj;
  UInt    i, def, dep, decnj;
  Obj     cnj;

  dep  =  DEG_PERM2(p);
  def  = DEG_TRANS4(f);
  decnj=MAX(dep,def);
  cnj  = NEW_TRANS4(decnj);

  ptcnj=ADDR_TRANS4(cnj);
  ptf  =ADDR_TRANS4(f);
  ptp  = ADDR_PERM2(p);
  
  if(def==dep){
    for(i=0;i<decnj;i++) ptcnj[ptp[i]]=ptp[ptf[i]];
  } else {
    for(i=0;i<decnj;i++){
      ptcnj[IMAGE(i, ptp, dep)]=IMAGE(IMAGE(i, ptf, def), ptp, dep);
    }
  }
  return cnj;
}

Obj PowTrans4Perm4(Obj f, Obj p){
  UInt4   *ptf, *ptp, *ptcnj;
  UInt    i, def, dep, decnj;
  Obj     cnj;

  dep  =  DEG_PERM4(p);
  def  = DEG_TRANS4(f);
  decnj=MAX(dep,def);
  cnj  = NEW_TRANS4(decnj);

  ptcnj=ADDR_TRANS4(cnj);
  ptf  =ADDR_TRANS4(f);
  ptp  = ADDR_PERM4(p);
  
  if(def==dep){
    for(i=0;i<decnj;i++) ptcnj[ptp[i]]=ptp[ptf[i]];
  } else {
    for(i=0;i<decnj;i++){
      ptcnj[IMAGE(i, ptp, dep)]=IMAGE(IMAGE(i, ptf, def), ptp, dep);
    }
  }
  return cnj;
}

/* f*p^-1 */
Obj QuoTrans2Perm2(Obj f, Obj p){ 
  UInt    def, dep, deq, i;
  UInt2   *ptf, *ptquo, *ptp;
  UInt4   *pttmp;
  Obj     quo;

  def=DEG_TRANS2(f);
  dep= DEG_PERM2(p);
  deq=MAX(def, dep);
  quo=NEW_TRANS2( deq );
  ResizeTmpTrans(SIZE_OBJ(p));

  /* invert the permutation into the buffer bag */
  pttmp = ADDR_TRANS4(TmpTrans);
  ptp =   ADDR_PERM2(p);
  for(i=0;i<dep;i++) pttmp[*ptp++]=i;

  ptf   = ADDR_TRANS2(f);
  ptquo = ADDR_TRANS2(quo);

  if(def<=dep){
    for(i=0;i<def;i++) *(ptquo++)=pttmp[*(ptf++)];
    for(i=def;i<dep;i++) *(ptquo++)=pttmp[i];
  }
  else {
    for(i=0;i<def;i++) *(ptquo++)=IMAGE(ptf[i], pttmp, dep );
  }
  return quo;
}

Obj QuoTrans2Perm4(Obj f, Obj p){ 
  UInt    def, dep, deq, i;
  UInt2   *ptf;
  UInt4   *ptquo, *ptp, *pttmp;
  Obj     quo;

  def=DEG_TRANS2(f);
  dep= DEG_PERM4(p);
  deq=MAX(def, dep);
  quo=NEW_TRANS4( deq );
  ResizeTmpTrans(SIZE_OBJ(p));

  /* invert the permutation into the buffer bag */
  pttmp = ADDR_TRANS4(TmpTrans);
  ptp =   ADDR_PERM4(p);
  for(i=0;i<dep;i++) pttmp[*ptp++]=i;

  ptf   = ADDR_TRANS2(f);
  ptquo = ADDR_TRANS4(quo);

  if(def<=dep){
    for(i=0;i<def;i++) *(ptquo++)=pttmp[*(ptf++)];
    for(i=def;i<dep;i++) *(ptquo++)=pttmp[i];
  }
  else {
    for(i=0;i<def;i++) *(ptquo++)=IMAGE(ptf[i], pttmp, dep );
  }
  return quo;
}

Obj QuoTrans4Perm2(Obj f, Obj p){ 
  UInt    def, dep, deq, i;
  UInt4   *ptf, *ptquo, *pttmp;
  UInt2   *ptp;
  Obj     quo;

  def=DEG_TRANS4(f);
  dep= DEG_PERM2(p);
  deq=MAX(def, dep);
  quo=NEW_TRANS4( deq );
  
  ResizeTmpTrans(SIZE_OBJ(p));

  /* invert the permutation into the buffer bag */
  pttmp = ADDR_TRANS4(TmpTrans);
  ptp =   ADDR_PERM2(p);
  for(i=0;i<dep;i++) pttmp[*ptp++]=i;

  ptf   = ADDR_TRANS4(f);
  ptquo = ADDR_TRANS4(quo);

  if(def<=dep){
    for(i=0;i<def;i++) *(ptquo++)=pttmp[*(ptf++)];
    for(i=def;i<dep;i++) *(ptquo++)=pttmp[i];
  }
  else {
    for(i=0;i<def;i++) *(ptquo++)=IMAGE(ptf[i], pttmp, dep );
  }
  return quo;
}

Obj QuoTrans4Perm4(Obj f, Obj p){ 
  UInt    def, dep, deq, i;
  UInt4   *ptf, *pttmp, *ptquo, *ptp;
  Obj     quo;

  def=DEG_TRANS4(f);
  dep= DEG_PERM4(p);
  deq=MAX(def, dep);
  quo=NEW_TRANS4( deq );

  ResizeTmpTrans(SIZE_OBJ(p));

  /* invert the permutation into the buffer bag */
  pttmp = ADDR_TRANS4(TmpTrans);
  ptp =   ADDR_PERM4(p);
  for(i=0;i<dep;i++) pttmp[*ptp++]=i;

  ptf   = ADDR_TRANS4(f);
  ptquo = ADDR_TRANS4(quo);

  if(def<=dep){
    for(i=0;i<def;i++) *(ptquo++)=pttmp[*(ptf++)];
    for(i=def;i<dep;i++) *(ptquo++)=pttmp[i];
  }
  else {
    for(i=0;i<def;i++) *(ptquo++)=IMAGE(ptf[i], pttmp, dep );
  }
  return quo;
}

/* p^-1*f */
Obj LQuoPerm2Trans2(Obj opL, Obj opR){ 
  UInt   degL, degR, degM, p;
  Obj    mod;
  UInt2  *ptL, *ptR, *ptM; 

  degL = DEG_PERM2(opL);
  degR = DEG_TRANS2(opR);
  degM = degL < degR ? degR : degL;
  mod = NEW_TRANS2( degM );

  /* set up the pointers                                                 */
  ptL = ADDR_PERM2(opL);
  ptR = ADDR_TRANS2(opR);
  ptM = ADDR_TRANS2(mod);

  /* its one thing if the left (inner) permutation is smaller            */
  if ( degL <= degR ) {
      for ( p = 0; p < degL; p++ )
          ptM[ *(ptL++) ] = *(ptR++);
      for ( p = degL; p < degR; p++ )
          ptM[ p ] = *(ptR++);
  }

  /* and another if the right (outer) permutation is smaller             */
  else {
      for ( p = 0; p < degR; p++ )
          ptM[ *(ptL++) ] = *(ptR++);
      for ( p = degR; p < degL; p++ )
          ptM[ *(ptL++) ] = p;
  }

  /* return the result                                                   */
  return mod;
}

Obj LQuoPerm2Trans4(Obj opL, Obj opR){ 
  UInt   degL, degR, degM, p;
  Obj    mod;
  UInt2  *ptL;
  UInt4  *ptR, *ptM; 

  degL = DEG_PERM2(opL);
  degR = DEG_TRANS4(opR);
  degM = degL < degR ? degR : degL;
  mod = NEW_TRANS4( degM );

  /* set up the pointers                                                 */
  ptL = ADDR_PERM2(opL);
  ptR = ADDR_TRANS4(opR);
  ptM = ADDR_TRANS4(mod);

  /* its one thing if the left (inner) permutation is smaller            */
  if ( degL <= degR ) {
      for ( p = 0; p < degL; p++ )
          ptM[ *(ptL++) ] = *(ptR++);
      for ( p = degL; p < degR; p++ )
          ptM[ p ] = *(ptR++);
  }

  /* and another if the right (outer) permutation is smaller             */
  else {
      for ( p = 0; p < degR; p++ )
          ptM[ *(ptL++) ] = *(ptR++);
      for ( p = degR; p < degL; p++ )
          ptM[ *(ptL++) ] = p;
  }

  /* return the result                                                   */
  return mod;
}

Obj LQuoPerm4Trans2(Obj opL, Obj opR){ 
  UInt   degL, degR, degM, p;
  Obj    mod;
  UInt4  *ptL, *ptM;
  UInt2  *ptR; 

  degL = DEG_PERM4(opL);
  degR = DEG_TRANS2(opR);
  degM = degL < degR ? degR : degL;
  mod = NEW_TRANS4( degM );

  /* set up the pointers                                                 */
  ptL = ADDR_PERM4(opL);
  ptR = ADDR_TRANS2(opR);
  ptM = ADDR_TRANS4(mod);

  /* its one thing if the left (inner) permutation is smaller            */
  if ( degL <= degR ) {
      for ( p = 0; p < degL; p++ )
          ptM[ *(ptL++) ] = *(ptR++);
      for ( p = degL; p < degR; p++ )
          ptM[ p ] = *(ptR++);
  }

  /* and another if the right (outer) permutation is smaller             */
  else {
      for ( p = 0; p < degR; p++ )
          ptM[ *(ptL++) ] = *(ptR++);
      for ( p = degR; p < degL; p++ )
          ptM[ *(ptL++) ] = p;
  }

  /* return the result                                                   */
  return mod;
}

Obj LQuoPerm4Trans4(Obj opL, Obj opR){ 
  UInt   degL, degR, degM, p;
  Obj    mod;
  UInt4  *ptL, *ptR, *ptM; 

  degL = DEG_PERM4(opL);
  degR = DEG_TRANS4(opR);
  degM = degL < degR ? degR : degL;
  mod = NEW_TRANS4( degM );

  /* set up the pointers                                                 */
  ptL = ADDR_PERM4(opL);
  ptR = ADDR_TRANS4(opR);
  ptM = ADDR_TRANS4(mod);

  /* its one thing if the left (inner) permutation is smaller            */
  if ( degL <= degR ) {
      for ( p = 0; p < degL; p++ )
          ptM[ *(ptL++) ] = *(ptR++);
      for ( p = degL; p < degR; p++ )
          ptM[ p ] = *(ptR++);
  }

  /* and another if the right (outer) permutation is smaller             */
  else {
      for ( p = 0; p < degR; p++ )
          ptM[ *(ptL++) ] = *(ptR++);
      for ( p = degR; p < degL; p++ )
          ptM[ *(ptL++) ] = p;
  }

  /* return the result                                                   */
  return mod;
}

/* i^f */
Obj PowIntTrans2(Obj i, Obj f){
  Int    img;
 
  if(TNUM_OBJ(i)==T_INTPOS) return i; 

  if(TNUM_OBJ(i)!=T_INT){
    ErrorQuit("usage: the first argument should be a positive integer", 0L, 0L);
  }
  
  img=INT_INTOBJ(i);
  
  if(img<=0){
    ErrorQuit("usage: the first argument should be a positive integer", 0L, 0L);
  }
  
  if(img<=DEG_TRANS2(f)){
    img=(ADDR_TRANS2(f))[img-1]+1;
  }
  
  return INTOBJ_INT(img);
}

Obj PowIntTrans4(Obj i, Obj f){
  Int    img;
 
  if(TNUM_OBJ(i)==T_INTPOS) return i; 

  if(TNUM_OBJ(i)!=T_INT){
    ErrorQuit("usage: the first argument should be a positive integer", 0L, 0L);
  }
  
  img=INT_INTOBJ(i);
  
  if(img<=0){
    ErrorQuit("usage: the first argument should be a positive integer", 0L, 0L);
  }
  
  if(img<=DEG_TRANS4(f)){
    img=(ADDR_TRANS4(f))[img-1]+1;
  }
  
  return INTOBJ_INT(img);
}

/* OnSetsTrans for use in FuncOnSets */
Obj OnSetsTrans (Obj set, Obj f){
  UInt2  *ptf2;
  UInt4  *ptf4;
  UInt   deg;
  Obj    *ptset, *ptres, tmp, res;
  UInt   i, isint, k, h, len;

  res=NEW_PLIST(IS_MUTABLE_PLIST(set)?T_PLIST:T_PLIST+IMMUTABLE,LEN_LIST(set));
  ADDR_OBJ(res)[0]=ADDR_OBJ(set)[0]; 

  /* get the pointer                                                 */
  ptset = ADDR_OBJ(set) + LEN_LIST(set);
  ptres = ADDR_OBJ(res) + LEN_LIST(set);
  if(TNUM_OBJ(f)==T_TRANS2){   
    ptf2 = ADDR_TRANS2(f);
    deg = DEG_TRANS2(f);
    /* loop over the entries of the tuple                              */
    isint = 1;
    for ( i =LEN_LIST(set) ; 1 <= i; i--, ptset--, ptres-- ) {
      if ( TNUM_OBJ( *ptset ) == T_INT && 0 < INT_INTOBJ( *ptset ) ) {
        k = INT_INTOBJ( *ptset );
        if ( k <= deg ) k = ptf2[k-1] + 1 ; 
        *ptres = INTOBJ_INT(k);
      } else {/* this case cannot occur since I think POW is not defined */
        ErrorQuit("not yet implemented!", 0L, 0L); 
      }
    }
  } else {
    ptf4 = ADDR_TRANS4(f);
    deg = DEG_TRANS4(f);

    /* loop over the entries of the tuple                              */
    isint = 1;
    for ( i =LEN_LIST(set) ; 1 <= i; i--, ptset--, ptres-- ) {
      if ( TNUM_OBJ( *ptset ) == T_INT && 0 < INT_INTOBJ( *ptset ) ) {
        k = INT_INTOBJ( *ptset );
        if ( k <= deg ) k = ptf4[k-1] + 1 ; 
        *ptres = INTOBJ_INT(k);
      } else {/* this case cannot occur since I think POW is not defined */
        ErrorQuit("not yet implemented!", 0L, 0L); 
      }
    }
  }
  /* sort the result */
  len=LEN_LIST(res);
  h = 1;  while ( 9*h + 4 < len )  h = 3*h + 1;
  while ( 0 < h ) {
    for ( i = h+1; i <= len; i++ ) {
      tmp = ADDR_OBJ(res)[i];  k = i;
      while ( h < k && ((Int)tmp < (Int)(ADDR_OBJ(res)[k-h])) ) {
        ADDR_OBJ(res)[k] = ADDR_OBJ(res)[k-h];
        k -= h;
      }
      ADDR_OBJ(res)[k] = tmp;
    }
    h = h / 3;
  }

  /* remove duplicates */
  if ( 0 < len ) {
    tmp = ADDR_OBJ(res)[1];  k = 1;
    for ( i = 2; i <= len; i++ ) {
      if ( ! EQ( tmp, ADDR_OBJ(res)[i] ) ) {
        k++;
        tmp = ADDR_OBJ(res)[i];
        ADDR_OBJ(res)[k] = tmp;
      }
    }
    if ( k < len ) {
      ResizeBag( res, (k+1)*sizeof(Obj) );
      SET_LEN_PLIST(res, k);
    }
  }

  /* retype if we only have integers */
  if(isint){
    RetypeBag( res, IS_MUTABLE_PLIST(set) ? T_PLIST_CYC_SSORT :
     T_PLIST_CYC_SSORT + IMMUTABLE ); 
  }

  return res;
}

/* OnTuplesTrans for use in FuncOnTuples */
Obj OnTuplesTrans (Obj tup, Obj f){
  UInt2  *ptf2;
  UInt4  *ptf4;
  UInt   deg, isint, i, k;
  Obj    *pttup, *ptres, res;

  res=NEW_PLIST(IS_MUTABLE_PLIST(tup)?T_PLIST:T_PLIST+IMMUTABLE,LEN_LIST(tup));
  ADDR_OBJ(res)[0]=ADDR_OBJ(tup)[0]; 

  /* get the pointer                                                 */
  pttup = ADDR_OBJ(tup) + LEN_LIST(tup);
  ptres = ADDR_OBJ(res) + LEN_LIST(tup);
  if(TNUM_OBJ(f)==T_TRANS2){
    ptf2 = ADDR_TRANS2(f);
    deg = DEG_TRANS2(f);

    /* loop over the entries of the tuple                              */
    isint=1;
    for ( i =LEN_LIST(tup) ; 1 <= i; i--, pttup--, ptres-- ) {
      if ( TNUM_OBJ( *pttup ) == T_INT && 0 < INT_INTOBJ( *pttup ) ) {
        k = INT_INTOBJ( *pttup );
        if ( k <= deg ) k = ptf2[k-1] + 1 ; 
        *ptres = INTOBJ_INT(k);
      } else {/* this case cannot occur since I think POW is not defined */
        ErrorQuit("not yet implemented!", 0L, 0L);
      }
    }
  } else {
    ptf4 = ADDR_TRANS4(f);
    deg = DEG_TRANS4(f);

    /* loop over the entries of the tuple                              */
    isint=1;
    for ( i =LEN_LIST(tup) ; 1 <= i; i--, pttup--, ptres-- ) {
      if ( TNUM_OBJ( *pttup ) == T_INT && 0 < INT_INTOBJ( *pttup ) ) {
        k = INT_INTOBJ( *pttup );
        if ( k <= deg ) k = ptf4[k-1] + 1 ; 
        *ptres = INTOBJ_INT(k);
      } else {/* this case cannot occur since I think POW is not defined */
        ErrorQuit("not yet implemented!", 0L, 0L);
      }
    }
  }
  if(isint){
    RetypeBag( res, IS_MUTABLE_PLIST(tup) ? T_PLIST_CYC_SSORT :
     T_PLIST_CYC_SSORT + IMMUTABLE );
  }
  return res;
}

Obj FuncOnPosIntSetsTrans (Obj self, Obj set, Obj f){
  UInt2  *ptf2;
  UInt4  *ptf4;
  UInt   deg;
  Obj    *ptset, *ptres, tmp, res;
  UInt   i, k, h, len;

  if(LEN_LIST(set)==0) return set;

  if(LEN_LIST(set)==1&&INT_INTOBJ(ELM_LIST(set, 1))==0){
    return FuncIMAGE_SET_TRANS(self, f);
  }  

  PLAIN_LIST(set);
  res=NEW_PLIST(IS_MUTABLE_PLIST(set)?T_PLIST:T_PLIST+IMMUTABLE,LEN_LIST(set));
  ADDR_OBJ(res)[0]=ADDR_OBJ(set)[0]; 

  /* get the pointer                                                 */
  ptset = ADDR_OBJ(set) + LEN_LIST(set);
  ptres = ADDR_OBJ(res) + LEN_LIST(set);
  
  if(TNUM_OBJ(f)==T_TRANS2){   
    ptf2 = ADDR_TRANS2(f);
    deg = DEG_TRANS2(f);
    for ( i =LEN_LIST(set) ; 1 <= i; i--, ptset--, ptres-- ) {
      k = INT_INTOBJ( *ptset );
      if ( k <= deg ) k = ptf2[k-1] + 1 ; 
      *ptres = INTOBJ_INT(k);
    }
  } else {
    ptf4 = ADDR_TRANS4(f);
    deg = DEG_TRANS4(f);
    for ( i =LEN_LIST(set) ; 1 <= i; i--, ptset--, ptres-- ) {
      k = INT_INTOBJ( *ptset );
      if ( k <= deg ) k = ptf4[k-1] + 1 ; 
      *ptres = INTOBJ_INT(k);
    }
  }
  /* sort the result */
  len=LEN_LIST(res);
  h = 1;  while ( 9*h + 4 < len )  h = 3*h + 1;
  while ( 0 < h ) {
    for ( i = h+1; i <= len; i++ ) {
      tmp = ADDR_OBJ(res)[i];  k = i;
      while ( h < k && ((Int)tmp < (Int)(ADDR_OBJ(res)[k-h])) ) {
        ADDR_OBJ(res)[k] = ADDR_OBJ(res)[k-h];
        k -= h;
      }
      ADDR_OBJ(res)[k] = tmp;
    }
    h = h / 3;
  }

  /* remove duplicates */
  if ( 0 < len ) {
    tmp = ADDR_OBJ(res)[1];  k = 1;
    for ( i = 2; i <= len; i++ ) {
      if ( ! EQ( tmp, ADDR_OBJ(res)[i] ) ) {
        k++;
        tmp = ADDR_OBJ(res)[i];
        ADDR_OBJ(res)[k] = tmp;
      }
    }
    if ( k < len ) {
      ResizeBag( res, (k+1)*sizeof(Obj) );
      SET_LEN_PLIST(res, k);
    }
  }

  /* retype if we only have integers */
  RetypeBag( res, IS_MUTABLE_PLIST(set) ? T_PLIST_CYC_SSORT :
   T_PLIST_CYC_SSORT + IMMUTABLE ); 

  return res;
}

/******************************************************************************/
/******************************************************************************/

/* other internal things */

/* so that kernel and image set are preserved during garbage collection */
void MarkTrans2SubBags( Obj f ){
  MARK_BAG(IMG_TRANS2(f));
  MARK_BAG(KER_TRANS2(f));
}

void MarkTrans4SubBags( Obj f ){
  MARK_BAG(IMG_TRANS4(f));
  MARK_BAG(KER_TRANS4(f));
}

/* Save and load */
void SaveTrans2( Obj f){
  UInt2   *ptr;
  UInt    len, i;
  ptr=ADDR_TRANS2(f); /* save the image list */
  len=DEG_TRANS2(f);
  for (i = 0; i < len; i++) SaveUInt2(*ptr++);
}

void LoadTrans2( Obj f){
  UInt2   *ptr;
  UInt    len, i;
  len=DEG_TRANS2(f);
  ptr=ADDR_TRANS2(f);
  for (i = 0; i < len; i++) *ptr++=LoadUInt2();
}

void SaveTrans4( Obj f){
  UInt4   *ptr;
  UInt    len, i;
  ptr=ADDR_TRANS4(f); /* save the image list */
  len=DEG_TRANS4(f);
  for (i = 0; i < len; i++) SaveUInt4(*ptr++);
}

void LoadTrans4( Obj f){
  UInt4   *ptr;
  UInt    len, i;
  len=DEG_TRANS4(f);
  ptr=ADDR_TRANS4(f);
  for (i = 0; i < len; i++) *ptr++=LoadUInt4();
}

Obj TYPE_TRANS2;

Obj TypeTrans2(Obj f){
  return TYPE_TRANS2;
}

Obj TYPE_TRANS4;

Obj TypeTrans4(Obj f){
  return TYPE_TRANS4;
}

Obj IsTransFilt;

Obj IsTransHandler (
    Obj                 self,
    Obj                 val )
{
    /* return 'true' if <val> is a transformation and 'false' otherwise       */
    switch ( TNUM_OBJ(val) ) {

        case T_TRANS2:
        case T_TRANS4:
            return True;

        case T_COMOBJ:
        case T_POSOBJ:
        case T_DATOBJ:
            return DoFilter( self, val );

        default:
            return False;
    }
}

/*F * * * * * * * * * * * * * initialize package * * * * * * * * * * * * * * */

/****************************************************************************
**

*V  GVarFilts . . . . . . . . . . . . . . . . . . . list of filters to export
*/
static StructGVarFilt GVarFilts [] = {

    { "IS_TRANS", "obj", &IsTransFilt,
      IsTransHandler, "src/trans.c:IS_TRANS" },

    { 0 }

};

/******************************************************************************
*V  GVarFuncs . . . . . . . . . . . . . . . . . . list of functions to export
*/
static StructGVarFunc GVarFuncs [] = {

  { "HAS_KER_TRANS", 1, "f",
     FuncHAS_KER_TRANS,
    "src/TRANS.c:FuncHAS_KER_TRANS" },

  { "HAS_IMG_TRANS", 1, "f",
     FuncHAS_IMG_TRANS,
    "src/TRANS.c:FuncHAS_IMG_TRANS" },

  { "TransformationNC", 1, "list",
     FuncTransformationNC,
    "src/trans.c:FuncTransformationNC" },

  { "TransformationListListNC", 2, "src, ran",
     FuncTransformationListListNC,
    "src/trans.c:FuncTransformationListListNC" },

  { "DegreeOfTransformation", 1, "f",
     FuncDegreeOfTransformation,
    "src/trans.c:FuncDegreeOfTransformation" },

  { "RANK_TRANS", 1, "f",
     FuncRANK_TRANS,
    "src/trans.c:FuncRANK_TRANS" },

  { "LARGEST_MOVED_PT_TRANS", 1, "f",
     FuncLARGEST_MOVED_PT_TRANS,
    "src/trans.c:FuncLARGEST_MOVED_PT_TRANS" },

  { "LARGEST_IMAGE_PT", 1, "f",
     FuncLARGEST_IMAGE_PT,
    "src/trans.c:FuncLARGEST_IMAGE_PT" },

  { "SMALLEST_MOVED_PT_TRANS", 1, "f",
     FuncSMALLEST_MOVED_PT_TRANS,
    "src/trans.c:FuncSMALLEST_MOVED_PT_TRANS" },

  { "SMALLEST_IMAGE_PT", 1, "f",
     FuncSMALLEST_IMAGE_PT,
    "src/trans.c:FuncSMALLEST_IMAGE_PT" },

  { "NR_MOVED_PTS_TRANS", 1, "f",
     FuncNR_MOVED_PTS_TRANS,
    "src/trans.c:FuncNR_MOVED_PTS_TRANS" },

  { "MOVED_PTS_TRANS", 1, "f",
     FuncMOVED_PTS_TRANS,
    "src/trans.c:FuncMOVED_PTS_TRANS" },

  { "IMAGE_TRANS", 2, "f, n",
     FuncIMAGE_TRANS,
    "src/trans.c:FuncIMAGE_TRANS" },

  { "FLAT_KERNEL_TRANS", 1, "f",
     FuncFLAT_KERNEL_TRANS,
    "src/trans.c:FuncFLAT_KERNEL_TRANS" },

  { "IMAGE_SET_TRANS", 1, "f",
     FuncIMAGE_SET_TRANS,
    "src/trans.c:FuncIMAGE_SET_TRANS" },

  { "IMAGE_SET_TRANS_INT", 2, "f, n",
     FuncIMAGE_SET_TRANS_INT,
    "src/trans.c:FuncIMAGE_SET_TRANS_INT" },

  { "KERNEL_TRANS", 2, "f, n",
     FuncKERNEL_TRANS,
    "src/trans.c:FuncKERNEL_TRANS" },

  { "PREIMAGES_TRANS_INT", 2, "f, pt",
     FuncPREIMAGES_TRANS_INT,
    "src/trans.c:FuncPREIMAGES_TRANS_INT" },

  { "ID_TRANS", 1, "deg",
     FuncID_TRANS,
    "src/trans.c:FuncID_TRANS" },

  { "AS_TRANS_PERM", 1, "f",
     FuncAS_TRANS_PERM,
    "src/trans.c:FuncAS_TRANS_PERM" },

  { "AS_TRANS_PERM_INT", 2, "f, n",
     FuncAS_TRANS_PERM_INT,
    "src/trans.c:FuncAS_TRANS_PERM_INT" },

  { "AS_PERM_TRANS", 1, "f",
     FuncAS_PERM_TRANS,
    "src/trans.c:FuncAS_PERM_TRANS" },

  { "RESTRICTED_TRANS", 2, "f, list",
     FuncRESTRICTED_TRANS,
    "src/trans.c:FuncRESTRICTED_TRANS" },

  { "AS_TRANS_TRANS", 2, "f, m",
     FuncAS_TRANS_TRANS,
    "src/trans.c:FuncAS_TRANS_TRANS" },

  { "TRIM_TRANS", 2, "f, m",
     FuncTRIM_TRANS,
    "src/trans.c:FuncTRIM_TRANS" },

  { "IS_INJECTIVE_LIST_TRANS", 2, "t, l",
     FuncIS_INJECTIVE_LIST_TRANS,
    "src/trans.c:FuncIS_INJECTIVE_LIST_TRANS" },

  { "PERM_LEFT_QUO_TRANS_NC", 2, "f, g",
     FuncPERM_LEFT_QUO_TRANS_NC,
    "src/trans.c:FuncPERM_LEFT_QUO_TRANS_NC" },

  { "TRANS_IMG_KER_NC", 2, "img, ker",
     FuncTRANS_IMG_KER_NC,
    "src/trans.c:FuncTRANS_IMG_KER_NC" },

  { "IDEM_IMG_KER_NC", 2, "img, ker",
     FuncIDEM_IMG_KER_NC,
    "src/trans.c:FuncIDEM_IMG_KER_NC" },

  { "INV_TRANS", 1, "f",
     FuncINV_TRANS,
    "src/trans.c:FuncINV_TRANS" },

  { "INV_LIST_TRANS", 2, "list, f",
     FuncINV_LIST_TRANS,
    "src/trans.c:FuncINV_LIST_TRANS" },

  { "TRANS_IMG_CONJ", 2, "f,g",
     FuncTRANS_IMG_CONJ,
    "src/trans.c:FuncTRANS_IMG_CONJ" },

  { "INDEX_PERIOD_TRANS", 1, "f",
     FuncINDEX_PERIOD_TRANS,
    "src/trans.c:FuncINDEX_PERIOD_TRANS" },

  { "SMALLEST_IDEM_POW_TRANS", 1, "f",
     FuncSMALLEST_IDEM_POW_TRANS,
    "src/trans.c:FuncSMALLEST_IDEM_POW_TRANS" },

  { "ON_KERNEL_ANTI_ACTION", 2, "ker, f",
     FuncON_KERNEL_ANTI_ACTION,
    "src/trans.c:FuncON_KERNEL_ANTI_ACTION" },

  { "INV_KER_TRANS", 2, "ker, f",
     FuncINV_KER_TRANS,
    "src/trans.c:FuncINV_KER_TRANS" },

  { "IS_IDEM_TRANS", 1, "f",
    FuncIS_IDEM_TRANS,
    "src/trans.c:FuncIS_IDEM_TRANS" },

  { "IS_ID_TRANS", 1, "f",
    FuncIS_ID_TRANS,
    "src/trans.c:FuncIS_ID_TRANS" },
  
  { "COMPONENT_REPS_TRANS", 1, "f",
    FuncCOMPONENT_REPS_TRANS,
    "src/trans.c:FuncCOMPONENT_REPS_TRANS" },
  
  { "NR_COMPONENTS_TRANS", 1, "f",
    FuncNR_COMPONENTS_TRANS,
    "src/trans.c:FuncNR_COMPONENTS_TRANS" },

  { "COMPONENTS_TRANS", 1, "f",
    FuncCOMPONENTS_TRANS,
    "src/trans.c:FuncCOMPONENTS_TRANS" },

  { "COMPONENT_TRANS_INT", 2, "f, pt",
    FuncCOMPONENT_TRANS_INT,
    "src/trans.c:FuncCOMPONENT_TRANS_INT" },

  { "CYCLE_TRANS_INT", 2, "f, pt",
    FuncCYCLE_TRANS_INT,
    "src/trans.c:FuncCYCLE_TRANS_INT" },

  { "CYCLES_TRANS_LIST", 2, "f, pt",
    FuncCYCLES_TRANS_LIST,
    "src/trans.c:FuncCYCLES_TRANS_LIST" },

  { "LEFT_ONE_TRANS", 1, "f",
    FuncLEFT_ONE_TRANS,
    "src/trans.c:FuncLEFT_ONE_TRANS" },

  { "RIGHT_ONE_TRANS", 1, "f",
    FuncRIGHT_ONE_TRANS,
    "src/trans.c:FuncRIGHT_ONE_TRANS" },
  
  { "OnPosIntSetsTrans", 2, "set, f", 
    FuncOnPosIntSetsTrans, 
    "src/trans.c:FuncOnPosIntSetsTrans" },

  { 0 }

};
/******************************************************************************
*F  InitKernel( <module> )  . . . . . . . . initialise kernel data structures
*/
static Int InitKernel ( StructInitInfo *module )
{

    /* install the marking function                                        */
    InfoBags[ T_TRANS2 ].name = "transformation (small)";
    InfoBags[ T_TRANS4 ].name = "transformation (large)";
    InitMarkFuncBags( T_TRANS2, MarkTrans2SubBags );
    InitMarkFuncBags( T_TRANS4, MarkTrans4SubBags );
    
    /* install the kind function                                           */
    ImportGVarFromLibrary( "TYPE_TRANS2", &TYPE_TRANS2 );
    ImportGVarFromLibrary( "TYPE_TRANS4", &TYPE_TRANS4 );

    TypeObjFuncs[ T_TRANS2 ] = TypeTrans2;
    TypeObjFuncs[ T_TRANS4 ] = TypeTrans4;

    /* init filters and functions                                          */
    InitHdlrFiltsFromTable( GVarFilts );
    InitHdlrFuncsFromTable( GVarFuncs );

    /* make the buffer bag                                                 */
    InitGlobalBag( &TmpTrans, "src/trans.c:TmpTrans" );
    
    /* install the saving functions */
    SaveObjFuncs[ T_TRANS2 ] = SaveTrans2;
    LoadObjFuncs[ T_TRANS2 ] = LoadTrans2; 
    SaveObjFuncs[ T_TRANS4 ] = SaveTrans4;
    LoadObjFuncs[ T_TRANS4 ] = LoadTrans4; 

    /* install the comparison methods                                      */
    EqFuncs  [ T_TRANS2  ][ T_TRANS2  ] = EqTrans22;    
    EqFuncs  [ T_TRANS2  ][ T_TRANS4  ] = EqTrans24;    
    EqFuncs  [ T_TRANS4  ][ T_TRANS2  ] = EqTrans42;    
    EqFuncs  [ T_TRANS4  ][ T_TRANS4  ] = EqTrans44;    
    LtFuncs  [ T_TRANS2  ][ T_TRANS2  ] = LtTrans22;
    LtFuncs  [ T_TRANS2  ][ T_TRANS4  ] = LtTrans24;
    LtFuncs  [ T_TRANS4  ][ T_TRANS2  ] = LtTrans42;
    LtFuncs  [ T_TRANS4  ][ T_TRANS4  ] = LtTrans44;
    
    /* install the binary operations */
    ProdFuncs [ T_TRANS2  ][ T_TRANS2 ] = ProdTrans22;
    ProdFuncs [ T_TRANS4  ][ T_TRANS4 ] = ProdTrans44;
    ProdFuncs [ T_TRANS2  ][ T_TRANS4 ] = ProdTrans24;
    ProdFuncs [ T_TRANS4  ][ T_TRANS2 ] = ProdTrans42;
    ProdFuncs [ T_TRANS2  ][ T_PERM2 ] = ProdTrans2Perm2;
    ProdFuncs [ T_TRANS2  ][ T_PERM4 ] = ProdTrans2Perm4;
    ProdFuncs [ T_TRANS4  ][ T_PERM2 ] = ProdTrans4Perm2;
    ProdFuncs [ T_TRANS4  ][ T_PERM4 ] = ProdTrans4Perm4;
    ProdFuncs [ T_PERM2  ][ T_TRANS2 ] = ProdPerm2Trans2;
    ProdFuncs [ T_PERM4  ][ T_TRANS2 ] = ProdPerm4Trans2;
    ProdFuncs [ T_PERM2  ][ T_TRANS4 ] = ProdPerm2Trans4;
    ProdFuncs [ T_PERM4  ][ T_TRANS4 ] = ProdPerm4Trans4;
    PowFuncs  [ T_TRANS2  ][ T_PERM2 ] = PowTrans2Perm2;    
    PowFuncs  [ T_TRANS2  ][ T_PERM4 ] = PowTrans2Perm4;    
    PowFuncs  [ T_TRANS4  ][ T_PERM2 ] = PowTrans4Perm2;    
    PowFuncs  [ T_TRANS4  ][ T_PERM4 ] = PowTrans4Perm4;    
    QuoFuncs  [ T_TRANS2  ][ T_PERM2 ] = QuoTrans2Perm2;
    QuoFuncs  [ T_TRANS2  ][ T_PERM4 ] = QuoTrans2Perm4;
    QuoFuncs  [ T_TRANS4  ][ T_PERM2 ] = QuoTrans4Perm2;
    QuoFuncs  [ T_TRANS4  ][ T_PERM4 ] = QuoTrans4Perm4;
    LQuoFuncs [ T_PERM2  ][ T_TRANS2 ] = LQuoPerm2Trans2;
    LQuoFuncs [ T_PERM4  ][ T_TRANS2 ] = LQuoPerm4Trans2;
    LQuoFuncs [ T_PERM2  ][ T_TRANS4 ] = LQuoPerm2Trans4;
    LQuoFuncs [ T_PERM4  ][ T_TRANS4 ] = LQuoPerm4Trans4;
    PowFuncs  [ T_INT    ][ T_TRANS2 ] = PowIntTrans2;
    PowFuncs  [ T_INT    ][ T_TRANS4 ] = PowIntTrans4;
  
    /* install the 'ONE' function for transformations */
    OneFuncs[ T_TRANS2 ] = OneTrans2;
    OneMutFuncs[ T_TRANS2 ] = OneTrans2;
    OneFuncs[ T_TRANS4 ] = OneTrans4;
    OneMutFuncs[ T_TRANS4 ] = OneTrans4;

    /* return success                                                      */
    return 0;
}

/******************************************************************************
*F  InitLibrary( <module> ) . . . . . . .  initialise library data structures
*/
static Int InitLibrary ( StructInitInfo *module )
{
    /* init filters and functions                                          */
    InitGVarFuncsFromTable( GVarFuncs );
    InitGVarFiltsFromTable( GVarFilts );
    TmpTrans = NEW_TRANS4(1000);
    /* return success                                                      */
    return 0;
}

/****************************************************************************
**
*F  InitInfoTrans()  . . . . . . . . . . . . . . . table of init functions
*/
static StructInitInfo module = {
    MODULE_BUILTIN,                     /* type                           */
    "trans",                            /* name                           */
    0,                                  /* revision entry of c file       */
    0,                                  /* revision entry of h file       */
    0,                                  /* version                        */
    0,                                  /* crc                            */
    InitKernel,                         /* initKernel                     */
    InitLibrary,                        /* initLibrary                    */
    0,                                  /* checkInit                      */
    0,                                  /* preSave                        */
    0,                                  /* postSave                       */
    0                                   /* postRestore                    */
};

StructInitInfo * InitInfoTrans ( void )
{
    FillInVersion( &module );
    return &module;
}

