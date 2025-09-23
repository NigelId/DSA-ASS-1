#include "VectorStore.h"
#include <algorithm>

int main()
{
   ArrayList<int> A { 100000LL };

   for (ulong i {}; i < 1e5; i++)
   {
      A.add(i);
   }
   for (auto &T : A)
   {
      T++;
   }
   std::cout << A.toString() << "\n";
   return 0;
}
