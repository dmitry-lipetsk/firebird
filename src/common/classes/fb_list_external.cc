////////////////////////////////////////////////////////////////////////////////
#ifndef COMMON_CLASSES__FB_LIST_EXTERNAL_CC
#define COMMON_CLASSES__FB_LIST_EXTERNAL_CC

namespace Firebird{
////////////////////////////////////////////////////////////////////////////////
//class FB_ListExternal_base__with_size

inline FB_ListExternal_base__with_size::size_type
 FB_ListExternal_base__with_size::size()const
{
 return m_size;
}//size

//------------------------------------------------------------------------
inline void FB_ListExternal_base__with_size::set_size(size_type x)
{
 m_size=x;
}//set_size

//------------------------------------------------------------------------
inline void FB_ListExternal_base__with_size::dec_size()
{
 --m_size;
}//dec_size

//------------------------------------------------------------------------
inline void FB_ListExternal_base__with_size::inc_size()
{
 ++m_size;
}//inc_size

//------------------------------------------------------------------------
#ifdef DEV_BUILD
inline void FB_ListExternal_base__with_size::verify_size__eq(size_type x)const
{
 fb_assert(m_size==x);
}//verify_size__eq

//------------------------------------------------------------------------
inline void FB_ListExternal_base__with_size::verify_size__eq_or_large(size_type x)const
{
 fb_assert(m_size>=x);
}//verify_size__eq_or_large
#endif

////////////////////////////////////////////////////////////////////////////////
//class FB_ListExternal

template<class t_traits,class t_base>
FB_RELEASE_CODE(inline)
FB_ListExternal<t_traits,t_base>::FB_ListExternal()
{
 this->clear();
}

//------------------------------------------------------------------------
template<class t_traits,class t_base>
FB_RELEASE_CODE(inline)
bool FB_ListExternal<t_traits,t_base>::empty()const
{
 return m_head==traits_type::null_ptr();
}//empty

//------------------------------------------------------------------------
template<class t_traits,class t_base>
FB_RELEASE_CODE(inline)
typename FB_ListExternal<t_traits,t_base>::node_ptr_type
 FB_ListExternal<t_traits,t_base>::head()const
{
 return m_head;
}//head

//------------------------------------------------------------------------
template<class t_traits,class t_base>
FB_RELEASE_CODE(inline)
typename FB_ListExternal<t_traits,t_base>::node_ptr_type
 FB_ListExternal<t_traits,t_base>::tail()const
{
 return m_tail;
}//tail

//------------------------------------------------------------------------
template<class t_traits,class t_base>
FB_RELEASE_CODE(inline)
void FB_ListExternal<t_traits,t_base>::clear()
{
 this->set_size(0);

 m_head =traits_type::null_ptr();
 m_tail =traits_type::null_ptr();
}//clear

//------------------------------------------------------------------------
#ifdef DEV_BUILD
template<class t_traits,class t_base>
void FB_ListExternal<t_traits,t_base>::verify(const traits_type& traits)const
{
 if(m_head==traits.null_ptr())
 {
  fb_assert(m_tail==traits.null_ptr());

  this->verify_size__eq(0);
 }
 else
 {
  fb_assert(m_tail!=traits.null_ptr());

  this->verify_size__eq_or_large(1);

  this->verify_item(traits,m_head);
  this->verify_item(traits,m_tail);
 }
}//verify

//------------------------------------------------------------------------
template<class t_traits,class t_base>
void FB_ListExternal<t_traits,t_base>::verify_item(const traits_type&  traits,
                                                   node_ptr_type const x)const
{
 fb_assert(x!=traits.null_ptr());
 fb_assert(m_head!=traits.null_ptr());
 fb_assert(m_tail!=traits.null_ptr());

 fb_assert(traits.get_prev(m_head)==traits.null_ptr());
 fb_assert(traits.get_next(m_tail)==traits.null_ptr());

 node_ptr_type const x_prev=traits.get_prev(x);
 node_ptr_type const x_next=traits.get_next(x);

 if(x_prev==traits.null_ptr())
 {
  fb_assert(x==m_head);

  this->verify_size__eq_or_large(1);
 }
 else
 if(x_next==traits.null_ptr())
 {
  fb_assert(x==m_tail);

  this->verify_size__eq_or_large(1);
 }
 else
 {
  fb_assert(m_head!=m_tail);

  this->verify_size__eq_or_large(3);
 }

 //check of links
 if(x_prev!=traits.null_ptr())
 {
  fb_assert(traits.get_next(x_prev)==x);
 }

 if(x_next!=traits.null_ptr())
 {
  fb_assert(traits.get_prev(x_next)==x);
 }
}//verify_item

#endif //NDEBUG

//------------------------------------------------------------------------
template<class t_traits,class t_base>
void FB_ListExternal<t_traits,t_base>::remove(const traits_type&  traits,
                                              node_ptr_type const x)
{
#ifdef DEV_BUILD
 this->verify_size__eq_or_large(1);
 this->verify(traits);
 this->verify_item(traits,x);
#endif

 if(m_head==x)
 {
  if(m_tail==x)
  {
#ifdef DEV_BUILD
   this->verify_size__eq(1);
#endif

   this->clear();
  }
  else
  {
   fb_assert(m_head!=m_tail);

   fb_assert(traits.get_next(x)!=traits.null_ptr());

#ifdef DEV_BUILD
   this->verify_size__eq_or_large(2);
#endif

   //set second item as first
   const node_ptr_type new_head=traits.get_next(x);

   traits.set_prev(new_head,traits.null_ptr());

   m_head=new_head;

   this->dec_size();
  }
 }
 else
 if(m_tail==x)
 {
  //remove last list item

  fb_assert(m_head!=m_tail);

  fb_assert(traits.get_prev(x)!=traits.null_ptr());

#ifdef DEV_BUILD
  this->verify_size__eq_or_large(2);
#endif

  const node_ptr_type new_tail=traits.get_prev(x);

  traits.set_next(new_tail,traits.null_ptr());

  m_tail=new_tail;

  this->dec_size();
 }
 else
 {
  //remove middle list item
  fb_assert(m_head!=m_tail);

#ifdef DEV_BUILD
  this->verify_size__eq_or_large(3);
#endif

  fb_assert(traits.get_prev(x)!=traits.null_ptr());
  fb_assert(traits.get_next(x)!=traits.null_ptr());

  traits.set_next(traits.get_prev(x),traits.get_next(x));
  traits.set_prev(traits.get_next(x),traits.get_prev(x));

  this->dec_size();
 }//else

 traits.set_prev(x,traits.null_ptr());
 traits.set_next(x,traits.null_ptr());

#ifdef DEV_BUILD
 this->verify(traits);
#endif
}//remove

//------------------------------------------------------------------------
template<class t_traits,class t_base>
void FB_ListExternal<t_traits,t_base>::push_front(const traits_type&  traits,
                                                  node_ptr_type const x)
{
 fb_assert(x!=traits.null_ptr());
 fb_assert(x!=m_head);
 fb_assert(x!=m_tail);

#ifdef DEV_BUILD
 this->verify(traits);
#endif

 if(m_head==traits.null_ptr())
 {
  //empty list
  fb_assert(m_tail==traits.null_ptr());

#ifdef DEV_BUILD
  this->verify_size__eq(0);
#endif

  traits.set_prev(x,traits.null_ptr());
  traits.set_next(x,traits.null_ptr());

  m_head =x;
  m_tail =x;

  this->set_size(1);
 }
 else
 {
  //not empty list
  fb_assert(m_head!=traits.null_ptr());
  fb_assert(m_tail!=traits.null_ptr());

#ifdef DEV_BUILD
  this->verify_size__eq_or_large(1);
#endif

  fb_assert(traits.get_prev(m_head)==traits.null_ptr());
  fb_assert(traits.get_next(m_tail)==traits.null_ptr());

  traits.set_prev(x,traits.null_ptr());
  traits.set_next(x,m_head);

  traits.set_prev(m_head,x);

  m_head=x;

  this->inc_size();
 }//else

#ifdef DEV_BUILD
 this->verify(traits);
#endif
}//push_front

//------------------------------------------------------------------------
template<class t_traits,class t_base>
void FB_ListExternal<t_traits,t_base>::push_back(const traits_type&  traits,
                                                 node_ptr_type const x)
{
 fb_assert(x!=traits.null_ptr());
 fb_assert(x!=m_head);
 fb_assert(x!=m_tail);

#ifdef DEV_BUILD
 this->verify(traits);
#endif

 if(m_tail==traits.null_ptr())
 {
  //empty list
  fb_assert(m_head==traits.null_ptr());

#ifdef DEV_BUILD
  this->verify_size__eq(0);
#endif

  traits.set_prev(x,traits.null_ptr());
  traits.set_next(x,traits.null_ptr());

  m_head =x;
  m_tail =x;

  this->set_size(1);
 }
 else
 {
  //not empty list
  fb_assert(m_head!=traits.null_ptr());
  fb_assert(m_tail!=traits.null_ptr());

#ifdef DEV_BUILD
  this->verify_size__eq_or_large(1);
#endif

  fb_assert(traits.get_prev(m_head)==traits.null_ptr());
  fb_assert(traits.get_next(m_tail)==traits.null_ptr());

  traits.set_next(m_tail,x);

  traits.set_prev(x,m_tail);

  traits.set_next(x,traits.null_ptr());

  m_tail=x;

  this->inc_size();
 }//else

#ifdef DEV_BUILD
 this->verify(traits);
#endif
}//push_back

////////////////////////////////////////////////////////////////////////////////
}//namespace Firebird
#endif
