typedef struct data {
	char *line;
	int size, pos;
}data;
typedef struct node {
	data d;
	struct node *prev, *next;
}node;
typedef struct buffer {
	node *head, *tail;
}buffer;
typedef struct cursor {
	int y, x;
}cursor;
typedef struct cnode {
	char *cline;
	int csize, flag;
	struct cnode *cprev, *cnext;
}cnode;
typedef struct cbuffer {
	cnode *chead, *ctail;
}cbuffer;
void initBuffer(buffer *b);
void destroyBuffer(buffer *b); 
void appendBuffer(buffer *b, data c);
char *getlineBuffer(buffer *b, int line_no);
int getlinesizeBuffer(buffer *b, int line_no);
int getlengthBuffer(buffer *b);
void insertcharBuffer(buffer *b, int yc, int xc, char ch);
void deletecharBuffer(buffer *b, int yc, int xc);
data *storelineBuffer(buffer *b, int cp);
void copylineBuffer(buffer *b, int yp, data *c);
void deletelineBuffer(buffer *b, int yp);
void replaceBuffer(buffer *b, char *sf, char *sr);
void initCBuffer(cbuffer *cb);
void destroyCBuffer(cbuffer *cb);
void copyCBuffer(cbuffer *cb, buffer *b, cursor c1, cursor c2);
void pasteCBuffer(cbuffer *cb, buffer *b, int yp, int xp);
void saveBuffer(buffer *b, FILE *fp);
int searchBuffer(buffer *b, int yp, char *str); 
