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
   ArrayList(std::initializer_list<T> init) noexcept(std::is_nothrow_copy_constructible_v<T>)
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

   ArrayList<T> &operator=(std::initializer_list<T> init) noexcept(std::is_nothrow_copy_constructible_v<T>)
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
      os << "[";
      for (int i {}; i < list.size(); i++)
      {
         if (i)
         {
            os << ", ";
         }
         os << list.data[i];
      }
      os << "]";
      return os;
   }

 public:
   [[nodiscard]] friend bool operator==(const ArrayList<T> &lhs, const ArrayList<T> &rhs) noexcept
   {
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
   [[nodiscard]] inline bool empty() const noexcept { return count == 0; };
   [[nodiscard]] inline int size() const noexcept { return count; };
   [[nodiscard]] int indexOf(T item) const;
   [[nodiscard]] bool contains(T item) const;

   string toString(string (*item2str)(T &) = 0) const;
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
      friend class TestHelper;
#endif
    private:
      int cursor;
      ArrayList<T> *pList;

    public:
      Iterator(ArrayList<T> *pList = nullptr, int index = 0);
      Iterator &operator=(const Iterator &other); // Deep Copy
      T &operator*();
      bool operator!=(const Iterator &other) const;
      Iterator &operator++();
      Iterator operator++(int);
      Iterator &operator--();
      Iterator operator--(int);
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

   static Node dummy;

 public:
   class Iterator;
   friend class Iterator;

   SinglyLinkedList();
   ~SinglyLinkedList();

   void add(T e);
   void add(int index, T e);
   T removeAt(int index);
   bool removeItem(T item);
   bool empty() const;
   int size() const;
   void clear();
   T &get(int index);
   int indexOf(T item) const;
   bool contains(T item) const;
   string toString(string (*item2str)(T &) = 0) const;

   Iterator begin();
   Iterator end();

   // Inner class Iterator
   class Iterator
   {
#ifdef TESTING
      friend class TestHelper;
#endif
    private:
      Node *current;

    public:
      Iterator(Node *node = nullptr);
      Iterator &operator=(const Iterator &other); // Deep Copy
      T &operator*();
      bool operator!=(const Iterator &other) const;
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
