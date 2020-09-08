/*
 * isa-tazatel.c
 *
 *  Created on: 23. 10. 2019
 *      Author: Petr Marek (xmarek69)
 */
#include "isa-tazatel.h"

int main(int argc , char *argv[])
 {

	int is_hostname = 0; // Priznak, urcujici, zda je adresa zadana v parametru -q hostname nebo ne
	Argument *args = NULL; // Struktura do ktere se ukladaji argumenty programu
	struct DNS_ANSWERS *dns_ans = NULL; // Struktura do ktere se ukaldaji ziskane DNS zaznamy
	struct DNS_ANSWERS *dns_ans_edited = NULL; // Struktura do ktere se ukladaji ziskane DNS zaznamy upravene adresy (ziskane pridanim/odebranim "www.", pokud se uzivatel pta na domenu

	if(check_args(argc, argv, &args) != SUCCESS){// Kontrola argumentu a ulozeni do promenne args, pokud dojde k chybe (napr. chybejici povinne argumenty), tak program skonci s chybou
		exit(EXIT_FAILURE);
	}

	// Inicializace resolveru (resolv.h)
	res_init();

	// Pokud uzivatel specifikoval DNS server paramterem -d a jedna se o IPv4, tak se nastavi pro resolv.h
	if(strlen(args->dns) > 0){
		if(get_type_of_query_name(args->dns) == QUERY_IPv4){
			_res.nscount = 1;
			_res.nsaddr_list[0].sin_port = htons(53);
			_res.nsaddr_list[0].sin_family = AF_INET;
			inet_pton(AF_INET, args->dns, &(_res.nsaddr_list[0].sin_addr));
			_res.retrans = 1;
		}else if(get_type_of_query_name(args->dns) == QUERY_IPv6){//resolv.h neumi pracovat s IPv6 DNS serverem
			printf("resolv.c nefunguje s IPv6 adresou.\nProgram tedy použije výchozí DNS server\n\n");
		}else{ // Jako DNS server nebyla zadana validni IP adresa
			printf("Špatně zadaný parametr -d (musí být IP adresa).\nProgram tedy použije výchozí DNS server\n\n");
		}
	}

	// Pokud se uzivatel pta na hostname, nastavi se priznak is_hostname
	if((get_type_of_query_name(args->query) == QUERY_HOSTNAME_WITHOUT_WWW)
		|| (get_type_of_query_name(args->query) == QUERY_HOSTNAME_WITH_WWW)){
			is_hostname = 1;
		}

	// Ziskaji se DNS zaznamy pro adresu na kterou se uzivatel pta
	get_DNS_entries(&dns_ans, args->query);

	// Ziskane DNS zaznamy se vytisknou
	print_DNS_entries(&dns_ans, args->query);

	if(dns_ans->data != NULL){// Pokud byli ziskany nejake DNS zaznamy...
		if(is_hostname){//Pokud adresa na kterou se uzivatel pta je hostname a preje si uzivatel rozsirene funkce programu (argumentem -e), tak se ziskaji a vytisknou DNS zaznamy pro editovanou adresu (tzn. pridani/odebrani "www.")
			if(args->extended == 1){
				get_DNS_entries(&dns_ans_edited, get_edited_www(args->query));
				print_DNS_entries(&dns_ans_edited, get_edited_www(args->query));
			}
		}else{// Nejedna se o hostname, ale o PTR
			if(args->extended == 1){ // Pokud si uzivatel preje rozsirene funkce programu (argumentem -e)...
				struct DNS_ANSWERS *dns_tmp;

				// Vezme se ziskana DNS odpoved, kde se bude nalezat domena (protoze byl ziskan PTR zaznam) a posle se jako argument funkci get_DNS_entries() pro ziskani DNS zaznamu prislusici k teto domene. Ziskane zaznamy se ulozi do docasne promenne pro strukturu DNS_ANSWERS
				get_DNS_entries(&dns_tmp, (char *)dns_ans->data);

				// Ziskane DNS zaznamy se nastavi jako dalsi polozky v puvodne ziskanem DNS zaznamu (DNS zaznamy se ukladaji do linearniho seznamu)
				dns_ans->next = dns_tmp;

				// Vsechny takto ziskane DNS zaznamy se vytisknou
				print_DNS_entries(&dns_ans, (char *)dns_ans->data);

				// Nakonec se jeste ziskaji DNS zaznamy upravene adresy domeny ziskane z DNS serveru, ktera byla upravena pridanim/odstranenim "www."
				get_DNS_entries(&dns_ans_edited, get_edited_www((char *)dns_ans->data));
				// Tyto ziskane zaznamy se vytisknou
				print_DNS_entries(&dns_ans_edited, get_edited_www((char *)dns_ans->data));
			}

		}
	}

	int whois_success_count = 0; // Pocitadlo uspesnych WHOIS odpovedi
	int w = get_whois(args->query, args->whois, args); // Ziskani WHOIS odpovedi pro uzivatelem zadanou adresu (argumentem -q). Zaroven se odpoved v teto funkci tiskne
	whois_success_count -= (w-1); // Protoze funkce get_whois vraci 1 (FAILURE) v pripade neuspechu a 0 (SUCCESS) v pripade uspechu, musime pocitani uspechu pocitat trochu "krkolomnym" zpusobem

	char *edited;
	if(args->extended == 1){// Pokud si uzivatel preje rozsirene funkce programu (argumentem -e)...
		if(w == FAILURE && get_type_of_query_name(args->query) == QUERY_HOSTNAME_WITHOUT_WWW){// Pokud uzivatel zadal adresu bez "www."
			edited = get_edited_www(args->query); // Ziskani editovane adresy
			if(args->show_failures == 1){// Pokud si uzivatel preje tisknout "ladici" (chybove) hlasky (argumentem -s)...
				printf("Požadujete whois záznam pro hostname bez 'www.'\nJelikož nebyl nalezen žádný záznam, tak se nyní pokusíme zaslat požadavek s upravenou adresou (%s):\n\n",edited);
			}
			w =get_whois(edited, args->whois, args); // Ziskani WHOIS odpovedi pro editovanou adresu (pridanim "www." k uzivatelem zadane adrese (argumentem -q)). Zaroven se odpoved v teto funkci tiskne
			whois_success_count -= (w-1);
		}
		if(w == FAILURE && get_type_of_query_name(args->query) == QUERY_HOSTNAME_WITH_WWW){// Pokud uzivatel zadal adresu s "www."
			edited = get_edited_www(args->query); // Ziskani editovane adresy
			if(args->show_failures == 1){// Pokud si uzivatel preje tisknout "ladici" (chybove) hlasky (argumentem -s)...
				printf("Požadujete whois záznam pro hostname s 'www.'\nJelikož nebyl nalezen žádný záznam, tak se nyní pokusíme zaslat požadavek s upravenou adresou (%s):\n\n",edited);
			}
			w = get_whois(edited, args->whois, args); // Ziskani WHOIS odpovedi pro editovanou adresu (odebranim "www." k uzivatelem zadane adrese (argumentem -q)). Zaroven se odpoved v teto funkci tiskne
			whois_success_count -= (w-1);
		}
		while(dns_ans != NULL){//Pro vsechny DNS odpovedi - tedy dokud prvek v seznamu na kterem se aktualne program nachazi neni NULL
			if(dns_ans->data == NULL){//Pro pripad ze by uzivatel zadal spatny DNS server, tak se sice dns_ans na-alokuje, ale data hned prvniho prvku budou null, je tedy potreba vyskocit z cyklu
				break;
			}
			if(strcmp((char *)args->query,(char *)dns_ans->data) == 0){ // Pokud jsou DNS data stejna jako puvodni adresa kterou uzivatel zadal, tak program preskoci tisknuti WHOIS odpovedi pro tuto adresu, protoze ji tiskl jiz jako prvni (byla by vytisknuta 2*)
				dns_ans = dns_ans->next;
				continue;
			}
			if((dns_ans->type == ns_t_a)
					||(dns_ans->type == ns_t_ptr)
					||(dns_ans->type == ns_t_aaaa)){ // Program se WHOIS serveru taze pouze v pripade, ze se jedna o A, AAAA ci PTR zaznam
				w = get_whois((char *)dns_ans->data, args->whois, args); // Ziskani WHOIS odpovedi pro dany DNS zaznam. Zaroven se odpoved v teto funkci tiskne
				whois_success_count -= (w-1);
				if(dns_ans->type == ns_t_ptr){
					if(w == FAILURE && get_type_of_query_name((char *)dns_ans->data) == QUERY_HOSTNAME_WITHOUT_WWW){ // Pokud uzivatel zadal adresu bez "www."
						edited = get_edited_www((char *)dns_ans->data); // Ziskani editovane adresy
						w = get_whois(edited, args->whois, args); // Ziskani WHOIS odpovedi pro editovanou adresu (pridanim "www."). Zaroven se odpoved v teto funkci tiskne
						whois_success_count -= (w-1);
					}
					if(w == FAILURE && get_type_of_query_name((char *)dns_ans->data) == QUERY_HOSTNAME_WITH_WWW){ // Pokud uzivatel zadal adresu s "www."
						edited = get_edited_www((char *)dns_ans->data); // Ziskani editovane adresy
						w = get_whois(edited, args->whois, args); // Ziskani WHOIS odpovedi pro editovanou adresu (odebranim "www."). Zaroven se odpoved v teto funkci tiskne
						whois_success_count -= (w-1);
					}
				}
			}
			dns_ans = dns_ans->next; // Presun na dalsi prvek linearniho seznamu typu struktury DNS_ANSWERS
		}

		if(whois_success_count == 0){//V pripade ze nebyl nalezen zadny WHOIS zaznam, tak se zkusi jeste volat pozadavky na WHOIS server s daty ziskanymi z DNS serveru kdyz jsme editovali ziskanou adresu (pridani/odebrani www.)
			while(dns_ans_edited != NULL){
				if(dns_ans_edited->data == NULL){ //Pro pripad ze by uzivatel zadal spatny DNS server, tak se sice dns_ans na-alokuje, ale data hned prvniho prvku budou null, je tedy potreba vyskocit z cyklu
					break;
				}
				if(strcmp((char *)args->query,(char *)dns_ans_edited->data) == 0){ // Pokud jsou DNS data stejna jako puvodni adresa kterou uzivatel zadal, tak program preskoci tisknuti WHOIS odpovedi pro tuto adresu, protoze ji tiskl jiz jako prvni (byla by vytisknuta 2*)
					dns_ans_edited = dns_ans_edited->next;
					continue;
				}
				if((dns_ans_edited->type == ns_t_a)
						||(dns_ans_edited->type == ns_t_ptr)
						||(dns_ans_edited->type == ns_t_aaaa)){ // Program se WHOIS serveru taze pouze v pripade, ze se jedna o A, AAAA ci PTR zaznam
					w = get_whois((char *)dns_ans_edited->data, args->whois, args); // Ziskani WHOIS odpovedi pro dany DNS zaznam. Zaroven se odpoved v teto funkci tiskne
					whois_success_count -= (w-1);
					if(dns_ans_edited->type == ns_t_ptr){
						if(w == FAILURE && get_type_of_query_name((char *)dns_ans_edited->data) == QUERY_HOSTNAME_WITHOUT_WWW){
							edited = get_edited_www((char *)dns_ans_edited->data); // Ziskani editovane adresy
							w = get_whois(edited, args->whois, args); // Ziskani WHOIS odpovedi pro editovanou adresu (pridanim "www."). Zaroven se odpoved v teto funkci tiskne
							whois_success_count -= (w-1);
						}
						if(w == FAILURE && get_type_of_query_name((char *)dns_ans_edited->data) == QUERY_HOSTNAME_WITH_WWW){
							edited = get_edited_www((char *)dns_ans_edited->data); // Ziskani editovane adresy
							w = get_whois(edited, args->whois, args); // Ziskani WHOIS odpovedi pro editovanou adresu (odebranim "www."). Zaroven se odpoved v teto funkci tiskne
							whois_success_count -= (w-1);
						}
					}
				}
				dns_ans_edited = dns_ans_edited->next; // Presun na dalsi prvek linearniho seznamu typu struktury DNS_ANSWERS
			}
		}
	}
	if(whois_success_count == 0){// Pokud je pocet ziskanych odpovedi z WHOIS serveru rovno 0
		printf("!!! Pro danou adresu nebyl nalezen žádný WHOIS záznam !!!\n");

	}
	if(args->extended != 1){// Pokud uzivatel nepredal argument -e, je vyzvan aby tak ucinil, pokud chce proves hlubsi hledani, viz bonusova cast zadani v dokumentaci
		printf("Pro hlubší prohledávání spusťte program s bezhodnotovým argumentem -e.\n");
	}

	return 0;
}

