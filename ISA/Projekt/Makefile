# Projekt: isa-tazatel
# Autor:   Petr Marek (xmarek69)
# Datum:   23. 10. 2019

#Preklad
hello: isa-tazatel.c
	gcc isa-tazatel.c -o isa-tazatel -lresolv

#Příklad použití domeny na kterou se tážeme WHOIS serveru
test1: isa-tazatel.c
	gcc isa-tazatel.c -o isa-tazatel -lresolv
	./isa-tazatel -q seznam.cz -w whois.ripe.net

#Příklad použití IPv4 adresy na kterou se tážeme WHOIS serveru
test2: isa-tazatel.c
	gcc isa-tazatel.c -o isa-tazatel -lresolv
	./isa-tazatel -q 77.75.75.176 -w whois.ripe.net

#Příklad použití IPv6 adresy na kterou se tážeme WHOIS serveru
test3: isa-tazatel.c
	gcc isa-tazatel.c -o isa-tazatel -lresolv
	./isa-tazatel -q 2001:67c:1220:809::93e5:917 -w whois.ripe.net

#Příklad specifikování IPv4 adresy DNS serveru
test4: isa-tazatel.c
	gcc isa-tazatel.c -o isa-tazatel -lresolv
	./isa-tazatel -q www.seznam.cz -w whois.nic.cz -d 8.8.8.8

#Příklad použití IPv6 adresy DNS serveru
test5: isa-tazatel.c
	gcc isa-tazatel.c -o isa-tazatel -lresolv
	./isa-tazatel -q www.seznam.cz -w whois.nic.cz -d 2001:67c:1220:809::93e5:917

#Příklad použití IPv4 adresy WHOIS serveru
test6: isa-tazatel.c
	gcc isa-tazatel.c -o isa-tazatel -lresolv
	./isa-tazatel -q 2a02:598:3333:1::2 -w 193.0.6.135 

#Příklad použití IPv6 adresy WHOIS serveru
test7: isa-tazatel.c
	gcc isa-tazatel.c -o isa-tazatel -lresolv
	./isa-tazatel -q 2a02:598:3333:1::2 -w 2001:67c:2e8:22::c100:687

#Příklad použití argumentu -e (rozšířené funkčnosti programu)
test8: isa-tazatel.c
	gcc isa-tazatel.c -o isa-tazatel -lresolv
	./isa-tazatel -q www.seznam.cz -w whois.ripe.net -e

#Příklad použití argumentu -s (vypis oznameni o chybach)
test9: isa-tazatel.c
	gcc isa-tazatel.c -o isa-tazatel -lresolv
	./isa-tazatel -q fit.vutbr.cz -w whois.nic.cz -e -s

#Odstraneni vytvorene binarky
clean:
	rm -f isa-tazatel


