/* title:            Jabble/SMall-xmpp ,Jabber simple client in ANSI C */
/* author:		Sir Mespompes */
/* email:		erteqa@free.fr */
/* jabber:		sir_mespompes@jabber.org */
/* web site:	http://in.the.slyp.free.fr/jabber/xmpp-handler */

#include <stdio.h>
#include <string.h> // strlen
#include <stdlib.h> // atoi
#include <time.h> // time, ctime

#define WINDOWS 0 // set to 0 for Unix, 1 for Windows
#define CLIENT_NAME "Jabble"
#define CLIENT_VERSION "not used at this time"
#define REMOTEPORT 5222
#define PROXY "proxy.eu.jabber.org"
#define STDIN 0  // file descriptor for standard input
#define ENCODING "UTF-8"
#define START_DEBUGMODE 0

#if WINDOWS
#define __OS__ "Windows"
#else
#define __OS__ "Unix"
#endif

#include <sys/types.h>
#if WINDOWS
#include <conio.h>	// kbhit
#include <winsock.h>
#else /* UNIX */
//#include <poll.h>	// kbhit with poll()
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>	/* REMove this line for BeOS */
short   kbhit(void);
#endif


/* socket variables */
int sock;
struct sockaddr_in client_addr;
struct sockaddr_in server_addr;
struct hostent *hostname;
int sin_size;
int si_sock;
struct sockaddr_in si_client_addr;
struct sockaddr_in si_server_addr;
struct hostent *si_hostname;

/* send/recv prototypes */
int connectsock_si_receive(char *siname,char *size,char *sihost,char *siport,char *siid,char *sijid,char *sifrom,char *sito);
int connectsock_si_send(char *siname,char *size,char *sihost,char *siport,char *siid,char *sijid,char *sifrom,char *sito);
char sihost[64],siport[8],siid[128],sijid[128],siname[256],sisize[16];
char proxy[128];
char temp[256];
void sha1sum(char *temp);
void filesize(char *siname);
int si_sendout(char *message_out, int nb);
void si_getin(char *siname, char *host,int size);
void connectsock(void);
void getin(void);
void fd_getin(void);
int ansi_kbhit(void);
void flapon(void);
void clientsignon(void);
void toc_signon(void);
void toc_init_done(void);
void init_roster(void);
void toc_list_buddy(void);
void toc_list_buddy_online(void);
void toc_add_buddy(char *xmlfromlong, char *xmlavailable, char *xmlsubscription, char *xmlsubscription_ask);
void toc_select_buddy_online(char *option);
void toc_catch_buddy(char *option,char *option2,char *option3);
void commands_interpreter(void);
void exitclient(void);
int debugmode,autoresult,notifmode,antifloodmode;
char text_old[15000];
char victimes[255][128];
int online_victimes[255];
char subscription_ask_victimes[255][16];
char subscription_victimes[255][8];
int victimes_nb;
	char victime[128];
	int value;
	char disco[128];
	char node[128];
	char option[255];
	char option2[255];
	char option3[255];
	char xmlkey[128];
int sendout(char *message_out);
void printbuf(char* buf,int length);
void getargs (char *commande);
short nbo_short(short);
char *converse_temps(long unsigned int uptime);
/* send/recv variables */
int buflen;
fd_set onrecv_set;
struct timeval tv;
unsigned short sequence_number_client;
char message_out[3000];

/* user/client vars */
char ressource[128];
char client[32];
char user[127];
char userjid[384];
char pass[32];
char remotehost[128];
char remoteport[6];
char prior[4];
void userlogin(void);
char domain[128];
char nick[127];
char room[50];
char jabberservername[255];
char roomname[255];






int main(int argc, char *argv[])
{
/* presentation */
printf(
"\n"
" %s, a Jabber command-Line cLient - by Sir Mespompes\n"
" version: %s\n"
" jid: sir_mespompes@jabber.org , website: http://in.the.slyp.free.fr\n"
"\n"
,CLIENT_NAME,__FILE__);

if (argc > 0 && ((argv[1]== NULL) || (argv[2]== NULL)))
  {
	printf("   Usage: %s <jid>[/ressource] <pass> [server] [port]\n\n", argv[0]);
	exit(0);
	}

/* encodage du pass */
strcpy(client,argv[0]);
strcpy(user,argv[1]);
if (strstr(user,"@") != NULL){
	strcpy(nick,user);
	if (strstr(user,"/") != NULL)
		sscanf(nick,"%[^@]@%[^/]/%s",user,domain,ressource);
	else
		{sscanf(nick,"%[^@]@%s",user,domain);
		strcpy(ressource,CLIENT_NAME);}
	}
strcpy(pass,argv[2]);

if (argv[3] != NULL)
	strcpy(remotehost,argv[3]);
else
	strcpy(remotehost,domain);
if (domain==NULL)
	strcpy(domain,remotehost);

//if (argv[4] != NULL)
if ((argv[4] != NULL) && (argv[3] != NULL))
	strcpy(remoteport,argv[4]);
else
	strcpy(remoteport,"5222");



//printf("\n\tCONNECTED as %s@%s on %s:%s!\n\n",user,domain,remotehost,remoteport);


	connectsock();
	userlogin();
        flapon(); printf("get stream...\n");
        toc_signon(); printf("server signon...\n");
	toc_init_done();






while(1)  /*Boucle permanente */
{
if(ansi_kbhit()!=0)
	{
	gets(message_out);
	 
	commands_interpreter();
	sendout(message_out);

	}
//printf("kbhit:'%s'\n",message_out);
fd_getin();
}
 


 
} // end of main()





/* ========================== FONCTIONS ================================ */