// Funkce otestuje argumenty programu a ulozi je do struktury k tomu urcene
int check_args(int argc , char *argv[], Argument **args){
	int argument;
	int q,w,d;
	q = w = d = -1;

	*args=malloc(sizeof (Argument));

	// Nastaveni implicitnich hodnot argumentu, pokud nebudou nastaveny argumenty programu jinak...
	(*args)->extended = 0;
	(*args)->filtered = 0;
	(*args)->show_failures = 0;
	(*args)->dns[0] = '\0';

	while((argument = getopt(argc, argv, ":q:w:d:hefs")) != -1){ // Pouzivame getopt pro parsovani argumentu programu
		switch(argument){
			case 'q': // Adresa na kterou se uzivatel chce zeptat WHOIS serveru
				strcpy((*args)->query,optarg);
				q = 1;
				break;
			case 'w': // Adresa WHOIS serveru
				strcpy((*args)->whois,optarg);
				w = 1;
				break;
			case 'd': // Adresa DNS serveru
				strcpy((*args)->dns,optarg);
				d = 1;
				break;
			case 'e': // Rozrirene funkce (viz bonusova cast zadani v manualu)
				(*args)->extended = 1;
				break;
			case 'f': // Filtrovani vystupu WHOIS odpovedi
				(*args)->filtered = 1;
				break;
			case 's': // Zobrazovani oznameni o neuspesich pri ziskavani WHOIS odpovedi
				(*args)->show_failures = 1;
				break;
			case 'h': // Zobrazeni napovedy
				printf("Nápověda:\nPoužití: ./isa-tazatel -q <IP|hostname serveru o němž hledáme informace> -w <IP|hostname WHOIS serveru> [-d <IP DNS serveru> -e -f -s]\nArgumenty -q a -w jsou povinné, -d je dobrovolný a v případě absence bude použit výchozí DNS server (8.8.8.8), -e zapíná rozšířené funkce programu (viz dokumentace), -f vyfiltruje WHOIS odpovědi (zobrazí se pouze některé předem dané položky) a -s zapne vypisování oznámení o neúspešných požadavcích o získání WHOIS odpovědi\n");
				break;
			case ':': // Argument potrebuje hodnotu
				fprintf( stderr, "CHYBA v argumentech programu - argument %c potřebuje hodnotu!\n", optopt);
				return FAILURE;
			case '?': // Neznamy argument
				printf("CHYBA: neznámý argument! %c\n", optopt);
				break;
		}
	}

	while(optind < argc){ // Predany argumenty navic
		printf("CHYBA: Předány argumenty navíc: %s\n", argv[optind]);
		optind++;
	}
	if(q == -1 || w == -1){ // Argumenty -q a -w jsou povinne
		fprintf( stderr, "Málo argumentů!\nPoužití: ./isa-tazatel -q <IP|hostname serveru o němž hledáme informace> -w <IP|hostname WHOIS serveru> -d <IP DNS serveru>\nArgumenty -q a -w jsou povinné, -d je dobrovolný a v případě absence bude použit výchozí DNS server (8.8.8.8)\n");
		return FAILURE;
   }

	return SUCCESS;
}

