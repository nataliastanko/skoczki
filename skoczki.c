#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_PRZEDMIOT_LEN 200

// Funkcja cgi_name2value zwraca 0 jesli pobranie pola sie powiedzie i kod bledu
int cgi_name2value (const char *form_data, const char *name, char *value, int max_length);
/* Funkcja rozwin_procenty jest zdefiniowana w pliku name2value.o */
int rozwin_procenty (char *str);

void docheader ();
void head ();
void body ();
char *pobierz_form_data_metoda_get ();
void wypisz_form_data (const char *form_data);
void rules ();
void board (int pionki[8][8], int czyj_ruch);
void footer ();
void pionki_start (int pionki[8][8]);
int pionki_koniec (int pionki[8][8]);
void pobierz_info_ruch (const char *from_data);
void komunikat (char *komunikat);
void wypisz_form_info_ruch (int czyj_ruch, int pionki[8][8]);
void wykonaj_ruch (const char *form_data, int pionki[8][8], int *czyj_ruch);
int pobierz_ustawienie_pionkow (const char *form_data , int pionki[8][8]);
int czy_ruch (int cyfra_from, int litera_from, int cyfra_where, int litera_where);
int czy_skok (int cyfra_from, int litera_from, int cyfra_where, int litera_where, int pionki[8][8]);
// globalna
char tresc[200];

