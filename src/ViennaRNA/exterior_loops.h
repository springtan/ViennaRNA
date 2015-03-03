#ifndef VIENNA_RNA_PACKAGE_EXTERIOR_LOOPS_H
#define VIENNA_RNA_PACKAGE_EXTERIOR_LOOPS_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <string.h>
#include <ViennaRNA/fold_vars.h>
#include <ViennaRNA/energy_par.h>
#include <ViennaRNA/params.h>
#include <ViennaRNA/constraints.h>
#include <ViennaRNA/gquad.h>

#ifdef __GNUC__
# define INLINE inline
#else
# define INLINE
#endif

/**
 *  \file exterior_loops.h
 *  \brief Energy evaluation of exterior loops for MFE and partition function calculations
 */

/**
 *  <H2>Compute the Energy contribution of an Exterior loop stem</H2>
 *  This definition is a wrapper for the E_Stem() funtion.
 *  It is substituted by an E_Stem() funtion call with argument
 *  extLoop=1, so the energy contribution returned reflects a
 *  stem introduced in an exterior-loop.<BR>
 *  As for the parameters si1 and sj1 of the substituted
 *  E_Stem() function, you can inhibit to take 5'-, 3'-dangles
 *  or mismatch contributions to be taken into account by passing
 *  -1 to these parameters.
 * 
 *  \see    E_Stem()
 *  \param  type  The pair type of the stem-closing pair
 *  \param  si1   The 5'-mismatching nucleotide
 *  \param  sj1   The 3'-mismatching nucleotide
 *  \param  P     The datastructure containing scaled energy parameters
 *  \return       The energy contribution of the introduced exterior-loop stem
 */
INLINE  PRIVATE int E_ExtLoop(int type,
                              int si1,
                              int sj1,
                              vrna_param_t *P);

/**
 *  This is the partition function variant of \ref E_ExtLoop()
 *  \see E_ExtLoop()
 *  \return The Boltzmann weighted energy contribution of the introduced exterior-loop stem
 */
INLINE  PRIVATE double exp_E_ExtLoop( int type,
                                      int si1,
                                      int sj1,
                                      vrna_exp_param_t *P);

/**
 *  <H2>Compute the energy contribution of a stem branching off a loop-region</H2>
 *  This function computes the energy contribution of a stem that branches off
 *  a loop region. This can be the case in multiloops, when a stem branching off
 *  increases the degree of the loop but also <I>immediately interior base pairs</I>
 *  of an exterior loop contribute free energy.
 *  To switch the bahavior of the function according to the evaluation of a multiloop-
 *  or exterior-loop-stem, you pass the flag 'extLoop'.
 *  The returned energy contribution consists of a TerminalAU penalty if the pair type
 *  is greater than 2, dangling end contributions of mismatching nucleotides adjacent to
 *  the stem if only one of the si1, sj1 parameters is greater than 0 and mismatch energies
 *  if both mismatching nucleotides are positive values.
 *  Thus, to avoid incooperating dangling end or mismatch energies just pass a negative number,
 *  e.g. -1 to the mismatch argument.
 * 
 *  This is an illustration of how the energy contribution is assembled:
 *  <PRE>
 *        3'  5'
 *        |   |
 *        X - Y
 *  5'-si1     sj1-3'
 *  </PRE>
 * 
 *  Here, (X,Y) is the base pair that closes the stem that branches off a loop region.
 *  The nucleotides si1 and sj1 are the 5'- and 3'- mismatches, respectively. If the base pair
 *  type of (X,Y) is greater than 2 (i.e. an A-U or G-U pair, the TerminalAU penalty will be
 *  included in the energy contribution returned. If si1 and sj1 are both nonnegative numbers,
 *  mismatch energies will also be included. If one of sij or sj1 is a negtive value, only
 *  5' or 3' dangling end contributions are taken into account. To prohibit any of these mismatch
 *  contributions to be incoorporated, just pass a negative number to both, si1 and sj1.
 *  In case the argument extLoop is 0, the returned energy contribution also includes
 *  the <I>internal-loop-penalty</I> of a multiloop stem with closing pair type.
 * 
 *  \see    E_MLstem()
 *  \see    E_ExtLoop()
 *  \note   This function is threadsafe
 * 
 *  \param  type    The pair type of the first base pair un the stem
 *  \param  si1     The 5'-mismatching nucleotide
 *  \param  sj1     The 3'-mismatching nucleotide
 *  \param  extLoop A flag that indicates whether the contribution reflects the one of an exterior loop or not
 *  \param  P       The datastructure containing scaled energy parameters
 *  \return         The Free energy of the branch off the loop in dcal/mol
 * 
 */