// Funkce posle pozadavek na DNS server, podle typu zaznamu (entry_type) specifikovaneho v parametrech funkce a vrati nalezene zaznamy. Vyuziva resolv.h
struct DNS_ANSWERS * search_in_DNS(struct DNS_ANSWERS **dns_ans, char *address, int entry_type, int ans_type, int *moved){


	int res_error = 1;
	unsigned char nsbuf[PACKETSZ];
	char dispbuf[PACKETSZ];

	memset(dispbuf, 0, PACKETSZ);
	memset(nsbuf, 0, PACKETSZ);
	ns_msg msg;
	ns_rr rr;
	int msg_count;
	int idx = 0;
	int data_idx = 0;
	char *data = NULL;


	res_error = res_query(address, ns_c_in, entry_type, nsbuf, sizeof(nsbuf)); // Vysle pozadavek na DNS server

	if(res_error != -1){ // Pokud res_query() nevratilo chybu...
		if(*moved == 1){ // Pokud jsme se jiz presunuli z prvniho prvku, ktery byl puvodne jiz naalokovany ve funkci ze ktere je tato funkce volana, tak se naalokuje pro dalsi prvek a presuneme se na nej.
			(*dns_ans)->next = malloc(sizeof (struct DNS_ANSWERS));
			(*dns_ans) = (*dns_ans)->next;
		}
		ns_initparse(nsbuf, res_error, &msg);
		msg_count = ns_msg_count(msg, ans_type); // Pocet DNS zaznamu daneho typu
		for (int i = 0; i < msg_count; i++){
			ns_parserr(&msg, ans_type, i, &rr);
			ns_sprintrr(&msg, &rr, NULL, NULL, dispbuf, sizeof(dispbuf)); // ulozeni odpovedi do dispbuf

			int ix;
			int is_soa=1;
			char *soa_str = "IN SOA";
			int mname_start=0;
			int mname_end=0;
			int rname_start=0;
			int rname_end=0;
			if(entry_type == ns_t_soa){ // Pokud se jedna o SOA zaznam, bude se zpracovavat jinak nez ostatni
				for(idx = 0;idx<strlen(dispbuf);idx++){ // V tomto cyklu se ziskaji indexy kde zacina a konci nazev primarniho DNS serveru pro danou zonu (mname) a rovnez kde zacina a konci kontakt na spravce zonoveho souboru (rname)
					for(ix = idx; ix<strlen(dispbuf);ix++){
						if(((ix-idx) < strlen(soa_str)) && (dispbuf[ix] != soa_str[ix-idx])){//vzdy se zkousi zda se nachazi na danem indexu textovy retezec "IN SOA"
							is_soa = 0;
						}
						if(is_soa == 1 && (ix-idx >= strlen(soa_str))){
							while(valid_mx_ns_ptr_char(dispbuf[ix]) == NO_MX_CHARACTER){ //valid_mx_ns_ptr_char() vraci i to zda se jedna o validni nazev DNS serveru/ validni kontakt na spravce
								ix++;
							}
							mname_start = ix;
							while(valid_mx_ns_ptr_char(dispbuf[ix]) != NO_MX_CHARACTER){
								ix++;
							}
							mname_end = ix-1; //-1 protoze chceme odstranit zaverecnou tecku

							while(valid_mx_ns_ptr_char(dispbuf[ix]) == NO_MX_CHARACTER){
								ix++;
							}
							rname_start = ix;
							while(valid_mx_ns_ptr_char(dispbuf[ix]) != NO_MX_CHARACTER){
								ix++;
							}
							rname_end = ix-1; //-1 protoze chceme odstranit zaverecnou tecku
							is_soa=0;
						}

					}

					is_soa = 1;
				}

				// Nasleduje alokace pro data a kopirovani obsahu do teto promenne

				data = (char *)calloc((mname_end-mname_start)+(rname_end-rname_start)+strlen("SOA:\t\t\nadmin email:\t\t")+1, sizeof(char));

				strcpy(data,"SOA:\t\t");

				data_idx = strlen("SOA:\t\t");
				for(int mname_idx = mname_start; mname_idx < mname_end; mname_idx++){
					data[data_idx++] = dispbuf[mname_idx];
				}

				memcpy(data+data_idx,"\nadmin email:\t\t", sizeof("\nadmin email:\t"));
				data_idx += strlen("\nadmin email:\t");
				for(int rname_idx = rname_start; rname_idx < rname_end; rname_idx++){
					data[data_idx++] = dispbuf[rname_idx];
				}
			}else{ // Nejedna se o SOA
				for(idx = strlen(dispbuf)-1; idx >= 0; idx--){//Divame se od konce textoveho retezce ulozeneho v dispbuf a dokud se jedna o validni text, tak snizujeme index, kterym se pak ridime - od tohoto indexu po konec textoveho retezce ulozeneho v dispbuf se nachazi hledana data
					if(((entry_type == ns_t_a) && (valid_ipv4_char(dispbuf[idx]) == NO_IPv4_CHARACTER))
							|| ((entry_type == ns_t_aaaa) && (valid_ipv6_char(dispbuf[idx]) == NO_IPv6_CHARACTER))
							|| ((entry_type == ns_t_mx) && (valid_mx_ns_ptr_char(dispbuf[idx]) == NO_MX_CHARACTER))
							|| ((entry_type == ns_t_ns) && (valid_mx_ns_ptr_char(dispbuf[idx]) == NO_MX_CHARACTER))
							|| ((entry_type == ns_t_ptr) && (valid_mx_ns_ptr_char(dispbuf[idx]) == NO_MX_CHARACTER))){
						idx++; // Pokud se nejedna o validni data, inkrementujeme index a ukoncime cyklus
						break;
					}
				}

				// Nasleduje alokace pro data a kopirovani obsahu do teto promenne
				data = (char *)calloc(strlen(dispbuf)-idx+1, sizeof(char));
				data_idx = 0;
				while(idx < (strlen(dispbuf))){
					data[data_idx++] = dispbuf[idx++];
				}
				if(data[data_idx-1] == '.'){//pokud data konci teckou (za adresou), odstrani se
					data[data_idx-1] = '\0';
				}
			}

			//Ulozi se ziskana data do dns_ans (typ a data)
			(*dns_ans)->type = entry_type;
			(*dns_ans)->data = calloc(strlen(data)+1, sizeof(char));
			strcpy((char*) (*dns_ans)->data,data);

			if(i < (msg_count-1)){ // Pokud jsou jeste nejaka dostupna data, tak se naalokuje misto pro dalsi prvek seznamu a presuneme se na tento prvek
				(*dns_ans)->next = malloc(sizeof (struct DNS_ANSWERS));
				(*dns_ans) = (*dns_ans)->next;
			}else{
				(*dns_ans)->next = NULL;
			}
			*moved = 1; // Nastavime moved na 1, aby se pri kazdem dalsim volani funkce na pocatku muselo pro dalsi prvek alokovat pamet (pro uplne prvni prvek seznamu se pamet alokuje ve funkci, ktera tuto funkci vola)
		}
	}

