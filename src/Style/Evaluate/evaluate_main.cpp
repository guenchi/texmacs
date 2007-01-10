
/******************************************************************************
* MODULE     : evaluate_main.cpp
* DESCRIPTION: standard style evaluation
* COPYRIGHT  : (C) 2006  Joris van der Hoeven
*******************************************************************************
* This software falls under the GNU general public license and comes WITHOUT
* ANY WARRANTY WHATSOEVER. See the file $TEXMACS_PATH/LICENSE for more details.
* If you don't have this file, write to the Free Software Foundation, Inc.,
* 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
******************************************************************************/

#include "evaluate_main.hpp"
#include "memorizer.hpp"
#include "vars.hpp"

environment std_env;

/******************************************************************************
* Memorizing evaluations
******************************************************************************/

static tree no_tree (UNINIT);

class memorizer;
class evaluate_memorizer_rep: public compound_memorizer_rep {
  environment env_in;
  tree t_in;
  environment env_out;
  tree t_out;

public:
  inline evaluate_memorizer_rep (environment env, tree t):
    env_in (env), t_in (t), env_out (), t_out (no_tree) {}
  void print (ostream& out) {
    out << "evaluate_memorizer (" << t_in << ")"; }

  int type () { return MEMORIZE_EVALUATE; }
  int hash () { return weak_hash (env_in) ^ weak_hash (t_in); }
  bool equal (memorizer_rep* mem) {
    evaluate_memorizer_rep* rep= (evaluate_memorizer_rep*) mem;
    return weak_equal (env_in, rep->env_in) && weak_equal (t_in, rep->t_in); }

  void set_environment (environment env) { env_out= env; }
  environment get_environment () { return env_out; }
  void set_tree (tree t) { t_out= t; }
  tree get_tree () { return t_out; }
};

inline memorizer
evaluate_memorizer (environment env, tree t) {
  return (memorizer_rep*) new evaluate_memorizer_rep (env, t);
}

/******************************************************************************
* Main dispatching
******************************************************************************/

