/****************************************************************************************************/
/*****************                       LISTE DES DEFINITIONS                      *****************/
/****************************************************************************************************/

message    =  [ ":" prefix SPACE ] command [ params ] crlf   (crlf <=> CR-LF <=> \r\n)
prefix     =  servername / ( nickname [ [ "!" user ] "@" host ] )
command    =  1*letter / 3digit
params     =  *14( SPACE middle ) [ SPACE ":" trailing ] =/ 14( SPACE middle ) [ SPACE [ ":" ] trailing ]
nospcrlfcl =  any octet except NUL, CR, LF, " " and ":"
middle     =  nospcrlfcl *( ":" / nospcrlfcl )
trailing   =  *( ":" / " " / nospcrlfcl )

target     =  nickname / server
msgtarget  =  msgto *( "," msgto )
msgto      =  channel / ( user [ "%" host ] "@" servername )
msgto      =/ ( user "%" host ) / targetmask
msgto      =/ nickname / ( nickname "!" user "@" host )

channel    =  ( "#" / "+" / ( "!" channelid ) / "&" ) chanstring [ ":" chanstring ]
channelid  =  lettres maj ou chiffres donc A-Z/0-9
chanstring =  any octet except NUL, BELL, CR, LF, " ", "," and ":"
servername =  hostname
host       =  hostname / hostaddr
hostname   =  shortname *( "." shortname )		/*	taille max de 63 chars	*/
shortname  =  ( letter / digit ) *( letter / digit / "-" ) *( letter / digit ) ; as specified in RFC 1123 [HNAME]
hostaddr   =  ip4addr / ip6addr
ip4addr    =  1*3digit "." 1*3digit "." 1*3digit "." 1*3digit
ip6addr    =  1*hexdigit 7( ":" 1*hexdigit )
ip6addr    =/ "0:0:0:0:0:" ( "0" / "FFFF" ) ":" ip4addr

nickname   =  ( letter / special ) *8( letter / digit / special / "-" )
user       =  1*( %x01-09 / %x0B-0C / %x0E-1F / %x21-3F / %x41-FF ) ; any octet except NUL, CR, LF, " " and "@"
key        =  1*23( %x01-05 / %x07-08 / %x0C / %x0E-1F / %x21-7F ) ; any 7-bit US_ASCII character, ; except NUL, CR, LF, FF, h/v TABs, and " "
letter     =  A-Z / a-z
digit      =  0-9
hexdigit   =  digit / "A" / "B" / "C" / "D" / "E" / "F"
special    =  "[", "]", "\"", "`", "_", "^", "{", "|", "}"

Le wilcard * peut tout valoir sauf NUL (0 en ascii) ni \ (92 en ascii) mais infinite de fois
Le wilcard ? peut tout valoir sauf NUL (0 en ascii) ni \ (92 en ascii) mais un seul character
Mais les wilcards ne sont pas a gerer forcement sur tous les servers IRC donc balec 

/****************************************************************************************************/
/*****************                            ECRITURE                              *****************/
/****************************************************************************************************/

Les noms de channel commencent par '&', '#', '+' ou '!' avec une longueur max de 50 chars et ne doit 
pas contenir d espaces (' '), de controle G (^G) ou de virgules (',').

Les espaces (' ') sont utilises comme separateur de parametres et les commandes sont une liste
avec comme separateur les virgules (',').

// En general, les noms de channel commencent par '#' ou '&'.
// En general, les string (comme par exemple un parametre <comment> ou <topic>) commencent par ':'.		J ai pas encore compris cette merde qui commence par quoi

On peut aussi utiliser les deux points (':') comme delimiteur d un channel mask (masque de canal en fr).
Un "channel mask" (ou masque de canal en français) est un modèle de nom de canal qui peut être utilisé
pour sélectionner plusieurs canaux à la fois. Les masques de canal sont souvent utilisés dans les
commandes IRC pour effectuer des opérations sur plusieurs canaux simultanément. Par exemple, le masque
de canal "#test*" sélectionnera tous les canaux dont le nom commence par "#test".

Chaque prefix caracterise un type de channel different. La definiton des types de channel n est pas
pertinente pour un protocol client-server et ainsi va au dela du cadre de la documentation IRC 2812.

