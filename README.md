# 2D Projekat Akvarijum

Predmet: Racunarska grafika
Opis: 
  -2D mini igra, uradjena u C++ koriscenjem OpenGL-a
  -Tema: Ribe u akvarijumu
Specifikacija:
Napraviti 2D grafičku aplikaciju za simulaciju akvarijuma.

Akvarijum treba ima pravougaoni oblik i da zauzima celu širinu ekrana i >50% visine ekrana, tako da dodiruje donju ivicu ekrana. Donja, leva i desna ivica akvarijuma treba da budu označene crnim linijama sa debljinom od 10 piksela. Preko cele površine akvarijuma je prisutan četvorougao bele boje koji ima providnost 0,2.

U akvarijumu je potrebno napuniti dno teksturom peska. Na proizvoljnim mestima se nalaze bar dve stabljike morske trave koja rastu iz peska. Na proizvoljnom mestu na pesku se nalazi kovčeg koji je na početku programa zatvoren, a otvara se pritiskom na taster C. Kada je otvoren, u njemu se nalazi nekoliko zlatnih novčića i jedan dragulj, a moguće ga je zatvoriti pritiskom na isti taster.

U akvarijumu plivaju dve ribice: zlatna ribica i riba-klovn. Zlatna ribica se pomera po vodi pomoću WASD, a riba-klovn strelicama. Svaki put kada promeni smer kretanja levo-desno, tekstura ribice se izokrene kao u ogledalu (ovaj način kretanja važi za obe ribice). Pritiskom na taster Z, zlatna ribica ispusti tri mehurčića koji idu ka površini i nestanu kad izađu iz vode. Klovn-riba isto ovo radi, ali pritiskom na taster K.

Pritiskom na taster Enter, odgore (van ekrana) u vodu padne nekoliko čestica hrane za ribice: padaju konstantnom brzinom i zaustavljaju se kad dođu do peska. Dok su čestice u vodi, svaki put kada neka ribica pređe preko jedne čestice, ta čestica nestane, a visina poligona koja predstavlja ribicu (debljina ribice) koja je “pojela” česticu se poveća za 0,01.

Kursor treba da ima oblik sidra, tako da je vrh sidra u gornjem levom uglu, a kuke za prizemljenje u donjem levom i gornjem desnom uglu.

