#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<unistd.h>
#include<fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include"buffer.h"
#include<ncurses.h>
#include<string.h>
#include<unistd.h>

#define SCR_MAX 43
#define SCR_MIN 0
#define TAB_WIDTH 8

typedef struct str {
	int s, e;
}str;

int lines[45], columns[185], tab[45][24], l = 0, save = 0, exist = 0;
str linesize[45];

void initTab() {
	int i = 45, j = 24;
	for(i = 0; i < 45; i++)
		for(j = 0; j < 24; j++)
			tab[i][j] = -1;
}

int isTab(int yc, int xc) {
	int i;
	for(i = 0; i < 24; i++) {
		if(tab[yc][i] == -1)
			return -1;
		if(tab[yc][i] > xc)
			return -1;
		if(tab[yc][i] <= xc && tab[yc][i] + TAB_WIDTH - 1 >= xc)
			return tab[yc][i];
	}
	return -1;
}
int countTab(int yc, int xc) {
	int i, cnt = 0;
	for(i = 0; i < 24; i++) {
		if(tab[yc][i] == -1)
			return cnt;
		if(tab[yc][i] + TAB_WIDTH - 1 < xc)
			cnt++;
	}
	return cnt;
}
int checkBigger(cursor c1, cursor c2) {
	if(c1.y < c2.y)
		return 1;
	if(c1.y > c2.y)
		return 2;
	if(c1.y == c2.y) {
		if(c1.x < c2.x)
			return 1;
		if(c1.x > c2.x)
			return 2;
	}
	return 0;
}
void initlinesize() {
	int i;
	for(i = 0; i < 45; i++)
		linesize[i].s = linesize[i].e = -1;
}
void printScreen(buffer b, int *start_line, int *end_line) {
	int i, j, xc, yc, ym, xm, size, k, temp = 0;
	char *line;
	for(i = 0; i < 45; i++)
		lines[i] = -1;
	for(i = 0; i < 185; i++)
		columns[i] = -1;
	lines[0] = columns[0] = 0;
	initTab();
	initlinesize();
	i = (*start_line);
	getmaxyx(stdscr, ym, xm);
	while((size = getlinesizeBuffer(&b, i))) {
		line = getlineBuffer(&b, i);
		getyx(stdscr, yc, xc);
		linesize[i].s = yc;
		for(j = 0; j < size; j++) {
			if(line[j] == '\t') {
				for(k = 0; k < TAB_WIDTH; k++)
					addch(' ');
				getyx(stdscr, yc, xc);
				for(k = 0; k < 24; k++) {
					if(tab[yc][k] == -1) {
						tab[yc][k] = xc - TAB_WIDTH;
						break;
					}
				}
			}
			else {
				addch(line[j]);
				getyx(stdscr, yc, xc);
			}
			if(columns[xc] <= yc)
				columns[xc] = yc;
			if(lines[yc] <= xc && xc != xm - 1)
				lines[yc] = xc;
			if(xc == xm -1) {
				move(yc + 1, 0);
				yc++;
			}
			if(yc == ym - 1) {
				temp = 1;
				break;
			}
		}
		linesize[i].e = yc - 1;
		i++;
		free(line);
		refresh();
		if(temp)
			break;
	}
	*end_line = i - 1;
}
int main(int argc, char *argv[]) {
	if(argc > 2) {
		printf("Usage ./project or ./project file_name\n");
		return errno;
	}
	initscr();
	noecho();
	cbreak();
	keypad(stdscr, TRUE);

	int i = 0, j = 0, k, fd, x, yc, xc, xm, ym, ch, length, end_line = SCR_MAX, start_line = 0, xt, cnt, yp, xp, tmp, s, e, cp = -1, search;
	FILE *fp;
	WINDOW *prompt;
	data d, *c = NULL;
	cursor c1, c2;
	char str[128], *find, *replace;
	buffer b;
	cbuffer cb;
	initBuffer(&b);
	initCBuffer(&cb);

	start_color();
	init_color(COLOR_BLACK, 239, 239, 239);
	init_color(COLOR_WHITE, 1000, 1000, 1000);
	init_pair(1, COLOR_WHITE, COLOR_BLACK);
	init_pair(2, COLOR_BLACK, COLOR_WHITE);
	wbkgd(stdscr, COLOR_PAIR(1));
	if(argc == 2) {
		if( access( argv[1], F_OK ) != -1 ) 
			exist = 1;
		else 
			exist = -1;

		fd = open(argv[1], O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
		if(fd == -1) {
			perror("Can't open!!!\n");
			endwin();
			return errno;
		}
		while(1) {
			d.line = (char *)malloc(128);
			d.size = 128;
			i = 0;
			while((x = read(fd, &ch, 1))) {
				d.line[i] = ch;
				i++;
				if(i == d.size) {
					d.size = 2 * d.size;
					d.line = (char *)realloc(d.line, d.size);
				}
				if(ch == '\n')
					break;
			}
			d.pos = i;
			if(i == 0) {
				d.line[0] = '\n';
				d.pos += 1;
			}
			appendBuffer(&b, d);
			free(d.line);
			j++;
			if(x == 0)
				break;
		}
		close(fd);
	}
	getmaxyx(stdscr, ym, xm);
	length = getlengthBuffer(&b);
	printScreen(b, &start_line, &end_line);
	getyx(stdscr, yc, xc);
	lines[yc] = -1;
	move(0, 0);
	while(1) {
		ch = getch();
		switch(ch) {
			case KEY_UP :
				getyx(stdscr, yc, xc);
				if(yc == 0 && start_line != 0) {
					start_line--;
					end_line--;
					move(0, 0);
					printScreen(b, &start_line, &end_line);
					getyx(stdscr, yc, xc);
					lines[yc] = -1;
					move(0, 0);
					break;
				}
				if(yc == 0) {
					beep();
					break;
				}
				if(lines[yc - 1] < xc) {
					move(yc - 1, lines[yc - 1]);
					break;
				}
				if(lines[yc] == xc) {
					if(lines[yc - 1] > xc) {
						move(yc - 1, xc);
						break;
					}
					move(yc - 1, lines[yc - 1]);
					break;
				}
				if((xt = isTab(yc - 1, xc)) != -1) {
					move(yc - 1, xt);
					break;
				}
				move(yc - 1, xc);
				break;
			case KEY_DOWN :
				getyx(stdscr, yc, xc);
				if(yc == ym - 2 && end_line <= length - 2) {
					start_line++;
					end_line++;
					move(0, 0);
					printScreen(b, &start_line, &end_line);
					getyx(stdscr, yc, xc);
					lines[yc] = -1;
					move(yc - 1, 0);
					break;
				}
				if(yc == ym - 2) {
					beep();
					break;
				}
				if(lines[yc + 1] < xc) {
					if(lines[yc + 1] != -1) {
						move(yc + 1, lines[yc + 1]);
						break;
					}
					beep();
					break;
				}
				if(lines[yc] == xc) {
					if(lines[yc + 1] >= xc) {
						move(yc + 1, xc);
						break;
					}
					if(lines[yc -1] != -1) {
						move(yc + 1, lines[yc + 1]);
						break;
					}
					beep();
					break;
				}
				if((xt = isTab(yc + 1, xc)) != -1) {
					move(yc + 1, xt);
					break;
				}
				move(yc + 1, xc);
				break;
			case KEY_LEFT :
				getyx(stdscr, yc, xc);
				if(xc == 0) {
					beep();
					break;
				}
				move(yc, xc - 1);
				if((xt = isTab(yc, xc - 1)) != -1) {
					move(yc, xt);
					break;
				}
				break;
			case KEY_RIGHT :
				getyx(stdscr, yc, xc);
				if(lines[yc] <= xc) {
					beep();
					break;
				}
				if(xc == xm - 1) {
					beep();
					break;
				}
				if((xt = isTab(yc, xc)) != -1) {
					move(yc, xt + TAB_WIDTH);
					break;
				}
				move(yc, xc + 1);
				break;
			case 27 :
				ch = getch();
				switch(ch) {
					case 'y' :
						getyx(stdscr, yc, xc);
						cnt = 0;
						for(i = start_line; i <= end_line; i++) {
							if(yc <= linesize[i].e && yc >= linesize[i].s)
								break;
							if(linesize[i].s == -1)
								break;
						}
						cp = i;
						free(c);
						c = storelineBuffer(&b, cp);
						break;
					case 'p' :
						if(!c)
							break;
						getyx(stdscr, yc, xc);
						cnt = 0;
						for(i = start_line; i <= end_line; i++) {
							if(yc <= linesize[i].e && yc >= linesize[i].s)
								break;
							if(linesize[i].s == -1)
								break;
						}
						yp = i;
						copylineBuffer(&b, yp, c);
						move(0, 0);
						if(end_line < ym - 2)
							end_line++;
						printScreen(b, &start_line, &end_line);
						getyx(stdscr, i, j);
						lines[i] = -1;
						move(yc, xc);
						break;
					case 'd' :
						getyx(stdscr, yc, xc);
						cnt = 0;
						for(i = start_line; i <= end_line; i++) {
							if(yc <= linesize[i].e && yc >= linesize[i].s)
								break;
							if(linesize[i].s == -1)
								break;
						}
						yp = i;
						deletelineBuffer(&b, yp);
						if(end_line == length - 2 && start_line) {
							end_line--;
							start_line--;
						}
						clear();
						move(0, 0);
						printScreen(b, &start_line, &end_line);
						getyx(stdscr, i, j);
						lines[i] = -1;
						move(yc, 0);
						break;
					case 'c' :
						getyx(stdscr, yc, xc);
						cnt = 0;
						for(i = start_line; i <= end_line; i++) {
							if(yc <= linesize[i].e && yc >= linesize[i].s)
								break;
							if(linesize[i].s == -1)
								break;
						}
						yp = i;
						free(c);
						c = storelineBuffer(&b, yp);
						deletelineBuffer(&b, yp);
						if(end_line == length - 2 && start_line) {
							end_line--;
							start_line--;
						}
						if(end_line < ym - 2)
							end_line++;
						clear();
						move(0, 0);
						printScreen(b, &start_line, &end_line);
						getyx(stdscr, i, j);
						lines[i] = -1;
						move(yc, 0);
						break;
					case 'r' :
						getyx(stdscr, yc, xc);
						move(ym - 1, 0);
						echo();
						scanw("%s", str);
						noecho();
						move(yc, xc);
						find = strtok(str, "/");
						replace = strtok(NULL, "/");
						if(replace)
							replaceBuffer(&b, find, replace);
						clear();
						move(0, 0);
						printScreen(b, &start_line, &end_line);
						getyx(stdscr, i, j);
						lines[i] = -1;
						move(yc, xc);
						break;
					case 'v' :
						getyx(stdscr, yc, xc);
						cnt = 0;
						for(i = start_line; i <= end_line; i++) {
							if(yc <= linesize[i].e && yc >= linesize[i].s)
								break;
							if(linesize[i].s == -1)
								break;
						}
						s = linesize[i].s;
						e = linesize[i].e;
						k = yc - s;
						while(k) {
							j = 0;
							cnt = cnt + countTab(s + j, xm - 2);
							k--;
							j++;
						}
						cnt = cnt + countTab(yc, xc);
						c1.y = i;
						c1.x = xc - ((TAB_WIDTH - 1) * cnt) + ((yc - s) * (xm - 1));
						l = 1;
						break;
					case 'V' :
						if(l) {
							getyx(stdscr, yc, xc);
							cnt = 0;
							for(i = start_line; i <= end_line; i++) {
								if(yc <= linesize[i].e && yc >= linesize[i].s)
									break;
								if(linesize[i].s == -1)
									break;
							}
							s = linesize[i].s;
							e = linesize[i].e;
							k = yc - s;
							while(k) {
								j = 0;
								cnt = cnt + countTab(s + j, xm - 2);
								k--;
								j++;
							}
							cnt = cnt + countTab(yc, xc);
							c2.y = i;
							c2.x = xc - ((TAB_WIDTH - 1) * cnt) + ((yc - s) * (xm - 1));
							l = checkBigger(c1, c2);
							destroyCBuffer(&cb);
							if(l == 1)
								copyCBuffer(&cb, &b, c1, c2);
							if(l == 2)
								copyCBuffer(&cb, &b, c2, c1);
							l = 0;
						}
						break;
					case 'P' :
						getyx(stdscr, yc, xc);
						cnt = 0;
						for(i = start_line; i <= end_line; i++) {
							if(yc <= linesize[i].e && yc >= linesize[i].s)
								break;
							if(linesize[i].s == -1)
								break;
						}
						s = linesize[i].s;
						e = linesize[i].e;
						k = yc - s;
						while(k) {
							j = 0;
							cnt = cnt + countTab(s + j, xm - 2);
							k--;
							j++;
						}
						cnt = cnt + countTab(yc, xc);
						yp = i;
						xp = xc - ((TAB_WIDTH - 1) * cnt) + ((yc - s) * (xm - 1));
						pasteCBuffer(&cb, &b, yp, xp);
						clear();
						move(0, 0);
						if(end_line < ym - 2)
							end_line = SCR_MAX;
						printScreen(b, &start_line, &end_line);
						getyx(stdscr, i, j);
						lines[i] = -1;
						move(yc, xc);
						break;
					case 'w' :
						if(argc == 1) {
							getyx(stdscr, yc, xc);
							move(ym - 1, 0);
							attron(A_STANDOUT | A_BOLD);
							printw("Enter the file name :- ");
							echo();
							scanw("%s", str);
							noecho();
							attroff(A_STANDOUT | A_BOLD);
							fp = fopen(str, "w");
							move(ym - 1, 0);
							clrtobot();
							move(yc, xc);
						}
						else
							fp = fopen(argv[1], "w");
						saveBuffer(&b, fp);
						save = 1;
						fclose(fp);
						flash();
						break;
					case 's' :
						getyx(stdscr, yc, xc);
						move(ym - 1, 0);
						echo();
						scanw("%s", str);
						noecho();
						move(yc, xc);
						k = start_line = 0;
						tmp = 1;
						search = searchBuffer(&b, start_line, str);
						while(1) {
							if(search) {
								clear();
								move(0, 0);
								printScreen(b, &start_line, &end_line);
								getyx(stdscr, i, j);
								lines[i] = -1;
								move(0, 0);
								ch = getch();
								switch(ch) {
									case 'e' :
										tmp = 0;
										break;
									default :
										move(0, 0);
										break;
								}
								k++;
							}
							if(tmp == 0)
								break;
							if(start_line == length - 1) {
								start_line = 0;
								if(k == 0)
									break;
							}
							start_line++;
							search = searchBuffer(&b, start_line, str);
						}
						clear();
						move(0, 0);
						printScreen(b, &start_line, &end_line);
						getyx(stdscr, i, j);
						lines[i] = -1;
						move(0, 0);
						break;
					default :
						break;
				}
				break;
			case KEY_BACKSPACE :
				getyx(stdscr, yc, xc);
				cnt = 0;
				for(i = start_line; i <= end_line; i++) {
					if(yc <= linesize[i].e && yc >= linesize[i].s)
						break;
					if(linesize[i].s == -1)
						break;
				}
				s = linesize[i].s;
				e = linesize[i].e;
				k = yc - s;
				while(k) {
					j = 0;
					cnt = cnt + countTab(s + j, xm - 2);
					k--;
					j++;
				}
				cnt = cnt + countTab(yc, xc);
				yp = i;
				xp = xc - ((TAB_WIDTH - 1) * cnt) + ((yc - s) * (xm - 1));
				if(yp == 0 && xp == 0) {
					beep();
					break;
				}
				if((xt = isTab(yc, xc - 1)) != -1)
					xc = xc -(TAB_WIDTH - 1);
				tmp = lines[yc - 1];
				deletecharBuffer(&b, yp, xp);
				clear();
				move(0, 0);
				if(yc == 0 && xc == 0) {
					start_line--;
					end_line--;
				}
				if(xp == 0 && end_line >= length - 2 && start_line) {
					start_line--;
					end_line--;
					yc += 1;
				}
				printScreen(b, &start_line, &end_line);
				getyx(stdscr, i, j);
				lines[i] = -1;
				if(yc == 0 && xc == 0) {
					move(0, 0);
					break;
				}
				if(xc == 0) {
					move(yc - 1, tmp);
					s = linesize[start_line].s;
					e = linesize[start_line].e;
					if((e - s) && yc != ym - 2 && yp != end_line)
						move(yc -(e - s) + 1, tmp);
					break;
				}
				move(yc, xc - 1);
				break;
			case KEY_F(1) :
				getyx(stdscr, yc, xc);
				xp = (COLS - 30) / 2;
				yp = (LINES - 8) / 2;
				prompt = newwin(8, 30, yp, xp);
				box(prompt, 0 , 0);
				wbkgd(prompt, COLOR_PAIR(2));
				mvwprintw(prompt, 1, 5, "Do you Want to exit?");
				mvwprintw(prompt, 3, 1, "Press 'y' for YES");
				mvwprintw(prompt, 4, 1, "Press 'n' for NO");
				wrefresh(prompt);
				tmp = 1;
				while(tmp) {
					ch = getch();
					if(ch == 'y' || ch == 'Y') {
						if(exist == -1 && save == 0)
							remove(argv[1]);
						delwin(prompt);
						endwin();
						destroyBuffer(&b);
						return 0;
					}
					if(ch == 'n' || ch == 'N') {
						move(0, 0);
						printScreen(b, &start_line, &end_line);
						getyx(stdscr, i, j);
						lines[i] = -1;
						clrtobot();
						move(yc, xc);
						tmp = 0;
					}
				}
				delwin(prompt);
				break;
			default :
				getyx(stdscr, yc, xc);
				cnt = 0;
				for(i = start_line; i <= end_line; i++) {
					if(yc <= linesize[i].e && yc >= linesize[i].s)
						break;
					if(linesize[i].s == -1)
						break;
				}
				s = linesize[i].s;
				e = linesize[i].e;
				k = yc - s;
				while(k) {
					j = 0;
					cnt = cnt + countTab(s + j, xm - 2);
					k--;
					j++;
				}
				cnt = cnt + countTab(yc, xc);
				yp = i;
				xp = xc - ((TAB_WIDTH - 1) * cnt) + ((yc - s) * (xm - 1));
				insertcharBuffer(&b, yp, xp, ch);
				move(0, 0);
				if(yc == SCR_MAX && ch == '\n') {
					start_line++;
					end_line++;
				}
				printScreen(b, &start_line, &end_line);
				getyx(stdscr, i, j);
				lines[i] = -1;
				clrtobot();
				if(ch == '\n') {
					if(yc == SCR_MAX) {
						move(yc, 0);
						if(end_line - start_line != ym - 2){
							move(i - 1, 0);
						}
					}
					else {
						move(yc + 1, 0);
						if(end_line - start_line != ym - 2 && yc == end_line){
							move(i - 1, 0);
						}
					}
					break;
				}
				if(ch == '\t') {
					move(yc, xc + TAB_WIDTH);
					break;
				}
				if(xc == xm - 1) {
					move(yc + 1, 0);	
				}
				move(yc, xc + 1);
				break;
		}
		length = getlengthBuffer(&b);
	}
	destroyBuffer(&b);
	endwin();
	return 0;
}