A cause de son origine scandinave les caracteres {}|^ sont equivalents a leurs homonymes en miniscule []\~
et ca pose beaucoup de problemes dans la determinisation des equivalences entre nicknames ou nom de chan

Chaque message IRC est compose de 3 parties:
- Un prefix optionnel
- Une commande
- Les parametres commande dun montant max de 15

Le prefix, la commande et ses parametres sont separes d un espace (' ') chacun.
La presence d un prefix est indique par un deux-points (':') qui est forcement le premier caractere et il
n y a pas d espace entre les deux-points (':') et le prefix.

La commande doit etre une commande IRC valide

Les messages IRC ont une longueur max de 512 caracteres en comptant le CR-LF (\r-\n) de fin donc 510.
Donc il y a 510 caracteres max pour la commande et ses parametres.  

/****************************************************************************************************/
/*****************                             PREQUEL                              *****************/
/****************************************************************************************************/

COMMANDES a coder:

1) PASS NICK USER
2) PRIVMSG NOTICE
3) JOIN	PART (Chanop)MODE(surement juste le +o) TOPIC NAMES LIST INVITE KICK


Les replies RPL sont des reponses aux commandes donc quand elles fonctionnent alors que ERR
sont les erreurs des commandes.
Les replies ont un numero (entre 1 et 99 pour les reponses client-server)(200 et 399 pour les 
reponses aux commandes)(400 et 599 pour les erreurs de commande), un nom et une string qui indique
ce qu est la reponse.

!!!!!!!!
Si une erreur de type ERR_NOSUCHSERVER est renvoye, le server ne doit pas renvoyer d autres erreurs
!!!!!!!!
A part cette erreur, toutes les erreurs doivent etre renvoye quand il y en a plusieurs.


La commande PASS n est pas requise a la connection d un client mais requise avant utilisation
de NICK et USER qui servent a la connection d un user. (aussi avant SERVICE qui est une commande
serveur a serveur et donc pas a gerer)
Ordre recommande et donc non obligatoire:
1. PASS
2. NICK
3. USER

En revanche certains clients comme IRSSI appliquent ces 3 commandes de base sans qu on ait a le faire
donc on peut juste changer son nickname parce que c est "un" puis "deux"... de base.

Apres avoir renseigne PASS, NICK, USER il faut envoyer les 4 premiers reply RPL 001 002 003 004

/****************************************************************************************************/
/*****************                    COMMANDES DE BASE POUR USER                   *****************/
/****************************************************************************************************/

	PASS 
parametres: <password>
Erreurs possibles:
ERR_NEEDMOREPARAMS		461		"<command> :Not enough parameters"
ERR_ALREADYREGISTRED	462		":Unauthorized command (already registered)"



	NICK
parametres: <nickname>
Permet de donner ou changer le nom du user avec un max de 9 characters.

Erreurs possibles:
ERR_NONICKNAMEGIVEN		431		":No nickname given"       
ERR_ERRONEUSNICKNAME	432		"<nick> :Erroneous nickname"
ERR_NICKNAMEINUSE		433		"<nick> :Nickname is already in use"
ERR_NICKCOLLISION		436		"<nick> :Nickname collision KILL from <user>@<host>"
ERR_UNAVAILRESOURCE     437	    "<nick/channel> :Nick/channel is temporarily unavailable" (quand on essaye de join un chan bloque par le delai ou quand on essaye de changer un )
ERR_RESTRICTED			484		":Your connection is restricted!" (quand le mode +r est actif)



	USER
parametres: <user><mode><unused><realname>
Le parametre <user> a une longueur max de 9 characters.

Le parametre <unused> est litteralement non utilise qu on doit souvent remplir par un tiret
ou du coup un wilcard 

Le parametre <mode> est numerique et permet de set un mode automatiquement a l enregistrement
a un server.
Si le bit 2 est set on passe en mode w.
Le mode w (wallops) permet que les messages de type wallops soient envoye a tous les utilisateurs
avec le mode w actif.
(il suffit de decaller le nombre de 2 bit et comparer avec 1 
(ex: 9 >> 2 & 1 = 0 donc le bit 2 n est pas a 1) Le bit 2 etant en verite le 3eme bit
et vaut donc minimum 4 (100))
Si le bit 3 (minimum 8(1000)) est set on passe en mode i.
Le mode i (invisible) permet qu on ne le voit pas dans la liste des utilisateurs connecte au channel
ou au reseau sauf pour les autres utilisateurs qui l ont dans leur liste d ami. Mais le sujet ne
parle pas d amis donc nique.