INLINE  PRIVATE int E_Stem( int type,
                            int si1,
                            int sj1,
                            int extLoop,
                            vrna_param_t *P);

/**
 *  <H2>Compute the Boltzmann weighted energy contribution of a stem branching off a loop-region</H2>
 *  This is the partition function variant of \ref E_Stem()
 *  \see E_Stem()
 *  \note This function is threadsafe
 * 
 *  \return The Boltzmann weighted energy contribution of the branch off the loop
 */
INLINE  PRIVATE double exp_E_Stem(int type,
                                  int si1,
                                  int sj1,
                                  int extLoop,
                                  vrna_exp_param_t *P);


/*
#################################
# BEGIN OF FUNCTION DEFINITIONS #
#################################
*/

INLINE PRIVATE int
E_ext_loop( int i,
            int j,
            vrna_fold_compound *vc){

  int     ij, en, e, type;

  int     cp        = vc->cutpoint;
  short   *S        = vc->sequence_encoding;
  int     *idx      = vc->jindx;
  char    *ptype    = vc->ptype;
  vrna_param_t  *P  = vc->params;
  vrna_md_t     *md = &(P->model_details);
  char            *hard_constraints = vc->hc->matrix;

  e     = INF;
  ij    = idx[j] + i;
  type  = ptype[ij];

  if((cp < 0) || (((i)>=cp)||((j)<cp))){ /* regular exterior loop */
    switch(md->dangles){
      case 0:   if(hard_constraints[ij] & VRNA_HC_CONTEXT_EXT_LOOP)
                  e = E_ExtLoop(type, -1, -1, P);
                break;
      case 2:   if(hard_constraints[ij] & VRNA_HC_CONTEXT_EXT_LOOP)
                  e = E_ExtLoop(type, S[i-1], S[j+1], P);
                break;
      default:  if(hard_constraints[ij] & VRNA_HC_CONTEXT_EXT_LOOP)
                  e = E_ExtLoop(type, -1, -1, P);
                ij = idx[j - 1] + i;
                if(hard_constraints[ij] & VRNA_HC_CONTEXT_EXT_LOOP){
                  type = vc->ptype[ij];
                  en = E_ExtLoop(type, -1, S[j], P);
                  e = MIN2(e, en);
                }
                ij = idx[j] + i + 1;
                if(hard_constraints[ij] & VRNA_HC_CONTEXT_EXT_LOOP){
                  type = vc->ptype[ij];
                  en = E_ExtLoop(type, S[i], -1, P);
                  e = MIN2(e, en);
                }
                break;
    }
  }

  return e;
}