	return (struct DNS_ANSWERS *)(*dns_ans);

}

// Funkce ziska a do parametru dns_ans ulozi vsechny dostupne DNS zaznamy
int get_DNS_entries(struct DNS_ANSWERS **dns_ans, char *address){
	*dns_ans = malloc(sizeof (struct DNS_ANSWERS));
	struct DNS_ANSWERS *dns_record = (struct DNS_ANSWERS*)(*(dns_ans));
	int moved; // Timto flagem rikame, ze jsme se jiz posunuly z prvniho prvku v seznamu dns odpovedi a je tudiz nutne se (pozdeji) posunovat na dalsi prvek a alokovat pro nej pamet
	moved = 0;

	if(get_type_of_query_name(address) == QUERY_IPv4){ // V pripade IPv4 musime proves prevod adresy na PTR tvar
		dns_record = search_in_DNS((struct DNS_ANSWERS **)(&dns_record), edit_ipv4_to_ptr(address), ns_t_ptr, ns_s_an, &moved);
	}else if(get_type_of_query_name(address) == QUERY_IPv6){ // V pripade IPv6 musime proves prevod adresy na PTR tvar
		dns_record = search_in_DNS((struct DNS_ANSWERS **)(&dns_record), edit_ipv6_to_ptr(address), ns_t_ptr, ns_s_an, &moved);
	}else if((get_type_of_query_name(address) == QUERY_HOSTNAME_WITHOUT_WWW)
			|| (get_type_of_query_name(address) == QUERY_HOSTNAME_WITH_WWW)){
		dns_record = search_in_DNS((struct DNS_ANSWERS **)(&dns_record), address, ns_t_a, ns_s_an, &moved);
		dns_record = search_in_DNS((struct DNS_ANSWERS **)(&dns_record), address, ns_t_aaaa, ns_s_an,&moved);
		dns_record = search_in_DNS((struct DNS_ANSWERS **)(&dns_record), address, ns_t_mx, ns_s_an, &moved);
		dns_record = search_in_DNS((struct DNS_ANSWERS **)(&dns_record), address, ns_t_cname, ns_s_an, &moved);
		dns_record = search_in_DNS((struct DNS_ANSWERS **)(&dns_record), address, ns_t_soa, ns_s_an, &moved);
		dns_record = search_in_DNS((struct DNS_ANSWERS **)(&dns_record), address, ns_t_ns, ns_s_an, &moved);
	}else{
		return FAILURE;
	}

	return SUCCESS;
}

// Funkce tiskne ziskane DNS odpovedi
int print_DNS_entries(struct DNS_ANSWERS **dns_ans, char *query){
	struct DNS_ANSWERS *dns_record;
	dns_record = *dns_ans;

	if(*query != '\0'){		
		printf("--DNS zaznamy pro '%s'--\n", query);
		if((dns_record->data == NULL) || (*dns_record->data == '\0')){
			printf("Nenalezeny žádné DNS záznamy\n");
		}else{
			while(dns_record != NULL){
				if(dns_record->type == ns_t_soa){ // Rozdeleni zpravy na typ a data se pro SOA zaznam provadi jiz ve funkci search_in_DNS()
					printf("%s\n", dns_record->data);
				}else{
					printf("%s:\t\t%s\n", get_dns_type(dns_record->type), dns_record->data);
				}
				if(dns_record->next == NULL){
					break;
				}
				dns_record = dns_record->next;
			}
		}
		printf("\n");
	}
	return SUCCESS;
}