int main () {
  printf ("Content-type: text/html\n\n");

  // ustawienie zmiennej tresc
  strcpy(tresc, "Wykonaj ruch.");
  docheader ();
  printf ("<html xmlns=\"http://www.w3.org/1999/xhtml\" xml:lang=\"pl-PL\" lang=\"pl-PL\">\n");
  printf ("<head>\n");
  head ();
  printf ("</head>\n");
  printf ("<body>\n");
  body ();
  printf ("</body>\n");
  printf ("</html>\n");

  return 0;
}
void docheader () {
  printf ("<?xml version=\"1.0\" encoding=\"iso-8859-2\"?>\n");
  printf ("<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">\n");
}
void head () {
  printf ("<title>Skoczki, gra planszowa w CGI</title>\n");
  printf ("<meta http-equiv=\"Content-Type\" content=\"text/html; charset=iso-8859-2\" />\n");
  printf ("<link rel=\"stylesheet\" href=\"css/style.css\" type=\"text/css\" charset=\"utf-8\"/>\n");
  printf ("<script src=\"js/jquery-1.2.6.min.js\" type=\"text/javascript\" charset=\"utf-8\"></script>\n");
  printf ("<script src=\"js/skoczki.js\" type=\"text/javascript\" charset=\"utf-8\"></script>\n");
}
void body () {
  char *form_data;
  char value[3];
/* 0 - pole bia³e, pole puste, nieuzywane
 * 1 - pionek zielony
 * 2 - pionek czerwony
 * 3 - pole CZARNE, docelowe
 */
  int pionki[8][8]; // [cyfra] [litera]

  int d;
  int czyj_ruch;
  form_data = pobierz_form_data_metoda_get ();
  if (form_data == NULL) {
      printf ("Pobranie formy metoda GET nie powiodlo sie!\n");
      return;
  }
  // sprawdzenie czy jakas form data sie wyslala, jesli nie to zeruj pionki
  if ( 0 != cgi_name2value( form_data, "8a", value, 3 ) ) {
  pionki_start (pionki);
  // ruch dla zielonych pionkonw
  czyj_ruch=1;
  }

  //rozwin_procenty(form_data);
  rules ();
  if (pobierz_ustawienie_pionkow (form_data, pionki))
  wykonaj_ruch (form_data, pionki, &czyj_ruch);
  board (pionki, czyj_ruch);
  pobierz_info_ruch(form_data); // tu info o ruchu
  wypisz_form_info_ruch(czyj_ruch, pionki);
  footer ();
  //wypisz_form_data (form_data);

  free (form_data); 
}

void board (int pionki[8][8], int czyj_ruch) {
	int i=0;
	int j=0;
	
	printf ("<div id=\"board\">");
	printf ("<form method=\"get\" name=\"form-board\" id=\"plansza\">");
	printf ("<div id=\"ground\">\n");	
	
	for (i=8; i > 0; i--) {
		printf ("<div id=\"letter%d\"><p class=\"nr-position\">%d</p></div>\n",i,i);
		// kody liter
		for (j=0; j < 8; j++) {
			printf ("<div id=\"field%d%c\" class=\"pawn\">",i,j+97);
			// i od 8
			if (pionki[i-1][j] == 1) 
			  printf ("<img src=\"img/pawnb.png\" alt=\"pawnb\" />");
			else if (pionki[i-1][j] == 2)
 			  printf ("<img src=\"img/pawnc.png\" alt=\"pawnc\" />");
			printf ("<input type=\"hidden\" id=\"input_%d%c\" name=\"%d%c\" value=\"%d\" /></div>\n",i, j+97, i, j+97, pionki[i-1][j]);
		}
	}
		
	printf ("<div id=\"noletter\"></div>\n");

	for(i=0; i < 8; i++){
		printf ("<div class=\"letters\"><p>%c</p></div>\n",i+65);
	}

	printf ("<!-- hiddeny -->\n");	
	
	printf ("<input type=\"hidden\" name=\"from\" value=\"\" id=\"from\" />\n");
	printf ("<input type=\"hidden\" name=\"where\" value=\"\" id=\"where\" />\n");

	printf ("<input type=\"hidden\" id=\"whose_move\" name=\"whose_move\" value=\"%d\" />\n", czyj_ruch);

	printf ("</div><!--ground end -->");
	printf ("</form>");
	printf ("</div><!--board end -->	");
}

void wykonaj_ruch(const char *form_data, int pionki[8][8], int *czyj_ruch) {

int j,i;
// tablice, value pól // 2 elementy w srodku + \0
  char from[3];   char where[3];   char whose_move[2];
// wyciaganie wartosci z pol by atoi
  int litera_from, cyfra_from, litera_where, cyfra_where;

  if ( 0 == cgi_name2value (form_data, "whose_move", whose_move, 2 ) )
	*czyj_ruch= atoi(whose_move);

  if ( 0 == cgi_name2value (form_data, "from", from, 3 ) ) { 
    if ( strlen(from) == 0 )
	return; // brak pola potrzebnego 
    if ( 0 == cgi_name2value (form_data, "where", where, 3 ) ) {
	//wyciaganie  wartosci z value=""
	cyfra_from= (from[0]-'0')-1; // atoi,daje wartosc numeryczna, -1 do indexu
	cyfra_where= (where[0]-'0')-1;

  // zamiana wszystkich liter z tabolicy where na indexy, 104 to w ascii h
  for (i=0, j=97; j<105 ;i++,j++) {
	if ( where[1] == j )
	  litera_where= i;
	if ( from[1] == j )
	  litera_from= i;
  }

// dla czerwonych pionków
  if (*czyj_ruch == 2) {  //czy pionek byl czerwony
	if (pionki[cyfra_from][litera_from] == 2){ // czy pole, gdzie chce isc jest puste i czarne
	 if (pionki[cyfra_where][litera_where] == 3) { //bez skoku, ruch o 1 pole
	   if ( (!czy_ruch(cyfra_from, litera_from, cyfra_where, litera_where)) || (!czy_skok (cyfra_from, litera_from, cyfra_where, litera_where, pionki)) ){
		pionki[cyfra_where][litera_where]=2; // zamiana
		pionki[cyfra_from][litera_from]=3; 
		*czyj_ruch=1; // nastepny ruch dla zielonych
	   }
	  }
	}
  }
// dla zielonych pionków
  if (*czyj_ruch == 1) {
	if (pionki[cyfra_from][litera_from] == 1){
	 if (pionki[cyfra_where][litera_where] == 3) {
	  if ( (!czy_ruch(cyfra_from, litera_from, cyfra_where, litera_where)) || (!czy_skok (cyfra_from, litera_from, cyfra_where, litera_where, pionki)) ){
		pionki[cyfra_where][litera_where]=1;
		pionki[cyfra_from][litera_from]=3;	
		*czyj_ruch=2;
	   }
	 }	
	}
  }
   }
  }
}

/* zwraca 0 jesli ruch nieprawidlowy
 * jesli zwraca 1 jesli mozna wykonac taki ruch
 */
int czy_ruch (int cyfra_from, int litera_from, int cyfra_where, int litera_where) { // czy ruch jest mozliwy
	if ( ((cyfra_from+1 == cyfra_where) && (litera_from-1 == litera_where)) ||  
		((cyfra_from-1 == cyfra_where) && (litera_from-1 == litera_where)) ||  
		((cyfra_from+1 == cyfra_where) && (litera_from+1 == litera_where)) ||  
		((cyfra_from-1 == cyfra_where) && (litera_from+1 == litera_where)) 
	) return 0;
return 1;		
}

/* zwraca 0 jesli ruch nieprawidlowy
 * jesli zwraca 1 jesli mozna wykonac taki ruch
 */
int czy_skok (int cyfra_from, int litera_from, int cyfra_where, int litera_where, int pionki[8][8]) { // czy skok mozliwy
	if ( ( ((pionki[cyfra_from+1][litera_from-1] !=3) && (cyfra_from+2 == cyfra_where) && (litera_from-2 == litera_where)) ) ||  
		(((pionki[cyfra_from-1][litera_from-1] !=3) && (cyfra_from-2 == cyfra_where) && (litera_from-2 == litera_where)) )||  
		((pionki[cyfra_from+1][litera_from+1] !=3) && ((cyfra_from+2 == cyfra_where) && (litera_from+2 == litera_where)) )||  
		((pionki[cyfra_from-1][litera_from+1] !=3) && ((cyfra_from-2 == cyfra_where) && (litera_from+2 == litera_where)) )
	) return 0;
return 1;		
}

/* zwraca zero - ma blokowac ruch 
 * jesli zwraca 1 - wykonuje ruch
 */
int pobierz_ustawienie_pionkow (const char *form_data , int pionki[8][8]) {

int i,j;
char name[3];
char value[3];

for (i=1; i<=8; i++) {
	// a to 97, h to 104 w tabeli znakow ascii
	for (j=97; j<105; j++ ) {
	sprintf ( name , "%d%c", i, j );

	  if ( 0 == cgi_name2value (form_data, name, value, 3 ) ) 
		pionki[i-1][j-97]= atoi (value);
	}

// sprawdzamy czy wygra³y zielone
if (pionki_koniec(pionki) == 1) {
	// nalezy zablokowac ruch
	return 0;
}
// sprawdzamy czy wygraly czerwone
else if (pionki_koniec(pionki) == 2) {
	return 0;
}
}
// zwrocenie zera blokuje ruch i pozwala oglosic zywciezce w dalszej czesci programu
// rozne od zera umozliwia nastepny ruch
return 1;
}

void pobierz_info_ruch (const char *form_data) {

// tablice, value pól
  char from[3]; // 2 elementy w srodku + \0
  char where[3];

// wyciaganie wartosci z pol
  char litera_from;
  int cyfra_from;
  char litera_where;
  int cyfra_where;

  if ( 0 == cgi_name2value (form_data, "from", from, 3 ) ) { 
    if ( strlen(from) == 0 ) {
	strcpy(tresc, "Wybierz pionek.<br />");
	// informcja o ruchu
	return;
    }
    if ( 0 == cgi_name2value (form_data, "where", where, 3 ) ) {
	//wyciaganie  wartosci z value=""	
	cyfra_from= (from[0]-'0'); // atoi
	cyfra_where= (where[0]-'0'); // atoi
	litera_from= from[1];
	litera_where= where[1];
	sprintf(tresc, "Ostatni: <br /> Z pola <em>%s</em> na pole <em>%s</em> ", from, where);
	return;
    }
  }
}

void komunikat (char *komunikat) {
  printf ("%s\n", komunikat);
}

void wypisz_form_info_ruch (int czyj_ruch, int pionki[8][8]) {
	
// extern char *p_tresc;

 	printf ("\n<div id=\"move\"> ");
	printf ("<p>Informacje o ruchu:<br />");
   if (pionki_koniec(pionki) == 1) {
	strcpy(tresc, "Koniec gry! Zwyciêstwo zielonych pionków!<br />");
   }
// sprawdzamy czt wygraly czerwone
   else if (pionki_koniec(pionki) == 2) {
	strcpy(tresc, "Koniec gry! Zwyciêstwo czerwonych pionków!<br />");
}

   else if (czyj_ruch == 1)
	komunikat ("<p>Ruch dla zielonych pionków.</p>");
   else if (czyj_ruch == 2)
	komunikat ("<p>Ruch dla czerwonych pionków.</p>");
	printf ("<p>");
	komunikat (tresc);
	printf ("</p>");
	printf ("</div>\n");
}

void footer () {
	printf ("<div id=\"footer\">");
	printf ("<p class=\"small\">Projekt CGI - Natalia Stanko 2009 &copy; </p>");
	printf ("</div>");

	printf ("</div> <!-- container end -->\n"); 
}

void rules () {
	printf ("<div id=\"container\">");
	printf ("<div id=\"header\">Skoczki</div>");
	printf ("<div id=\"rules\">\n");
	printf ("<p class=\"text\">Gra dla dwóch osób, wymaga 8 pionów dla ka¿dego gracza. Grê rozpoczyna graj±cy zielonymi pionkami. Korzystamy tylko z czarnych pól.</p>");
	printf ("<p class=\"title\">Cel:</p>");
	printf ("<p class=\"text\">Przestawienie wszystkich swoich pionków na pozycje zajmowane na pocz±tku przez przeciwnika, czyli przeciwleg³e. Gracz, który pierwszy tego dokona - wygrywa.</p>");
	printf ("<p class=\"title\">Ruch polega na:</p>");
		printf ("<ul>");
		printf ("<li>przesuniêciu swojego pionka na dowolne s±siednie pole wolne uko¶nie w prawo lub w lewo (mo¿na cofaæ ruch)</li>");
		printf ("<li>przeskoczeniu przez pionki w³asne lub przeciwnika z pola bezpo¶rednio s±siaduj±cego uko¶nie z przeskakiwanym pionkiem na pole bezpo¶rednio za nim w linii prostej</li>");
//		printf ("<li>wykananiu ca³ej serii skoków jednym pionkiem zgodnie z dwiema poprzednimi zasadami - zmiana kierunku kolejnych skoków jest mo¿liwa</li>");
	printf ("</ul>");
	printf ("<p class=\"text\">Pionki nie bij± siê wzajemnie. Mo¿liwy jest remis tylko i wy³±cznie je¶li to czerwone pionki skoñcz± grê.</p>\n");

  // nowa gra
  printf ("<form action=\"skoczki.cgi\" method=\"get\">");
  printf ("<p class=\"center\" > <input type=\"submit\" value=\"Zagraj\" /> </p>");
  printf ("</form>");
	printf ("</div><!--rules end -->\n");

}
// sprawdzenie, czy pionki sa na pozycjach koncowych
int pionki_koniec (int pionki[8][8]) {

int i;
int z=0;
int c=0;

for (i=0; i<8; i+=2) {
	if(( pionki[7][i+1] == 1 ) && ( pionki[6][i] == 1 ))
		z +=1;
	if(( pionki[0][i] == 2 ) && ( pionki[1][i+1] == 2 ))
		c += 1;
}

if (z == 4) return 1;
else if (c == 4) return 2;
else return 0;
}

/* 0 - pole bia³e, pole puste, nieuzywane w grze
 * 1 - pionek zielony
 * 2 - pionek czerwony
 * 3 - pole CZARNE, docelowe
 */
void pionki_start (int pionki[8][8]) {

  int i=0, j=0;
// zerowanie pol (pola bia³e), nieuzywane
  for (i=7; i>=0; i--)  {
    for (j=0; j<8; j++ )  {		
	pionki[i][j]=0;	
	pionki[i][j]=0;
//printf ("[%d][%d]", i, j);
	}
//		printf("<br />"); 
}

//ustawianie pól czarnych, pustych - wartosc 3
for (i=0; i<8; i+=2) {
	pionki[2][i]=3;
	pionki[3][i+1]=3;
	pionki[4][i]=3;
	pionki[5][i+1]=3;
}

// ustawianie pionow na startowych pozycjach
  for (i=0; i<8; i+=2) {
	pionki[0][i]=1;
	pionki[1][i+1]=1;
	pionki[6][i]=2;
	pionki[7][i+1]=2;
}

}
char* pobierz_form_data_metoda_get () {
  char	     *form_data, *form_data_copy;
  int       dl;

  form_data = getenv ("QUERY_STRING");
  if (NULL == form_data)
    return NULL;
  dl = strlen (form_data);
  form_data_copy = (char*)malloc ((dl+1)*sizeof (char));
  if (NULL == form_data_copy)
    return NULL;
  strcpy (form_data_copy, form_data);
  return form_data_copy;
}
void wypisz_form_data (const char *form_data) {
  int   i;
  printf ("<p>\n");
  printf ("przeslane dane: ");
  for (i=0;'\0'!=form_data[i];i++)
    if ('&' == form_data[i])
      printf ("&amp;");
    else
      printf ("%c", form_data[i]);
  printf ("<br /><br />\n");
  printf ("</p>\n");
}

int cgi_name2value (const char *form_data, const char *name, char *value, int max_length)
{
    int i=0,dlugosc=0;
   
    if (strstr(form_data, name)){
                   if(form_data!=strstr(form_data, name)){
                   form_data=strstr(form_data, name);
                   if (*(form_data-1)!='&'){
                                           return 1;
                                               }
                                               }
                    dlugosc=strlen(name)+1;
                    form_data+=dlugosc;
                    if (*(form_data-1)=='='){
                    while(i!=max_length){
                                       if(*form_data=='&'){
                                               value[i]='\0';
                                               return 0;
                                                         }
                                       if(*form_data=='\0'){
                                               value[i]='\0';
                                               return 0;
                                                         }
                                       value[i]=*form_data;
                                               i++;
                                               form_data++;
                                               }
                                               return 1;
                                               }
                                               return 1;
                                               }
    return 1;
}
