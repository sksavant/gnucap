 /* vim:ts=8:sw=2:et:
  * "adp" container -- list of aging parms
 */
#ifndef E_ADPLIST_H
#define E_ADPLIST_H
#include "md.h"
#include "e_adp.h"
/*--------------------------------------------------------------------------*/
// defined here
class ADP_NODE_LIST;
class ADP_NODE;
class ADP_LIST;
class ADP_CARD;
/*--------------------------------------------------------------------------*/
// external
class PARAM_LIST;
class NODE_MAP;
class LANGUAGE;
class TIME_PAIR;
/*--------------------------------------------------------------------------*/
class INTERFACE ADP_LIST { // cardlist, needed?
private:
  const ADP_LIST* _parent;
  mutable NODE_MAP* _nm;
  mutable PARAM_LIST* _params;
  LANGUAGE* _language;
  std::list<ADP_CARD*> _al;
public:
  // internal types
  typedef std::list<ADP_CARD*>::iterator iterator;
  typedef std::list<ADP_CARD*>::const_iterator const_iterator;
  class fat_iterator {
  private:
    ADP_LIST*  _list;
    iterator   _iter;
  private:
    explicit	  fat_iterator()	{unreachable();}
  public:
		  fat_iterator(const fat_iterator& p)
			: _list(p._list), _iter(p._iter) {}
    explicit	  fat_iterator(ADP_LIST* l, iterator i)
			: _list(l), _iter(i) {}
    bool	  is_end()const		{return _iter == _list->end();}
    ADP_CARD*	  operator*()		{return (is_end()) ? NULL : *_iter;}
    fat_iterator& operator++()	{assert(!is_end()); ++_iter; return *this;}
    fat_iterator  operator++(int)
		{assert(!is_end()); fat_iterator t(*this); ++_iter; return t;}
    bool	  operator==(const fat_iterator& x)const
    	     {unreachable(); assert(_list==x._list); return (_iter==x._iter);}
    bool	  operator!=(const fat_iterator& x)const
			{assert(_list==x._list); return (_iter!=x._iter);}
    iterator	  iter()const		{return _iter;}
    ADP_LIST*	  list()const		{return _list;}
    fat_iterator  end()const	{return fat_iterator(_list, _list->end());}

    //void	  insert(ADP_CARD* c)	{list()->insert(iter(),c);}
  };

  // status queries
  bool is_empty()const			{return _al.empty();}
  const ADP_LIST* parent()const	{return _parent;}
  const LANGUAGE* language()const	{untested(); return _language;}

  // return an iterator
  iterator begin()			{return _al.begin();}
  iterator end()			{return _al.end();}
  iterator find_again(const std::string& short_name, iterator);
  iterator find_(const std::string& short_name) 
					{return find_again(short_name, begin());}

  ADP_LIST& erase(iterator i);
  ADP_LIST& erase(ADP_CARD* c);
  ADP_LIST& erase_all();
  ADP_LIST& expand();
  ADP_LIST& do_forall( void (ADP_CARD::*thing)( void ) );


  // return a const_iterator
  const_iterator begin()const		{return _al.begin();}
  const_iterator end()const		{return _al.end();}
  const_iterator find_again(const std::string& short_name, const_iterator)const;
  const_iterator find_(const std::string& short_name)const
					{return find_again(short_name, begin());}

  // add to it
  void print();


  ADP_LIST& push_front(ADP_CARD* c) {_al.push_front(c); return *this;}
  ADP_LIST& push_back(ADP_CARD* c)  {_al.push_back(c);  return *this;}


  TIME_PAIR tt_review() const;

  TIME_PAIR tt_preview();

  ADP_LIST& save();

  NODE_MAP*   nodes()const {assert(_nm); return _nm;}
  PARAM_LIST* params();
  PARAM_LIST* params()const;

  // more complex stuff
  void attach_params(PARAM_LIST* p, const ADP_LIST* scope);
  void shallow_copy(const ADP_LIST*);
  void map_subckt_nodes(const ADP_CARD* model, const ADP_CARD* owner);

