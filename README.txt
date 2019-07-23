
					------------------------------------------------------
						Name             : Bhosale Tushar Dinanath
						MIS No.          : 111703009
						Title of Project : text_editor
					-----------------------------------------------------

Step followed by me during developing of mini project :-

1) First I learned the basic functions in ncurses which I required.
2) Then I design a ADT called buffer to store data in file line by line which contains a list data structure.
3) To print data on screen, I print the data line by line upto max screen height.
4) To do copy paste there is another ADT called CBuffer which also store data line by line.
5) CBuffer store the data which is to be copied the by merging the buffer and cbuffer paste is done
6) Then it also contains a features line copy_line, cut_line, delete_line, search world, search and replace, tab, newlinw, etc.
7) Commands required :
	a) ESC - y  : copy line
	b) ESC - p  : paste line
	c) ESC - c  : cut line
	d) ESC - r  : search and replace by giving input as "text_to_be_replaced/text_for_replace"
	e) ESC - s  : search the text
	f) ESC - w  : save file
	g) ESC - F1 : exit the editor
