#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "lcd_lib.h"

void clean_stdin(void)
{
    int c;

    printf("clean stdin ");

    do {
        c = getchar();
    } while (c != '\n' && c != EOF);

    printf("done\n");
}

int main()
{
	char c;
    char user_string[100];

	while(1) {
		printf("0: Exit\n");
		printf("1: Trong Dung\n");
		printf("2: Dungnt98 Linux\n");
		printf("3; Linux Embedded\n");
        printf("4. User typing content\n");

        memset(user_string, 0, sizeof(user_string));
		
        scanf("%d", &c);
        clean_stdin();

		switch(c) {
			case 0:
				return 0;
			case 1:
				lcd_gotoxy(0, 0);
				lcd_clear_screen();
				draw_string("Trong Dung", Pixel_Set, FontSize_5x7);
				break;
			case 2:
				lcd_gotoxy(0, 10);
				lcd_clear_screen();
				draw_string("Dungnt98 Linux", Pixel_Set, FontSize_5x7);
				break;
			case 3:
				lcd_gotoxy(0, 20);
				lcd_clear_screen();
				draw_string("Linux Embedded", Pixel_Set, FontSize_5x7);
				break;
            case 4:
                fgets(user_string, sizeof(user_string), stdin);
                lcd_gotoxy(0,0);
                lcd_clear_screen();
                draw_string(user_string, Pixel_Set, FontSize_5x7);
                break;

			default:
				lcd_gotoxy(0, 0);
				lcd_clear_screen();
				break;
		}

		lcd_send_buff();
	}
	return 0;
}

