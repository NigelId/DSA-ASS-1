#include "VectorStore.h"
#include <utility>
// ----------------- ArrayList Implementation -----------------
// nested array and linkedlist for debugging and checking
#define INSTANTIATE_ARRAYLIST_NESTED(T)                                                                      \
   template class ArrayList<ArrayList<T>>;                                                                   \
   template class ArrayList<ArrayList<ArrayList<T>>>;                                                        \
   template class ArrayList<ArrayList<ArrayList<ArrayList<T>>>>;

template <class T>
ArrayList<T>::ArrayList(int initCapacity)
    : capacity { initCapacity }, count {}, data { (T *)::operator new(initCapacity * sizeof(T)) }
{
}

template <class T>
ArrayList<T>::ArrayList(const ArrayList<T> &other) noexcept(std::is_nothrow_copy_constructible_v<T>)
    : count { other.size() }, capacity { other.capacity }, data((T *)::operator new(other.size() * sizeof(T)))
{
   for (int i = 0; i < count; i++)
   {
      new (&this->data[i]) T { other.data[i] };
   }
}

template <class T>
ArrayList<T> &ArrayList<T>::operator=(const ArrayList<T> &other)

    noexcept(std::is_nothrow_copy_constructible_v<T> && std::is_nothrow_copy_assignable_v<T>)
{
   if (this != &other)
   {
      for (int i = 0; i < count; i++)
      {
         this->data[i].~T();
      }
      ::operator delete(data, count * sizeof(T));

      this->count = other.count;
      this->capacity = other.capacity;
      this->data = (T *)::operator new(capacity * sizeof(T));

      for (int i {}; i < this->count; i++)
      {
         new (&this->data[i]) T { other.data[i] };
      }
   }
   return *this;
}

template <typename T> void ArrayList<T>::clear() noexcept(std::is_nothrow_destructible_v<T>)
{
   if (data != nullptr)
   {
      for (int i {}; i < count; i++)
      {
         this->data[i].~T();
      }

      ::operator delete(data, count * sizeof(T));

      this->count = 0;
      this->capacity = 10;
      this->data = (T *)::operator new(capacity * sizeof(T));
   }
}

template <typename T> ArrayList<T>::~ArrayList() noexcept
{
   for (int i {}; i < count; i++)
   {
      this->data[i].~T();
   }

   ::operator delete(data, count * sizeof(T));
}

template <typename T> void ArrayList<T>::ensureCapacity(int cap)
{
   if (cap >= this->capacity)
   {
      this->capacity *= 1.5;

      T *new_data { (T *)::operator new(capacity * sizeof(T)) };

      for (int i {}; i < this->count; i++)
      {
         new (&new_data[i]) T { std::move(data[i]) };
         this->data[i].~T();
      }
      ::operator delete(this->data, capacity * sizeof(T));
      this->data = new_data;
   }
}

template <typename T> void ArrayList<T>::add(T e)
{
   ensureCapacity(this->count + 1);
   return add(this->count, e);
}

template <typename T> void ArrayList<T>::add(int index, T e)
{
   if (index > count || index < 0)
      throw std::out_of_range("Index is invalid!");

   ensureCapacity(count + 1);

   std::move_backward(&data[index], &data[count], &data[count + 1]);
   new (&data[index]) T(std::move(e));

   this->count++;
}

template <typename T> T ArrayList<T>::removeAt(int index)
{
   if (index >= count || index < 0)
   {
      throw std::out_of_range("Index is invalid!");
   }

   T tmp { std::move(this->data[index]) };

   if (index < count - 1)
   {
      std::move(&data[index + 1], &data[count], &data[index]);
   }

   this->data[count - 1].~T();

   count--;

   return tmp;
}

template <typename T> T &ArrayList<T>::get(int index)
{
   if (index < 0 || index >= count)
   {
      throw std::out_of_range("Index is invalid!");
   }
   return this->data[index];
}

template <typename T> void ArrayList<T>::set(int index, T e)
{
   if (index < 0 || index >= count)
   {
      throw std::out_of_range("Index is invalid!");
   }
   this->data[index] = std::move(e);
}

template <typename T> int ArrayList<T>::indexOf(T e) const
{
   for (int i {}; i < this->count; i++)
   {
      if (this->data[i] == e)
      {
         return i;
      }
   }
   return -1;
}