void commands_interpreter(void)
{
	if (strstr(message_out,"quit:") != NULL)
		{
		strcpy(option,"");
		sscanf(message_out,"quit: %[^\n]",option);
		sprintf(message_out,"<presence type='unavailable'><status>%s</status></presence>",option);
		sendout(message_out);
		sprintf(message_out,"</stream:stream>");
		printf("%s\n",message_out);
		exitclient();
		}
	if (strcmp(message_out,"debug") == 0)
		{
		debugmode=debugmode^1;
		if (debugmode==1)
			printf("debug mode is now ON\n");
		else
			printf("debug mode is now OFF\n");			
		}
	if (strcmp(message_out,"notif") == 0)
		{
		notifmode=notifmode^1;
		if (notifmode==1)
			printf("presence notification is now ON\n");
		else
			printf("presence notification is now OFF\n");			
		}
	if (strcmp(message_out,"antiflood") == 0)
		{
		antifloodmode=antifloodmode^1;
		if (antifloodmode==1)
			printf("presence antiflood is now ON\n");
		else
			printf("presence antiflood is now OFF\n");			
		}
	if (strcmp(message_out,"autoresult") == 0)
		{
		autoresult=autoresult^1;
		if (autoresult==1)
			printf("autoresult mode is now ON\n");
		else
			printf("autoresult mode is now OFF\n");			
		}
	if (strstr(message_out,"prior:") != NULL)
		{
		sscanf(message_out,"prior: %s",prior);
		sprintf(message_out,"<presence><priority>%s</priority></presence>",prior);
		}
	if (strstr(message_out,"status:") != NULL)
		{
		sscanf(message_out,"status: %[^\n]",option);
		sprintf(message_out,
			"<presence><status>%s</status><priority>%s</priority></presence>"
			"<presence to='%s'><status>%s</status><priority>%s</priority></presence>"
			,option,prior,roomname,option,prior);
		}
	if (strstr(message_out,"away:") != NULL)
		{
		strcpy(option,"Not here for the moment!");
		sscanf(message_out,"away: %[^\n]",option);
		sprintf(message_out,
			"<presence><show>away</show><status>%s</status><priority>%s</priority></presence>"
			"<presence to='%s'><show>away</show><status>%s</status><priority>%s</priority></presence>"
			,option,prior,roomname,option,prior);
		}
	if (strstr(message_out,"vcard:") != NULL)
		{
		sscanf(message_out,"vcard: %s",jabberservername);
		sprintf(message_out,"<iq type='get' to='%s' id='vcardget'><vCard xmlns='vcard-temp'/></iq>",jabberservername);
		}
	if (strcmp(message_out,"list:") == 0)
		{
		//sprintf(message_out,"<iq type='get' id='rosterget'><query xmlns='jabber:iq:roster'/></iq>");
		toc_list_buddy_online();
		}
	if (strcmp(message_out,"adv:") == 0)
		{
		sprintf(message_out,"<presence>"
			"<priority>%s</priority>"
			"<show>chat</show>"
			"<c node='http://www.google.com/xmpp/client/caps' ext='voice-v1 video-v1 share-v1 sidebar' xmlns='http://jabber.org/protocol/caps'/>"
			"<c xmlns='http://jabber.org/protocol/caps' node='http://gajim.org/caps' ext='ftrans' ver='0.10.1'/>"
			"<c node='http://psi-im.org/caps' ext='cs ep pep achat vchat' xmlns='http://jabber.org/protocol/caps'/>"
			"<c xmlns='http://jabber.org/protocol/caps' ext='ftrans tins' node='http://exodus.jabberstudio.org/caps'/>"
			"<c node='http://coccinella.sourceforge.net/protocol/caps' ext='ftrans iax voip_h323 voip_sip voipgm2' xmlns='http://jabber.org/protocol/caps'/><x xmlns='http://jabber.org/protocol/jingle/media/audio' type='available'/>"
			"<x xmlns='vcard-temp:x:update'><photo>5e02082fa065f4a438a0eee1a5b41d893467abf2</photo></x>"
			"<x xmlns='jabber:x:signed'>iD8DBQBEViKV/xvnckCDIgsRAkgnAJ0W7caawD5vYjDIlf5Dixh6TNf66QCfa7pT\nqyrgLj+BjDUSP5iIj1ZnALs=\n=w6pd</x>"
			"</presence>",prior);
		//printf("%s\n",message_out);
		}
	if (strcmp(message_out,"online:") == 0)
		{
		sprintf(message_out,
			"<presence><priority>%s</priority></presence>"
			"<presence to='%s'><priority>%s</priority></presence>"
			,prior,roomname,prior);
		}
	if (strcmp(message_out,"invisible:") == 0)
		{
		sprintf(message_out,
			"<presence type='invisible'><priority>%s</priority></presence>"
			,roomname);
		}
	if (strstr(message_out,"ich:") != NULL)
		{
		sscanf(message_out,"ich: %[^\n]",option);
		sprintf(message_out,
			"<presence type='invisible' to='%s'><status>%s</status></presence>"
			,roomname,option);
		}
	if (strcmp(message_out,"offline:") == 0)
		{
		strcpy(message_out,"<presence type='unavailable'></presence>");
		}
	if (strstr(message_out,"last:") != NULL)
		{
		sscanf(message_out,"last: %[^\n]",jabberservername);
		sprintf(message_out,"<iq type='get' id='1111' to='%s'><query xmlns='jabber:iq:last'/></iq>",jabberservername);
		}
	if (strstr(message_out,"join:") != NULL)
		{
		strcpy(option,"");
		sscanf(message_out,"join: %s %[^\n]",roomname,option);
		if (strcmp(option,"") == 0)
			sprintf(option,"%s",user/*,CLIENT_NAME*/);
		sprintf(message_out,"<presence to='%s/%s'></presence>",roomname,option);
		}
	if (strstr(message_out,"nick:") != NULL)
		{
		strcpy(option,"");
		sscanf(message_out,"nick: %[^\n]",option);
		if (strcmp(option,"") == 0)
			sprintf(option,"%s_%s",user,CLIENT_NAME);
		sprintf(message_out,"<presence to='%s/%s'></presence>",roomname,option);
		}
	if (strstr(message_out,"leave:") != NULL)
		{
		strcpy(option,"");
		sscanf(message_out,"leave: %s",option);
		if (strcmp(option,"") == 0)
			sprintf(option,"%s",roomname);
		sprintf(message_out,"<presence to='%s' type='unavailable'></presence>",option);
		}
	if (strstr(message_out,"ch:") != NULL)
		{
		sscanf(message_out,"ch: %[^\n]",option);
		sprintf(message_out,"<message type='groupchat' to='%s'><body>%s</body></message>",roomname,option);
		}
	if (strstr(message_out,"invite:") != NULL)
		{
		sscanf(message_out,"invite: %s %[^\n]",victime,roomname);
		sprintf(message_out,"<message to='%s'><x xmlns='http://jabber.org/protocol/muc#user'><invite to='%s'><reason/></invite></x></message>",roomname,victime);
		}
	if (strstr(message_out,"ver:") != NULL)
		{
		sscanf(message_out,"ver: %[^\n]",jabberservername);
		sprintf(message_out,"<iq type='get' id='2222' to='%s'><query xmlns='jabber:iq:version'/></iq>",jabberservername);
		}
	if (strstr(message_out,"items:") != NULL)
		{
		sscanf(message_out,"items: %[^\n]",jabberservername);
		printf("---%s: has item(s):\n",jabberservername);
		sprintf(message_out,"<iq type='get' id='3333' to='%s'><query xmlns='http://jabber.org/protocol/disco#items'/></iq>",jabberservername);
		}
	if (strstr(message_out,"info:") != NULL)
		{
		sscanf(message_out,"info: %[^\n]",jabberservername);
		printf("---%s: has variable(s):\n",jabberservername);
		sprintf(message_out,"<iq type='get' id='4444' to='%s'><query xmlns='http://jabber.org/protocol/disco#info'/></iq>",jabberservername);
		}
	if (strstr(message_out,"brow:") != NULL)
		{
		sscanf(message_out,"brow: %[^\n]",jabberservername);
		printf("---%s: has variable(s):\n",jabberservername);
		sprintf(message_out,"<iq type='get' id='4545' to='%s'><query xmlns='jabber:iq:browse'/></iq>",jabberservername);
		}
	if (strstr(message_out,"oob:") != NULL)
		{
		//sscanf(message_out,"oob: %s %[^\n]",option,victime);
		sscanf(message_out,"oob: \"%[^\"]\" %s",option,victime);
		sprintf(message_out,"<iq type='set' to='%s' id='oob'><query xmlns='jabber:iq:oob'><url>%s</url></query></iq>",victime,option);
		}
	if (strstr(message_out,"proxy:") != NULL)
		{
		sscanf(message_out,"proxy: %s",jabberservername);
		sprintf(proxy,"proxy.%s",jabberservername);
		sprintf(message_out,"<iq type='get' to='%s' id='select_proxy'>"
			"<query xmlns='http://jabber.org/protocol/bytestreams'/>"
			"</iq>",proxy);
		}
	if (strstr(message_out,"send:") != NULL)
		{
		sscanf(message_out,"send: \"%[^\"]\" %s",siname,victime);
		sprintf(message_out,"<iq type='get' to='%s' id='si_sendfile14'>"
			"<query xmlns='http://jabber.org/protocol/bytestreams'/>"
			"</iq>",proxy);
			sendout(message_out);
			getin();
		filesize(siname);
		//sprintf(siid,"%s@%s/%s%s",user,remotehost,ressource)
		tmpnam(siid);
		sprintf(message_out,"<iq type='set' to='%s' id='si_sendfile24'>"
			"<si xmlns='http://jabber.org/protocol/si' profile='http://jabber.org/protocol/si/profile/file-transfer' id='%s'>"
			"<file xmlns='http://jabber.org/protocol/si/profile/file-transfer' size='%s' name='%s'>"
			"<range/>"
			"</file>"
			"<feature xmlns='http://jabber.org/protocol/feature-neg'>"
			"<x xmlns='jabber:x:data' type='form'>"
			"<field var='stream-method' type='list-single'>"
			"<option><value>http://jabber.org/protocol/bytestreams</value></option>"
			"</field>"
			"</x>"
			"</feature>"
			"</si>"
			"</iq>",victime,siid,sisize,siname);
			sendout(message_out);
			getin();
		sprintf(message_out,"<iq type='set' to='%s' id='si_sendfile34'>"
			"<query xmlns='http://jabber.org/protocol/bytestreams' mode='tcp' sid='%s'>"
			"<streamhost port='%s' host='%s' jid='%s'/>"
			"</query>"
			"</iq>",victime,siid,siport,sihost,sijid);
			//sendout(message_out);
			//getin();
		/*sprintf(message_out,"<iq type='set' to='%s' id='si_sendfile44'>"
			"<query xmlns='http://jabber.org/protocol/bytestreams' sid='%s'>"
			"<activate>%s</activate>"
			"</query>"
			"</iq>",proxy,siid,victime);*/
		}
	if (strstr(message_out,"cancel:") != NULL)
		{
		/*sprintf(message_out,"<iq type='result' id='%s' to='%s'><query xmlns='%s'/><error code='501' type='cancel'/></iq>",xmlid,xmlfromlong,xmlns);*/
		}
	if (strstr(message_out,"node:") != NULL)
		{
		sscanf(message_out,"node: %[^\n]",node);
		if (strcmp(disco,"") == 0)
			strcpy(disco,"disco#items");
		printf("---%s %s%s/%s: has variable(s):\n",jabberservername,"http://jabber.org/protocol/",disco,node);
		sprintf(message_out,"<iq type='get' id='5555' to='%s'><query xmlns='http://jabber.org/protocol/%s' node='%s'/></iq>",jabberservername,disco,node);
		}
	if (strstr(message_out,"vnode:") != NULL)
		{
		sscanf(message_out,"vnode: %s %s %s",node,disco,jabberservername);
		if (strcmp(disco,"") == 0)
			strcpy(disco,"http://jabber.org/protocol/disco#items");
		printf("---%s %s/%s: has variable(s):\n",jabberservername,disco,node);
		sprintf(message_out,"<iq type='get' id='5555' to='%s'><query xmlns='%s' node='%s'/></iq>",jabberservername,disco,node);
		}
	if (strstr(message_out,"get:") != NULL)
		{
		sscanf(message_out,"get: %s %s",disco,jabberservername);
		sprintf(message_out,"<iq type='get' id='5555' to='%s'><query xmlns='http://jabber.org/protocol/%s'/></iq>",jabberservername,disco);
		}
	if (strstr(message_out,"var:") != NULL)
		{
		sscanf(message_out,"var: %s %s",disco,jabberservername);
		sprintf(message_out,"<iq type='get' id='5555' to='%s'><query xmlns='%s'/></iq>",jabberservername,disco);
		}
	if (strstr(message_out,"pass:") != NULL)
		{
		sscanf(message_out,"pass: %s",option);
		sprintf(message_out,"<iq type='set' id='3615'><query xmlns='jabber:iq:auth'><username>%s</username><resource>%s</resource><password>%s</password></query></iq>",user,client,option);
		}
	if (strstr(message_out,"im:") != NULL)
		{
		sscanf(message_out,"im: %s %[^\n]",victime,option);
		sprintf(message_out,"<message to='%s' type='chat'>"
			"<x xmlns='jabber:x:event'><composing/></x>"
			"<body>%s</body></message>",victime,option);
		}
	if (strstr(message_out,"re:") != NULL)
		{
		sscanf(message_out,"re: %[^\n]",option);
		sprintf(message_out,"<message to='%s' type='chat'>"
			"<x xmlns='jabber:x:event'><composing/></x>"
			"<body>%s</body></message>",victime,option);
		}
	if (strstr(message_out,"sel:") != NULL)
		{
		sscanf(message_out,"sel: %s",option);
		if (strcmp(option,"") != 0)
			toc_select_buddy_online(option);
		}
	if (strstr(message_out,"seek:") != NULL)
		{
		sscanf(message_out,"seek: %s %s %[^\n]",option,option2,option3);
		//printf("toc_catch_buddy(%s)",option);
		if (strcmp(option,"") != 0)
			toc_catch_buddy(option,option2,option3);
		}
	if (strstr(message_out,"roster:") != NULL)
		{
		sscanf(message_out,"roster: %s",option2);
		if (strcmp(option2,"init") == 0)
			init_roster();
		toc_list_buddy();
		}
	if (strstr(message_out,"auth:") != NULL)
		{
		sscanf(message_out,"auth: %s %s %[^\n]",option,victime,option2);
		sprintf(message_out,"<iq type='set' id='auth'><query xmlns='jabber:iq:roster'><item jid='%s' subscription='%s'><group>%s</group></item></query></iq>",victime,option,option2);
		}
	if ((strstr(message_out,"nickserv:") != NULL))
		{
		sscanf(message_out,"nickserv: %s %s",option,jabberservername);
		sprintf(message_out,"<iq type='set' to='%s' id='register'><query xmlns='jabber:iq:register'><name>%s</name><key>%s</key></query></iq>",jabberservername,option,xmlkey);
		}
	if ((strstr(message_out,"reg:") != NULL) && (strstr(message_out,"unreg:") == NULL))
		{
		sscanf(message_out,"reg: %s %s %s",option,option2,jabberservername);
		if (strcmp(xmlkey,"") == 0)
			sprintf(message_out,"<iq type='set' to='%s' id='register'><query xmlns='jabber:iq:register'><username>%s</username><password>%s</password></query></iq>",jabberservername,option,option2);
		else
			sprintf(message_out,"<iq type='set' to='%s' id='register'><query xmlns='jabber:iq:register'><name>%s</name><key>%s</key></query></iq>",jabberservername,option,xmlkey);
		}
	if (strstr(message_out,"unreg:") != NULL)
		{
		sscanf(message_out,"unreg: %s",jabberservername);
		if (strcmp(user,"sir_mespompes") !=0 )
		sprintf(message_out,"<iq type='set' to='%s' id='unregister'><query xmlns='jabber:iq:register'><remove/></query></iq>",jabberservername);
		else
		printf("UNAUTHORIZED TO ERASE SIR_MESPOMPES!\n");			
		}
	if (strstr(message_out,"sub:") != NULL)
		{
		sscanf(message_out,"sub: %s %s",option,victime);
		sprintf(message_out,"<presence to='%s' type='%s'/>",victime,option);
		}
	if (strcmp(message_out,"stat") == 0)
		{
		printf("\tuser          : %s@%s/%s\n",user,remotehost,ressource);
		printf("\tclient        : %s\n",__FILE__);
		printf("\tpriority      : %s\n",prior);
		printf("\troomname room : %s (%s)\n",roomname,room);
		printf("\tdest          : %s\n",victime);
		printf("\toption        : %s\n",option);
		}
	if (strcmp(message_out,"help") == 0)
		{
		printf(
				"\tonline:                         (become online)\n"
				"\toffline:                        (become offline)\n"
				"\taway:    <message>              (become away with a message)\n"
				"\tprior:    <priority>            (define priority (-127 -> 127)\n"
				"\tlist:                           (list your online contacts)\n"
				"\troster:  [init]                 (list (or reload) your roster contacts)\n"
				"\tseek:    <part of jid>          (search a contact and use him with re:)\n"
				"\tsel:     <number>               (select a roster contact by its number)\n"
				"\tim:      <jid> <message>        (send message)\n"
				"\tre:      <message>              (answer to last message)\n"
				"\tproxy:   <server jid>           (select a bytestream proxy)\n"
				"\tsend:    \"file name\" <jid>      (send file)\n"
				"\toob:     \"link\" <jid>           (send link via oob)\n"
				"\tjoin:    <room jid> [ressource] (join chat room with optional name)\n"
				"\tnick:    <ressource>            (change name in muc)\n"
				"\tinvite:  <jid> [room jid]       (invite contact in chat room)\n"
				"\tch:      <message>              (send message in current chat)\n"
				"\tleave:   [jid]                  (join chat room)\n"
				"\tlast:    <jid>                  (ask last activity time)\n"
				"\tver:     <jid>                  (ask version)\n"
				"\titems:   <jid>                  (ask services)\n"
				"\tnode:    <jid>                  (show node)\n"
				"\tvar:     <namespace> [jid]      (make request with namespace)\n"
				"\tbrow:    <jid>                  (ask browse)\n"
				"\tinfo:    <jid>                  (ask disco#info)\n"
				"\tvcard:   <jid>                  (ask vcard)\n"
				"\tquit:    [message]              (quit this program!)\n"
				"\tauth:    none/to/from/both/remove <jid> [group]\n"
				"\tsub:     (un)subscribe(d)         <jid>\n"
				"\treg:     <username> <password> <server> (register on the server)\n"
				"\tunreg:   <server>                       (unregister on the server)\n"
				);
		}
strcpy(option,"");
strcpy(option2,"");
}


