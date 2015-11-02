////////////////////////////////////////////////////////////////////////////////
#ifndef COMMON_CLASSES__FB_LIST_EXTERNAL_H
#define COMMON_CLASSES__FB_LIST_EXTERNAL_H

#include "firebird.h"

namespace Firebird{
////////////////////////////////////////////////////////////////////////////////
//containings

class FB_ListExternal_base__with_size;

template<class t_traits,class t_base=FB_ListExternal_base__with_size>
class FB_ListExternal;

////////////////////////////////////////////////////////////////////////////////
//class FB_ListExternal_base__with_size

class FB_ListExternal_base__with_size
{
 public: //typedefs ------------------------------------------------------
  typedef size_t                                         size_type;

 public:
  size_type size()const;

 protected:
  void set_size(size_type x);

  void dec_size();

  void inc_size();

 #ifdef DEV_BUILD
  void verify_size__eq(size_type x)const;

  void verify_size__eq_or_large(size_type x)const;
 #endif

 private:
  size_type m_size;
};//FB_ListExternal_base__with_size

////////////////////////////////////////////////////////////////////////////////
//class FB_ListExternal

//t_traits specification
// - typedef node_ptr_type
// - node_ptr_type null_ptr()const
// - node_ptr_type get_prev(node_ptr_type node)const;
// - node_ptr_type get_next(node_ptr_type node)const;
// - void set_prev(node_ptr_type node,node_ptr_type new_prev)const;
// - void set_next(node_ptr_type node,node_ptr_type new_next)const;
template<class t_traits,class t_base>
class FB_ListExternal:public t_base
{
 public: //typedefs ------------------------------------------------------
  typedef t_traits                                       traits_type;

  typedef typename traits_type::node_ptr_type            node_ptr_type;

 public:
  FB_ListExternal();

  //interface ------------------------------------------------------------
  bool empty()const;

  node_ptr_type head()const;

  node_ptr_type tail()const;

  /// <summary>
  ///  Clear list
  /// </summary>
  void clear();

  /// <summary>
  ///  Remove node from list
  /// </summary>
  //! \param[in] traits
  //! \param[in] x
  //!  Pointer of list item
  void remove(const traits_type& traits,
              node_ptr_type      x);

  /// <summary>
  ///  Insert new node into list header
  /// </summary>
  //! \param[in] traits
  //! \param[in] x
  //!  Pointer to new list item
  void push_front(const traits_type& traits,
                  node_ptr_type      x);

  /// <summary>
  ///  Insert new node into list tail
  /// </summary>
  //! \param[in] traits
  //! \param[in] x
  //!  Pointer to new list item
  void push_back(const traits_type& traits,
                 node_ptr_type      x);

 private:
 #ifdef DEV_BUILD
  void verify(const traits_type& traits)const;

  void verify_item(const traits_type& traits,
                   node_ptr_type      x)const;
 #endif

 private:
  node_ptr_type  m_head;
  node_ptr_type  m_tail;
};//class FB_ListExternal

////////////////////////////////////////////////////////////////////////////////
}//namespace Firebird
////////////////////////////////////////////////////////////////////////////////
#include <../../common/classes/fb_list_external.cc>
////////////////////////////////////////////////////////////////////////////////
#endif
