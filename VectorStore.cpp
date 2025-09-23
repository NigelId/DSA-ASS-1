#include "VectorStore.h"
#include <string>

// ----------------- ArrayList Implementation -----------------

template <class T>
ArrayList<T>::ArrayList(int initCapacity)
    : capacity { initCapacity }, count {}, data { (T *)::operator new(initCapacity * sizeof(T)) }
{
}

template <class T>
ArrayList<T>::ArrayList(const ArrayList<T> &other)
    : count { other.size() }, capacity { other.capacity }, data((T *)::operator new(other.size() * sizeof(T)))
{
   for (int i = 0; i < count; i++)
   {
      new (&this->data[i]) T { other.data[i] };
   }
}

template <class T> ArrayList<T> &ArrayList<T>::operator=(const ArrayList<T> &other)
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

      for (int i = 0; i < count; i++)
      {
         new (&this->data[i]) T { other.data[i] };
      }
   }
   return *this;
}

template <typename T> void ArrayList<T>::clear()
{
   if (data != nullptr)
   {
      for (size_t i {}; i < count; i++)
      {
         this->data[i].~T();
      }

      ::operator delete(data, count * sizeof(T));

      this->count = 0;
      this->capacity = 10;
      this->data = (T *)::operator new(capacity * sizeof(T));
   }
}

template <typename T> ArrayList<T>::~ArrayList()
{
   for (size_t i {}; i < count; i++)
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

      for (size_t i {}; i < this->count; i++)
      {
         new (&new_data[i]) T { std::move(data[i]) };
         this->data[i].~T();
      }
      ::operator delete(this->data, count * sizeof(T));
      this->data = new_data;
   }
}

template <typename T> void ArrayList<T>::add(T e)
{
   ensureCapacity(this->count);
   this->data[count++] = e;
}

template <typename T> void ArrayList<T>::add(int index, T e)
{
   if (index > count || index < 0)
   {
      throw std::out_of_range("Index is invalid!");
   }

   ensureCapacity(this->count + 1);

   new (&data[count]) T {};

   std::move_backward(&data[index], &data[count], &data[count + 1]);

   this->data[index] = std::move(e);

   this->count++;
}

template <typename T> T ArrayList<T>::removeAt(int index)
{
   if (index >= count || index < 0)
   {
      throw std::out_of_range("Index is invalid!");
   }
   T tmp { this->data[index] };

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
   this->data[index] = e;
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
   std::stringstream ss;
   ss << "[";
   ss << this->data[0];

   for (int i = 1; i < count; i++)
   {
      ss << ", ";
      item2str ? (ss << item2str(data[i])) : (ss << data[i]);
   }

   ss << "]";
   return ss.str();
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

template <class T> typename ArrayList<T>::Iterator ArrayList<T>::begin() { return Iterator(this, 0); }

template <class T> typename ArrayList<T>::Iterator ArrayList<T>::end() { return Iterator(this, count); }

// TODO: implement other methods of ArrayList::Iterator

// ----------------- SinglyLinkedList Implementation -----------------

template <typename T> SinglyLinkedList<T>::SinglyLinkedList() : head { nullptr }, tail { nullptr }, count {}
{
}

template <typename T> SinglyLinkedList<T>::~SinglyLinkedList() { clear(); }

template <typename T> void SinglyLinkedList<T>::add(int index, T e)
{
   if (index < 0 || index > count)
   {
      throw std::out_of_range("Index is invalid!");
   }

   Node dummy {};
   Node *prev { &dummy };

   prev->next = this->head;

   for (int i = -1; i < index - 1; ++i)
   {
      prev = prev->next;
   }

   prev->next = new Node { e, prev->next };

   if (index == count)
   {
      tail = prev->next;
   }

   head = dummy.next;
   ++count;
}

template <typename T> void SinglyLinkedList<T>::add(T e) { add(count, e); }

template <typename T> T SinglyLinkedList<T>::removeAt(int index)
{
   if (index < 0 || index >= count)
   {
      throw std::out_of_range("Index is invalid!");
   }

   Node dummy {};
   Node *prev { &dummy };

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

   this->head = dummy.next;
   count--;

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

template class SinglyLinkedList<char>;
template class SinglyLinkedList<string>;
template class SinglyLinkedList<int>;
template class SinglyLinkedList<double>;
template class SinglyLinkedList<float>;
template class SinglyLinkedList<Point>;