void flapon(void)
{
sprintf(message_out,
	"<?xml version='1.0' encoding='%s'?>"
	"<stream:stream xmlns='jabber:client' xmlns:stream='http://etherx.jabber.org/streams' xml:lang='en' version='1.0' to='%s'>"
	,ENCODING,domain);
sendout(message_out);
sequence_number_client=3615;
getin();
}


int sendout(char *message_out)
{
if ((debugmode==1) || (strstr(ressource,"debug") != NULL)){
	printf("%s\n",message_out);}	
//	buflen=0;
//	memset(&flap_out, 0, sizeof(flap_out));
if (strstr(message_out,"</stream:") != 0){
	exitclient();}
//if ((strlen(message_out))>7)
//if ((strstr(message_out,"</") != NULL) || (strstr(message_out,"<?xml") != NULL) || (strstr(message_out,"<stream") != NULL))
if ((strstr(message_out,"<") != NULL))
if(send(sock,message_out,strlen(message_out),0) == -1) {
	perror("send");
	exit(1);}
//		printf(" fin send out - buflen = %i (%i)\n",strlen(message_out), flap_out.data_length);	/* Resultat buffer lecture */
return(0);
}


void getin(void)
{
char sockbuf[12000];
//memset sockbuf a une grande taille, puis effacement de sockbuf)
		if((buflen=recv(sock,sockbuf, sizeof(sockbuf),0)) == -1)	/* Lire nouvelle co */
		 {
			printf(" brk  recv in - buflen = %i\n",buflen);		/* Si erreur lecture */
			perror("recv");
			exit(1);}
		//printf(" fin  recv in - buflen = %i (%i)\n",buflen, strlen(sockbuf));	/* Resultat buffer lecture */
		/*if ((debugmode==1) || (strstr(ressource,"debug") != NULL))
			printf("[%i bytes]\n",buflen);*/
		printbuf(sockbuf,buflen);
}



int si_sendout(char *message_out, int nb)
{
int i;

//strncat(message_out,"\n",5);
//printf("si_send...\n");
			for (i=0;i<=nb-1;i++){
			if ((debugmode==1) || (strstr(ressource,"debug") != NULL)){
			if (message_out[i] >= 32)
				printf("%c",message_out[i]);
			else
				printf(" %.2x ",message_out[i]);}
			}
			printf("sending %i bytes...\r",nb);

if ((debugmode==1) || (strstr(ressource,"debug") != NULL)){
	printf("%s\n",message_out);}	
if(send(si_sock,message_out,nb,0) == -1) {
	perror("send");
	exit(1);}
return(0);
}



void si_getin(char *name,char *host,int size)
{
int i;
FILE *fileget;
char sockbuf[1388];
int filecount;
int header;
	
//printf("si_get...\n");
filecount=0;
header=5+strlen(host)+2;
if (strcmp(name,"si-temp") != 0){
	fileget=fopen(name,"wb");
	filecount=filecount-header;}
	while (filecount < size){
		if((buflen=recv(si_sock,sockbuf, sizeof(sockbuf),0)) == -1)	/* Lire nouvelle co */
		 {
			printf(" brk  recv in - buflen = %i\n",buflen);		/* Si erreur lecture */
			perror("recv");
			return;}
		//printf(" fin  recv in - buflen = %i (%i)\n",buflen, strlen(sockbuf));	/* Resultat buffer lecture */
		//filecount=filecount+buflen;
		for (i=0;i<buflen;i++){
			if ((filecount >= 0) && (i < buflen)){
			//if (i >= (header) && (i < (buflen))){
				if (strcmp(name,"si-temp") != 0)
					putc(sockbuf[i],fileget);
				}
			if ((debugmode==1) || (strstr(ressource,"debug") != NULL)){
			if (sockbuf[i] >= 32)
				printf("%c",sockbuf[i]);
			else
				printf(" %.2x ",sockbuf[i]);}
		filecount++;
		}
		printf("receiving %i bytes (%i/%i)...\r",buflen,filecount,size);
		}
printf("\n");
if (strcmp(name,"si-temp") != 0)
	fclose(fileget);
}



void fd_getin(void)
{
	buflen=0;
	FD_ZERO(&onrecv_set);
	FD_SET(sock, &onrecv_set);
	tv.tv_sec = 1;
	tv.tv_usec = 0;
	if ((select(sock+1, &onrecv_set, NULL, NULL, &tv)) > 0)
		{
			if (FD_ISSET(sock, &onrecv_set))
				{
					getin();
				}
			}
}



int ansi_kbhit(void)
{
#if WINDOWS
	return(kbhit());
#else
//struct pollfd fds[1];
//int nfds;
//int timeout;
//int rvalue;

//fds[0].fd=0;        /* file descriptor - 0 is for STDIN */
//fds[0].events=POLLIN;    /* requested events */
//fds[0].revents=POLLIN;   /* returned events */


//poll(fds,1,1);
//			if ((fds[0].revents&&POLLIN)==POLLIN)
	FD_ZERO(&onrecv_set);
	FD_SET(STDIN, &onrecv_set);
	tv.tv_sec = 1;
	tv.tv_usec = 0;
	if ((select(STDIN+1, &onrecv_set, NULL, NULL, &tv)) > 0)
		{
			if (FD_ISSET(STDIN, &onrecv_set))
				{
				/*recv(STDIN,kbdbuf, sizeof(kbdbuf),0);
				printf("\"%s\" (%i)\n",kbdbuf,strlen(kbdbuf));
				if (strlen(kbdbuf) > 0)*/
				return(1);
				}
		}
return(0);
#endif
}




void printbuf(char *sockbuf,int length)
{
char html[15000];
char text[15000];
int inbalise,apostrophe;
int i, i_html;
//printf("message_out: %i / sockbuf: %i / text: %i\n",strlen(message_out),strlen(sockbuf),strlen(text));
//printf("lenght: %i\n",length);

i_html=0;
for(i=0; i<length; i++)
		{
			html[i_html]=sockbuf[i];
			i_html++;
			//printf("%c",sockbuf[i]);
		}

html[i_html]='\0';
inbalise=0;apostrophe=0;
i_html=0;
strcpy(text, "xml:\n");

for(i=0; i<= length; i++)
	{
	text[i_html]= html[i];
	if (inbalise==1 && html[i]=='\"')
		{text[i_html]= '\'';}
	if (inbalise==1 && html[i]=='>')
		{text[i_html]= '\n';i_html++;text[i_html]= '>';}
	if (inbalise==0 && html[i]=='\n' && html[i-1]!='<' && html[i-1]!='>')
		{text[i_html]= ' ';}
		/*{text[i_html]= '<';i_html++;text[i_html]= 'b';i_html++;text[i_html]= 'r';i_html++;text[i_html]= '>'}*/
	if (inbalise==1 && html[i]==' ' && apostrophe==0)
		{text[i_html]= '\n';}
	if (inbalise==1 && html[i-1]=='\'' && html[i]=='>')
		{text[i_html]= '\n';i_html++;text[i_html]= '>';}
	if (html[i-1]=='/' && html[i]=='>')
		{text[i_html]= '\n';i_html++;text[i_html]= '>';}
	if (html[i-1]=='>' && html[i]=='<')
		{text[i_html]= '\n';i_html++;text[i_html]= '<';}
	if (html[i-5]=='&' && html[i-4]=='q' && html[i-3]=='u' && html[i-2]=='o' && html[i-1]=='t' && html[i]==';')
		{i_html=i_html-5;text[i_html]= '\"';}
	if ((apostrophe == 0) && (html[i-5]=='&' && html[i-4]=='a' && html[i-3]=='p' && html[i-2]=='o' && html[i-1]=='s' && html[i]==';'))
		{i_html=i_html-5;text[i_html]= '\'';}
	if (html[i-4]=='&' && html[i-3]=='a' && html[i-2]=='m' && html[i-1]=='p' && html[i]==';')
		{i_html=i_html-4;text[i_html]= '&';}
		i_html++;

	if (html[i]=='<' && inbalise==0)
		inbalise=1;
	if (html[i]=='>' && inbalise==1)
		inbalise=0;
	if (html[i]=='\'' && inbalise==1)
		apostrophe=apostrophe ^ 1;


		}

//printf("\n");
text[i_html]= '\0';
getargs(text);
}