// Funkce vraci textovy retezec, ktery reprezentuje typ DNS zaznamu, predany jako parametr
char *get_dns_type(uint8_t type){
	//Nejdelsi textovy retezec je zde CNAME, proto se bere jeho velikost
	char *str = calloc(strlen("CNAME")+1, sizeof(char));
	char typy[7][5]={"A", "AAAA",  "MX", "CNAME", "NS", "SOA", "PTR"};
	switch (type) {
		case ns_t_a:
			strcpy(str, typy[0]);
			break;
		case ns_t_aaaa:
			strcpy(str, typy[1]);
			break;
		case ns_t_mx:
			strcpy(str, typy[2]);
			break;
		case ns_t_cname:
			strcpy(str, typy[3]);
			break;
		case ns_t_ns:
			strcpy(str, typy[4]);
			break;
		case ns_t_soa:
			strcpy(str, typy[5]);
			break;
		case ns_t_ptr:
			strcpy(str, typy[6]);
			break;
		default:
			break;
	}
	return str;
}

// Funkce vrací typ predavane adresy
int get_type_of_query_name(char *qname){
	int len = strlen(qname);
	int octet = 0;
	int hextet = 0;
	char octets[4][4];
	char hextets[8][5];
	char decompressed_ipv6[8][5];


	int IPv4 = 0;
	int IPv6 = 0;
	int hostname = 0;
	char *tmp_qname = calloc(len, sizeof(char));
	strcpy(tmp_qname, qname);

	if (strstr(tmp_qname, ":") != NULL) {
		IPv6 = 1;
	}else if (strstr(tmp_qname, ".") != NULL) {
		IPv4 = 1;
		hostname = 1;
	}


	if(IPv6){// Pokud byla zadana IPv6
		int double_colon_pos = double_colon_position(qname);

		if(double_colon_pos == ERROR_MORE_DOUBLE_COLON){
			return QUERY_ERROR_MORE_DOUBLE_COLONS;
		}

		//rozdeli se prijaty retezec do hextetu (podle znaku ":")
		char delim[] = ":";
		char *ptr = strtok(tmp_qname, delim);
		while(ptr != NULL)
		{
			/*Pokud je hextetu vice nez 8,
			 * nebo pokud nektery z nich ma vic nez 4 znaky,
			 * pak vime ze se nejedna o IPv6.
			 * V opacnem pripade se testuji dale dalsi podminky */
			if(hextet > 7 || strlen(ptr) > 4){
				return QUERY_ERROR_INVALID_IPv6;
			}else{
				strcpy(hextets[hextet], ptr);
			}
			ptr = strtok(NULL, delim);
			hextet++;
		}

		if((double_colon_pos == -1 && hextet != 8)
				|| (double_colon_pos >= 0 && hextet > 7)){ // Ziskani pozice dvojite dvojtecky, pokud je v IPv6 adrese obsazena
			return QUERY_ERROR_INVALID_IPv6;
		}
		int get_char_position = 0;
		for(int i = 0; i < 8; i++){ //Dekomprese IPv6 adresy na plne rozepsanou
			if(i >= double_colon_pos && i < (8 - hextet + double_colon_pos)){
				decompressed_ipv6[i][0] ='0';
				decompressed_ipv6[i][1] ='0';
				decompressed_ipv6[i][2] ='0';
				decompressed_ipv6[i][3] ='0';
			}else{
				for(int j = 0; j < 4; j++){
					decompressed_ipv6[i][j] ='0';
					if(j >= (4-strlen(hextets[get_char_position]))){
						decompressed_ipv6[i][j] = hextets[get_char_position][j-(4-strlen(hextets[get_char_position]))];
					}
				}
				get_char_position++;
			}


			decompressed_ipv6[i][4] ='\0';

		}


		//Test, zda vsechny znaky adresy jsou cisla, či znaky a-f (a A-F). Pokud ne, tak vime ze se nejedna o IPv6
		for(int i = 0; i < 8; i++){
			for(int j = 0; j < 4; j++){
				if(valid_ipv6_char(decompressed_ipv6[i][j]) == NO_IPv6_CHARACTER){
					return QUERY_ERROR;
				}
			}
		}

		//Pokud se jedna o IPv6 tak se vrati odpoved. v opacnem pripade se jedna o chybu
		if(IPv6 == 1){
			return QUERY_IPv6;
		}
	}else if(hostname == 1 || IPv4 == 1){//Jinak pokud byla zadana IPv4 nebo hostname


		//pokud qname obsahuje retezec ".in-addr.arpa", pak se jedna o PTR adresu
		if (strstr(qname, ".in-addr.arpa") != NULL) {
		    if(len <= 28){//28 je nejvetsi pocet znaku, ktery muze obsahovat PTR IPv4 adresa. Zaroven vime, ze zadna PTR IPv6 adresa neni tak kratka
		    	return QUERY_PTR_IPv4;
		    }else{
		    	return QUERY_PTR_IPv6;
		    }
		}


		//Pokud prijaty retezec obsahuje dve tecky za sebou, je chyba v zapise
		for(int i = 0; i < strlen(qname)-1; i++){
			if((qname[i] == qname[i+1]) && (qname[i] == '.')){
				return QUERY_ERROR_MORE_DOTS;
			}
		}

		//rozdeli se prijaty retezec do oktetu (podle znaku ".")
		char delim[] = ".";
		char *ptr = strtok(tmp_qname, delim);
		while(ptr != NULL)
		{
			/*Pokud je oktetu vice nez 4,
			 * nebo pokud nektery z nich ma vic nez 3 znaky,
			 * pak vime ze se nejedna o IPv4, ale hostname.
			 * V opacenm pripade dale testujeme dalsi podminky*/
			if(octet > 3 || strlen(ptr) > 3){
				IPv4 = 0;
			}else{
				strcpy(octets[octet], ptr);
			}
			ptr = strtok(NULL, delim);
			octet++;
		}

		//Test, zda vsechny znaky adresy jsou cisla. Pokud ne, tak vime ze se nejedna o IPv4
		if(IPv4 == 1){
			for(int i = 0; i < 4; i++){
				for(int j = 0; j < strlen(octets[i]); j++){
					if(((int)octets[i][j] < 48) || ((int)octets[i][j] > 57)){
						IPv4 = 0;
					}
				}
			}
		}

		//Test, zda je oktet v intervalu <0, 255>. Pokud ne, tak vime ze se nejedna o IPv4
		if(IPv4 == 1){
			for(int i = 0; i < 4; i++){
				if(atoi(octets[i]) < 0 ||atoi(octets[i]) >= 256){
					IPv4 = 0;
					break;
				}
			}
		}

		//Pokud se jedna o IPv4 tak se vrati odpoved. v opacnem pripade se jedna o hostname testuje se v jakem je formatu
		if(IPv4 == 1){
			return QUERY_IPv4;
		}else{
			//Pokud prijaty retezec nekonci pismenem, pomlckou nebo cislem, tak se nejedna o validni hostname, ani IP adresu
			if(!(qname[strlen(qname)-1] == '-' || (qname[strlen(qname)-1] >= '0' && qname[strlen(qname)-1] <= '9') || (qname[strlen(qname)-1] >= 'A' && qname[strlen(qname)-1] <= 'Z') || (qname[strlen(qname)-1] >= 'a' && qname[strlen(qname)-1] <= 'z'))){
				return QUERY_ERROR_NOT_ALLOWED_CHARACTERS;
			}

			if(starts_with_www(qname)){
				if(count_char(qname, '.')>1){ // Pokud adresa zacina s "www." a zaroven se v cele adrese nachazeji alespon 2 tecky, pak se jedna o hostname s pocatecnim "www."
					return QUERY_HOSTNAME_WITH_WWW;
				}
				return QUERY_ERROR_INVALID_HOSTNAME; // Pokud adresa zacina s "www.", a zaroven obsahuje pouze jednu tecku celkove, pak se jedna o chybu
			}else{
				return QUERY_HOSTNAME_WITHOUT_WWW; // Jinak se jedna o hostname bez pocatecniho "www."
			}
		}
	}

	return QUERY_ERROR; // Pokud zadny typ nebyl vracen, jedna se o chybu
}