<realname> peut contenir des espaces

exemple de USER: 
USER guest 8 * :Bob		On enregistre un user qui a pour username "guest" avec un realname "Bob"
						8>>2&1=0
						8>>3&1=1
						Donc on rajoute MODE+i
					
Erreurs possibles:	pareil que PASS
ERR_NEEDMOREPARAMS     
ERR_ALREADYREGISTRED


	MODE
Il va surement falloir faire +-i,w,o
parametres: <nickname> *( ( "+" / "-" ) *( "i" / "w" / "o" / "O" / "r" ) )

Le MODE user sert a faire avoir les privileges operateur serveur.
Un operateur serveur est une sorte de super operateur puisque peut influer sur le server lui meme mais 
aussi tous ses canaux.
Le mode w pour wallops n est pas obligatoire parce que pour le rendre possible il faut faire la commande
WALLOPS qui est optionnel pour tout vrai server IRC alors pour ft_IRC mdr 
Mais la commande WALLOPS est utilise pour envoye un message a tous les utilisateurs qui ont le mode w
actif.
Pareil pour le mode i qui n est utilise que dans WHO

Erreurs possibles:
ERR_NEEDMOREPARAMS			 
ERR_USERSDONTMATCH		502		":Cannot change mode for other users"  quand tu essayes de changer le mode d un utilisateur qui n est pas toi
ERR_UMODEUNKNOWNFLAG	501		":Unknown MODE flag"     

Reponse commande:
RPL_UMODEIS				221		"<user mode string>"	utiliser dans fonction WHOIS pour connaitre le mode utilisateur en parametre

/****************************************************************************************************/
/*****************                            MESSAGES                              *****************/
/****************************************************************************************************/

	PRIVMSG
parametres: <msgtarget> <text to be sent>
<msgtarget> est la cible qui recevra le <text to be sent> et peut etre un channel ou un <user>.
Ca peut etre autre chose mais pas a coder.
Les messages IRC sont limites a 510 chars avec \r\n a la fin

Erreurs possibles:
ERR_NORECIPIENT			411		":No recipient given (<command>)"          
ERR_NOTEXTTOSEND		412		":No text to send"
ERR_CANNOTSENDTOCHAN	404		"<channel name> :Cannot send to channel"		surement useless            
ERR_NOTOPLEVEL			413		"<mask> :No toplevel domain specified"	
ERR_WILDTOPLEVEL        414		"<mask> :Wildcard in toplevel domain"      
ERR_TOOMANYTARGETS		407		"<target> :<error code> recipients. <abort message>"	quand tu essayes d envoyer un message a trop de target (chan ou user) ou quand tu essayes de rejoindre un chan qui a plusieurs fois le meme nom avec son nom court
ERR_NOSUCHNICK			401		"<nickname> :No such nick/channel"
           
Reponse commande:
RPL_AWAY				301		"<nick> :<away message>" ca veut dire quand on envoie un message a qqun ca renvoie <pseudo>: le message



	NOTICE
parametres: <msgtarget> <text>
NOTICE est pareil que PRIVMSG mais ne recoit pas d erreurs. Ca sert a eviter les boucles inf
pour les reponses automatiques quand on recoit un message.

Reponse commande:
RPL_AWAY


/****************************************************************************************************/
/*****************                       CHANNEL OPERATIONS                         *****************/
/****************************************************************************************************/


	JOIN
parametres: ( <channel> *( "," <channel> ) [ <key> *( "," <key> ) ] ) / "0"
La commande join est utilisee pour creer un nouveau chan s il n a pas encore ete cree sinon juste
le rejoindre. Si on le cree on devient chanop de ce channel.

L argument special "0" (c est bien soit channel... soit 0 on ne peut pas mettre channel 0) fait en sorte
que le user quitte tous les channel ou il est actuellement membre.

L argument <key> est un mot de passe pour restreindre l acces a un channel.
On peut set une <key> avec MODE +k ou en creant le channel avec JOIN.

