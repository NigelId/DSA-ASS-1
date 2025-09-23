#include <algorithm>
#include <cstddef>
#include <stdexcept>

template <typename T> class ArrayList
{
 protected:
   T *data;
   int capacity;
   int count;

 public:
   ArrayList()
   {
      capacity = 10;
      count = 0;
      data = (T *)::operator new(capacity * sizeof(T));
   }
   ~ArrayList()
   {
      for (size_t i {}; i < count; i++)
      {
         data[i].~T();
      }
      ::operator delete(data, count * sizeof(T));
   }
   void add(T e);
   int size() const;
   void add(int index, T e);
   void ensureCapacity(int index);
   T removeAt(int index);
};

template <typename T> void ArrayList<T>::ensureCapacity(int cap)
{
   if (cap >= this->capacity)
   {
      capacity = capacity * 1.5;

      T *new_data { (T *)::operator new(capacity * sizeof(T)) };

      for (size_t i {}; i < this->count; i++)
      {
         new (&new_data[i]) T { std::move(data[i]) };
         data[i].~T();
      }
      ::operator delete(data, count * sizeof(T));
      data = new_data;
   }
}

template <typename T> void ArrayList<T>::add(T e)
{
   ensureCapacity(count);
   data[count++] = e;
}

template <typename T> void ArrayList<T>::add(int index, T e)
{
   if (index > count || index < 0)
   {
      throw std::out_of_range("the input index is out of range!");
   }

   ensureCapacity(count + 1);

   new (&data[count]) T {};

   std::move_backward(&data[index], &data[count], &data[count + 1]);

   data[index] = std::move(e);

   count++;
}

template <typename T> int ArrayList<T>::size() const { return count; }

template <class T> T ArrayList<T>::removeAt(int index)
{
   if (index >= count || index < 0)
   {
      throw std::out_of_range("the input index is out of range!");
   }
   T tmp { data[index] };

   if (index < count - 1)
   {
      std::move(&data[index + 1], &data[count], &data[index]);
   }

   data[count - 1].~T();

   count--;

   return tmp;
}
