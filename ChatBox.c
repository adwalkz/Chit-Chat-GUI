
#include<gtk/gtk.h>

#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<sys/socket.h>
#include<arpa/inet.h>

#include<pthread.h>

#define MAX_LEN 1000
//_________________________________________________________________________________________________

GtkWidget *FWin;
GtkWidget *window, *workArea;
GtkWidget *Back, *IP, *PORT, *Connect, *Send, *DisCon;
GtkWidget *CBWin, *nb, *NBwA;
GtkWidget *scrlBox, *Message, *label[100];

//_________________________________________________________________________________________________

int W = -1, UT = 0, LT = 0;
int i = 0;
int y = 5;
int wrt = 0;
int rd = 0;
int l;

//_________________________________________________________________________________________________

struct sockaddr_in Server, Client;
int socketfd, c, len, new_socket;
const char *ip;
int port;

char message[MAX_LEN];
char *m;

pthread_t thrd;

//_________________________________________________________________________________________________

void hideFS()
{
	gtk_widget_hide(FWin);
}

void showFS()
{
	gtk_widget_show(FWin);
	
	if(W == 1)
	{
		W = -1;
		gtk_widget_hide(window);
	}
	
	if(W == 2)
	{
		W = -1;
		gtk_widget_destroy(CBWin);
		printf("Disconnected\n");
		close(socketfd);
	}
}

/*
This function
create new top level window
*/
void NewWin()
{
	W = 1;
	
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_default_size(GTK_WINDOW(window), 200, 200);
	gtk_window_set_resizable(GTK_WINDOW(window), FALSE);
	gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER_ALWAYS);
	g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(gtk_main_quit), NULL);

	workArea = gtk_fixed_new();
	gtk_container_add(GTK_CONTAINER(window), workArea);
}

//_________________________________________________________________________________________________

void Error(char[]);

/*
This function
display msg on screen
*/
void Display(void *msg)
{
	label[i] = gtk_label_new(msg);
	gtk_widget_set_size_request(label[i], sizeof(msg), 5);
	
	//if(i%2==0)
	if(LT == 1)
	{
		gtk_label_set_justify(GTK_LABEL(label[i]), GTK_JUSTIFY_RIGHT);
		gtk_fixed_put(GTK_FIXED(NBwA), label[i++], 350, y);
	}
	else if(LT == 0)
		gtk_fixed_put(GTK_FIXED(NBwA), label[i++], 0, y+20);
		
	y+=40;
	
	gtk_widget_show_all(nb);
	
	if(UT == 3)
	{
		gtk_button_set_label(GTK_BUTTON(DisCon), "CLOSE");
		gtk_widget_set_sensitive(Message, FALSE);
		gtk_widget_set_sensitive(Send, FALSE);
	}
}

/*
This function
main logic
for server side
*/
void ServerLogic()
{
	gtk_widget_hide(window);
	
	socketfd = socket(AF_INET, SOCK_STREAM, 0);
	if(socketfd == -1)
	{
		puts("ERR: Socket Creation Failed");
		Error("ERR: Socket Creation Failed");
		//return -1;
	}
	
	puts("SUCC: Socket Creation Success");
	
	Server.sin_addr.s_addr = INADDR_ANY;
	Server.sin_family = AF_INET;
	Server.sin_port = htons(port);

	if(bind(socketfd, (struct sockaddr*)&Server, sizeof(Server)) < 0)
	{
		puts("ERR: Bind Failed");
		Error("ERR: Bind Failed");
		//return -1;
	}
	
	puts("SUCC: Bind Success");
	
	listen(socketfd, 1);
	
	puts("Server is Live Now...");
	
	c = sizeof(struct sockaddr_in);
	len = sizeof(Client);
	
	new_socket = accept(socketfd, (struct sockaddr*)&Client, (socklen_t *)&len);
	
	if(new_socket < 0)
	{
		puts("ERR: Acceptance Failed");
		Error("ERR: Acceptance Failed");
		//return -1;
	}
	
	puts("New Connection Accepted");
	shutdown(socketfd, SHUT_RD);
	
	while(1)
	{
		//while(rd == 0)
		{
			bzero(message, sizeof(message));
			read(new_socket, message, sizeof(message));
			LT = 0;
			Display(message);
		
			/*if(rd == 1)
			{		
				write(new_socket, message, MAX_LEN);
				rd = 0;
			}*/
			//gtk_entry_set_text(GTK_ENTRY(Message), "");
			//gtk_widget_grab_focus(Message);
		}
	}
}