void getargs (char *text)
{
int pchv;
int text_change;
char *pch[1];
char xmlcommand[16],xmlto[128],xmlfromlong[128],xmlfrom[128],xmlfrombare[128],xmlbody[1024],xmlcs[16],xmlavailable[16];
char xmlcode[5],xmlshow[5],xmlstatus[300],xmljid[64],xmlitems[8000],xmlid[256],xmlns[80],xmlnode[128],xmlext[80];
char xmlsubscription[8],xmlsubscription_ask[16];
char xmltype[64],xmlcategory[64],xmlname[64];
char xmlrole[64],xmlchatgrade[128],xmlfromfirst[128];
long unsigned int xmluptime;
char xmlvername[128],xmlverversion[128],xmlveros[128];
char FN[64],NICKNAME[64],USERID[64],EMAIL[64],URL[64],ROLE[64],BDAY[32],HOME[64],LOCALITY[64],REGION[64],PCODE[32],CTRY[64],NUMBER[32],DESC[1024];
char xmlns_x[15], xmlcommand_x[15], xmljid_x[64], from_x[128];
	

/*printf("%s\n",text_old);
printf("============================================================\n");
printf("%s\n",text);
printf("============================================================\n");*/
text_change=1;
if (antifloodmode == 1){
	if ((strcmp(text,text_old) != 0) || (strcmp(text_old,"") == 0)) {
		strcpy(text_old,text);
		text_change=1;}
		else
		text_change=0;
}
	
strcpy(xmlcommand,"none");
pchv=0;
pch[pchv] = strtok (text,"\n");
/*if (debugmode==1)
	printf("%s\t%s\n",xmlcommand,pch[pchv]);// DEBUG*/
//pchv++;
if (text_change==1)
{
while (pch[pchv] != NULL)
{
	if (pch[pchv] == NULL)
		break;
	if ((debugmode==1) || (strstr(ressource,"debug") != NULL)){
		printf("%s",xmlcommand/*,xmlns,xmlid*/);
		for (value=/*strlen(xmlid)+strlen(xmlns)+*/strlen(xmlcommand);value<10;value++)
			printf(" ");
		printf("%s\n",pch[pchv]);}// DEBUG
	if ((strstr(pch[pchv],"from=") != NULL) && (strcmp(xmlfrom,"") == 0)){
		sscanf(pch[pchv],"from='%[^@]",xmlfrom);
		sscanf(pch[pchv],"from='%[^']",xmlfromlong);//
		sscanf(pch[pchv],"from='%[^/]",xmlfrombare);
		if (strstr(pch[pchv],"'/") != NULL){
			strcpy(pch[pchv],"</presence");}
		if (strstr(xmlfrom,"'") != NULL)
			sscanf(xmlfrom,"%[^']",xmlfrom);
		if (strstr(xmlfromlong,"/") != NULL)
			strcpy(xmlfromfirst,xmlfromlong);}
	if (strstr(pch[pchv],"to=") != NULL) {
		sscanf(pch[pchv],"to='%[^']",xmlto);
		if (strstr(pch[pchv],"'/") != NULL)
			strcpy(pch[pchv],"</presence");}
	if (strstr(pch[pchv],"xml:lang=") != NULL) {
		if (strcmp(xmlcommand,"presence") == 0)
		if (strstr(pch[pchv],"'/") != NULL)
			strcpy(pch[pchv],"</presence");}
	if (strcmp(pch[pchv],"<stream:error") == 0){
		strcpy(xmlcode,"");
		strcpy(xmlbody,"");
		strcpy(xmlcommand,"stream");}
	if (strcmp(pch[pchv],"<stream:stream") == 0){
		strcpy(xmlbody,"");
		strcpy(xmlcommand,"stream");}
	/* Stream errors */
	if (strcmp(pch[pchv],"<x") == 0) {
		strcpy(xmlcommand_x,"x");}
	if (strcmp(xmlcommand_x,"x") == 0){
		//if (strstr(pch[pchv],"'/") != NULL){
		//	strcpy(pch[pchv],"'</x"); //strcpy(xmlcommand_x,"");}
		//	strcpy(xmlcommand_x,"");}
		if (strstr(pch[pchv],"jid=") != NULL)
			sscanf(pch[pchv],"jid='%[^']",xmljid_x);
		if (strstr(pch[pchv],"xmlns=") != NULL)
			sscanf(pch[pchv],"xmlns='%[^']",xmlns_x);
		if (strstr(pch[pchv],"from=") != NULL)
			sscanf(pch[pchv],"from='%[^']",from_x);	}
	if (strcmp(pch[pchv],"<c") == 0) {
		strcpy(xmlcommand_x,"c");}
	if (strcmp(xmlcommand_x,"c") == 0){
		if (strstr(pch[pchv],"'/") != NULL)
			strcpy(pch[pchv],"'</c");
		if (strstr(pch[pchv],"jid=") != NULL)
			sscanf(pch[pchv],"jid='%[^']",xmljid_x);
		if (strstr(pch[pchv],"xmlns=") != NULL)
			sscanf(pch[pchv],"xmlns='%[^']",xmlns_x);
		if (strstr(pch[pchv],"from=") != NULL)
			sscanf(pch[pchv],"from='%[^']",from_x); }
	if (strcmp(pch[pchv],"<iq") == 0) {
		strcpy(xmlcommand,"query");
		strcpy(xmlcommand_x,"");
		strcpy(xmlns_x,"");
		strcpy(xmljid_x,"");
		strcpy(from_x,"");
		//strcpy(sihost,"");
		//strcpy(siport,"33000");
		strcpy(xmlfrom,"");
		strcpy(xmlfromlong,"");
		strcpy(xmlfrombare,"");
		strcpy(xmlavailable,"");
		strcpy(xmlbody,"");
		strcpy(xmlcode,"");
		strcpy(xmlid,"");
		strcpy(xmlns,"");
		strcpy(xmlnode,"");
		strcpy(xmltype,"");
		strcpy(xmlcategory,"");
		//strcpy(xmlkey,"");
		strcpy(xmlname,"");
		strcpy(xmlitems,"");
		strcpy(xmlvername,"");
		strcpy(xmlverversion,"");
		strcpy(xmlveros,"");
		strcpy(FN,"");
		strcpy(NICKNAME,"");
		strcpy(USERID,"");
		strcpy(EMAIL,"");
		strcpy(URL,"");
		strcpy(ROLE,"");
		strcpy(BDAY,"");
		strcpy(HOME,"");
		strcpy(LOCALITY,"");
		strcpy(REGION,"");
		strcpy(PCODE,"");
		strcpy(CTRY,"");
		strcpy(NUMBER,"");
		strcpy(DESC,"");
		}
	if (strcmp(pch[pchv],"<presence") == 0) {
		strcpy(xmlfrom,"");
		strcpy(xmlns,"");
		strcpy(xmlcommand,"presence");
		strcpy(xmlcommand_x,"");
		strcpy(xmlns_x,"");
		strcpy(xmljid_x,"");
		strcpy(from_x,"");
		strcpy(xmlext,"");
		strcpy(xmlstatus,"");
		strcpy(xmlbody,"");
		strcpy(xmlcode,"");
		strcpy(xmlchatgrade,"");
		strcpy(xmlavailable,"online");}
	if (strcmp(pch[pchv],"type='set'") == 0)
		strcpy(xmlavailable,"send set");
	if (strcmp(pch[pchv],"type='result'") == 0)
		strcpy(xmlavailable,"send result");
	if (strcmp(pch[pchv],"type='get'") == 0)
		strcpy(xmlavailable,"send request");
//	if (strcmp(pch[pchv],"type='subscribe'") == 0){
	if (strstr(pch[pchv],"type='subscribe'") != NULL){
		strcpy(xmlavailable,"send subscription");
		sscanf(pch[pchv],"type='%[^']",xmltype);
		if (strcmp(xmlcommand,"presence") == 0)
		if (strstr(pch[pchv],"'/") != NULL)
			strcpy(pch[pchv],"</presence");}
	if (strcmp(pch[pchv],"type='error'") == 0){
		strcpy(xmlbody,"");
		strcpy(xmlavailable,"error");
		strcpy(xmlcommand,"error");}
	if (strcmp(pch[pchv],"type='unavailable'") == 0)
		if (strcmp(xmlcommand,"presence") == 0)
		strcpy(xmlavailable,"quits");
	if (strstr(pch[pchv],"type='unavailable'/") != NULL){
		strcpy(xmlavailable,"quits");
		if (strcmp(xmlcommand,"presence") == 0)
		strcpy(pch[pchv],"</presence");}
	if (strstr(pch[pchv],"</show") != NULL) {
		sscanf(pch[pchv],">%[^<]</show",xmlshow);
		if (strcmp(xmlshow,"away") == 0)
			strcpy(xmlavailable,"is away");
		if (strcmp(xmlshow,"chat") == 0)
			strcpy(xmlavailable,"is free for chat");
		if (strcmp(xmlshow,"dnd") == 0)
			strcpy(xmlavailable,"is busy");
		if (strcmp(xmlshow,"xa") == 0)
			strcpy(xmlavailable,"is away");}
	if (strcmp(xmlns,"jabber:iq:register") ==0){
		strcpy(xmlcommand,"register");}
	if (strcmp(xmlns,"vcard-temp") ==0){
		strcpy(xmlcommand,"vcard");
		if (strstr(pch[pchv],"</FN") != NULL)
			sscanf(pch[pchv],">%[^<]</FN",FN);
		if (strstr(pch[pchv],"</NICKNAME") != NULL)
			sscanf(pch[pchv],">%[^<]</NICKNAME",NICKNAME);
		if (strstr(pch[pchv],"</EMAIL") != NULL)
			sscanf(pch[pchv],">%[^<]</EMAIL",EMAIL);
		if (strstr(pch[pchv],"</USERID") != NULL)
			sscanf(pch[pchv],">%[^<]</USERID",USERID);
		if (strstr(pch[pchv],"</URL") != NULL)
			sscanf(pch[pchv],">%[^<]</URL",URL);
		if (strstr(pch[pchv],"</ROLE") != NULL)
			sscanf(pch[pchv],">%[^<]</ROLE",ROLE);
		if (strstr(pch[pchv],"</BDAY") != NULL)
			sscanf(pch[pchv],">%[^<]</BDAY",BDAY);
		if (strstr(pch[pchv],"</HOME") != NULL)
			sscanf(pch[pchv],">%[^<]</HOME",HOME);
		if (strstr(pch[pchv],"</LOCALITY") != NULL)
			sscanf(pch[pchv],">%[^<]</LOCALITY",LOCALITY);
		if (strstr(pch[pchv],"</REGION") != NULL)
			sscanf(pch[pchv],">%[^<]</REGION",REGION);
		if (strstr(pch[pchv],"</PCODE") != NULL)
			sscanf(pch[pchv],">%[^<]</PCODE",PCODE);
		if (strstr(pch[pchv],"</COUNTRY") != NULL)
			sscanf(pch[pchv],">%[^<]</COUNTRY",CTRY);
		if (strstr(pch[pchv],"</NUMBER") != NULL)
			sscanf(pch[pchv],">%[^<]</NUMBER",NUMBER);
		if (strstr(pch[pchv],"</DESC") != NULL)
			sscanf(pch[pchv],">%[^<]</DESC",DESC);}
	if (strstr(pch[pchv],"var=") != NULL) {
		if (strcmp(xmlns,"http://jabber.org/protocol/disco#info") ==0){
			strcpy(xmlcommand,"info");
			sscanf(pch[pchv],"var='%[^']",xmljid);
			printf("%s\n",xmljid);}}
	if (strstr(pch[pchv],"subscription=") != NULL) {
		if (strcmp(xmlns,"jabber:iq:roster") ==0){
			strcpy(xmlcommand,"roster");
			sscanf(pch[pchv],"subscription='%[^']",xmlsubscription);
		if (strstr(pch[pchv],"'/") != NULL)
			strcpy(pch[pchv],"</item");}}
	if (strstr(pch[pchv],"ask=") != NULL) {
		if (strcmp(xmlns,"jabber:iq:roster") ==0){
			strcpy(xmlcommand,"roster");
			sscanf(pch[pchv],"ask='%[^']",xmlsubscription_ask);}}
	if (strstr(pch[pchv],"jid=") != NULL) {
		if (strcmp(xmlns,"") ==0){
			strcpy(xmlcommand,"(none yet)");
			sscanf(pch[pchv],"jid='%[^']",xmljid);
			printf("%s\n",xmljid);}
		if (strcmp(xmlns,"jabber:iq:browse") ==0){
			strcpy(xmlcommand,"browse");
			sscanf(pch[pchv],"jid='%[^']",xmljid);
			printf("%s\n",xmljid);}
		if (strcmp(xmlns,"jabber:iq:roster") ==0){
			strcpy(xmlcommand,"roster");
			sscanf(pch[pchv],"jid='%[^']",xmljid);
			if (strstr(pch[pchv],"'/") != NULL)
				strcpy(pch[pchv],"</item");
			/*toc_add_buddy(xmljid,"added");*/}
		if (strcmp(xmlns,"http://jabber.org/protocol/disco#items") ==0){
			strcpy(xmlcommand,"items");
			sscanf(pch[pchv],"jid='%[^']",xmljid);
			//sscanf(xmljid,"%[^@]",xmljid);
			printf("%s\n",xmljid);}}
	if ((strstr(pch[pchv],"</item") != NULL)){
		if (strcmp(xmlns,"jabber:iq:roster") ==0){
			strcpy(xmlcommand,"roster");
			//printf("%s [%s] %s\n",xmljid,xmlsubscription,xmlsubscription_ask);
			toc_add_buddy(xmljid,"added",xmlsubscription,xmlsubscription_ask);
			strcpy(xmlsubscription,"");
			strcpy(xmlsubscription_ask,"");}}
	if ((strstr(pch[pchv],"ext=") != NULL) && (strcmp(xmlcommand,"presence") == 0)){
			sscanf(pch[pchv],"ext='%[^']",xmlnode);
			if (strcmp(xmlext,"") != 0)
				strncat(xmlext," ",2);
			strncat(xmlext,xmlnode,80);
			/*printf("\b   ext: %s\n",xmlext);*/}
	if ((strstr(pch[pchv],"node=") != NULL) && (strcmp(xmlcommand,"presence") != 0)){
			sscanf(pch[pchv],"node='%[^']",xmlnode);
			printf("\b   node: %s\n",xmlnode);}
	if (strstr(pch[pchv],"name=") != NULL) {
		if (strcmp(xmlns,"") ==0){
			strcpy(xmlcommand,"(none yet)");
			sscanf(pch[pchv],"name='%[^']",xmljid);
			printf("\b = %s\n",xmljid);}
		if (strcmp(xmlns,"jabber:iq:browse") ==0){
			strcpy(xmlcommand,"browse");
			sscanf(pch[pchv],"name='%[^']",xmljid);
			printf("\b = %s\n",xmljid);}
		if (strcmp(xmlns,"http://jabber.org/protocol/disco#items") ==0){
			strcpy(xmlcommand,"items");
			sscanf(pch[pchv],"name='%[^']",xmljid);
			printf("\b = %s\n",xmljid);}}
	if (strstr(pch[pchv],"</value") != NULL) {
		if (strcmp(xmlns,"http://jabber.org/protocol/disco#info") ==0){
			strcpy(xmlcommand,"info");
			sscanf(pch[pchv],">%[^<]</value",xmljid);
			printf("\b = %s\n",xmljid);}}
	if (strstr(pch[pchv],"role=") != NULL) {
		strcpy(xmlcommand,"presence");
		sscanf(pch[pchv],"role='%[^']",xmlrole);
		sscanf(xmljid,"%[^@]",xmljid);
		if (strcmp(xmlstatus,"") == 0)
			sprintf(xmlchatgrade,"%s as %s",xmlavailable,xmlrole/*,xmljid*/);
		else
			sprintf(xmlchatgrade,"%s as %s (%s)",xmlavailable,xmlrole,xmlstatus/*,xmljid*/);}
	if ((strstr(pch[pchv],"id='") != NULL) && (strstr(pch[pchv],"jid='") == NULL)){
		if (strcmp(xmlid,"") == 0)
			sscanf(pch[pchv],"id='%[^']",xmlid);}
	if (strcmp(xmlns,"http://jabber.org/protocol/bytestreams") == 0){
		if (strcmp(xmlcommand,"streamhost-used") != 0)
			strcpy(xmlcommand,"bytestreams");
		if ((strstr(pch[pchv],"jid='") != NULL)){
			if (strstr(pch[pchv],"proxy") != NULL){
				sscanf(pch[pchv],"jid='%[^']",sijid);}
			else
				{sscanf(pch[pchv],"jid='%[^']",sijid);}}}
	if (strcmp(xmlns,"http://jabber.org/protocol/si") == 0){
		if ((strstr(pch[pchv],"id='") != NULL) && (strstr(pch[pchv],"jid='") == NULL))
			sscanf(pch[pchv],"id='%[^']",siid);
		if (strstr(pch[pchv],"name='") != NULL)
			sscanf(pch[pchv],"name='%[^']",siname);
		if (strstr(pch[pchv],"size='") != NULL)
			sscanf(pch[pchv],"size='%[^']",sisize);}
	if ((strstr(pch[pchv],"xmlns='") != NULL) && (strcmp(xmlns,"") == 0)){
		sscanf(pch[pchv],"xmlns='%[^']",xmlns);
		if (strcmp(xmlns,"jabber:client") == 0)
			strcpy(xmlns,"");}
	if (strstr(pch[pchv],"type='") != NULL)
		sscanf(pch[pchv],"type='%[^']",xmltype);
	if (strstr(pch[pchv],"category='") != NULL){
		sscanf(pch[pchv],"category='%[^']",xmlcategory);
		/*strcpy(xmlcommand,"info")*/;}
	if (strstr(pch[pchv],"name='") != NULL)
		sscanf(pch[pchv],"name='%[^']",xmlname);
	if (strstr(pch[pchv],"code='") != NULL) {
		sscanf(pch[pchv],"code='%[^']",xmlcode);
		sprintf(xmlavailable,"error %s",xmlcode);}
	if (strstr(pch[pchv],"seconds='") != NULL) {
		sscanf(pch[pchv],"seconds='%lu'",&xmluptime);
		strcpy(xmlcommand,"uptime");}
	if (strstr(pch[pchv],"</key") != NULL)
		sscanf(pch[pchv],">%[^<]</key",xmlkey);
	if (strstr(pch[pchv],"</url") != NULL)
		sscanf(pch[pchv],">%[^<]</url",sihost);
	if (strstr(pch[pchv],"host='") != NULL)
		sscanf(pch[pchv],"host='%[^']",sihost);
	if (strstr(pch[pchv],"port='") != NULL)
		sscanf(pch[pchv],"port='%[^']",siport);
	if (strstr(pch[pchv],"</name") != NULL) {
		sscanf(pch[pchv],">%[^<]</name",xmlvername);
			if (strcmp(xmlns,"jabber:iq:version") ==0)
			strcpy(xmlcommand,"version");}
	if (strstr(pch[pchv],"</version") != NULL)
		sscanf(pch[pchv],">%[^<]</version",xmlverversion);
	if (strstr(pch[pchv],"</os") != NULL)
		sscanf(pch[pchv],">%[^<]</os",xmlveros);
/*	if (strcmp(pch[pchv],"<invite") == 0)
		strcpy(roomname,xmlfromlong);*/
	if (strcmp(pch[pchv],"<message") == 0){
		strcpy(xmlns,"");
		strcpy(xmlcs,"(not typing)");
		strcpy(xmlbody,"");
		strcpy(xmlcommand,"message");
		strcpy(xmlfrom,"");}
	if (strstr(pch[pchv],"type='groupchat'") != NULL)
		strcpy(xmlcommand,"chat");
	if (strstr(pch[pchv],"<streamhost-used") != NULL)
		strcpy(xmlcommand,"streamhost-used");
	if (strstr(pch[pchv],"</error") != NULL)
		sscanf(pch[pchv],">%[^<]</error",xmlbody);
	if (strstr(pch[pchv],"</body") != NULL)
		sscanf(pch[pchv],">%[^<]</body",xmlbody);
	if (strstr(pch[pchv],"<active") != NULL)
		strcpy(xmlcs,"(active)");
	if (strstr(pch[pchv],"<inactive") != NULL)
		strcpy(xmlcs,"(inactive)");
	if (strstr(pch[pchv],"<composing") != NULL)
		strcpy(xmlcs,"(composing)");
	if (strstr(pch[pchv],"</instructions") != NULL)
		sscanf(pch[pchv],">%[^<]</instructions",xmlbody);
	if (strstr(pch[pchv],"</str:text") != NULL)
		sscanf(pch[pchv],">%[^<]</str:text",xmlbody);
	if (strstr(pch[pchv],"</text") != NULL)
		sscanf(pch[pchv],">%[^<]</text",xmlbody);
	if (strstr(pch[pchv],"</status") != NULL)
		sscanf(pch[pchv],">%[^<]</status",xmlstatus);

/* Here are where the commands are interpreted */
	
	/* STREAM */
	if ((debugmode == 0) && (strstr(pch[pchv],"</stream:") != NULL)) {
		if (strstr(pch[pchv],"</stream:error") != NULL) {
			sprintf(xmlfrom,"stream:error");
			sprintf(xmlavailable,"error %s (%s)",xmlcode,xmlbody);
			printf("---%s: %s %s---\n",xmlfrom,xmlavailable,xmlns);
		}
		if (strstr(pch[pchv],"</stream:stream") != NULL) {
			sprintf(xmlfrom,"stream:stream");
			printf("\n\tYou've been disconnected.\n");
			exitclient();
		}
	}
	

	/* PING */
	if (strcmp(pch[pchv],"\t") == 0) {
		if (strcmp(xmlcommand,"none") == 0)
			strcpy(xmlcommand,"\\t ping");}
	if (strcmp(pch[pchv],"\r") == 0) {
		if (strcmp(xmlcommand,"none") == 0)
			strcpy(xmlcommand,"\\r ping");}
	if (strcmp(pch[pchv],"\n") == 0) {
		if (strcmp(xmlcommand,"none") == 0)
			strcpy(xmlcommand,"\\n ping");}
	if (strcmp(pch[pchv]," ") == 0) {
		if (strcmp(xmlcommand,"none") == 0)
			strcpy(xmlcommand,"' ' ping");}
	if (strstr(xmlcommand,"ping") != NULL)
		if ((debugmode==1) || (strstr(ressource,"debug") != NULL)){
			printf("%s\n",xmlcommand/*,xmlns,xmlid*/);}
	if (strstr(pch[pchv],"urn:xmpp:ping") != NULL) {
		if (strcmp(xmlcommand,"none") == 0)
			strcpy(xmlcommand,"URN ping");}

			
	/* IQ */
	if (strcmp(pch[pchv],"</iq") == 0) {
			if (strcmp(xmlcommand,"error") == 0) {
				if (strcmp(xmlbody,"") == 0){
				if (strcmp(xmlcode,"401") == 0)
					strcpy(xmlbody,"Not authorized");
				if (strcmp(xmlcode,"404") == 0)
					strcpy(xmlbody,"Not found");
				if (strcmp(xmlcode,"405") == 0)
					strcpy(xmlbody,"Not allowed");
				if (strcmp(xmlcode,"501") == 0)
					strcpy(xmlbody,"Not implemented");
				if (strcmp(xmlcode,"502") == 0)
					strcpy(xmlbody,"Remote Server Error");
				if (strcmp(xmlcode,"503") == 0)
					strcpy(xmlbody,"Service unavailable");}
				/*if (strcmp(xmlbody,"") == 0)
					strcpy(xmlbody,xmltype);*/
				sprintf(xmlavailable,"error %s (%s)",xmlcode,xmlbody);}
			if (strcmp(xmlcommand,"info") == 0) {
				sprintf(xmlavailable,"%s %s %s",xmltype,xmlcategory,xmlname);}
				//sprintf(xmlavailable,"%s %s %s, have variable(s):\n%s",xmltype,xmlcategory,xmlname,xmlitems);}
			if (strcmp(xmlcommand,"browse") == 0) {
				/*sprintf(xmlavailable,"have service(s):\n%s",xmlitems);*/}
			if (strcmp(xmlcommand,"items") == 0) {
				/*sprintf(xmlavailable,"have service(s):\n%s",xmlitems);*/}
			if (strcmp(xmlcommand,"roster") == 0) {
				sprintf(xmlavailable,"---operation on your roster---"/*,xmlitems*/);}
			if (strcmp(xmlcommand,"register") == 0) {
				sprintf(xmlavailable,"%s",xmlbody);}
			if (strcmp(xmlcommand,"vcard") == 0) {
				sprintf(xmlavailable,"vcard infos...\n"
				"Name        : %s\n"
				"Nickname    : %s\n"
				"Email       : %s\n"
				"User ID     : %s\n"
				"URL         : %s\n"
				"Role        : %s\n"
				"Birthday    : %s\n"
				"Home        : %s\n"
				"Locality    : %s\n"
				"Region      : %s\n"
				"Postal code : %s\n"
				"Country     : %s\n"
				"Phone       : %s\n"
				"Description : %s\n"
				"---"
				,FN,NICKNAME,EMAIL,USERID,URL,ROLE,BDAY,HOME,LOCALITY,REGION,PCODE,CTRY,NUMBER,DESC);}
			if (strcmp(xmlcommand,"uptime") == 0) {
				sprintf(xmlavailable,"is online/offline for %s",converse_temps(xmluptime));}
			if (strcmp(xmlcommand,"version") == 0) {
				sprintf(xmlavailable,"is using %s %s (%s)",xmlvername,xmlverversion,xmlveros);}
			if (strcmp(xmlcommand,"streamhost-used") == 0) {
				sprintf(xmlavailable,"%s sent",siname);
				connectsock_si_send(siname,sisize,sihost,siport,siid,sijid,xmlto,xmlfromlong);}
			if (strcmp(xmlcommand,"bytestreams") == 0) /*&& (strcmp(xmlfromlong,proxy) == 0))*/{
				printf("%s: use server %s:%s (%s)\n",xmlfrom,sihost,siport,sijid);
				}
				
				
			/* IQ SEND RESULT */
			/*if (strcmp(xmlavailable,"send result") == 0) {
								
			}*/
				
				
			/* IQ SEND REQUEST */
			if (strcmp(xmlavailable,"send request") == 0) {
				if (strcmp(xmlns,"jabber:iq:version") == 0){
					sprintf(message_out,"<iq type='result' id='%s' to='%s'><query xmlns='%s'><name>%s</name><version>%s</version><os>%s</os></query></iq>",xmlid,xmlfromlong,xmlns,CLIENT_NAME,__FILE__,__OS__);
					sendout(message_out);}
				if (strcmp(xmlns,"jabber:iq:last") == 0){
					sprintf(message_out,"<iq type='result' id='%s' to='%s'><query xmlns='%s'><error code='501' type='cancel'/></query></iq>",xmlid,xmlfromlong,xmlns);
					sendout(message_out);}
				if (strcmp(xmlns,"jabber:iq:browse") == 0){
					sprintf(message_out,"<iq type='result' id='%s' to='%s'><query xmlns='%s'><error code='501' type='cancel'/></query></iq>",xmlid,xmlfromlong,xmlns);
					sendout(message_out);}
				if (strcmp(xmlns,"urn:xmpp:ping") == 0){
					sprintf(message_out,"<iq type='result' id='%s' to='%s'><query xmlns='%s'><error code='501' type='cancel'/></query></iq>",xmlid,xmlfromlong,xmlns);
					sendout(message_out);}
				if (strcmp(xmlns,"jabber:iq:time") == 0){
					sprintf(message_out,"<iq type='result' id='%s' to='%s'><query xmlns='%s'><error code='501' type='cancel'/></query></iq>",xmlid,xmlfromlong,xmlns);
					sendout(message_out);}
				if (strcmp(xmlns,"http://jabber.org/protocol/disco#info") == 0){
					sprintf(message_out,"<iq type='result' id='%s' to='%s'><query xmlns='%s'>"
					"<identity category='client' type='pc' name='%s'/>"
					"<feature var='http://in.the.slyp.free.fr/jabber'/>"
					"<feature var='http://jabber.org/protocol/disco#info'/>"
					"<feature var='http://jabber.org/protocol/bytestreams'/>"
					"<feature var='http://jabber.org/protocol/si'/>"
					"<feature var='http://jabber.org/protocol/si/profile/file-transfer'/>"
					"<feature var='http://jabber.org/protocol/muc'/>"
					"<feature var='jabber:iq:version'/>"
					"</query></iq>",xmlid,xmlfromlong,xmlns,CLIENT_NAME);
					sendout(message_out);}
				}

			/* IQ SEND SET */
			if ((strcmp(xmlavailable,"send set") == 0) && (autoresult == 1)){
					sprintf(message_out,"<iq type='result' id='%s' to='%s'><query xmlns='%s'/></iq>",xmlid,xmlfromlong,xmlns);
					//sprintf(message_out,"<iq type='error' id='%s' to='%s'><query xmlns='%s'/><error code='503' type='cancel'><service-unavailable/></error></iq>",xmlid,xmlfromlong,xmlns);
					//sprintf(message_out,"<iq type='result' id='%s' to='%s'><query xmlns='%s'/><error code='501' type='cancel'/></iq>",xmlid,xmlfromlong,xmlns);
				if (strcmp(xmlns,"jabber:iq:oob") == 0){
					printf("%s: wants to send you %s\n",xmlfrom,sihost);
					}
				if ((strcmp(xmlns,"http://jabber.org/protocol/si") == 0) && (strcmp(xmlfromlong,proxy) != 0)){
					sprintf(message_out,"<iq type='result' id='%s' to='%s'>"
					"<si xmlns='http://jabber.org/protocol/si'>"
					"<feature xmlns='http://jabber.org/protocol/feature-neg'>"
					"<x xmlns='jabber:x:data' type='submit'>"
					"<field var='stream-method'>"
					"<value>http://jabber.org/protocol/bytestreams</value>"
					"</field>"
					"</x>"
					"</feature>"
					"</si>"
					"</iq>",xmlid,xmlfromlong);
					printf("%s: wants to send you the file %s (%s bytes)\n",xmlfrom,siname,sisize);
					}
				if ((strcmp(xmlns,"http://jabber.org/protocol/bytestreams") == 0) && (strcmp(xmlfromlong,proxy) != 0)){
					sprintf(message_out,"<iq type='result' to='%s' id='%s'>"
					"<query xmlns='http://jabber.org/protocol/bytestreams'>"
					"<streamhost-used jid='%s'/>"
					"</query></iq>",xmlfromlong,xmlid,sijid/*xmlfromlong,sihost,siport*/);
					//printf("%s: use server %s:%s (%s)\n",xmlfrom,sihost,siport,sijid);
					connectsock_si_receive(siname,sisize,sihost,siport,siid,sijid,xmlfromlong,xmlto);
					strcpy(message_out,"\t");
					}
				sendout(message_out);
				}
			printf("---%s: %s %s---\n",xmlfromlong,xmlavailable,xmlns);}
			
	/* INVITE */
/*	if ((strcmp(pch[pchv],"</invite") == 0) && (strcmp(xmlcommand,"error") != 0)){
			printf("%s: invite you in %s\n",xmlfrom,roomname);
			strcpy(victime,xmlfromlong);}*/
			
	/* X */
	if ((strstr(pch[pchv],"</x") != NULL) && (strcmp(xmlcommand_x,"x") == 0)){
		if (strcmp(xmlns_x,"vcard-temp:x:update") != 0)
		if (strcmp(xmlns_x,"jabber:x:signed") != 0)
		if (strcmp(xmlns_x,"jabber:x:delay") != 0)
		if (strcmp(xmlns_x,"jabber:x:data") != 0)
		if (strcmp(xmlns_x,"jabber:x:avatar") != 0)
		if (strcmp(xmlns_x,"jisp:x:jep-0038") != 0) // animated smileys (Pandion)
		if (strcmp(xmlns_x,"http://jabber.org/protocol/nick") != 0)
		if (strcmp(xmlns_x,"http://www.w3.org/1999/xhtml") != 0)
		if (strcmp(xmlns_x,"jabber:x:event") != 0)
		if (strcmp(xmlns_x,"jabber:x:conference") != 0)
		if (strcmp(xmlns_x,"http://jabber.org/protocol/muc") != 0)
		if (strcmp(xmlns_x,"http://jabber.org/protocol/muc#user") != 0)
			printf(
				"xmlfrom_x: %s\n"
				"xmlns_x  : %s\n"
				"xmljid   : %s\n",from_x,xmlns_x,xmljid_x);
//		if (strcmp(xmlns_x,"http://jabber.org/protocol/muc#user") == 0){
//			sprintf(xmlchatgrade,"%s as %s",xmlavailable,xmlrole/*,xmljid*/);
//			/*printf("%s: %s...\n",xmlfromfirst,xmlchatgrade);*/}
		if (strcmp(xmlns_x,"jabber:x:event") == 0){
			if (antifloodmode == 0)
			if (strcmp(xmlbody,"") == 0)
				printf("%s:> %s\n",xmlfrom,xmlcs);
			}
		if (strcmp(xmlns_x,"jabber:x:conference") == 0){
			printf("%s: invite you in %s\n",from_x,xmljid_x);
			strcpy(victime,xmlfromlong);
			strcpy(roomname,xmljid_x);
			}
		}
			
			
	/* MESSAGE */
	if ((strcmp(pch[pchv],"</message") == 0) /*&& (strcmp(xmlbody,"") != 0)*/) {
			if (strcmp(xmlcommand,"error") == 0) {
				if (strcmp(xmlbody,"") == 0){
				if (strcmp(xmlcode,"401") == 0)
					strcpy(xmlbody,"Not authorized");
				if (strcmp(xmlcode,"404") == 0)
					strcpy(xmlbody,"Not found");
				if (strcmp(xmlcode,"405") == 0)
					strcpy(xmlbody,"Not allowed");
				if (strcmp(xmlcode,"501") == 0)
					strcpy(xmlbody,"Not implemented");
				if (strcmp(xmlcode,"502") == 0)
					strcpy(xmlbody,"Remote Server Error");
				if (strcmp(xmlcode,"503") == 0)
					strcpy(xmlbody,"Service unavailable");}
				sprintf(xmlavailable,"error %s (%s)",xmlcode,xmlbody);
				printf("---%s: %s %s---\n",xmlfromlong,xmlavailable,xmlns);}
			if (strcmp(xmlcommand,"message") == 0){
				/*if ((strcmp(xmlns,"http://jabber.org/protocol/chatstates") == 0) ||
					(strcmp(xmlns,"jabber:x:event") == 0))
					printf("%s:> %s\n",xmlfrom,xmlcs);*/
				if (strcmp(xmlbody,"") != 0){
					printf("%s:>             \r",xmlfrom);
					printf("%s:> %s\n",xmlfrom,xmlbody);}
				strcpy(victime,xmlfromlong);
				toc_add_buddy(xmlfromlong,xmlavailable,"","");}
			if (strcmp(xmlcommand,"chat") == 0){
				/*if ((strcmp(xmlns,"http://jabber.org/protocol/chatstates") == 0) ||
					(strcmp(xmlns,"jabber:x:event") == 0))
					printf("%s:> %s\r",xmlfromlong,xmlcs);*/
				if (strcmp(xmlbody,"") != 0){
					printf("%s:>             \r",xmlfrom);
					printf("%s:> %s\n",xmlfromlong,xmlbody);}}
		}
	
	/* PRESENCE */
	if ((strcmp(pch[pchv],"</presence") == 0) && (notifmode == 1)) {
		if ((strcmp(xmlchatgrade,"") == 0) && (strcmp(xmlavailable,"send subscription") != 0)){
			if (strcmp(xmlcommand,"error") == 0) {
				if (strcmp(xmlbody,"") == 0){
				if (strcmp(xmlcode,"401") == 0)
					strcpy(xmlbody,"Not authorized");
				if (strcmp(xmlcode,"404") == 0)
					strcpy(xmlbody,"Not found");
				if (strcmp(xmlcode,"405") == 0)
					strcpy(xmlbody,"Not allowed");
				if (strcmp(xmlcode,"501") == 0)
					strcpy(xmlbody,"Not implemented");
				if (strcmp(xmlcode,"502") == 0)
					strcpy(xmlbody,"Remote Server Error");
				if (strcmp(xmlcode,"503") == 0)
					strcpy(xmlbody,"Service unavailable");}
				sprintf(xmlavailable,"error %s (%s)",xmlcode,xmlbody);}
		if (antifloodmode==1)
			if ((strcmp(xmlcode,"404") != 0) && (strcmp(xmlcode,"502") != 0)){
			printf("***%s",xmlfromlong);
			if ((strcmp(xmlext,"") != 0) && (strcmp(xmlcommand,"presence") == 0))
				printf(" [%s]",xmlext);
			printf(": %s",xmlavailable);
			if ((strcmp(xmlstatus,"") != 0) && (strcmp(xmlcommand,"presence") == 0))
				printf(" (%s)",xmlstatus);
			printf("***\n");
			if ((strcmp(xmlcode,"404") != 0) && (strcmp(xmlcode,"502") != 0))
				toc_add_buddy(xmlfromlong,xmlavailable,"","");
			}
		if (antifloodmode==0)
			{
			printf("***%s",xmlfromlong);
			if ((strcmp(xmlext,"") != 0) && (strcmp(xmlcommand,"presence") == 0))
				printf(" [%s]",xmlext);
			printf(": %s",xmlavailable);
			if ((strcmp(xmlstatus,"") != 0) && (strcmp(xmlcommand,"presence") == 0))
				printf(" (%s)",xmlstatus);
			printf("***\n");
			if ((strcmp(xmlcode,"404") != 0) && (strcmp(xmlcode,"502") != 0))
				toc_add_buddy(xmlfromlong,xmlavailable,"","");
			}
		}
		if (strcmp(xmlavailable,"send subscription") == 0){
			toc_add_buddy(xmlfromlong,xmlavailable,"","");
			printf("***%s: %s: %s***\n",xmlfromlong,xmlavailable,xmltype);}
		/* CHAT */
		if (strcmp(xmlchatgrade,"") != 0)
			printf("%s: %s...\n",xmlfromfirst,xmlchatgrade);
		}

	pch[pchv] = strtok (NULL,"\n");
} // text_old
}


}