  explicit ADP_LIST();
  ADP_LIST(const ADP_CARD* model, ADP_CARD* owner, const ADP_LIST* scope,
  	    PARAM_LIST* p);
  ~ADP_LIST();
private:
  explicit ADP_LIST(const ADP_LIST&) {unreachable(); incomplete();}
public:
  static ADP_LIST adp_list; // in globals.cc
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
class INTERFACE ADP_NODE_LIST {
private:
  const ADP_NODE_LIST* _parent;
  mutable NODE_MAP* _nm;
  mutable PARAM_LIST* _params;
  LANGUAGE* _language;
  std::list<ADP_NODE*> _al;
public:
  // internal types
  typedef std::list<ADP_NODE*>::iterator iterator;
  typedef std::list<ADP_NODE*>::const_iterator const_iterator;
  class fat_iterator {
  private:
    ADP_NODE_LIST*  _list;
    iterator   _iter;
  private:
    explicit	  fat_iterator()	{unreachable();}
  public:
		  fat_iterator(const fat_iterator& p)
			: _list(p._list), _iter(p._iter) {}
    explicit	  fat_iterator(ADP_NODE_LIST* l, iterator i)
			: _list(l), _iter(i) {}
    bool	  is_end()const		{return _iter == _list->end();}
    ADP_NODE*	  operator*()		{return (is_end()) ? NULL : *_iter;}
    fat_iterator& operator++()	{assert(!is_end()); ++_iter; return *this;}
    fat_iterator  operator++(int)
		{assert(!is_end()); fat_iterator t(*this); ++_iter; return t;}
    bool	  operator==(const fat_iterator& x)const
    	     {unreachable(); assert(_list==x._list); return (_iter==x._iter);}
    bool	  operator!=(const fat_iterator& x)const
			{assert(_list==x._list); return (_iter!=x._iter);}
    iterator	  iter()const		{return _iter;}
    ADP_NODE_LIST*	  list()const		{return _list;}
    fat_iterator  end()const	{return fat_iterator(_list, _list->end());}

    //void	  insert(ADP_NODE* c)	{list()->insert(iter(),c);}
  };

  // status queries
  bool is_empty()const			{return _al.empty();}
  const ADP_NODE_LIST* parent()const	{return _parent;}
  const LANGUAGE* language()const	{untested(); return _language;}

  // return an iterator
  iterator begin()			{return _al.begin();}
  iterator end()			{return _al.end();}
  iterator find_again(const std::string& short_name, iterator);
  iterator find_(const std::string& short_name) 
					{return find_again(short_name, begin());}

  ADP_NODE_LIST& erase(iterator i);
  ADP_NODE_LIST& erase(ADP_NODE* c);
  ADP_NODE_LIST& erase_all();
  ADP_NODE_LIST& expand();
  ADP_NODE_LIST& do_forall( void (ADP_NODE::*thing)( void ) );




  // return a const_iterator
  const_iterator begin()const		{return _al.begin();}
  const_iterator end()const		{return _al.end();}
  const_iterator find_again(const std::string& short_name, const_iterator)const;
  const_iterator find_(const std::string& short_name)const
					{return find_again(short_name, begin());}

  // add to it
  void print();

  //ADP_NODE_LIST& insert(ADP::iterator i, ADP_NODE* c) {
  //  _al.insert(i, c);  return *this;
  //}

  ADP_NODE_LIST& push_front(ADP_NODE* c) {_al.push_front(c); return *this;}
  ADP_NODE_LIST& push_back(ADP_NODE* c)  {_al.push_back(c);  return *this;}


  TIME_PAIR tt_review();

  TIME_PAIR tt_preview();

  ADP_NODE_LIST& save();

  NODE_MAP*   nodes()const {assert(_nm); return _nm;}
  PARAM_LIST* params();
  PARAM_LIST* params()const;

  // more complex stuff
  void attach_params(PARAM_LIST* p, const ADP_NODE_LIST* scope);
  void shallow_copy(const ADP_NODE_LIST*);
  void map_subckt_nodes(const ADP_NODE* model, const ADP_NODE* owner);

  explicit ADP_NODE_LIST();
  ADP_NODE_LIST(const ADP_NODE* model, ADP_NODE* owner, const ADP_NODE_LIST* scope,
  	    PARAM_LIST* p);
  ~ADP_NODE_LIST();
private:
  explicit ADP_NODE_LIST(const ADP_NODE_LIST&) {unreachable(); incomplete();}
public:
  static ADP_NODE_LIST adp_node_list; // in globals.cc
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
