struct hidden; 
typedef struct hidden server_context_t;

int udp_send(server_context_t*, char*, int);
int udp_recv(server_context_t*, char*, int);
server_context_t* server_init(void);
int server_run(server_context_t *ctx);
int server_exit(server_context_t *ctx);
server_context_t* client_init(void);
//int client_run()server_context_t* ;
//int client_free(vserver_context_t*);

