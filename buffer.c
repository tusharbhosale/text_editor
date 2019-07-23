#include<stdlib.h>
#include<stdio.h>
#include"buffer.h"
#include<string.h>
#include<ncurses.h>

void initBuffer(buffer *b) {
	b->head = b->tail = NULL;
}
void destroyBuffer(buffer *b) {
	node *p, *q;
	p = b->head;
	while(p) {
		q = p;
		p = p->next;
		free(q->d.line);
		free(q);
	}
	b->head = b->tail = NULL;
}
void appendBuffer(buffer *b, data c) {
	node *temp;
	int i;
	temp = (node *)malloc(sizeof(node));
	temp->d.line = (char *)malloc(c.size);
	temp->d.size = c.size;
	temp->d.pos = c.pos;
	for(i = 0; i < c.pos; i++) 
		temp->d.line[i] = c.line[i];
	if(b->head == NULL) {
		temp->next = temp->prev = NULL;
		b->head = b->tail = temp;
		return;
	}
	b->tail->next = temp;
	temp->prev = b->tail;
	temp->next = NULL;
	b->tail = temp;
}
char *getlineBuffer(buffer *b, int line_no) {
	node *p;
	int cnt = 0, i;
	char *line;
	p = b->head;
	while(p) {
		if(cnt == line_no)
			break;
		p = p->next;
		cnt++;
	}
	if(p == NULL)
		return NULL;
	line = (char *)malloc(p->d.size);
	for(i = 0; i < p->d.pos; i++)
		line[i] = p->d.line[i];
	return line;
}
int getlinesizeBuffer(buffer *b, int line_no) {
	node *p;
	int cnt = 0;
	p = b->head;
	while(p) {
		if(cnt == line_no)
			break;
		p = p->next;
		cnt++;
	}
	if (p == NULL)
		return 0;
	return p->d.pos;
}
int getlengthBuffer(buffer *b) {
	node *p;
	int len = 0;
	p = b->head;
	while(p) {
		len++;
		p = p->next;
	}
	return len;
}
void insertcharBuffer(buffer *b, int yc, int xc, char ch) {
	node *p, *temp;
	int cnt = 0, i, j;
	p = b->head;
	if(p == NULL) {
		temp = (node *)malloc(sizeof(node));
		temp->d.line = (char *)malloc(128);
		temp->d.size = 128;
		temp->d.line[0] = ch;
		temp->d.line[1] = '\n';
		temp->d.pos = 2;
		temp->next = temp->prev = NULL;
		b->head = b->tail = temp;
		return;
	}
	while(p) {
		if(cnt == yc)
			break;
		p = p->next;
		cnt++;
	}
	if(p->d.pos == p->d.size) {
		p->d.size += 128;
		p->d.line = (char *)realloc(p->d.line, p->d.size);
	}
	for(i = p->d.pos; i > xc; i--)
		p->d.line[i] = p->d.line[i - 1];
	p->d.line[xc] = ch;
	p->d.pos++;
	if(ch == '\n') {
		temp = (node *)malloc(sizeof(node));
		temp->d.line = (char *)malloc(128);
		temp->d.size = 128;
		for(i = xc + 1, j = 0; i < p->d.pos; i++, j++){
			temp->d.line[j] = p->d.line[i];
			if(j == temp->d.size) {
				temp->d.size += 128;
				temp->d.line = (char *)realloc(temp->d.line, temp->d.size);	
			}
		}
		temp->d.pos = j;
		p->d.pos = xc + 1;
		if(p == b->tail) {
			p->next = temp;
			temp->prev = p;
			temp->next = NULL;
			b->tail = temp;
			return;
		}
		temp->next = p->next;
		p->next->prev = temp;
		temp->prev = p;
		p->next = temp;
	}
}