/*
This function
take text from
gtk_entry
*/
void ServerSend()
{
	LT = 1;
	m = gtk_entry_get_text(GTK_ENTRY(Message));
	write(new_socket, m, MAX_LEN);
	Display(m);
	gtk_entry_set_text(GTK_ENTRY(Message), "");
}

/*
This function
create window 
for Server Chat Box
*/
int ServerCB()
{
	W = 2;
	
	port = atoi(gtk_entry_get_text(GTK_ENTRY(PORT)));
	printf("PORT: %d\n", port);
	
	GtkWidget *CBwA;
	
	CBWin = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(CBWin), "ChatBox: Chat as Server");
	gtk_window_set_default_size(GTK_WINDOW(CBWin), 400, 500);
	gtk_window_set_resizable(GTK_WINDOW(CBWin), FALSE);
	gtk_container_set_border_width(GTK_CONTAINER(CBWin), 5);
	gtk_window_set_position(GTK_WINDOW(CBWin), GTK_WIN_POS_CENTER_ALWAYS);
	g_signal_connect(G_OBJECT(CBWin), "destroy", G_CALLBACK(gtk_main_quit), NULL);
	
	CBwA = gtk_fixed_new();
	gtk_container_add(GTK_CONTAINER(CBWin), CBwA);
	
	DisCon = gtk_button_new_with_label("DISCONNECT");
	gtk_fixed_put(GTK_FIXED(CBwA), DisCon, 0, 0);
	g_signal_connect(G_OBJECT(DisCon), "clicked", G_CALLBACK(showFS), NULL);
	
	nb = gtk_notebook_new();
	gtk_widget_set_size_request(nb, 400, 400);
	gtk_notebook_set_show_tabs(GTK_NOTEBOOK(nb), FALSE);
	gtk_fixed_put(GTK_FIXED(CBwA), GTK_WIDGET(nb), 0, 40);
	
	scrlBox = gtk_scrolled_window_new(NULL, NULL);
	
	NBwA = gtk_fixed_new();
	gtk_container_add(GTK_CONTAINER(scrlBox), NBwA);
	
	gtk_notebook_append_page(GTK_NOTEBOOK(nb), scrlBox, NULL);
	
	Message = gtk_entry_new();
	gtk_entry_set_placeholder_text(GTK_ENTRY(Message), "Write Something");
	gtk_widget_set_size_request(Message, 350, 5);
	gtk_fixed_put(GTK_FIXED(CBwA), Message, 0, 450);
	
	Send = gtk_button_new_with_label("SEND");
	gtk_fixed_put(GTK_FIXED(CBwA), Send, 350, 450);
	g_signal_connect(G_OBJECT(Send), "clicked", G_CALLBACK(ServerSend), NULL);
	
	gtk_widget_show_all(CBWin);
	gtk_widget_grab_focus(Message);
	pthread_create(&thrd, NULL, ServerLogic, NULL);
	
	return 0;
}