// Funkce vraci adresu ve formatu W.X.Y.Z.in-addr.arpa (pro PTR)
char *edit_ipv4_to_ptr(char *qname){
	if(get_type_of_query_name(qname) != QUERY_IPv4){
		return NULL;
	}
	char octets[4][5];
	int octet = 0;
	int pos = 0;
	memset(octets[octet], 0, 4);
	for(int i = 0; i < strlen(qname);i++){// Kopirovani znaku z qname do octets + vkladani tecek
			if(qname[i] == '.'){
				octets[octet][pos++] = '.';
				octets[octet][pos] = '\0';
				octet++;
				pos = 0;
				memset(octets[octet], 0, 5);
			}else{
				octets[octet][pos++] = qname[i];
			}
	}
	octets[octet][pos++] = '.';
	octets[octet][pos] = '\0';

	int len = strlen(octets[0])+strlen(octets[1])
			+ strlen(octets[2])+strlen(octets[3])
			+strlen("....in-addr.arpa");

	// Nasleduje alokace pro vysledny retezec + kopirovani do nej ziskanych retezcu
	char *edited = calloc(len, sizeof(char));
	for(int i = 3; i >=0; i--){
		strcat(edited, octets[i]);
	}
	strcat(edited, "in-addr.arpa");
	return edited;
}

// Funkce vraci adresu ve formatu 0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.ip6.arpa (pro PTR)
char *edit_ipv6_to_ptr(char *qname){
	if(get_type_of_query_name(qname) != QUERY_IPv6){
		return NULL;
	}
	char hextets[8][5];
	int hextet = 0;

	int double_colon_pos = double_colon_position(qname);
	char decompressed_ipv6[8][5];
	int len = strlen(qname);

	char *tmp_qname = calloc(len, sizeof(char));
	strcpy(tmp_qname, qname);


	//rozdelime prijaty retezec do hextetu (podle znaku ":")
	char delim[] = ":";
	char *ptr = strtok(tmp_qname, delim);
	while(ptr != NULL)
	{
		strcpy(hextets[hextet], ptr);

		ptr = strtok(NULL, delim);
		hextet++;
	}

	int get_char_position = 0;
	for(int i = 0; i < 8; i++){ //Dekomprese IPv6 adresy na plne rozepsanou
		if(i >= double_colon_pos && i < (8 - hextet + double_colon_pos)){
			decompressed_ipv6[i][0] ='0';
			decompressed_ipv6[i][1] ='0';
			decompressed_ipv6[i][2] ='0';
			decompressed_ipv6[i][3] ='0';
		}else{
			for(int j = 0; j < 4; j++){
				decompressed_ipv6[i][j] ='0';
				if(j >= (4-strlen(hextets[get_char_position]))){
					decompressed_ipv6[i][j] = hextets[get_char_position][j-(4-strlen(hextets[get_char_position]))];
				}
			}
			get_char_position++;
		}


		decompressed_ipv6[i][4] ='\0';

	}

	// Nasleduje alokace pro vysledny retezec + kopirovani do nej ziskanych retezcu
	int ptr_len = strlen("0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.ip6.arpa");
	char *edited = calloc(ptr_len+1, sizeof(char));
	int i, j, k;
	for(i = 0; i < 8; i++){
		for(j = 0, k = 0; k < 4;k++){
			edited[i*8+j] = decompressed_ipv6[7-i][3-k];
			edited[i*8+j+1] = '.';
			j += 2;
		}
	}
	edited[i*8+j+1] = '\0';
	strcat(edited, "ip6.arpa");

	return edited;
}

// Funkce vraci, zda se jedna o validni znak z IPv6 adresy
int valid_ipv6_char(char ch){
	if(ch >= '0' && ch <= '9'){
		return NUMBER_CHARACTER;
	}
	if(ch >= 'a' && ch <= 'f'){
		return L_CASE_CHARACTER;
	}
	if(ch >= 'A' && ch <= 'F'){
		return U_CASE_CHARACTER;
	}
	if(ch == ':'){
		return COLON_CHARACTER;
	}
	return NO_IPv6_CHARACTER;
}

// Funkce vraci, zda se jedna o validni znak z IPv4 adresy
int valid_ipv4_char(char ch){
	if(ch >= '0' && ch <= '9'){
		return NUMBER_CHARACTER;
	}
	if(ch == '.'){
		return DOT_CHARACTER;
	}
	return NO_IPv4_CHARACTER;
}

// Funkce vraci, zda se jedna o validni mx/ns/ptr znak
int valid_mx_ns_ptr_char(char ch){
	if(ch >= '0' && ch <= '9'){
		return NUMBER_CHARACTER;
	}
	if(ch >= 'a' && ch <= 'z'){
		return L_CASE_CHARACTER;
	}
	if(ch >= 'A' && ch <= 'Z'){
		return U_CASE_CHARACTER;
	}
	if(ch == '.'){
		return DOT_CHARACTER;
	}
	return NO_MX_CHARACTER;
}

// Funkce vraci pozici dvojite dvojtecky (::). V pripade jeji absence vraci -1
int double_colon_position(char *str){
	int count = 0;
	int position = -1;
	int i = 0;
	while(i < (strlen(str)-1)){
		if(str[i] == str[i+1] && str[i] == ':'){
			count++;
			position++;
		}else if(str[i] == ':' && count == 0){
			position++;
		}

		//V pripade ze prvni znak adresy neni ":", tak se zvysi pozice (aby napriklad adresa 1::2 nedetekovala pozici "::" jako 0, ale 1)
		if(i==0 && str[i] != ':'){
			position++;
		}
		i++;
	}

	if(count > 1){
		return ERROR_MORE_DOUBLE_COLON; // v IPv6 muze byt pouze jedna dvojita dvojtecka
	}
	if(count == 1){
		return position;
	}
	return -1;
}
//Spocita znaky specifikovane parametrem ch v prijatem retezci. Pouzivam v kodu napriklad pro kontrolu zda prijaty retezec neobsahuje jen jednu tecku a zda to zaroven neni tecka za "www.". Tehdy bychom vedeli ze se nejedna o validni hostname
int count_char(char *str, char ch){
	int i = 0;
	int count = 0;

	while(i < strlen(str)){
		if(str[i] == ch){
			count++;
		}
		i++;
	}
	return count;
}

