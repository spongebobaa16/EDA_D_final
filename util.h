#include <random>
#include <cassert>
#include <ctime>
using namespace std;

int myRand(int n) // generate [0, n] random integer
{
    /* 產生 [0, n) 均勻分布的整數亂數 */
    srand(time(NULL));
    if ((n - 1) == RAND_MAX)
    {
        return rand();
    }
    else
    {
        /* 計算可以被整除的長度 */
        long end = RAND_MAX / n;
        assert(end > 0L);
        end *= n;

        /* 將尾端會造成偏差的幾個亂數去除，
           若產生的亂數超過 limit，則將其捨去 */
        int r;
        while ((r = rand()) >= end)
            ;

        return r % n;
    }
}