/*
This function
main logic
for client side
*/
void ClientLogic()
{
	socketfd = socket(AF_INET, SOCK_STREAM, 0);
	if(socketfd == -1)
	{
		puts("ERR: Socket Creation Failed");
		Error("ERR: Socket Creation Failed");
		return -1;
	}
	
	puts("SUCC: Socket Creation Success");
	
	Server.sin_addr.s_addr = inet_addr(ip);
	Server.sin_family = AF_INET;
	Server.sin_port = htons(port);
	
	if(connect(socketfd, (struct sockaddr*)&Server, sizeof(Server)) < 0)
	{
		puts("ERR: Connection Failed");
		Error("ERR: Connection Failed");
		//return -1;
	}
	
	puts("SUCC: Connection Success");
	
	while(1)
	{
		while(wrt == 0)
			sleep(1);

		/*while(wrt == 1)
		{
			write(socketfd, m, l);
			wrt = 0;
		}*/
		bzero(message, sizeof(message));
		read(socketfd, message, sizeof(message));
		LT = 0;
		Display(message);
		
		//gtk_entry_set_text(GTK_ENTRY(Message), "");
		//gtk_widget_grab_focus(Message);
	}
}

/*
This function
take text from
gtk_entry
*/
void ClientSend()
{
	LT = 1;
	m = gtk_entry_get_text(GTK_ENTRY(Message));
	l = gtk_entry_get_text_length(GTK_ENTRY(Message));
	write(socketfd, m, l);
	Display(m);
	wrt = 1;
	rd = 0;
	gtk_entry_set_text(GTK_ENTRY(Message), "");
}

/*
This function
create window 
for Client Chat Box
*/
int ClientCB()
{
	W = 2;
	
	ip = gtk_entry_get_text(GTK_ENTRY(IP));
	port = atoi(gtk_entry_get_text(GTK_ENTRY(PORT)));
	
	gtk_widget_hide(window);
	
	GtkWidget *CBwA;
	
	CBWin = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(CBWin), "ChatBox: Chat as Client");
	gtk_window_set_default_size(GTK_WINDOW(CBWin), 400, 500);
	gtk_window_set_resizable(GTK_WINDOW(CBWin), FALSE);
	gtk_container_set_border_width(GTK_CONTAINER(CBWin), 5);
	gtk_window_set_position(GTK_WINDOW(CBWin), GTK_WIN_POS_CENTER_ALWAYS);
	g_signal_connect(G_OBJECT(CBWin), "destroy", G_CALLBACK(gtk_main_quit), NULL);
	
	CBwA = gtk_fixed_new();
	gtk_container_add(GTK_CONTAINER(CBWin), CBwA);
	
	DisCon = gtk_button_new_with_label("DISCONNECT");
	gtk_fixed_put(GTK_FIXED(CBwA), DisCon, 0, 0);
	g_signal_connect(G_OBJECT(DisCon), "clicked", G_CALLBACK(showFS), NULL);
	
	nb = gtk_notebook_new();
	gtk_widget_set_size_request(nb, 400, 400);
	gtk_notebook_set_show_tabs(GTK_NOTEBOOK(nb), FALSE);
	gtk_fixed_put(GTK_FIXED(CBwA), GTK_WIDGET(nb), 0, 40);
	
	scrlBox = gtk_scrolled_window_new(NULL, NULL);
	
	NBwA = gtk_fixed_new();
	gtk_container_add(GTK_CONTAINER(scrlBox), NBwA);
	
	gtk_notebook_append_page(GTK_NOTEBOOK(nb), scrlBox, NULL);
	
	Message = gtk_entry_new();
	gtk_entry_set_placeholder_text(GTK_ENTRY(Message), "Write Something");
	gtk_widget_set_size_request(Message, 350, 5);
	gtk_fixed_put(GTK_FIXED(CBwA), Message, 0, 450);
	
	Send = gtk_button_new_with_label("SEND");
	gtk_fixed_put(GTK_FIXED(CBwA), Send, 350, 450);
	g_signal_connect(G_OBJECT(Send), "clicked", G_CALLBACK(ClientSend), NULL);
	
	gtk_widget_show_all(CBWin);
	gtk_widget_grab_focus(Message);
	
	pthread_create(&thrd, NULL, ClientLogic, NULL);
	
	return 0;
}

void Error(char emsg[])
{
	UT = 3;
	Display(emsg);
}