template <typename T> std::string ArrayList<T>::toString(std::string (*item2str)(T &)) const
{
   std::ostringstream oss; // support only output,  much much much much less heavier than ss
   oss << "[";

   for (int i {}; i < count; i++)
   {
      if (i)
      {
         oss << ", ";
      }
      item2str ? (oss << item2str(data[i])) : (oss << data[i]);
   }

   oss << "]";
   return oss.str();
}

template <typename T> bool ArrayList<T>::contains(T e) const { return (indexOf(e) != -1); }

// ----------------- Iterator of ArrayList Implementation -----------------
template <class T>
ArrayList<T>::Iterator::Iterator(ArrayList<T> *pList, int index) : pList { pList }, cursor { index }
{
   if (cursor < 0 || cursor > this->pList->count)
   {
      throw std::out_of_range("Iterator cannot advance past end!");
   }
}

template <class T> typename ArrayList<T>::Iterator &ArrayList<T>::Iterator::operator=(const Iterator &other)
{
   if (this != &other)
   {
      this->cursor = other.cursor;
      this->pList = other.pList;
   }
   return *this;
}

template <class T> T &ArrayList<T>::Iterator::operator*()
{
   if (cursor >= this->pList->count)
   {
      throw std::out_of_range("Iterator is out of range");
   }
   return this->pList->data[cursor];
}

template <class T> bool ArrayList<T>::Iterator::operator!=(const Iterator &other) const
{
   return this->cursor != other.cursor || this->pList != other.pList;
}

template <class T> typename ArrayList<T>::Iterator &ArrayList<T>::Iterator::operator++()
{
   if (cursor == pList->count)
   {
      throw std::out_of_range("Iterator cannot advance past end!");
   }
   ++cursor;
   return *this;
}

template <class T> typename ArrayList<T>::Iterator ArrayList<T>::Iterator::operator++(int)
{
   if (cursor == pList->count)
   {
      throw std::out_of_range("Iterator cannot advance past end!");
   }
   Iterator temp = *this;
   ++cursor;
   return temp;
}

template <class T> typename ArrayList<T>::Iterator &ArrayList<T>::Iterator::operator--()
{
   if (cursor == 0)
   {
      throw std::out_of_range("Iterator cannot move before begin!");
   }
   --cursor;
   return *this;
}

template <class T> typename ArrayList<T>::Iterator ArrayList<T>::Iterator::operator--(int)
{
   if (cursor == 0)
   {
      throw std::out_of_range("Iterator cannot move before begin!");
   }
   Iterator temp = *this;
   --cursor;
   return temp;
}

template <class T> typename ArrayList<T>::Iterator ArrayList<T>::begin() noexcept
{
   return Iterator(this, 0);
}

template <class T> typename ArrayList<T>::Iterator ArrayList<T>::end() noexcept
{
   return Iterator(this, count);
}

// TODO: implement other methods of ArrayList::Iterator

// ----------------- SinglyLinkedList Implementation -----------------

template <typename T> SinglyLinkedList<T>::SinglyLinkedList() : head { nullptr }, tail { nullptr }, count {}
{
}

template <typename T> SinglyLinkedList<T>::~SinglyLinkedList() { clear(); }

template <typename T> typename SinglyLinkedList<T>::Node SinglyLinkedList<T>::dummy {};

template <typename T> void SinglyLinkedList<T>::add(int index, T e)
{
   if (index < 0 || index > count)
   {
      throw std::out_of_range("Index is invalid!");
   }

   Node *prev { &this->dummy };
   prev->next = head;

   for (int i = -1; i < index - 1; ++i)
   {
      prev = prev->next;
   }

   prev->next = new Node { e, prev->next };

   if (index == count)
   {
      tail = prev->next;
   }

   this->head = dummy.next;
   this->count++;
}

template <typename T> void SinglyLinkedList<T>::add(T e) { add(count, e); }

template <typename T> T SinglyLinkedList<T>::removeAt(int index)
{
   if (index < 0 || index >= count)
   {
      throw std::out_of_range("Index is invalid!");
   }

   Node *prev { &this->dummy };
   prev->next = this->head;

   for (int i = -1; i < index - 1; ++i)
   {
      prev = prev->next;
   }

   Node *Deleted = prev->next;
   prev->next = Deleted->next;

   if (Deleted == this->tail)
   {
      this->tail = prev;
   }

   T data = Deleted->data;
   delete Deleted;

   this->head = this->dummy.next;
   this->count--;

   return data;
}