INLINE PRIVATE void
E_ext_loop_5( vrna_fold_compound *vc){

  int en, i, j, ij, type;
  int               length        = (int)vc->length;
  char              *ptype        = vc->ptype;
  short             *S            = vc->sequence_encoding;
  int               *indx         = vc->jindx;
  char              *hc           = vc->hc->matrix;
  int               *hc_up        = vc->hc->up_ext;
  vrna_sc_t         *sc           = vc->sc;
  int               *f5           = vc->matrices->f5;
  int               *c            = vc->matrices->c;
  vrna_param_t      *P            = vc->params;
  int               dangle_model  = P->model_details.dangles;
  int               *ggg          = vc->matrices->ggg;
  int               with_gquad    = P->model_details.gquad;
  int               turn          = P->model_details.min_loop_size;

  f5[0] = 0;
  for(i = 1; i <= turn + 1; i++){
    if(hc_up[i]){
      f5[i] = f5[i-1];
      if(sc)
        if(sc->free_energies)
          f5[i] += sc->free_energies[i][1];
    } else {
      f5[i] = INF;
    }
  }

  /* duplicated code may be faster than conditions inside loop ;) */
  switch(dangle_model){
    /* dont use dangling end and mismatch contributions at all */
    case 0:   for(j=turn+2; j<=length; j++){
                f5[j] = INF;
                if(hc_up[j]){
                  f5[j] = f5[j-1];
                  if(sc)
                    if(sc->free_energies)
                      f5[j] += sc->free_energies[j][1];
                }
                for (i=j-turn-1; i>1; i--){
                  ij = indx[j]+i;
                  if(!(hc[ij] & VRNA_HC_CONTEXT_EXT_LOOP)) continue;

                  if(with_gquad){
                    f5[j] = MIN2(f5[j], f5[i-1] + ggg[indx[j]+i]);
                  }

                  en    = f5[i-1] + c[ij] + E_ExtLoop(ptype[ij], -1, -1, P);
                  f5[j] = MIN2(f5[j], en);
                }
                ij = indx[j] + 1;
                if(!(hc[ij] & VRNA_HC_CONTEXT_EXT_LOOP)) continue;

                if(with_gquad){
                  f5[j] = MIN2(f5[j], ggg[indx[j]+1]);
                }

                en    = c[ij] + E_ExtLoop(ptype[ij], -1, -1, P);
                f5[j] = MIN2(f5[j], en);
              }
              break;

    /* always use dangles on both sides */
    case 2:   for(j=turn+2; j<length; j++){
                f5[j] = INF;
                if(hc_up[j]){
                  f5[j] = f5[j-1];
                  if(sc)
                    if(sc->free_energies)
                      f5[j] += sc->free_energies[j][1];
                }
                for (i=j-turn-1; i>1; i--){
                  ij = indx[j] + i;
                  if(!(hc[ij] & VRNA_HC_CONTEXT_EXT_LOOP)) continue;

                  if(with_gquad){
                    f5[j] = MIN2(f5[j], f5[i-1] + ggg[indx[j]+i]);
                  }

                  en    = f5[i-1] + c[ij] + E_ExtLoop(ptype[ij], S[i-1], S[j+1], P);
                  f5[j] = MIN2(f5[j], en);
                }
                ij = indx[j] + 1;
                if(!(hc[ij] & VRNA_HC_CONTEXT_EXT_LOOP)) continue;

                if(with_gquad){
                  f5[j] = MIN2(f5[j], ggg[indx[j]+1]);
                }

                en    = c[ij] + E_ExtLoop(ptype[ij], -1, S[j+1], P);
                f5[j] = MIN2(f5[j], en);
              }
              if(hc_up[length]){
                f5[length] = f5[length-1];
                if(sc)
                  if(sc->free_energies)
                    f5[length] += sc->free_energies[length][1];
              }
              for (i=length-turn-1; i>1; i--){
                ij = indx[length] + i;
                if(!(hc[ij] & VRNA_HC_CONTEXT_EXT_LOOP)) continue;

                if(with_gquad){
                  f5[length] = MIN2(f5[length], f5[i-1] + ggg[indx[length]+i]);
                }

                en          = f5[i-1] + c[ij] + E_ExtLoop(ptype[ij], S[i-1], -1, P);
                f5[length]  = MIN2(f5[length], en);
              }
              ij = indx[length] + 1;
              if(!(hc[ij] & VRNA_HC_CONTEXT_EXT_LOOP)) break;

              if(with_gquad){
                f5[length] = MIN2(f5[length], ggg[indx[length]+1]);
              }

              en          = c[ij] + E_ExtLoop(ptype[ij], -1, -1, P);
              f5[length]  = MIN2(f5[length], en);
              break;

    /* normal dangles, aka dangle_model = 1 || 3 */
    default:  for(j=turn+2; j<=length; j++){
                f5[j] = INF;
                if(hc_up[j]){
                  f5[j] = f5[j-1];
                  if(sc)
                    if(sc->free_energies)
                      f5[j] += sc->free_energies[j][1];
                }
                for (i=j-turn-1; i>1; i--){
                  ij = indx[j] + i;
                  if(hc[ij] & VRNA_HC_CONTEXT_EXT_LOOP){

                    if(with_gquad){
                      f5[j] = MIN2(f5[j], f5[i-1] + ggg[indx[j]+i]);
                    }

                    type  = ptype[ij];
                    en    = f5[i-1] + c[ij] + E_ExtLoop(type, -1, -1, P);
                    f5[j] = MIN2(f5[j], en);
                    if(hc_up[i-1]){
                      en    = f5[i-2] + c[ij] + E_ExtLoop(type, S[i-1], -1, P);

                      if(sc)
                        if(sc->free_energies)
                          en += sc->free_energies[i-1][1];

                      f5[j] = MIN2(f5[j], en);
                    }
                  }
                  ij = indx[j-1] + i;
                  if(hc[ij] & VRNA_HC_CONTEXT_EXT_LOOP){
                    if(hc_up[j]){
                      type  = ptype[ij];
                      en    = f5[i-1] + c[ij] + E_ExtLoop(type, -1, S[j], P);

                      if(sc)
                        if(sc->free_energies)
                          en += sc->free_energies[j][1];

                      f5[j] = MIN2(f5[j], en);

                      if(hc_up[i-1]){
                        en    = f5[i-2] + c[ij] + E_ExtLoop(type, S[i-1], S[j], P);

                        if(sc)
                          if(sc->free_energies)
                            en += sc->free_energies[i-1][1] + sc->free_energies[j][1];

                        f5[j] = MIN2(f5[j], en);
                      }
                    }
                  }
                }
                ij = indx[j] + 1;
                if(hc[ij] & VRNA_HC_CONTEXT_EXT_LOOP){

                  if(with_gquad){
                    f5[j] = MIN2(f5[j], ggg[indx[j]+1]);
                  }

                  type  = ptype[ij];
                  en    = c[ij] + E_ExtLoop(type, -1, -1, P);
                  f5[j] = MIN2(f5[j], en);
                }
                ij = indx[j-1] + 1;
                if(hc[ij] & VRNA_HC_CONTEXT_EXT_LOOP){
                  if(hc_up[j]){
                    type  = ptype[ij];
                    en    = c[ij] + E_ExtLoop(type, -1, S[j], P);

                    if(sc)
                      if(sc->free_energies)
                        en += sc->free_energies[j][1];

                    f5[j] = MIN2(f5[j], en);
                  }
                }
              }
  }
}

