#include <iostream>

using namespace std;

struct Node
{
	int a;
	int *b;
};

void test(void *node)
{
	int a = ((Node*)node)->a;
	int *b = ((Node*)node)->b;
	cout << a << endl;
	cout << *b << endl;
}

int main()
{
	int b = 3;
	int *a = &b;
	Node node;
	node.a = b;
	node.b = a;
	test(&node);

	return 0;
}