// Funkce vraci 1, pokud prijimany textovy retezec zacina "www.". Jinak vraci 0.
int starts_with_www(char *address){
	int starts_with_www = 1;
	char *www = "www.";
	int i = 0;
	while (i<4){
		if(www[i] != address[i]){
			starts_with_www = 0;
		}
		i++;
	}

	// Dana adresa zacina 'www.'
	if(starts_with_www == 1){
		return 1;
	}else{//Jinak naopak na 'www.' dana adresa nezacina
		return 0;
	}
}

// Funkce odesle pozadavek na WHOIS server a vytiskne ziskanou odpoved
int get_whois(char *wquery, char *whois_server_address, struct ARGUMENT *args){
	struct sockaddr_in server_addr;
	struct sockaddr_in6 server_addr6;
	char query[DOMAIN_NAME_MAX_LENGTH] , buffer[1500];
	char *server_response;
	int total_response_size = 0, tmp_size = 0;
	int sock;

	int number_of_connections = 0;
	int response_available = 0;
	int connection_limit = 20;
	while(number_of_connections < connection_limit && response_available != 1){// Nektere WHOIS servery (alespon whois.nic.cz) trochu zlobi a potrebuji nekdy pripojit vicekrat - jinak vraceji "Your connection limit exceeded"
		if((get_type_of_query_name(whois_server_address) == QUERY_HOSTNAME_WITHOUT_WWW)
			|| (get_type_of_query_name(whois_server_address) == QUERY_HOSTNAME_WITHOUT_WWW)){ // WHOIS server jsme obdrzeli jako hostname a je treba ho prevest na IP adresu, abychom se k nemu mohli pozdeji pripojit
			struct DNS_ANSWERS *dns_ans;
			get_DNS_entries(&dns_ans, whois_server_address);

			if((dns_ans->type == ns_t_a)
					||(dns_ans->type == ns_t_aaaa)){
				//Prekopiruj IP adresu do adresy whois serveru
				strcpy(whois_server_address, (char *)dns_ans->data);
			}else{ // DNS nenasel zadny A ani AAAA zaznam
				return FAILURE;
			}

		}

		//Zde vytvorime socket na zaklade toho, zda je whois adresa ve formatu IPv4 nebo IPv6
		if(get_type_of_query_name(whois_server_address) == QUERY_IPv4){
			//Vytvorime socket, pres ktery se pripojime k WHOIS serveru
			sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

			//nastavime adrese serveru IPv4, port 43 (zde odposlouchava WHOIS server) a ip adresu
		    server_addr.sin_family = AF_INET;
		    server_addr.sin_port = htons(WHOIS_PORT);
			inet_pton(AF_INET, whois_server_address, &server_addr.sin_addr);

		    //Pripojime se na whois server
			connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr));
		}else if(get_type_of_query_name(whois_server_address) == QUERY_IPv6){
			//Vytvorime socket, pres ktery se pripojime k WHOIS serveru
			sock = socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP);

			//nastavime adrese serveru IPv4, port 43 (zde odposlouchava WHOIS server) a ip adresu
		    server_addr6.sin6_family = AF_INET6;
		    server_addr6.sin6_port = htons(WHOIS_PORT);
			inet_pton(AF_INET6, whois_server_address, &server_addr6.sin6_addr);

		    //Pripojime se na whois server
			connect(sock, (struct sockaddr *)&server_addr6, sizeof(server_addr6));


		}

		//Na konec whois dotazu musime pripojit znaky \r\n, ktere dotaz ukoncuji
		strcpy(query, wquery);
		strcat(query,"\r\n");

		if( send(sock , query , strlen(query) , 0) < 0){
			perror("send failed");
		}

		while( (tmp_size = recv(sock , buffer , sizeof(buffer) , 0)) >0 )
		{
			if(total_response_size >0){ // Pokud jiz je naalokovano, tak se realokuje
				server_response = realloc(server_response , tmp_size + total_response_size);
			}
			else{ // Jinak se alokuje
				server_response = malloc(tmp_size * sizeof(char));
			}

			if(server_response == NULL)
			{
				printf("realloc failed");
			}
			memcpy(server_response + total_response_size , buffer , tmp_size);
			total_response_size += tmp_size;
		}

		if(total_response_size >0){ // Pridani znaku, ukoncujiciho retezec (\0)
			server_response = realloc(server_response , total_response_size + 1);
			*(server_response + total_response_size) = '\0';
		}

		//nakonec socket zavreme
		close(sock);

		if((strstr(server_response, "%ERROR:101") != NULL)
				|| (strstr(server_response, "No entries found") != NULL)
				||(strstr(server_response, "No match found") != NULL)){ // Pokud WHOIS nema zadne zaznamy pro dany pozadavek
			if(args->show_failures == 1){ // Pokud uzivatel aktivoval zobrazovani chyb (argumentem programu -s), tak zobraz oznameni o chybe...
				parse_WHOIS_response("Pro tento požadavek neobsahuje daný whois server žádný záznam.", wquery, args);
			}
			return FAILURE;
		}else if((strstr(server_response, "Your connection limit exceeded") != NULL)){ // Pokud server vratil hlasku o prekroceni limitu pripojeni, tak se zkus pripojit znovu (protoze tohle je zvlastni chovani, ktere obcas provazi pripojovani se na whois.nic.cz)
			parse_WHOIS_response("Your connection limit exceeded", wquery, args);
			number_of_connections++;
		}else{ // Jinak byla vracena odpoved, takze se vytiskne
			parse_WHOIS_response(server_response, wquery, args);
			response_available = 1;
		}

	}




	return SUCCESS;
}

// Funkce vraci upravenou adresu - vezme parametr address a prida/odebere k nemu "www." (pokud zacinal "www.", tak se tento retezec odebere, jinak se prida)
char *get_edited_www(char *address){
	int starts_with_www = 1;
	char *new_address;
	char *www = "www.";
	int i = 0;
	while (i<4){
		if(www[i] != address[i]){
			starts_with_www = 0;
		}
		i++;
	}

	i = 0;

	if(starts_with_www == 1){ // Dana adresa zacina 'www.' a proto se tento zacatek odstani
		new_address = (char *)calloc((strlen(address)-4), sizeof(char));
		while(i < (strlen(address)-4)){
			new_address[i] = address[i+4];
			i++;
		}
		new_address[i] = '\0';
	}else{//Jinak naopak na 'www.' dana adresa nezacina a naopak se tento retezec prida na jeji zacatek
		new_address = (char *)calloc((strlen(address)+4),sizeof(char));
		while(i < (strlen(address))){
			if(i < 4){
				new_address[i] = www[i];
			}
			new_address[i+4] = address[i];
			i++;
		}
		new_address[i+4] = '\0';

	}
	return new_address;
}

