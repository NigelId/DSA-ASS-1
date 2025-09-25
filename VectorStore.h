#ifndef VECTORSTORE_H
#define VECTORSTORE_H

#include "main.h"

// ==============================
// Class ArrayList
// ==============================

template <class T> class ArrayList
{
#ifdef TESTING
   friend class TestHelper;
#endif
 private:
   T *data;
   int capacity;
   int count;

   void ensureCapacity(int cap);

 public:
   class Iterator;
   friend class Iterator;

 public:
   explicit ArrayList(int initCapacity = 10);
   ArrayList(const ArrayList<T> &other) noexcept(std::is_nothrow_copy_constructible_v<T>);
   ~ArrayList() noexcept;

 public:
   ArrayList<T> &operator=(const ArrayList<T> &other)

       noexcept(std::is_nothrow_copy_constructible_v<T> && std::is_nothrow_copy_assignable_v<T>);

 public:
   // bad habit, this could serve as watermark tho
   ArrayList(const std::initializer_list<T> &init) noexcept(std::is_nothrow_copy_constructible_v<T>)

       : count { static_cast<int>(init.size()) }
   {
      this->capacity = ((this->count + 1) * 1.5);
      this->data = (T *)::operator new(this->capacity * sizeof(T));

      T *p { this->data };
      for (const T &v : init)
      {
         new (p++) T { v };
      }
   }

   ArrayList<T> &operator=(const std::initializer_list<T> &init)

       noexcept(std::is_nothrow_copy_constructible_v<T>)
   {
      for (int i = 0; i < count; ++i)
      {
         data[i].~T();
      }
      ::operator delete(data, capacity * sizeof(T));

      this->count = static_cast<int>(init.size());
      this->capacity = (count + 1) * 1.5;
      this->data = (T *)::operator new(capacity * sizeof(T));

      T *p { this->data };
      for (const T &v : init)
      {
         new (p++) T { v };
      }

      return *this;
   }

 public:
   // quality of life stuff
   friend std::ostream &operator<<(std::ostream &os, const ArrayList<T> &list) noexcept
   {
      os << list.toString();
      return os;
   }

 public:
   [[nodiscard]] friend bool operator==(const ArrayList<T> &lhs, const ArrayList<T> &rhs) noexcept
   {
      if (&rhs == &lhs)
      {
         return true;
      }
      if (lhs.count != rhs.count)
      {
         return false;
      }
      for (int i {}; i < lhs.count; i++)
      {
         if (!(lhs.data[i] == rhs.data[i]))
         {
            return false;
         }
      }
      return true;
   }

   [[nodiscard]] friend bool operator!=(const ArrayList<T> &lhs, const ArrayList<T> &rhs) noexcept
   {
      return !(lhs == rhs);
   }

 public:
   void add(T e);
   void add(int index, T e);
   T removeAt(int index);

 public:
   // most definitely wants these to not be discarded
   [[nodiscard]] inline constexpr bool empty() const noexcept { return count == 0; };
   [[nodiscard]] inline constexpr int size() const noexcept { return count; };
   [[nodiscard]] int indexOf(T item) const;
   [[nodiscard]] bool contains(T item) const;

 public:
   string toString(string (*item2str)(T &) = 0) const;

 public:
   T &get(int index);
   void clear() noexcept(std::is_nothrow_destructible_v<T>);
   void set(int index, T e);

 public:
   Iterator begin() noexcept;
   Iterator end() noexcept;

   // Inner class Iterator
   class Iterator
   {
#ifdef TESTING
      noexcept friend class TestHelper;
#endif
    private:
      int cursor;
      ArrayList<T> *pList;

    public:
      Iterator(ArrayList<T> *pList = nullptr, int index = 0);
      Iterator &operator=(const Iterator &other) noexcept; // Deep Copy

      Iterator &operator++();
      Iterator operator++(int);
      Iterator &operator--();
      Iterator operator--(int);

      [[nodiscard]] T &operator*();
      [[nodiscard]] bool operator!=(const Iterator &other) const noexcept;
   };
};

