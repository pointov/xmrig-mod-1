#include <Windows.h>
#include <stdio.h>
#include <fstream>

int main()
{
	int shift;
	char pool[128] = "";
	char wallet[256] = "";

	printf("Shift: ");
	scanf("%d", &shift);

	printf("Pool: ");
	scanf("%s", &pool);

	printf("Wallet: ");
	scanf("%s", &wallet);

	for (int i = 0; i < strlen(pool); i++)
		pool[i] += shift;

	for (int i = 0; i < strlen(wallet); i++)
		wallet[i] += shift;

	std::ofstream myfile;
	myfile.open("encoded.txt");
	myfile << "Pool: " << pool << "\n";
	myfile << "Wallet: " << wallet << "";
	myfile.close();

	system("pause");
}