// Funkce vytiskne WHOIS odpoved
int parse_WHOIS_response(char  *resp, char *qname, struct ARGUMENT *args){
	int i = 0, line_count = 0, max_string_lenght = 0;
	int last_line_feed = 0;
	int qname_length = strlen (qname);
	char delim[] = "\n";

	//Spocitame pocet radku
	while(resp[i] != '\0'){
		if(resp[i] == '\n'){
			line_count++;
			max_string_lenght = get_max_string_lenght(last_line_feed, i, max_string_lenght);
			last_line_feed = i;
		}
		i++;
	}

	char lines[line_count][max_string_lenght];
	int last_i=0;
	int no_entries = strcmp(resp, "Pro tento požadavek neobsahuje daný whois server žádný záznam.");
	int slow_down = strcmp(resp, "Your connection limit exceeded");
	if(no_entries != 0 && slow_down != 0){//Server nasel nejake zaznamy pro pozadavek uzivatele

		for(int a = 0;a<line_count;a++){
			for(int b = 0;b<max_string_lenght;b++){
				lines[a][b] = '\0';
			}
		}
		char *ptr = strtok(resp, delim);
		i = 0;
		while(ptr != NULL)
		{
			if(((args->filtered == 1) && contains_wanted_word(ptr))
					||((args->filtered == 0) && is_not_comment(ptr))){ // Uzivatel chce filtrovat vystup a dany radek v ptr je mozne zobrazit, NEBO si uzivatel nepreje filtrovat vystup a radek v ptr neni komentar
				strcpy(lines[last_i], ptr);
				if(lines[last_i][strlen(lines[last_i])-1]=='\r'){
					lines[last_i][strlen(lines[last_i])-1] = '\0';
				}
				last_i++;
			}
			ptr = strtok(NULL, delim);
		}

		int len = 0;
		int max_len = 0;
		for(int i = 0; i < last_i; i++){
			len = strlen(lines[i]);
			if(len > max_len){
				max_len = len;
			}
		}

		// Dale nasleduji ruzne vypocty a vypisy znaku tak, aby se odpoved vypisovala do prehledne tabulky
		int header_len = qname_length + 20;

		int diff;
		if(header_len > max_len){
			diff = header_len - max_len;
			printf("--WHOIS odpověď pro '%s'--\n", qname);

			for(i = 0;i<last_i;i++){
				printf("|%s",lines[i]);
				for(int j = 0;j<(max_len - strlen(lines[i])+2);j++){
					printf(" ");
				}
				for(int i = 0; i < diff;i++){
					printf(" ");
				}
				printf("|\n");
			}

			for(int i = 0; i < header_len+4;i++){
				printf("-");
			}
			printf("\n\n");
		}else{
			diff = max_len - header_len;
			for(int i = 0; i < diff/2+2;i++){
				printf("-");
			}
			printf("WHOIS odpověď pro '%s'", qname);
			for(int i = 0; i < diff/2+2;i++){
				printf("-");
			}
			printf("\n");

			for(i = 0;i<last_i;i++){
				printf("|%s",lines[i]);
				for(int j = 0;j<(max_len - strlen(lines[i])+2);j++){
					printf(" ");
				}
				printf("|\n");
			}

			for(int i = 0; i < max_len+4;i++){
				printf("-");
			}
			printf("\n\n");
		}
	}else{//Server nema zadne zaznamy pro pozadavek uzivatele
		// Nasleduji ruzne vypocty a vypisy znaku tak, aby se odpoved vypisovala do prehledne tabulky
		if(slow_down != 0){
			int header_len = qname_length + 20;
			int no_entries_len = 62;
			int diff;
			if(header_len > no_entries_len){
				diff = header_len - no_entries_len;
				printf("--WHOIS odpověď pro '%s'--", qname);
				printf("\n|%s", resp);

				for(int i = 0; i < diff+2;i++){
					printf(" ");
				}
				printf("|\n");
				for(int i = 0; i < header_len+4;i++){
					printf("-");
				}
				printf("\n\n");
			}else{
				diff = no_entries_len - header_len;
				for(int i = 0; i < diff/2+1;i++){
					printf("-");
				}
				printf("WHOIS odpověď pro '%s'", qname);
				for(int i = 0; i < diff/2+1;i++){
					printf("-");
				}
				printf("\n|%s|\n", resp);

				for(int i = 0; i < no_entries_len+2;i++){
					printf("-");
				}
				printf("\n\n");
			}

		}else{//Your connection limit exceeded. Please slow down and try again later.

		}
	}




	return SUCCESS;
}

// Funkce vraci 1, pokud se v retezci str naleza chtene slovo (z words). Jinak vraci 0
int contains_wanted_word(char *str){
	char words[21][20] = {"inetnum:", "inet6num:", "netname:", "descr:", "country:", "person:", "org-name:", "address:", "phone:", "admin-c:", "NetRange", "NetName", "Address:", "City:", "StateProv:", "PostalCode:", "Country:", "NetRange:", "Address:", "name", "Name"};
	for(int i = 0; i < 21;i++){
		if(strstr(str, words[i]) != NULL){ //Musime zajistit, ze se v retezci str naleza nektere z hledanych slov a zaroven ze se nejedna o komentar
			if((str[0] != '%') && (strstr(str, "remarks:") == NULL) && (strstr(str, "comment:") == NULL)){
				return 1;
			}
		}
	}

	return 0;
}

//vraci konstantu TRUE, pokud se nejedna o komentar. Vyuzivame pri vypisu odpovedi WHOIS serveru pro vyfiltrovani komentaru
int is_not_comment(char *str){

	if(str[0] == '%'){
		return FALSE;
	}
	if (strstr(str, "remarks:") != NULL) {
		return FALSE;
	}
	if (strstr(str, "comment:") != NULL) {
		return FALSE;
	}

	return TRUE;
}

// Funkce vrací maximalni delku retezce - bud predavanou (max_string_lenght), nebo novou (danou rozdilem pos a last_line_feed)
int get_max_string_lenght(int last_line_feed, int pos, int max_string_lenght){
	if((pos - last_line_feed) > max_string_lenght){
		return (pos - last_line_feed);
	}
	return max_string_lenght;
}
