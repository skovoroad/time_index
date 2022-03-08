#pragma once

#include <assert.h>
#include <memory>
#include <unordered_map>

namespace Aux {


  template<typename T, typename Key, typename Timestamp> 
  struct TimedItem {
    using Ptr = std::shared_ptr<TimedItem<T, Key, Timestamp>>;
    Ptr prev;
    Ptr next;
    Key key;

    virtual ~TimedItem() {};

    virtual bool Expired(const Timestamp& t) const = 0;

    T& value() { 
      return static_cast<T&>(*this);
    }
  };


  template<typename T, typename Key, typename Timestamp>
  // TODO: set buckets_count ?
  class TimeIndex 
  {
      typename TimedItem<T, Key, Timestamp>::Ptr first;
      typename TimedItem<T, Key, Timestamp>::Ptr last;
      std::unordered_map<Key, typename TimedItem<T, Key, Timestamp>::Ptr> storage_;

   public:
      void CheckExpire(const Timestamp& t) {
        if(Empty())
          return;

        auto it = first;
        while(it != last) {
          if(!it->Expired(t)) 
            return;
        
          first = it->next;
          first->prev = nullptr;
          it->next = nullptr;
          storage_.erase(it->key);
          it = first;  
        }

        if(it->Expired(t)) {
          first = nullptr;
          last = nullptr;
        }
      }

      typename TimedItem<T, Key, Timestamp>::Ptr Find(const Key &k)  {
        auto retval = storage_.find(k);
        if(retval == storage_.end())
          return typename TimedItem<T, Key, Timestamp>::Ptr();

        return retval->second;  
      }

      void Insert(const Key& k, typename TimedItem<T, Key, Timestamp>::Ptr p) {
        storage_[k] = p;
        p->key = k;
        Update(p);
      }

      void Update(typename TimedItem<T, Key, Timestamp>::Ptr p) {
        if(Empty()) {
          first = p;
          last = p;
          return;
        }

        last->next = p;
        p->prev = last;
        p->next = nullptr;
        last = p;
      }

      void Remove(typename TimedItem<T, Key, Timestamp>::Ptr p) {
        storage_.erase(p->key);

        if(p == first) {
          if(first->next == nullptr) {
            assert(first == last); // (*)
            first = last = nullptr;
            return;
          }
          first = first->next; 
          first->prev = nullptr;
          return;
        }

        if(p == last) {
          assert(last->prev != nullptr); // remove last item (*) handled above
          last = last->prev;
          last->next = nullptr;
          return;
        }
       
        p->prev->next = p->next; // nullptr?
        p->next->prev = p->prev; // nullptr?
      }

      bool Empty() {
        assert( (first == nullptr && last == nullptr) ||
                (first != nullptr && last != nullptr));
        return first == nullptr;
      }
 
  };

}