char *converse_temps(long unsigned int uptime)
{
#if WINDOWS
	char converse_uptime[128];
#else /* UNIX */
	char *converse_uptime;
#endif
long unsigned int save_uptime;
int converse_uptime_days;
int converse_uptime_h;
int converse_uptime_mn;
int converse_uptime_s;

save_uptime=uptime;
converse_uptime_days=abs(uptime/60/60/24);
uptime=uptime-converse_uptime_days*(60*60*24);
converse_uptime_h   =abs(uptime/60/60);
uptime=uptime-converse_uptime_h*(60*60);
converse_uptime_mn  =abs(uptime/60);
uptime=uptime-converse_uptime_mn*(60);
converse_uptime_s   =abs(uptime);

sprintf(converse_uptime,"%i day(s) %02dh%02dmn%02ds",converse_uptime_days,converse_uptime_h,converse_uptime_mn,converse_uptime_s);
//printf("converse_uptime: <%s>\n",converse_uptime);
uptime=save_uptime;
return(converse_uptime);
}



short nbo_short(short value)
{
/* conversion d'un short en Network Byte Order short */
return(htons(value));
}


void toc_signon(void)
{
if (strstr(ressource,"nologin") == NULL){
	sprintf(message_out,"<iq type='set' id='3615'><query xmlns='jabber:iq:auth'><username>%s</username><resource>%s</resource><password>%s</password></query></iq>",user,ressource,pass);
	if (strstr(ressource,"invisible") != NULL)
		sprintf(message_out,"<iq type='set' id='3615'><query xmlns='jabber:iq:auth'><username>%s</username><resource>%s</resource><password>%s</password></query></iq>",user," ",pass);
	sendout(message_out);
	getin();
	//sprintf(message_out,"<starttls xmlns='urn:ietf:params:xml:ns:xmpp-tls'/><mechanism>PLAIN</mechanism></starttls>");
	//sprintf(message_out,"<starttls xmlns='urn:ietf:params:xml:ns:xmpp-tls'/>");
	//sendout(message_out);
	//getin();
}

victimes_nb=0;
strcpy(victimes[0],"");
}