/*
This function
create window for server user
*/
void server()
{
	UT = 1;
	
	hideFS();
	NewWin();
	gtk_window_set_title(GTK_WINDOW(window), "SERVER");

	Back = gtk_button_new_with_label("BACK");
	gtk_fixed_put(GTK_FIXED(workArea), Back, 5, 10);
	g_signal_connect(G_OBJECT(Back), "clicked", G_CALLBACK(showFS), NULL);
	
	PORT = gtk_entry_new();
	gtk_entry_set_placeholder_text(GTK_ENTRY(PORT), "Enter PORT Number");
	gtk_fixed_put(GTK_FIXED(workArea), PORT, 10, 70);
	
	Connect = gtk_button_new_with_label("CONNECT");
	gtk_fixed_put(GTK_FIXED(workArea), Connect, 90, 150);
	g_signal_connect(G_OBJECT(Connect), "clicked", G_CALLBACK(ServerCB), NULL);
	
	gtk_widget_show_all(window);
}

/*
This function
create window for client user
*/
void client()
{
	UT = 2;
	
	hideFS();
	NewWin();
	gtk_window_set_title(GTK_WINDOW(window), "CLIENT");
	
	Back = gtk_button_new_with_label("BACK");
	gtk_fixed_put(GTK_FIXED(workArea), Back, 5, 10);
	g_signal_connect(G_OBJECT(Back), "clicked", G_CALLBACK(showFS), NULL);
	
	IP = gtk_entry_new();
	gtk_entry_set_placeholder_text(GTK_ENTRY(IP), "Enter IP Address");
	gtk_fixed_put(GTK_FIXED(workArea), IP, 10, 50);
	
	PORT = gtk_entry_new();
	gtk_entry_set_placeholder_text(GTK_ENTRY(PORT), "Enter PORT Number");
	gtk_fixed_put(GTK_FIXED(workArea), PORT, 10, 100);
	
	Connect = gtk_button_new_with_label("CONNECT");
	gtk_fixed_put(GTK_FIXED(workArea), Connect, 90, 150);
	g_signal_connect(G_OBJECT(Connect), "clicked", G_CALLBACK(ClientCB), NULL);
	
	gtk_widget_show_all(window);
}

/*
This function
create First Screen
*/
void FirstScreen()
{
	GtkWidget *FwA;
	GtkWidget *Label, *bServer, *bClient;
	
	FWin = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_default_size(GTK_WINDOW(FWin), 200, 150);
	gtk_window_set_resizable(GTK_WINDOW(FWin), FALSE);
	gtk_window_set_position(GTK_WINDOW(FWin), GTK_WIN_POS_CENTER_ALWAYS);
	g_signal_connect(G_OBJECT(FWin), "destroy", G_CALLBACK(gtk_main_quit), NULL);
	
	FwA = gtk_fixed_new();
	gtk_container_add(GTK_CONTAINER(FWin), FwA);
	
	Label = gtk_label_new("Continue As..?");
	gtk_fixed_put(GTK_FIXED(FwA), Label, 10, 10);
	
	bServer = gtk_button_new_with_label("SERVER");
	gtk_fixed_put(GTK_FIXED(FwA), bServer, 70, 50);
	g_signal_connect(G_OBJECT(bServer), "clicked", G_CALLBACK(server), NULL);
	
	bClient = gtk_button_new_with_label("CLIENT");
	gtk_fixed_put(GTK_FIXED(FwA), bClient, 70, 100);
	g_signal_connect(G_OBJECT(bClient), "clicked", G_CALLBACK(client), NULL);
	
	gtk_widget_show_all(FWin);
}

//_________________________________________________________________________________________________

int main()
{
	gtk_init(NULL, NULL);
	FirstScreen();
	GtkCssProvider *css = gtk_css_provider_new();
	gtk_css_provider_load_from_path(css, "style.css", NULL);
	gtk_style_context_add_provider_for_screen(gdk_screen_get_default(), GTK_STYLE_PROVIDER(css), 									GTK_STYLE_PROVIDER_PRIORITY_USER);
	gtk_main();
	//pthread_exit(NULL);
	return 0;
}

