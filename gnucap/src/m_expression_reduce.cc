/*$Id: m_expression_reduce.cc,v 26.99 2008/11/13 17:55:40 al Exp $ -*- C++ -*-
 * Copyright (C) 2003 Albert Davis
 * Author: Albert Davis <aldavis@gnu.org>
 *
 * This file is part of "Gnucap", the Gnu Circuit Analysis Package
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 *------------------------------------------------------------------
 * Reconstructs an infix expression from the RPN.
 * Incomplete:
 *	Expressions in arg-lists print as ####false####
 *	Not sure what should happen when expression is empty
 */
#include "u_function.h"
#include "u_parameter.h"
/*--------------------------------------------------------------------------*/
Token* Token_BINOP::op(const Token* T1, const Token* T2)const
{
  assert(T1);
  assert(T2);
  assert(dynamic_cast<const Token_CONSTANT*>(T1));
  assert(dynamic_cast<const Token_CONSTANT*>(T2));
  assert(T1->data());
  assert(T2->data());

  // not "or" operator, both ops exist -- do it
  Base* b = 0;
  if (name() == "*") {
    b = (T1->data())->multiply(T2->data());
  }else if (name() == "+") {
    b = (T1->data())->add(T2->data());
  }else if (name() == "-") {
    b = (T1->data())->subtract(T2->data());
  }else if (name() == "/") {
    b = (T1->data())->divide(T2->data());
  }else if (name() == "==") {
    b = (T1->data())->equal(T2->data());
  }else if (name() == "!=") {
    b = (T1->data())->not_equal(T2->data());
  }else if (name() == "<") {
    b = (T1->data())->less(T2->data());
  }else if (name() == ">") {
    b = (T1->data())->greater(T2->data());
  }else if (name() == "<=") {untested();
    b = (T1->data())->leq(T2->data());
  }else if (name() == ">=") {
    b = (T1->data())->geq(T2->data());
  }else if (name() == "||") {
    b = (T1->data())->logic_or(T2->data());
  }else if (name() == "&&") {
    b = (T1->data())->logic_and(T2->data());
  }else{untested();
    unreachable();
    //std::cout << name() << '\n';
    return NULL;
  }
  if (b) {
    return new Token_CONSTANT(b->val_string(), b, (T1->aRgs()+T2->aRgs()));
  }else{
    return new Token_CONSTANT("false", NULL, "");
  }
}
/*--------------------------------------------------------------------------*/
Token* Token_UNARY::op(const Token* T1)const
{
  assert(T1);
  assert(dynamic_cast<const Token_CONSTANT*>(T1));
  assert(T1->data());
  
  const Base* b = 0;
  if (name() == "-") {
    b = (T1->data())->minus();
  }else if (name() == "+") {
    b = (T1->data())->plus();
  }else if (name() == "!") {
    b = (T1->data())->logic_not();
  }else{untested();
    unreachable();
    //std::cout << name() << '\n';
    return NULL;
  }
  if (b) {
    return new Token_CONSTANT(b->val_string(), b, (T1->aRgs()));
  }else{
    return new Token_CONSTANT("false", NULL, "");
  }
}
/*--------------------------------------------------------------------------*/
void Token_SYMBOL::stack_op(Expression* E)const
{
  assert(E);
  // replace single token with its value
  if (!E->is_empty() && dynamic_cast<const Token_PARLIST*>(E->back())) {
    // has parameters (table or function)
    if (FUNCTION* f = function_dispatcher[name()]) {
      const Token* T1 = E->back(); // arglist
      E->pop_back();
      CS cmd(CS::_STRING, T1->name());      
      std::string value = f->eval(cmd, &CARD_LIST::card_list);
      const Float* v = new Float(value);
      E->push_back(new Token_CONSTANT(value, v, T1->full_name()));
      delete T1;
    }else{
      E->push_back(clone());
    }
  }else{
    // has no parameters (scalar)
    //if (isdigit(name()[0])) {
    if (strchr("0123456789.", name()[0])) {
      // a number
      Float* n = new Float(name());
      E->push_back(new Token_CONSTANT(name(), n, ""));
    }else{
      // a name
      PARAMETER<double> p = (*(E->_params))[name()];
      if (p.has_hard_value()) {
	// can find value - push value
	double v = p.e_val(NOT_INPUT, &CARD_LIST::card_list);
	Float* n = new Float(v);
	E->push_back(new Token_CONSTANT(n->val_string(), n, ""));
      }else{
	// no value - push name (and accept incomplete solution later)
	String* s = new String(name());
	E->push_back(new Token_CONSTANT(name(), s, ""));	
      }
    }
  }
}
/*--------------------------------------------------------------------------*/
void Token_BINOP::stack_op(Expression* E)const
{
  assert(E);
  // replace 2 tokens (binop) with 1 (result)
  Token* t1 = E->back();
  E->pop_back();
  Token* t2 = E->back();
  E->pop_back();
  if (dynamic_cast<Token_CONSTANT*>(t1)) {
    if (dynamic_cast<Token_CONSTANT*>(t2)) {
      // have # # + .. becomes result (the usual)
      Token* t = op(t2, t1);
      if (t->data()) {
	// success
	E->push_back(t);
	delete t2;
	delete t1;
      }else{
	// fail - one arg is unknown, push back args
	if (strchr("+*", name()[0]) && !dynamic_cast<const Float*>(t1->data())) {
	  // change order to enable later optimization
	  E->push_back(t1);
	  E->push_back(t2);
	}else{
	  E->push_back(t2);
	  E->push_back(t1);
	}
	E->push_back(clone());
	delete t;
      }
    }else if (((*t2) == (*this)) && strchr("+*", name()[0])
	      && dynamic_cast<Token_CONSTANT*>(E->back())) {
      // have # + # + .. becomes result + (previous unknown, try to optimize)
      Token* t3 = E->back();
      E->pop_back();
      Token* t = op(t3, t1);
      if (t->data()) {
	// success
	E->push_back(t);
	E->push_back(t2);
	delete t3;
	delete t1;
      }else{
	// fail - push all
	E->push_back(t3);
	E->push_back(t2);
	E->push_back(t1);
	E->push_back(clone());
	delete t;
      }
    }else{
      // # - # - or something like that
      E->push_back(t2);
      E->push_back(t1);
      E->push_back(clone());
    }
  }else{
    // # - # - or something like that
    E->push_back(t2);
    E->push_back(t1);
    E->push_back(clone());
  }
}
/*--------------------------------------------------------------------------*/
void Token_STOP::stack_op(Expression* E)const
{
  assert(E);
  E->push_back(clone());
}
/*--------------------------------------------------------------------------*/
void Token_PARLIST::stack_op(Expression* E)const
{
  assert(E);
#if 0
  E->push_back(clone());
#else
  // replace multiple tokens of a PARLIST with a single token
  bool been_here = false;
  std::string tmp;//(")");
  for (;;) {
    const Token* t = E->back();
    E->pop_back();
    if (dynamic_cast<const Token_STOP*>(t)) {
      // tmp = "(" + tmp;
      break;
    }else{
      if (been_here) {
	tmp = ", " + tmp;
      }else{
	been_here = true;
      }
      tmp = t->full_name() + tmp;
    }
    delete t;
  }
  E->push_back(new Token_PARLIST(tmp));
#endif
}
/*--------------------------------------------------------------------------*/
void Token_UNARY::stack_op(Expression* E)const
{
  assert(E);
  // replace 1 token with 1 (result)
  Token* t1 = E->back();
  E->pop_back();
  if (dynamic_cast<Token_CONSTANT*>(t1)) {
    Token* t = op(t1);
    if (t->data()) {
      E->push_back(t);
      delete t1;
    }else{
      E->push_back(t1);
      E->push_back(clone());
      delete t;
    }
  }else{
    E->push_back(t1);
    E->push_back(clone());
  }
}
/*--------------------------------------------------------------------------*/
void Token_CONSTANT::stack_op(Expression* E)const
{untested();
  unreachable();
  assert(E);
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void Expression::reduce_copy(const Expression& Proto)
{
  // The Proto._list is the expression in RPN.
  // Attempt to build a reduced _list here, hopefully with only one item.
  for (const_iterator i = Proto.begin(); i != Proto.end(); ++i) {
    (**i).stack_op(this);
  }
  if (is_empty()) {untested();
    assert(Proto.is_empty());
  }else{
  }
}
/*--------------------------------------------------------------------------*/
Expression::Expression(const Expression& Proto, PARAM_LIST* Params)
  :_params(Params)
{
  reduce_copy(Proto);
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
