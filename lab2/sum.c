#include <stdio.h>

int main(){
	int sum=0;
	
	for(int i=0; i<=500; i++){
		sum+=i;
	}

	printf("The sum of 1 to 500 is %d\n", sum);

	return 0;
}