void deletecharBuffer(buffer *b, int yc, int xc) {
	node *p, *temp;
	int cnt = 0, i, j;
	if(xc == 0 && yc == 0)
		return;
	p = b->head;
	while(p) {
		if(cnt == yc)
			break;
		p = p->next;
		cnt++;
	}
	temp = p->prev;
	if(xc == 0) {
		if(temp->d.size < p->d.pos + temp->d.size) {
			temp->d.size += 128;
			temp->d.line = (char *)realloc(temp->d.line, temp->d.size);
		}
		j = temp->d.pos - 1;
		for(i = 0; i < p->d.pos; i++) {
			temp->d.line[j] = p->d.line[i];
			j++;
		}
		temp->d.pos = j;
		temp->next = p->next;
		if(temp->next)
			p->next->prev = temp;
		else
			b->tail = temp;
		free(p);
		return;
	}
	for(i = xc; i < p->d.pos; i++)
		p->d.line[i - 1] = p->d.line[i];
	p->d.pos--;
}
data *storelineBuffer(buffer *b, int cp) {
	node *p;
	data *temp;
	int cnt = 0, i;
	p = b->head;
	while(p) {
		if(cnt == cp)
			break;
		p = p->next;
		cnt++;
	}
	temp = (data *)malloc(sizeof(data));
	temp->line = (char *)malloc(p->d.size);
	temp->size = p->d.size;
	for(i = 0; i < p->d.pos; i++)
		temp->line[i] = p->d.line[i];
	temp->pos = p->d.pos;
	return temp;
}
void copylineBuffer(buffer *b, int yp, data *c) {
	node *p, *temp, *q;
	int cnt = 0, i;
	p = b->head;
	while(p) {
		if(cnt == yp)
			break;
		p = p->next;
		cnt++;
	}
	q = p->prev;
	temp = (node *)malloc(sizeof(node));
	temp->d.line = (char *)malloc(c->size);
	temp->d.size = c->size;
	for(i = 0; i < c->pos; i++)
		temp->d.line[i] = c->line[i];
	temp->d.pos = c->pos;
	temp->next = p;
	temp->prev = q;
	p->prev = temp;
	if(q)
		q->next = temp;
	else
		b->head = temp;
}
void deletelineBuffer(buffer *b, int yp) {
	node *p, *q;
	int cnt = 0;
	p = b->head;
	if(!p)
		return;
	while(p) {
		if(cnt == yp)
			break;
		p = p->next;
		cnt++;
	}
	q = p->prev;
	if(q)
		q->next = p->next;
	else
		b->head = p->next;
	if(p->next)
		p->next->prev = q;
	else
		b->tail = q;
}
void replaceBuffer(buffer *b, char *sf, char *sr) {
	node *p;
	int i = 0, j = 0, c = 0, k = 0, l, flag = 0, lf, lr, shift, length, m;
	p = b->head;
	lf = strlen(sf);
	lr = strlen(sr);
	length = getlengthBuffer(b);
	m = length - 2;
	while(m) {
		if(p->d.size < p->d.pos + lr) {
			p->d.size += 128;
			p->d.line = (char *)realloc(p->d.line, p->d.size);
		}
		while(flag != -1) {
			i = 0;
			while(i < p->d.pos) {
				c = j = 1;
				if(p->d.line[i] == sf[0]) {
					while(c < lf) {
						if(p->d.line[i + c] == sf[c]) 
							j++;
						else
							break;
						c++;
					}
					if(j == lf) {
						flag = 1;
						break;
					}
				}
				i++;
				flag = -1;
			}
			if(flag == 1) {
				shift = lr - lf;
				if(shift == 0) {
					for(k = i, l = 0; l < lr; k++, l++)
						p->d.line[k] = sr[l];
				}
				if(shift > 0) {
					for(k = p->d.pos - 1; k >= i + lf; k--)
						p->d.line[k + shift] = p->d.line[k];
					for(k = i, l = 0; l < lr; k++, l++)
						p->d.line[k] = sr[l];
				}
				if(shift < 0) {
					for(k = i + lf; k < p->d.pos; k++)
						p->d.line[k + shift] = p->d.line[k];
					for(k = i, l = 0; l < lr; k++, l++)
						p->d.line[k] = sr[l];
				}
				p->d.pos += shift;
			}
		}
		m--;
		flag = 0;
		p = p->next;
	}
}
void initCBuffer(cbuffer *cb) {
	cb->chead = cb->ctail = NULL;
}
void destroyCBuffer(cbuffer *cb) {
	cnode *p, *q;
	p = cb->chead;
	while(p) {
		q = p;
		p = p->cnext;
		free(q->cline);
		free(q);
	}
	cb->chead = cb->ctail = NULL;
}
void copyCBuffer(cbuffer *cb, buffer *b, cursor c1, cursor c2) {
	node *p;
	cnode *temp;
	int cnt = 0, i, s, e, j, k;
	i = c2.y - c1.y;
	p = b->head;
	if(!p)
		return;
	while(p) {
		if(cnt == c1.y)
			break;
		p = p->next;
		cnt++;
	}
	while(i + 1) {
		temp = (cnode *)malloc(sizeof(cnode));
		temp->cline = (char *)malloc(p->d.pos);
		temp->flag = 0;
		if(i == c2.y - c1.y)
			s = c1.x;
		else
			s = 0;
		if(i == 0)
			e = c2.x;
		else
			e = p->d.pos;
		for(j = s, k = 0; j < e; j++, k++) {
			temp->cline[k] = p->d.line[j];
			if(p->d.line[j] == '\n')
				temp->flag = 1;
		}
		temp->csize = k;
		if(cb->chead == NULL) {
			temp->cprev = temp->cnext = NULL;
			cb->chead = cb->ctail = temp;
		}
		else {
			cb->ctail->cnext = temp;
			temp->cprev = cb->ctail;
			temp->cnext = NULL;
			cb->ctail = temp;
		}
		p = p->next;
		i--;
	}
}
void pasteCBuffer(cbuffer *cb, buffer *b, int yp, int xp) {
	node *p, *temp;
	cnode *q, *r;
	int cnt = 0, i, j, k;
	char str[1024]; 
	p = b->head;
	q = cb->chead;
	if(!q)
		return;
	if(!p) {
		while(q) {
			temp = (node *)malloc(sizeof(node));
			temp->d.line = (char *)malloc(128);
			temp->d.size = 128;
			for(j = 0; j < q->csize; j++)
				temp->d.line[j] = q->cline[j];
			temp->d.pos = j;
			if(p == NULL) {
				temp->next = temp->prev = NULL;
				b->head = b->tail = temp;
			}
			else {
				b->tail->next = temp;
				temp->prev = b->tail;
				temp->next = NULL;
				b->tail = temp;
			}
			q = q->cnext;
			p = b->head;
		}
		return;
	}
	while(p) {
		if(cnt == yp)
			break;
		p = p->next;
		cnt++;
	}
	for(i = 0, j = xp; j < p->d.pos; j++, i++)
		str[i] = p->d.line[j];

	if(p->d.size < p->d.pos + q->csize) {
			p->d.size += 128;
			p->d.line = (char *)realloc(p->d.line, p->d.size);
	}
	for(k = 0, j = xp; k < q->csize; j++, k++)
		p->d.line[j] = q->cline[k];
	p->d.pos = j;
	r = q;
	q = q->cnext;
	while(q) {
		temp = (node *)malloc(sizeof(node));
		temp->d.line = (char *)malloc(128);
		temp->d.size = 128;
		for(j = 0; j < q->csize; j++)
			temp->d.line[j] = q->cline[j];
		temp->d.pos = j;
		temp->next = p->next;
		temp->prev = p;
		if(p->next)
			p->next->prev = temp;
		else
			b->tail = temp;
		if(p)
			p->next = temp;
		else
			b->head = temp;
		r = q;
		q = q->cnext;
		p = p->next;
	}
	if(r->flag == 0) {
		if(p->d.size < p->d.pos + i) {
			p->d.size += 128;
			p->d.line = (char *)realloc(p->d.line, p->d.size);
		}
		for(k = 0, j = p->d.pos; k < i; k++, j++)
			p->d.line[j] = str[k];
		p->d.pos = j;
	}
	else {
		temp = (node *)malloc(sizeof(node));
		temp->d.line = (char *)malloc(128);
		temp->d.size = 128;
		for(j = 0; j < i; j++)
			temp->d.line[j] = str[j];
		temp->d.pos = j;
		temp->next = p->next;
		temp->prev = p;
		if(p->next)
			p->next->prev = temp;
		else
			b->tail = temp;
		if(p)
			p->next = temp;
		else
			b->head = temp;
	}
}
void saveBuffer(buffer *b, FILE *fp) {
	node *p;
	int i;
	char ch;
	p = b->head;
	while(p) {
		for(i = 0; i < p->d.pos; i++) {
			ch = p->d.line[i];
			fwrite(&ch, 1, 1, fp);
		}
		p = p->next;
	}
}
int searchBuffer(buffer *b, int yp, char *str) {
	node *p;
	int cnt = 0, i = 0, c, j, len, search = 0;
	p = b->head;
	if(!p)
		return 0;
	while(p) {
		if(cnt == yp)
			break;
		p = p->next;
		cnt++;
	}
	len = strlen(str);
	while(i < p->d.pos && search == 0) {
		c = j = 1;
		if(p->d.line[i] == str[0]) {
			while(c < len) {
				if(p->d.line[i + c] == str[c]) 
					j++;
				else
					break;
				c++;
			}
			if(j == len) {
				search = 1;
				break;
			}
		}
		i++;
	}
	return search;
}