// =====================================
// Class SinglyLinkedList
// =====================================
template <class T> class SinglyLinkedList
{
#ifdef TESTING
   friend class TestHelper;
#endif
 private:
   class Node
   {
    public:
      T data;
      Node *next;

      Node() : data(), next(nullptr) {}
      Node(const T &data, Node *next = nullptr) : data(data), next(next) {}
   };

   Node *head;
   Node *tail;
   int count;

 public:
   class Iterator;
   friend class Iterator;

   using SList = SinglyLinkedList;

 public:
   SinglyLinkedList() noexcept;
   ~SinglyLinkedList() noexcept;

   SinglyLinkedList(const SinglyLinkedList<T> &other)

       noexcept(std::is_nothrow_copy_constructible_v<T> && std::is_nothrow_copy_assignable_v<T>)
       : head { nullptr }, tail { nullptr }, count {}
   {
      Node *current { other.head };
      while (current)
      {
         add(current->data);
         current = current->next;
      }
   }

 public:
   SinglyLinkedList<T>(const std::initializer_list<T> &init) noexcept(std::is_nothrow_copy_constructible_v<T>)

       : head { nullptr }, tail { nullptr }, count {}
   {
      for (const T &v : init)
      {
         add(v);
      }
   }

   SinglyLinkedList<T> &operator=(const std::initializer_list<T> &init)

       noexcept(std::is_nothrow_copy_constructible_v<T>)
   {
      clear();

      for (const T &v : init)
      {
         add(v);
      }

      return *this;
   }

   [[nodiscard]] friend bool operator==(const SinglyLinkedList<T> &lhs, const SinglyLinkedList<T> &rhs)

       noexcept
   {
      if (&lhs == &rhs)
      {
         return true;
      }

      if (lhs.count != rhs.count)
      {
         return false;
      }

      Node *n1 { lhs.head };
      Node *n2 { rhs.head };

      while (n1 != nullptr && n2 != nullptr)
      {
         if (!(n1->data == n2->data))
         {
            return false;
         }

         n1 = n1->next;
         n2 = n2->next;
      }

      return true;
   }

   [[nodiscard]] friend bool operator!=(const SinglyLinkedList<T> &lhs, const SinglyLinkedList<T> &rhs)

       noexcept
   {
      return !(lhs == rhs);
   }

 public:
   friend std::ostream &operator<<(std::ostream &os, const SinglyLinkedList<T> &list) noexcept
   {
      os << list.toString();
      return os;
   }

 public:
   [[nodiscard]] constexpr inline bool empty() const noexcept { return this->count == 0; }
   [[nodiscard]] constexpr inline int size() const noexcept { return this->count; }
   [[nodiscard]] int indexOf(T item) const;
   [[nodiscard]] bool contains(T item) const;

 public:
   void add(T e);
   void add(int index, T e);

 public:
   T removeAt(int index);
   T pop();
   bool removeItem(T item);
   void clear() noexcept(std::is_nothrow_destructible_v<T>);

 public:
   T &get(int index);

 public:
   string toString(string (*item2str)(T &) = 0) const;

 public:
   Iterator begin() noexcept;
   Iterator end() noexcept;

   // Inner class Iterator
   class Iterator
   {
#ifdef TESTING
      friend class TestHelper;
#endif
    private:
      Node *current;

    public:
      Iterator(Node *node = nullptr) noexcept;
      Iterator &operator=(const Iterator &other) noexcept; // Deep Copy

      [[nodiscard]] T &operator*();
      [[nodiscard]] bool operator!=(const Iterator &other) const noexcept;

      Iterator &operator++();
      Iterator operator++(int);
   };
};

// =====================================
// Class VectorStore
// =====================================

class VectorStore
{
#ifdef TESTING
   friend class TestHelper;
#endif
 public:
   struct VectorRecord
   {
      int id;
      string rawText;
      int rawLength;
      SinglyLinkedList<float> *vector;

      VectorRecord(int id, const string &rawText, SinglyLinkedList<float> *vector);
   };

   using EmbedFn = SinglyLinkedList<float> *(*)(const string &);

 private:
   ArrayList<VectorRecord *> records;
   int dimension;
   int count;
   EmbedFn embeddingFunction;

 public:
   VectorStore(int dimension = 512, EmbedFn embeddingFunction = nullptr);
   ~VectorStore();
   int size() const;
   bool empty() const;
   void clear();

   SinglyLinkedList<float> *preprocessing(string rawText);

   void addText(string rawText);
   SinglyLinkedList<float> &getVector(int index);
   string getRawText(int index) const;
   int getId(int index) const;
   bool removeAt(int index);
   bool updateText(int index, string newRawText);
   void setEmbeddingFunction(EmbedFn newEmbeddingFunction);

   void forEach(void (*action)(SinglyLinkedList<float> &, int, string &));

   double cosineSimilarity(const SinglyLinkedList<float> &v1, const SinglyLinkedList<float> &v2) const;
   double l1Distance(const SinglyLinkedList<float> &v1, const SinglyLinkedList<float> &v2) const;
   double l2Distance(const SinglyLinkedList<float> &v1, const SinglyLinkedList<float> &v2) const;

   int findNearest(const SinglyLinkedList<float> &query, const string &metric = "cosine") const;

   int *topKNearest(const SinglyLinkedList<float> &query, int k, const string &metric = "cosine") const;
};

#endif // VECTORSTORE_H