tree
evaluate_impl (tree t) {
  //cout << "Really evaluate " << t << LF;
  switch (L(t)) {
  /* Typesetting primitives with side effects */
  case DATOMS:
    return evaluate_formatting (t, ATOM_DECORATIONS);
  case DLINES:
    return evaluate_formatting (t, LINE_DECORATIONS);
  case DPAGES:
    return evaluate_formatting (t, PAGE_DECORATIONS);
  case TFORMAT:
    return evaluate_formatting (t, CELL_FORMAT);
  case TABLE:
    return evaluate_table (t);

  /* Primitives for macro expansion */
  case ASSIGN:
    return evaluate_assign (t);
  case WITH:
    return evaluate_with (t);
  case PROVIDES:
    return evaluate_provides (t);
  case VALUE:
    return evaluate_value (t);
  case QUOTE_VALUE:
    return evaluate_quote_value (t);
  case MACRO:
    return copy (t);
  case DRD_PROPS:
    return evaluate_drd_props (t);
  case ARG:
    return evaluate_arg (t);
  case QUOTE_ARG:
    return evaluate_quote_arg (t);
  case COMPOUND:
    return evaluate_compound (t);
  case XMACRO:
    return copy (t);
  case GET_LABEL:
    return evaluate_get_label (t);
  case GET_ARITY:
    return evaluate_get_arity (t);

  /* Primitives for quoting and evaluation */
  case MAP_ARGS:
    return evaluate_rewrite (t);
  case EVAL_ARGS:
    return evaluate_eval_args (t);
  case MARK:
    return tree (MARK, copy (t[0]), evaluate (t[1]));
  case EXPAND_AS:
    return evaluate (t[1]);
  case EVAL:
    return evaluate (evaluate (t[0]));
  case QUOTE:
    return t[0];
  case QUASI:
    return evaluate (evaluate_quasiquote (t[0]));
  case QUASIQUOTE:
    return evaluate_quasiquote (t[0]);
  case UNQUOTE:
  case VAR_UNQUOTE:
    return evaluate (t[0]);
  case COPY:
    return copy (evaluate (t[0]));    

  /* Control structures */
  case IF:
  case VAR_IF:
    return evaluate_if (t);
  case CASE:
    return evaluate_case (t);
  case WHILE:
    return evaluate_while (t);
  case FOR_EACH:
    return evaluate_for_each (t);
  case EXTERN:
    return evaluate_rewrite (t);
  case INCLUDE:
    return evaluate_rewrite (t);
  case USE_PACKAGE:
    return evaluate_use_package (t);
  case USE_MODULE:
    return evaluate_use_module (t);

  /* Computational markup */
  case OR:
    return evaluate_or (t);
  case XOR:
    return evaluate_xor (t);
  case AND:
    return evaluate_and (t);
  case NOT:
    return evaluate_not (t);
  case PLUS:
  case MINUS:
    return evaluate_plus_minus (t);
  case TIMES:
  case OVER:
    return evaluate_times_over (t);
  case DIV:
    return evaluate_divide (t);
  case MOD:
    return evaluate_modulo (t);
  case MATH_SQRT:
    return evaluate_math_sqrt (t);
  case EXP:
    return evaluate_exp (t);
  case LOG:
    return evaluate_log (t);
  case POW:
    return evaluate_pow (t);
  case COS:
    return evaluate_cos (t);
  case SIN:
    return evaluate_sin (t);
  case TAN:
    return evaluate_tan (t);
  case MERGE:
    return evaluate_merge (t);
  case LENGTH:
    return evaluate_length (t);
  case RANGE:
    return evaluate_range (t);
  case NUMBER:
    return evaluate_number (t);
  case _DATE:
    return evaluate_date (t);
  case TRANSLATE:
    return evaluate_translate (t);
  case CHANGE_CASE:
    return evaluate_change_case (t);
  case FIND_FILE:
    return evaluate_find_file (t);
  case IS_TUPLE:
    return evaluate_is_tuple (t);
  case LOOK_UP:
    return evaluate_lookup (t);
  case EQUAL:
    return evaluate_equal (t);
  case UNEQUAL:
    return evaluate_unequal (t);
  case LESS:
    return evaluate_less (t);
  case LESSEQ:
    return evaluate_lesseq (t);
  case GREATER:
    return evaluate_greater (t);
  case GREATEREQ:
    return evaluate_greatereq (t);

  /* Length units */
  case CM_LENGTH:
    return evaluate_cm_length ();
  case MM_LENGTH:
    return evaluate_mm_length ();
  case IN_LENGTH:
    return evaluate_in_length ();
  case PT_LENGTH:
    return evaluate_pt_length ();
  case BP_LENGTH:
    return evaluate_bp_length ();
  case DD_LENGTH:
    return evaluate_dd_length ();
  case PC_LENGTH:
    return evaluate_pc_length ();
  case CC_LENGTH:
    return evaluate_cc_length ();
  case FS_LENGTH:
    return evaluate_fs_length ();
  case FBS_LENGTH:
    return evaluate_fbs_length ();
  case EM_LENGTH:
    return evaluate_em_length ();
  case LN_LENGTH:
    return evaluate_ln_length ();
  case SEP_LENGTH:
    return evaluate_sep_length ();
  case YFRAC_LENGTH:
    return evaluate_yfrac_length ();
  case EX_LENGTH:
    return evaluate_ex_length ();
  case FN_LENGTH:
    return evaluate_fn_length ();
  case FNS_LENGTH:
    return evaluate_fns_length ();
  case BLS_LENGTH:
    return evaluate_bls_length ();
  case SPC_LENGTH:
    return evaluate_spc_length ();
  case XSPC_LENGTH:
    return evaluate_xspc_length ();
  case PAR_LENGTH:
    return evaluate_par_length ();
  case PAG_LENGTH:
    return evaluate_pag_length ();
  case GW_LENGTH:
    return evaluate_gw_length ();
  case GH_LENGTH:
    return evaluate_gh_length ();
  case TMPT_LENGTH:
    return evaluate_tmpt_length ();
  case PX_LENGTH:
    return evaluate_px_length ();
  case MSEC_LENGTH:
    return evaluate_msec_length ();
  case SEC_LENGTH:
    return evaluate_sec_length ();
  case MIN_LENGTH:
    return evaluate_min_length ();
  case H_LENGTH:
    return evaluate_h_length ();

  /* Primitives for stylesheet editing */
  case STYLE_WITH:
  case VAR_STYLE_WITH:
    return evaluate (t[N(t)-1]);
  case STYLE_ONLY:
  case VAR_STYLE_ONLY:
  case ACTIVE:
  case VAR_ACTIVE:
  case INACTIVE:
  case VAR_INACTIVE:
    return evaluate_compound (t);
  case REWRITE_INACTIVE:
    return evaluate_rewrite (t);

  /* Linking primitives */
  case HARD_ID:
    return evaluate_hard_id (t[0]);
  case SCRIPT:
    return evaluate_script (t);
  case HLINK:
  case ACTION:
    return evaluate_compound (t);
  case SET_BINDING:
    return evaluate_set_binding (t);
  case GET_BINDING:
    return evaluate_get_binding (t);

  /* Graphical primitives */
  case _POINT:
    return evaluate_point (t);
    /*
  case BOX_INFO:
    return evaluate_box_info (t);
  case FRAME_DIRECT:
    return evaluate_frame_direct (t);
  case FRAME_INVERSE:
    return evaluate_frame_inverse (t);
    */

  /* User extensions */
  default:
    if (L(t) < START_EXTENSIONS) {
      int i, n= N(t);
      tree r (t, n);
      for (i=0; i<n; i++)
	r[i]= evaluate (t[i]);
      //_set_ip (r, _get_ip (t));
      //cout << "Return " << r << LF;
      return r;
    }
    else {
      tree r= evaluate_compound (t);
      //_set_ip (r, _get_ip (t));
      return r;
    }      
  }
}

/******************************************************************************
* Error handling
******************************************************************************/

tree
evaluate_error (string error) {
  return tree (ERROR, error);
}

tree
evaluate_error (string error, tree arg) {
  return tree (ERROR, error, arg);
}

tree
evaluate_error (string error, array<tree> args) {
  tree r= tree (ERROR, error);
  r << args;
  return r;
}

/******************************************************************************
* Main evaluation routines
******************************************************************************/

tree
evaluate (tree t) {
  if (is_atomic (t)) return t;
  cout << "Evaluate "
    //<< "[" << (t.operator -> ())
    //<< ", " << (std_env.operator -> ()) << "] "
       << t << INDENT << LF;
  memorizer mem= evaluate_memorizer (std_env, t);
  if (is_memorized (mem)) {
    cout << UNINDENT << "Memorized " << mem->get_tree () << LF;
    std_env= mem->get_environment ();
    return mem->get_tree ();
  }
  memorize_start ();
  tree r= evaluate_impl (t);
  mem->set_tree (r);
  mem->set_environment (std_env);
  memorize_end ();
  cout << UNINDENT << "Computed " << mem->get_tree () << LF;
  return mem->get_tree ();
}

memorizer
evaluate (environment env, tree t) {
  environment old_env= std_env;
  memorize_initialize ();
  std_env= env;
  (void) evaluate (t);
  std_env= old_env;
  return memorize_finalize ();
}