void toc_init_done(void)
{
printf("\n\tCONNECTED as %s@%s on %s\n\n",user,domain,remotehost);
if (strstr(ressource,"nologin") == NULL){
sprintf(message_out,"<iq type='get' id='rosterget'><query xmlns='jabber:iq:roster'/></iq>");
sendout(message_out);
if (strcmp(ressource,"invisible") == 0)
	strcpy(message_out,"<presence type='invisible'></presence>");
else
	strcpy(message_out,"<presence></presence>");
sendout(message_out);
strcpy(text_old,"");}
}



void init_roster(void)
{
int i;

printf("reload roster...\n");
for (i=0;i<=victimes_nb+1;i++){
	strcpy(victimes[i],"");
	online_victimes[i]=0;
	strcpy(subscription_victimes[i],"");
	strcpy(subscription_ask_victimes[i],"");
	}
	
victimes_nb=0;
sprintf(message_out,"<iq type='get' id='rosterget'><query xmlns='jabber:iq:roster'/></iq>");
sendout(message_out);
strcpy(message_out,"<presence type='unavailable'></presence>");
sendout(message_out);
strcpy(message_out,"<presence></presence>");
sendout(message_out);
}



void toc_select_buddy_online(char *option)
{
int i;

i=atoi(option)-1;
printf("You have selected this contact...\n"/*,victimes_nb*/);
strcpy(victime,victimes[i]);
strcpy(jabberservername,victimes[i]);
		printf("\t%02d %s (%d)\n",i+1,victimes[i],online_victimes[i]);
}



