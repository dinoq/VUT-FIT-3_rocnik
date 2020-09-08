/*
 * isa-tazatel.h
 *
 *  Created on: 23. 10. 2019
 *      Author: Petr Marek (xmarek69)
 */
#include <stdio.h>	// printf
#include <arpa/nameser.h> //konstanty pro dns
#include <resolv.h> //res_query
#include <string.h>	//strtok
#include <stdlib.h>	//realloc
#include <sys/socket.h>	//socket
#include <netinet/in.h> //sockaddr_in
#include <arpa/inet.h>	//getsockname
#include <netdb.h>	//hostent
#include <unistd.h>	//close
#include <sys/types.h>
#include <netinet/in.h>

#define WHOIS_PORT 43
#define IPv4_MAX_LENGTH 16
#define IPv6_MAX_LENGTH 40
#define DOMAIN_NAME_MAX_LENGTH 254
#define MAX_ADDRES_COUNT 3
#define FAILURE 1
#define SUCCESS 0
#define TRUE 1
#define FALSE 0
#define KB_64 65536

#define QUERY_IPv4 0
#define QUERY_IPv6 1
#define QUERY_HOSTNAME_WITHOUT_WWW 2
#define QUERY_HOSTNAME_WITH_WWW 3
#define QUERY_PTR_IPv4 4
#define QUERY_PTR_IPv6 5
#define QUERY_ERROR_MORE_DOTS 6
#define QUERY_ERROR_MORE_DOUBLE_COLONS 7
#define QUERY_ERROR_NOT_ALLOWED_CHARACTERS 8
#define QUERY_ERROR_INVALID_HOSTNAME 9
#define QUERY_ERROR_INVALID_IPv6 10
#define QUERY_ERROR 20

#define NO_IPv4_CHARACTER -1
#define NO_IPv6_CHARACTER -1
#define NO_MX_CHARACTER -1
#define NUMBER_CHARACTER 1
#define L_CASE_CHARACTER 2
#define U_CASE_CHARACTER 3
#define COLON_CHARACTER 4
#define DOT_CHARACTER 4

#define ERROR_MORE_DOUBLE_COLON -2

// Zde ukladame argumenty se kterymi byl program spusten
typedef struct ARGUMENT{
	char query[DOMAIN_NAME_MAX_LENGTH]; // Argument -q =
	char whois[DOMAIN_NAME_MAX_LENGTH]; // Argument -w = adresa WHOIS serveru
	char dns[DOMAIN_NAME_MAX_LENGTH]; // Argument -d = adresa DNS serveru
	char filtered; // Argument -f = priznak, zda se ma WHOIS odpoved vyfiltrovat
	char extended; // Argument -e = priznak, zda se maji zapnout rozsirene funkce programu
	char show_failures; // Argument -s = priznak, zda se maji zobrazovat oznameni o chybach
} Argument;

// Jedna se o jednocestny spojovy seznam
struct DNS_ANSWERS{
	unsigned char *data; // Data DNS zaznamu
	uint8_t type; // Typ DNS zaznamu
	struct DNS_ANSWERS *next; // Odkaz na dalsi prvek seznamu
};



int check_args(int argc , char *argv[], Argument **args);
int get_whois(char *wquery, char *whois_server_address, struct ARGUMENT *args);
char *get_edited_www(char *address);
int parse_WHOIS_response(char  *resp, char *qname, struct ARGUMENT *args);
int get_max_string_lenght(int last_line_feed, int pos, int max_string_lenght);
int contains_wanted_word(char *str);
int is_not_comment(char *str);
int double_colon_position(char *str);
int count_char(char *str, char ch);
int get_DNS_entries(struct DNS_ANSWERS **dns_ans, char *address);
int print_DNS_entries(struct DNS_ANSWERS **dns_ans, char *query);
char *get_dns_type(uint8_t type);
int get_type_of_query_name(char *qname);
int starts_with_www(char *address);
int valid_ipv6_char(char ch);
int valid_ipv4_char(char ch);
int valid_mx_ns_ptr_char(char ch);
struct DNS_ANSWERS * search_in_DNS(struct DNS_ANSWERS **dns_ans, char *address, int entry_type, int ans_type, int *moved);
char *edit_ipv4_to_ptr(char *qname);
char *edit_ipv6_to_ptr(char *qname);