Erreurs possibles:
ERR_NEEDMOREPARAMS              
ERR_BANNEDFROMCHAN		474		"<channel> :Cannot join channel (+b)"
ERR_INVITEONLYCHAN      473 	"<channel> :Cannot join channel (+i)"
ERR_BADCHANNELKEY		475		"<channel> :Cannot join channel (+k)"
ERR_CHANNELISFULL       471    	"<channel> :Cannot join channel (+l)"   
ERR_BADCHANMASK			476		"<channel> :Bad Channel Mask"
ERR_NOSUCHCHANNEL               
ERR_TOOMANYCHANNELS		405		"<channel name> :You have joined too many channels"		on va surement mettre une limite a 10
ERR_TOOMANYTARGETS              
ERR_UNAVAILRESOURCE



	KICK
parametres: <channel> *( "," <channel> ) <user> *( "," <user> ) [<comment>]
On peut donc mettre autant de chan et de user a KICK qu on veut avec une potentielle raison <comment>
genre "il parle francais". La commande KICK utilise la commande PART. S il y a un commentaire on
affiche un message different de s il n y en a pas genre "bob has been kicked from hello" et avec un comp
on peut rajouter "because he speaks french bruh". Enfin normalement le message remplace completement
le message par defaut mais on verra.

Erreurs possibles:
ERR_NEEDMOREPARAMS              
ERR_NOSUCHCHANNEL		403		"<channel name> :No such channel"
ERR_BADCHANMASK         476    	"<channel> :Bad Channel Mask" 
ERR_CHANOPRIVSNEEDED	482		"<channel> :You're not channel operator"
ERR_USERNOTINCHANNEL	441		"<nick> <channel> :They aren't on that channel"         
ERR_NOTONCHANNEL		442		"<channel> :You're not on that channel"			quand tu essayes de kick qqun d un chan ou tu n es pas et donc ou tu n as pas de pouvoir


	PART
parametres: <channel> *( "," <channel> ) [ <Part Message> ]
Fait en sorte que le user qui utilise la fonction PART se fasse virer de la liste des channel en param.
Si <Part Message> est renseigne alors on remplace le message par defaut (qui est le nickname) par <Part
Message>.

Erreurs possibles:
ERR_NEEDMOREPARAMS              
ERR_NOSUCHCHANNEL
ERR_NOTONCHANNEL


	INVITE
parametres: <nickname> <channel>
Pas besoin d inviter sur un chan valide mais dans ce cas ca ne fait rien. 
Par defaut tout le monde peut utiliser la commande INVITE sauf si le chan est en mode invite only
auquel cas INVITE devient un privilege operateur.
En revanche il faut bien etre membre du channel anyway pour INVITE dedans.
Il n y a que l utilisateur qui invite et celui qui est invite qui recoivent la notification.
(Pas comme la commande MODE qui quand elle fait un truc, tout le monde est notifie)

Erreurs possibles:
ERR_NEEDMOREPARAMS              
ERR_NOSUCHNICK
ERR_NOTONCHANNEL                
ERR_USERONCHANNEL		443		"<user> <channel> :is already on channel"
ERR_CHANOPRIVSNEEDED

Reponses commande:
RPL_INVITING			341		"<channel> <nick>"             
RPL_AWAY


	TOPIC
parametres: <channel> [ <topic> ]
La commande topic sert a set, montrer ou supprimer un topic d un channel. 
On montre le topic si on ne met pas le parametre optionnel <topic>.
On supprime le topic si on met le parametre <topic> mais qu il vaut une string vide.
On set le topic si on renseigne le parametre optionnel <topic>.

Erreurs possibles:
ERR_NEEDMOREPARAMS              
ERR_NOTONCHANNEL
ERR_CHANOPRIVSNEEDED            
ERR_NOCHANMODES			477		"<channel> :Channel doesn't support modes"		je comprend pas cette erreur

Reponses commande:
RPL_NOTOPIC             331		"<channel> :No topic is set"        
RPL_TOPIC				332		"<channel> :<topic>"


	MODE
Le MODE dans les channel fait differentes choses, on peut ban des gens, lever la limite d utilisateur qui
peuvent rejoindre le channel... mais je pense qu il va suffire de faire le MODE +o -o qui sert a passer operateur
channel. 
En revanche pour passer operateur channel il faut qu un operateur channel nous octroie le MODE +o et du coup
pour etre operateur channel il y a un autre moyen, c est de JOIN un channel qui n existe pas encore pour le
creer et devenir operateur channel par defaut ou evidemment etre un operateur serveur.

