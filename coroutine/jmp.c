#include <stdio.h>  
#include <setjmp.h>  
   

   
void second(jmp_buf buf) {
	printf("second\n");         // 打印  
	longjmp(buf,1);             // 跳回setjmp的调用处 - 使得setjmp返回值为1  
}
   
void first(jmp_buf buf) {
	second(buf); 
	printf("first\n");          // 不可能执行到此行  
}
   
int main() {
	jmp_buf buf;
	if ( ! setjmp(buf) ) {
		first(buf);                // 进入此行前，setjmp返回0  
	} else {                    // 当longjmp跳转回，setjmp返回1，因此进入此行  
		printf("main\n");       // 打印  
	}

	return 0;
}  
