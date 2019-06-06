#ifndef CLASS_TAMPL_H
#define CLASS_TAMPL_H
//=================================================
#include <new>
#include <new.h>
//=============================================================================
template <class elemType>
class Tlist_item 
{
public:
   // конструктор по умолчанию
   Tlist_item( elemType value );
  
   elemType     value();
   void         value(elemType new_value); 
   
   Tlist_item*  next();
   Tlist_item*  prev();
   
   elemType     *ptr();
   
   void         next(Tlist_item *link );
   void         prev(Tlist_item *link );
   
private:
   elemType _value;
   Tlist_item *_next;
   Tlist_item *_back;
};
//=============================================================================
template <class elemType>
class Tlist 
{
public:
   // конструктор по умолчанию
   Tlist();
   Tlist(const Tlist<elemType> &rhs);
   ~Tlist();
   elemType         operator[]( int ix ) const;
   Tlist<elemType> &operator=( const Tlist<elemType>& ); 

   int          size();
   bool         empty();
   //void         insert(Tlist_item<elemType> *ptr, elemType value);
   void         push_front(elemType value);
   void         push_back(elemType value);
   void         pop_front();
   void         pop_back();
   
   
   bool         is_front(elemType elem);
   bool         is_back(elemType elem); 
   
   elemType*    front();
   elemType*    back();   
   //void         erase( int index );
   
   int          remove( elemType value );   
   void         clear ();
   void         concat( const Tlist<elemType> &i1 );
   void         reverse();
   
   Tlist_item<elemType>*  find(elemType value); 
   
   elemType     next(elemType elem);
   elemType     prev(elemType elem);
   
   elemType     foreach_next();
   void         set_interator(int);
   
   
    
private:  
  //Tlist( const Tlist& );
  void insert_all( const Tlist<elemType> &rhs );

  void bump_up_size();
  void bump_down_size();
  
  int                   interator;
  Tlist_item<elemType>  *_at_front;
  Tlist_item<elemType>  *_at_end;
  
  Tlist_item<elemType>  *_at_interator;
  int                   _size;
};
//=============================================================================

#define foreach(new_var, list)\
  list.set_interator(0);\
        while(new_var = list.foreach_next())

/*
#define foreach(new_var, array)\
        for(size_t index = 0; index < (sizeof(array)/sizeof(*array)); ++index)\
            for(new_var = array[index], flag = 1; flag; flag = 0)
*/
//=============================================================================
// TFifo declaration
//==================================== 
template <class Type>
class TFifo
{ 
public:
  TFifo(unsigned int sz);
  ~TFifo();
  
   Type       pop();
   void       push(const Type &);
   bool       empty() const;
   
   void       clear(void); 
   
  unsigned int        size;   
private: 
  unsigned int        max_size;
  
  Type        *Buf;
  unsigned int        front_ind;
  unsigned int        end_ind;
};
//=============================================================================

#endif 