INLINE  PRIVATE int
E_Stem( int type,
        int si1,
        int sj1,
        int extLoop,
        vrna_param_t *P){

  int energy = 0;
  int d5 = (si1 >= 0) ? P->dangle5[type][si1] : 0;
  int d3 = (sj1 >= 0) ? P->dangle3[type][sj1] : 0;

  if(type > 2)
    energy += P->TerminalAU;

  if(si1 >= 0 && sj1 >= 0)
    energy += (extLoop) ? P->mismatchExt[type][si1][sj1] : P->mismatchM[type][si1][sj1];
  else
    energy += d5 + d3;

  if(!extLoop) energy += P->MLintern[type];
  return energy;
}

INLINE  PRIVATE int
E_ExtLoop(int type,
          int si1,
          int sj1,
          vrna_param_t *P){

  int energy = 0;
  if(si1 >= 0 && sj1 >= 0){
    energy += P->mismatchExt[type][si1][sj1];
  }
  else if (si1 >= 0){
    energy += P->dangle5[type][si1];
  }
  else if (sj1 >= 0){
    energy += P->dangle3[type][sj1];
  }

  if(type > 2)
    energy += P->TerminalAU;

  return energy;
}

INLINE  PRIVATE double
exp_E_Stem( int type,
            int si1,
            int sj1,
            int extLoop,
            vrna_exp_param_t *P){

  double energy = 1.0;
  double d5 = (si1 >= 0) ? P->expdangle5[type][si1] : 1.;
  double d3 = (sj1 >= 0) ? P->expdangle3[type][sj1] : 1.;

  if(si1 >= 0 && sj1 >= 0)
    energy = (extLoop) ? P->expmismatchExt[type][si1][sj1] : P->expmismatchM[type][si1][sj1];
  else
    energy = d5 * d3;

  if(type > 2)
    energy *= P->expTermAU;

  if(!extLoop) energy *= P->expMLintern[type];
  return energy;
}

INLINE PRIVATE double
exp_E_ExtLoop(int type,
              int si1,
              int sj1,
              vrna_exp_param_t *P){

  double energy = 1.0;
  if(si1 >= 0 && sj1 >= 0){
    energy = P->expmismatchExt[type][si1][sj1];
  }
  else if(si1 >= 0){
    energy = P->expdangle5[type][si1];
  }
  else if(sj1 >= 0){
    energy = P->expdangle3[type][sj1];
  }

  if(type > 2)
    energy *= P->expTermAU;

  return energy;
}


#endif