template <typename T> bool SinglyLinkedList<T>::removeItem(T e)
{
   Node *current { this->head };

   for (int i {}; i < this->count; i++)
   {
      if (current->data == e)
      {
         removeAt(i);
         return true;
      }
      current = current->next;
   }

   return false;
}

template <typename T> bool SinglyLinkedList<T>::empty() const { return this->count == 0; }

template <typename T> bool SinglyLinkedList<T>::contains(T e) const
{
   Node *current { this->head };

   for (int i {}; i < this->count; i++)
   {
      if (current->data == e)
      {
         return true;
      }
      current = current->next;
   }
   return false;
}

template <typename T> int SinglyLinkedList<T>::size() const { return this->count; }

template <typename T> T &SinglyLinkedList<T>::get(int index)
{
   if (index < 0 || index >= count)
   {
      throw std::out_of_range("Index is invalid!");
   }

   Node *current { this->head };

   for (int i {}; i < (index); i++)
   {
      current = current->next;
   }

   return current->data;
}

template <typename T> void SinglyLinkedList<T>::clear()
{
   Node *current = head;
   while (current != nullptr)
   {
      Node *next = current->next;
      delete current;
      current = next;
   }
   head = nullptr;
   tail = nullptr;
   count = 0;
}

template <typename T> string SinglyLinkedList<T>::toString(string (*item2str)(T &)) const
{
   stringstream ss;
   Node *current = head;

   while (current != nullptr)
   {
      ss << "[";
      if (item2str)
      {
         ss << item2str(current->data);
      }

      else
      {
         ss << current->data;
      }
      ss << "]";
      if (current->next != nullptr)
      {
         ss << "->";
      }
      current = current->next;
   }

   return ss.str();
}

template <typename T> SinglyLinkedList<T>::Iterator::Iterator(Node *node) : current { node } {}

template <typename T>
typename SinglyLinkedList<T>::Iterator &SinglyLinkedList<T>::Iterator::operator=(const Iterator &other)
{
   if (this != &other)
   {
      this->current = other.current;
   }
   return *this;
}

template <typename T> T &SinglyLinkedList<T>::Iterator::operator*()
{
   if (current == nullptr)
   {
      throw std::out_of_range("Iterator is out of range!");
   }
   return current->data;
}

template <typename T> bool SinglyLinkedList<T>::Iterator::operator!=(const Iterator &other) const
{
   return current != other.current;
}

template <typename T> typename SinglyLinkedList<T>::Iterator &SinglyLinkedList<T>::Iterator::operator++()
{
   if (current == nullptr)
   {
      throw std::out_of_range("Iterator cannot advance past end!");
   }
   current = current->next;
   return *this;
}

template <typename T> typename SinglyLinkedList<T>::Iterator SinglyLinkedList<T>::Iterator::operator++(int)
{
   Iterator temp = *this;
   ++(*this);
   return temp;
}

template <typename T> typename SinglyLinkedList<T>::Iterator SinglyLinkedList<T>::begin()
{
   return Iterator { head };
}
template <typename T> typename SinglyLinkedList<T>::Iterator SinglyLinkedList<T>::end()
{
   return Iterator { nullptr };
}

// ----------------- VectorStore Implementation -----------------

VectorStore::VectorStore(int dimension, EmbedFn embeddingFunction)
{
   // TODO
}

VectorStore::~VectorStore()
{
   // TODO
}

// TODO: implement other methods of VectorStore

// Explicit template instantiation for char, string, int, double, float, and
// Point

template class ArrayList<char>;
template class ArrayList<string>;
template class ArrayList<int>;
template class ArrayList<double>;
template class ArrayList<float>;
template class ArrayList<Point>;

INSTANTIATE_ARRAYLIST_NESTED(char)
INSTANTIATE_ARRAYLIST_NESTED(string)
INSTANTIATE_ARRAYLIST_NESTED(int)
INSTANTIATE_ARRAYLIST_NESTED(double)
INSTANTIATE_ARRAYLIST_NESTED(float)

template class SinglyLinkedList<char>;
template class SinglyLinkedList<string>;
template class SinglyLinkedList<int>;
template class SinglyLinkedList<double>;
template class SinglyLinkedList<float>;
template class SinglyLinkedList<Point>;
