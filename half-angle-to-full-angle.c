#include<stdio.h>
#include<string.h>
#include <stdlib.h>

char* halfAngleToFullAngleOneChar(char half) {
	char *fullCharTableN = "０１２３４５６７８９";
	char *fullCharTablel = "ａｂｃｄｅｆｇｈｉｊｋｌｍｎｏｐｑｒｓｔｕｖｗｘｙｚ";
	char *fullCharTableL = "ＡＢＣＤＥＦＧＨＩＪＫＬＭＮＯＰＱＲＳＴＵＶＷＸＹＺ";
	char *full = (char *)malloc(sizeof(char)*3);
	
	// other case
	char *halfp = &half;
	strcpy(full, halfp);
	
	// number
	if(half >= 0x30 && half <= 0x39) {
		int index = half - 0x30;
		strncpy(full, fullCharTableN + index * 3, 3);
	// uppercase letter
	}else if(half >= 0x61 && half <= 0x7a) {
		int index = half - 0x61;
		strncpy(full, fullCharTablel + index * 3, 3);
	// lower case letters
	}else if(half >= 0x41 && half <= 0x5a) {
		int index = half - 0x41;
		strncpy(full, fullCharTableL + index * 3, 3);
	}
	return full;
}

void halfAngleToFullAngle(char *half, char *full) {
	int index = 0;
	char * ch;
	int len;
	for(int i = 0; half[i] != '\0'; i++) {
		ch = halfAngleToFullAngleOneChar(half[i]);
		len = strlen(ch);
		strncpy(full + index, ch, len);
		index += len;
	}
}

char * halfAngleToFullAngleF(char *half) {
	char *result = (char *)malloc(sizeof(char)*180);
	halfAngleToFullAngle(half, result);
	return result;
}

int main(int argc, char* argv[]) {
	// char p[] = {0xb0, 0xa3};
	// printf("%s\n", p);
	
	// char a[]="a";
	// printf("half: %x\n", a[0]);
	// char b[]="ａ";
	// printf("full: %x, %x, %x\n", b[0], b[1], b[2]);
	// printf("%s\n", b);
	
	// char *c = "０１２３４５６７８９";
	// for(int i=0;c[i]!='\0';i++) {
	// 	printf("%x\n", c[i]);
	// }
	// char *d = (char *)malloc(sizeof(char)*3);
	// strncpy(d, c, 3);
	// printf("%s\n", d);
	
	char *e = "1234567890abcdefghjklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZＸＹＺ";
	char *f = (char *)malloc(sizeof(char)*100);
	halfAngleToFullAngle(e, f);
	printf("%s\n", f);
	
	printf("%s\n", halfAngleToFullAngleF("45487ffhfjkghdfhkuiut"));
	return 0;
}