void toc_list_buddy_online(void)
{
int i;
int o=0;

printf("You have these contacts online...\n"/*,victimes_nb*/);
for (i=0;i<victimes_nb;i++)
	if (online_victimes[i]>=1){
	o++;
	if (online_victimes[i]>1)
		printf("\t%02d %s (%d)\n",i+1,victimes[i],online_victimes[i]);
	else
		printf("\t%02d %s\n",o,victimes[i]);}
}



void toc_list_buddy(void)
{
int i;

printf("You have %i contacts...\n",victimes_nb);
for (i=0;i<victimes_nb;i++)
	printf("\t%02d %s (%d) [%s] %s\n",i+1,victimes[i],online_victimes[i],subscription_victimes[i],subscription_ask_victimes[i]);
//	printf("\t%02d %s\n",i+1,victimes[i]);
}


void toc_add_buddy(char *xmlfromlong, char *xmlavailable, char *xmlsubscription, char *xmlsubscription_ask)
{
int i,i2;
int online;
int exist=0;

//printf("toc_add_buddy(%s,%s,%s,%s)\n",xmlfromlong,xmlavailable,xmlsubscription,xmlsubscription_ask);
	
if (strcmp(xmlavailable,"added") == 0)
	online=0;
	
if (
	(strcmp(xmlavailable,"online") == 0) ||
	(strcmp(xmlavailable,"is away") == 0) ||
	(strcmp(xmlavailable,"is free for chat") == 0) ||
	(strcmp(xmlavailable,"is busy") == 0) ||
	(strcmp(xmlavailable,"is idle") == 0)
	)
	online=1;
else
	online=0;
	
for (i=0;i<victimes_nb;i++)
	if (strcmp(victimes[i],xmlfromlong) == 0) {
		exist=1;
		if (online>0){
			if (strcmp(xmlsubscription,"") != 0)
				strcpy(subscription_victimes[i],xmlsubscription);
			if (strcmp(xmlsubscription_ask,"") != 0)
				strcpy(subscription_ask_victimes[i],xmlsubscription_ask);
			online_victimes[i]=online;}
		else
			{
			for (i2=i;i2<victimes_nb;i2++){
				strcpy(victimes[i2],victimes[i2+1]);
				strcpy(subscription_victimes[i2],subscription_victimes[i2+1]);
				strcpy(subscription_ask_victimes[i2],subscription_ask_victimes[i2+1]);
				online_victimes[i2]=online_victimes[i2+1];}
			victimes_nb--;
			}
				
	}

if (exist==0)
	{
	strcpy(victimes[victimes_nb],xmlfromlong);
	if (strcmp(xmlsubscription,"") != 0)
		strcpy(subscription_victimes[victimes_nb],xmlsubscription);
	if (strcmp(xmlsubscription_ask,"") != 0)
		strcpy(subscription_ask_victimes[victimes_nb],xmlsubscription_ask);
	online_victimes[victimes_nb]=online;
	victimes_nb++;
	}
}