Je pense qu on ne fera pas NAMES, LIST

#define ERR_NEEDMOREPARAMS " :Not enough parameters"
#define ERR_ALREADYREGISTRED 
#define ERR_NONICKNAMEGIVEN 
#define ERR_ERRONEUSNICKNAME 
#define ERR_NICKNAMEINUSE 
#define ERR_NICKCOLLISION 
#define ERR_UNAVAILRESOURCE 
#define ERR_RESTRICTED 
#define ERR_NOOPERHOST 
#define ERR_PASSWDMISMATCH 
#define ERR_USERSDONTMATCH 
#define ERR_UMODEUNKNOWNFLAG 
#define ERR_NORECIPIENT 
#define ERR_NOTEXTTOSEND 
#define ERR_CANNOTSENDTOCHAN 
#define ERR_NOTOPLEVEL 
#define ERR_WILDTOPLEVEL 
#define ERR_TOOMANYTARGETS 
#define ERR_NOSUCHNICK 
#define ERR_BANNEDFROMCHAN 
#define ERR_INVITEONLYCHAN 
#define ERR_BADCHANNELKEY 
#define ERR_CHANNELISFULL 
#define ERR_BADCHANMASK 
#define ERR_TOOMANYCHANNELS 
#define ERR_NOSUCHCHANNEL 
#define ERR_BADCHANMASK 
#define ERR_CHANOPRIVSNEEDED 
#define ERR_USERNOTINCHANNEL 
#define ERR_NOTONCHANNEL 
#define ERR_USERONCHANNEL 
#define ERR_NOCHANMODES 

enum ERR{
	ERR_NEEDMOREPARAMS = 461,
	ERR_ALREADYREGISTRED = 462,
	ERR_NONICKNAMEGIVEN = 431,
	ERR_ERRONEUSNICKNAME = 432,
	ERR_NICKNAMEINUSE = 433,
	ERR_NICKCOLLISION = 436,
	ERR_UNAVAILRESOURCE = 437,
	ERR_RESTRICTED = 484,
	ERR_NOOPERHOST = 491,
	ERR_PASSWDMISMATCH = 464,
	ERR_USERSDONTMATCH = 502,
	ERR_UMODEUNKNOWNFLAG = 501,
	ERR_NORECIPIENT = 411,
	ERR_NOTEXTTOSEND = 412,
	ERR_CANNOTSENDTOCHAN = 404,
	ERR_NOTOPLEVEL = 413,
	ERR_WILDTOPLEVEL = 414,
	ERR_TOOMANYTARGETS = 407,
	ERR_NOSUCHNICK = 401,
	ERR_BANNEDFROMCHAN = 474,
	ERR_INVITEONLYCHAN = 473,
	ERR_BADCHANNELKEY = 475,
	ERR_CHANNELISFULL = 471,
	ERR_BADCHANMASK = 476,
	ERR_TOOMANYCHANNELS = 405,
	ERR_NOSUCHCHANNEL = 403,
	ERR_BADCHANMASK = 476,
	ERR_CHANOPRIVSNEEDED = 482,
	ERR_USERNOTINCHANNEL = 441,
	ERR_NOTONCHANNEL = 442,
	ERR_USERONCHANNEL = 443,
	ERR_NOCHANMODES = 477
}

enum RPL{
	RPL_NOTOPIC = 331,
	RPL_TOPIC = 332,
	RPL_INVITING = 341,
	RPL_YOUREOPER = 381
}


Ensuite les commandes peuvent recevoir des arguments optionnel donc on ne peut
pas se permettre d avoir en parametre par exemple un std::string nickname et un 
channel channel pour la commande INVITE parce qu elle peut prendre un parametre 
optionnel <nom d utilisateur> pour specifier que nickname a ete invite par <nom
d utilisateur> dans channel.
En gros je pense qu il va falloir utiliser un bon vieux char **params :)

Non en vrai il vaut mieux avoir une classe pour les parametres puisqu ils 
fonctionnent tous pareil: prefix,commande,param et si on a bien cette classe 
param, on peut mettre la classe server et les classes clients et channel dedans direct
auquel cas on aura juste toujours en parametre de fonction command param param.