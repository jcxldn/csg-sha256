#include "SHA256.h"
#include <iostream>
using namespace std;
int main()
{
    SHA256 sha;
    sha.update("This is IN2029 formative task");
    cout << SHA256::toString(sha.digest()) << endl;
    return 0;
}