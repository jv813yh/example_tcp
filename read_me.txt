##################################################
## Diplomova  praca                             ##
## Meno studenta: Jozef Vendel                  ##
## Veduci DP: prof. Ing. Milos Drutarovsky CSc. ##
## Skola: KEMT FEI TUKE                         ##
## Datum vytvorenia: 21.12.2021	                ##
##################################################

Zoznam suborov
  
 simple_example_tcp
      |__compiling.bat
      |__tcp_client.c
      |__tcp_serve_toupper.c
      |__tcp_server.bat
      |__tcp_client.bat
      |__win_linux_socket.h


Program je kompilovatelny pomocou .bat suborov.
Subor compiling.bat prelozi zdrojove subory do spustitelnych programov.
Druhy a treti .bat subor zavola .exe subory servera a klienta.


Zlozka example_tcp obsahuje jednoduchy priklad servera a klienta
,ktory spolu kominukuju pomocou komunikacneho kanala TCP.
Server pracuje v Loopbacku na porte 8080 (hodnoty sa mozu menit,
su dane z prikazoveho riadku pri volani oboch zdrojovych kodov).
Klient, ktory pozna IP adresu
servera a cislo portu na ktorom pocuva server, sa pripoji na server
Klient odosiela data zadane z prikazoveho riadku.
Server poskytuje sluzbu, ktora prevadza male pismena v texte 
odosielaneho klientom na velke pismena a odosiela naspt klientovi.
Klient prijate data vypisuje na obrazovku.

Ziadne multiplexovanie socketov, je mozne obsluhovat iba jednu
obojsmernu komunikaciu bez vytvarania dalsich novych spojeni.

Windows/Linux