void toc_catch_buddy(char *option,char *option2,char *option3)
{
int i;
for (i=0;i<victimes_nb;i++){
	//printf("Looking: %s in %s\n",option,victimes[i]);
	if ((strstr(victimes[i],option) != NULL) && (online_victimes[i]==0)){
		strcpy(victime,victimes[i]);
		strcpy(jabberservername,victimes[i]);
		printf("Found: %s\n",victime);
	if (strstr(victime,"/") == NULL)
	{
		if (strcmp(option2,"remove") == 0){
			sprintf(message_out,"<iq type='set' id='auth'><query xmlns='jabber:iq:roster'><item jid='%s' subscription='%s'></item></query></iq>",victime,option2);
			sendout(message_out);}
		if (strcmp(option2,"subscribe") == 0){
			sprintf(message_out,"<presence to='%s' type='%s'/>",victime,option2);
			sendout(message_out);}
		if (strcmp(option2,"subscribed") == 0){
			sprintf(message_out,"<presence to='%s' type='%s'/>",victime,option2);
			sendout(message_out);}
		if (strcmp(option2,"unsubscribe") == 0){
			sprintf(message_out,"<presence to='%s' type='%s'/>",victime,option2);
			sendout(message_out);}
		if (strcmp(option2,"unsubscribed") == 0){
			sprintf(message_out,"<presence to='%s' type='%s'/>",victime,option2);
			sendout(message_out);}
		if (strcmp(option2,"both") == 0){
			sprintf(message_out,"<iq type='set' id='auth'><query xmlns='jabber:iq:roster'><item jid='%s' subscription='%s'><group>%s</group></item></query></iq>",victime,option2,option3);
			sendout(message_out);}
		if (strcmp(option2,"from") == 0){
			sprintf(message_out,"<iq type='set' id='auth'><query xmlns='jabber:iq:roster'><item jid='%s' subscription='%s'><group>%s</group></item></query></iq>",victime,option2,option3);
			sendout(message_out);}
		if (strcmp(option2,"to") == 0){
			sprintf(message_out,"<iq type='set' id='auth'><query xmlns='jabber:iq:roster'><item jid='%s' subscription='%s'><group>%s</group></item></query></iq>",victime,option2,option3);
			sendout(message_out);}
		if (strcmp(option2,"none") == 0){
			sprintf(message_out,"<iq type='set' id='auth'><query xmlns='jabber:iq:roster'><item jid='%s' subscription='%s'><group>%s</group></item></query></iq>",victime,option2,option3);
			sendout(message_out);}
		}

	}}
for (i=0;i<victimes_nb;i++){
	//printf("Looking: %s in %s\n",option,victimes[i]);
	if ((strstr(victimes[i],option) != NULL) && (online_victimes[i]>=1)){
		strcpy(victime,victimes[i]);
		strcpy(jabberservername,victimes[i]);
		printf("Found: %s\n",victime);
	if (strstr(victime,"/") == NULL)
	{
		if (strcmp(option2,"remove") == 0){
			sprintf(message_out,"<iq type='set' id='auth'><query xmlns='jabber:iq:roster'><item jid='%s' subscription='%s'></item></query></iq>",victime,option2);
			sendout(message_out);}
		if (strcmp(option2,"subscribe") == 0){
			sprintf(message_out,"<presence to='%s' type='%s'/>",victime,option2);
			sendout(message_out);}
		if (strcmp(option2,"subscribed") == 0){
			sprintf(message_out,"<presence to='%s' type='%s'/>",victime,option2);
			sendout(message_out);}
		if (strcmp(option2,"unsubscribe") == 0){
			sprintf(message_out,"<presence to='%s' type='%s'/>",victime,option2);
			sendout(message_out);}
		if (strcmp(option2,"unsubscribed") == 0){
			sprintf(message_out,"<presence to='%s' type='%s'/>",victime,option2);
			sendout(message_out);}
		if (strcmp(option2,"both") == 0){
			sprintf(message_out,"<iq type='set' id='auth'><query xmlns='jabber:iq:roster'><item jid='%s' subscription='%s'><group>%s</group></item></query></iq>",victime,option2,option3);
			sendout(message_out);}
		if (strcmp(option2,"from") == 0){
			sprintf(message_out,"<iq type='set' id='auth'><query xmlns='jabber:iq:roster'><item jid='%s' subscription='%s'><group>%s</group></item></query></iq>",victime,option2,option3);
			sendout(message_out);}
		if (strcmp(option2,"to") == 0){
			sprintf(message_out,"<iq type='set' id='auth'><query xmlns='jabber:iq:roster'><item jid='%s' subscription='%s'><group>%s</group></item></query></iq>",victime,option2,option3);
			sendout(message_out);}
		if (strcmp(option2,"none") == 0){
			sprintf(message_out,"<iq type='set' id='auth'><query xmlns='jabber:iq:roster'><item jid='%s' subscription='%s'><group>%s</group></item></query></iq>",victime,option2,option3);
			sendout(message_out);}
		}
	}}
strcpy(message_out,"");
}


void connectsock(void)
{
/* Start Winsock */
#if WINDOWS
	WSADATA wsadata;
	WORD version = (1 << 8) + 1;  /* Version 1.1 */
	if (WSAStartup(version, &wsadata) != 0)
	{	perror("initializing winsock");
		exit(1);
	}
#endif

/*  Create socket  */
	if ( (sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{	perror("opening steam socket in");
		exit(1);
	}
	/*printf("create socket %i\n", sock);*/
/*  Name socket using wildcards  */
	hostname=gethostbyname(remotehost);
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr( inet_ntoa(*((struct in_addr *)hostname->h_addr)) );
	server_addr.sin_port = htons((unsigned short int) atoi(remoteport));  /* Use specified port */
	memset(server_addr.sin_zero, 0, 8);

/* resolve DNS and connecting socket */
	printf("connecting to: %s port %s\n", inet_ntoa(server_addr.sin_addr),remoteport);
	sin_size=sizeof(struct sockaddr_in);
	if(connect(sock,(struct sockaddr*)&server_addr, sin_size) == -1) {
		perror("connect");
		exit(1);}
}


int connectsock_si_send(char *name,char *size,char *sihost,char *siport,char *siid,char *sifrom,char *sifromlong,char *sito)
{
char si_message_out[1400];
FILE *tempfile;
int sizefile;
int i,c;
int bytestreams;
sizefile=atoi(size);
	
	/*  Create socket  */
	if ( (si_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{	perror("opening steam socket in");
		exit(1);
	}
	/*printf("create socket %i\n", sock);*/
/*  Name socket using wildcards  */
	si_hostname=gethostbyname(sihost);
	si_server_addr.sin_family = AF_INET;
	si_server_addr.sin_addr.s_addr = inet_addr( inet_ntoa(*((struct in_addr *)si_hostname->h_addr)) );
	si_server_addr.sin_port = htons((unsigned short int) atoi(siport));  /* Use specified port */
	memset(si_server_addr.sin_zero, 0, 8);

/* resolve DNS and connecting socket */
	printf("connecting to: %s port %s\n", inet_ntoa(si_server_addr.sin_addr),siport);
	sin_size=sizeof(struct sockaddr_in);
	if(connect(si_sock,(struct sockaddr*)&si_server_addr, sin_size) == -1) {
		perror("connect");
		return(1);}
	printf("connected! try to send file...\n");

if (strstr(sifrom,"proxy") != NULL){
	printf("%s is a proxy\n",sifrom);
	sprintf(si_message_out,"%c%c%c%c",0x05,0x02,0x00,0x02);
	si_sendout(si_message_out,4);}
	else{
	printf("%s is not a proxy\n",sifrom);
	sprintf(si_message_out,"%c%c%c",0x05,0x01,0x00);
	si_sendout(si_message_out,3);}
si_getin("si-temp",sihost,2);
//si_getin(name,sihost,sizefile);
if ((debugmode==1) || (strstr(ressource,"debug") != NULL))
	printf("make hash... %s%s%s\n",siid,sifromlong,sito);
sprintf(temp,"%s%s%s",siid,sifromlong,sito);
sha1sum(temp);
sprintf(si_message_out,"%c%c%c%c%c%s%c%c",0x05,0x01,0x00,0x03,/*0x28*/strlen(temp),temp,0x00,0x00);
si_sendout(si_message_out,47);
si_getin("si-temp",sihost,47);
sprintf(message_out,"<iq type='set' to='%s' id='si_sendfile44'>"
	"<query xmlns='http://jabber.org/protocol/bytestreams' sid='%s'>"
	"<activate>%s</activate>"
	"</query>"
	"</iq>",proxy,siid,victime);
sendout(message_out);
getin();
strcpy(message_out,"");
tempfile=fopen(name,"rb");/* == NULL){
	perror("fopen");
	return(1);}*/
c=0;
for (i=0;i<=sizefile;i++){
	//strcpy(si_message_out,"");
	
	//printf("%i\r",c);
	bytestreams=fgetc(tempfile);
	si_message_out[c]=bytestreams;
	//if ((sizefile-i) >= 4096)
		{
		if (c >= 1387){
			si_sendout(si_message_out,c+1);
	printf("sending %i bytes => (%i / %i)\r",c,i,sizefile);
			c=-1;}
		}
	//else
		{
		//if (c >= (sizefile-i)){
		if (i == sizefile){
			si_sendout(si_message_out,(c));
	printf("sending %i bytes => (%i / %i)\r",c,i,sizefile);
			c=-1;}
		}
	c++;
	}
fclose(tempfile);
return(0);
}



int connectsock_si_receive(char *name,char *size,char *sihost,char *siport,char *siid,char *sifrom,char *sifromlong,char *sito)
{
char si_message_out[384];
int sizefile;
sizefile=atoi(size);
	
	/*  Create socket  */
	if ( (si_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{	perror("opening steam socket in");
		exit(1);
	}
	/*printf("create socket %i\n", sock);*/
/*  Name socket using wildcards  */
	si_hostname=gethostbyname(sihost);
	si_server_addr.sin_family = AF_INET;
	si_server_addr.sin_addr.s_addr = inet_addr( inet_ntoa(*((struct in_addr *)si_hostname->h_addr)) );
	si_server_addr.sin_port = htons((unsigned short int) atoi(siport));  /* Use specified port */
	memset(si_server_addr.sin_zero, 0, 8);

/* resolve DNS and connecting socket */
	printf("connecting to: %s port %s\n", inet_ntoa(si_server_addr.sin_addr),siport);
	sin_size=sizeof(struct sockaddr_in);
	if(connect(si_sock,(struct sockaddr*)&si_server_addr, sin_size) == -1) {
		perror("connect");
		return(1);}
	printf("connected! try to get file...\n");

if (strstr(sifrom,"proxy") != NULL){
	printf("%s is a proxy\n",sifrom);
	sprintf(si_message_out,"%c%c%c%c",0x05,0x02,0x00,0x02);
	si_sendout(si_message_out,4);}
	else{
	printf("%s is not a proxy\n",sifrom);
	sprintf(si_message_out,"%c%c%c",0x05,0x01,0x00);
	si_sendout(si_message_out,3);}
si_getin("si-temp",sihost,2);
//si_getin(name,sihost,sizefile);
if ((debugmode==1) || (strstr(ressource,"debug") != NULL))
	printf("make hash... %s%s%s\n",siid,sifromlong,sito);
sprintf(temp,"%s%s%s",siid,sifromlong,sito);
sha1sum(temp);
sprintf(si_message_out,"%c%c%c%c%c%s%c%c",0x05,0x01,0x00,0x03,/*0x28*/strlen(temp),temp,0x00,0x00);
si_sendout(si_message_out,47);
//gets(temp);
sendout(message_out);
//gets(temp);
if (strstr(sifrom,"proxy") != NULL)
	si_getin(name,temp,sizefile);
else
	si_getin(name,sihost,sizefile);

return(0);
}



void filesize(char *siname)
{
FILE *tempfile;
long size;
	
if ((tempfile=fopen(siname,"rb")) == NULL)
	perror("fopen");
printf("%s (",siname);
fseek(tempfile,0,SEEK_END);
size=ftell(tempfile);
fclose(tempfile);
sprintf(sisize,"%li",size);
printf("%s bytes)\n",sisize);
}



void sha1sum(char *temp)
{
FILE *tempfile;
char tempfile_name[256],tempfile_name2[256];

tmpnam(tempfile_name);
if ((tempfile=fopen(tempfile_name,"wt")) == NULL)
	perror("fopen");
fputs(temp,tempfile);
fclose(tempfile);
tmpnam(tempfile_name2);
if ((debugmode==1) || (strstr(ressource,"debug") != NULL))
	printf("sha1sum %s > %s\n",tempfile_name,tempfile_name2);
#if WINDOWS
	sprintf(temp,"sha1sum.exe %s > %s",tempfile_name,tempfile_name2);
#else /* UNIX */
	sprintf(temp,"sha1sum %s > %s",tempfile_name,tempfile_name2);
#endif
system(temp);
tempfile=fopen(tempfile_name2,"rt");
fgets(temp,41,tempfile);
sscanf(temp,"%[^ ]",temp);
fclose(tempfile);
remove(tempfile_name);
remove(tempfile_name2);
//printf("Enter hash value: ");
//gets(temp);
}



void userlogin(void)
{
	printf("Login with %s ...\n",user);
	strcpy(prior,"0");
	strcpy(proxy,PROXY);
	debugmode=START_DEBUGMODE;
	autoresult=1;
	notifmode=1;
	antifloodmode=0;
	sprintf(userjid,"%s@%s/%s",user,domain,ressource);
}



void exitclient(void)
{
printf("\n\n\tThanks for try %s!\n\n",CLIENT_NAME);
#if WINDOWS
	closesocket(sock);
	WSACleanup();
#else /* UNIX */
	shutdown(sock,SHUT_RDWR);
#endif
exit(0);
}
