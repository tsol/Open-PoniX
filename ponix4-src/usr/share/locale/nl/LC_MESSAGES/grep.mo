��    o      �  �         `	  �   a	  �   1
  0  �
  �  -     �  /    %  =  Q  c  g  �  k    G  �  %   �     �  0        <     Y  ,   u  ,   �  '   �  -   �      %  (   F  (   o     �     �     �     �     �     �  Q        Y  ?   s     �     �     �       $        @     R  <   m  <   �     �     �            5   -  1   c  :   �     �     �  #   �          4  3   K          �  &   �     �     �     �     �       (        B  �   W     5  ;   L  3   �  /   �  +   �  '     #   @     d     �     �     �  q   �     $  4   A     v  )   �  Z   �  F         a      }      �      �      �      �      �   $   !     5!     B!     Y!     j!  >   ~!     �!  M   �!  P   $"  ,   u"     �"     �"     �"     �"     �"     �"     #     &#     6#     B#  �  Q#  �   �$  %  �%  E  
'  �  P(  m   E*  �  �*  V  u,  y  �-  �  F/  �  1  �  �2  +   m4     �4  4   �4  (   �4     5  )   ,5  )   V5  &   �5  *   �5  $   �5  '   �5  '   6     G6     c6     6     �6     �6  +   �6  d   �6     )7  J   G7     �7     �7     �7     �7  *   �7     '8     <8  I   Z8  I   �8      �8     9     9      09  <   Q9  6   �9  ;   �9     :  $   !:  (   F:     o:  q   �:  E   ;     G;     N;  &   e;     �;     �;     �;     �;     �;  -   �;     <    0<     B=  ?   ]=  6   �=  2   �=  .   >  *   6>  &   a>  "   �>     �>     �>     �>  l   �>  $   P?  5   u?     �?  '   �?  [   �?  J   O@  &   �@  %   �@     �@     A  $   A     >A     \A  6   tA     �A     �A      �A     �A  ?   B     NB  V   iB  J   �B  +   C     7C     DC     QC     ^C     wC     �C     �C     �C     �C     �C     _   P   j   ;      C       .   Y   R   1       !       c   G           o   Z         L   =       V      
            `   n   7   O      i   [          -          @      4   ?          #           3   &           J   <       h   N           E   ]   /   g           >          (   a       8      S   2             K   0       %   9   ^   M   D       \      f      B       F       d   "   X       ,          *      6   b   $   )      e   :       W   5      T                 U   l       A       H      +               k       I                 '   Q   	   m           
Context control:
  -B, --before-context=NUM  print NUM lines of leading context
  -A, --after-context=NUM   print NUM lines of trailing context
  -C, --context=NUM         print NUM lines of output context
 
License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>.
This is free software: you are free to change and redistribute it.
There is NO WARRANTY, to the extent permitted by law.

 
Miscellaneous:
  -s, --no-messages         suppress error messages
  -v, --invert-match        select non-matching lines
  -V, --version             print version information and exit
      --help                display this help and exit
      --mmap                ignored for backwards compatibility
 
Output control:
  -m, --max-count=NUM       stop after NUM matches
  -b, --byte-offset         print the byte offset with output lines
  -n, --line-number         print line number with output lines
      --line-buffered       flush output on every line
  -H, --with-filename       print the filename for each match
  -h, --no-filename         suppress the prefixing filename on output
      --label=LABEL         print LABEL as filename for standard input
 
Report bugs to: %s
       --include=FILE_PATTERN  search only files that match FILE_PATTERN
      --exclude=FILE_PATTERN  skip files and directories matching FILE_PATTERN
      --exclude-from=FILE   skip files matching any file pattern from FILE
      --exclude-dir=PATTERN  directories that match PATTERN will be skipped.
   -E, --extended-regexp     PATTERN is an extended regular expression (ERE)
  -F, --fixed-strings       PATTERN is a set of newline-separated fixed strings
  -G, --basic-regexp        PATTERN is a basic regular expression (BRE)
  -P, --perl-regexp         PATTERN is a Perl regular expression
   -L, --files-without-match  print only names of FILEs containing no match
  -l, --files-with-matches  print only names of FILEs containing matches
  -c, --count               print only a count of matching lines per FILE
  -T, --initial-tab         make tabs line up (if needed)
  -Z, --null                print 0 byte after FILE name
   -NUM                      same as --context=NUM
      --color[=WHEN],
      --colour[=WHEN]       use markers to highlight the matching strings;
                            WHEN is `always', `never', or `auto'
  -U, --binary              do not strip CR characters at EOL (MSDOS)
  -u, --unix-byte-offsets   report offsets as if CRs were not there (MSDOS)

   -e, --regexp=PATTERN      use PATTERN for matching
  -f, --file=FILE           obtain PATTERN from FILE
  -i, --ignore-case         ignore case distinctions
  -w, --word-regexp         force PATTERN to match only whole words
  -x, --line-regexp         force PATTERN to match only whole lines
  -z, --null-data           a data line ends in 0 byte, not newline
   -o, --only-matching       show only the part of a line matching PATTERN
  -q, --quiet, --silent     suppress all normal output
      --binary-files=TYPE   assume that binary files are TYPE;
                            TYPE is `binary', `text', or `without-match'
  -a, --text                equivalent to --binary-files=text
 %s can only use the %s pattern syntax %s home page: <%s>
 %s home page: <http://www.gnu.org/software/%s/>
 %s%s argument `%s' too large %s: invalid option -- '%c'
 %s: option '%c%s' doesn't allow an argument
 %s: option '--%s' doesn't allow an argument
 %s: option '--%s' requires an argument
 %s: option '-W %s' doesn't allow an argument
 %s: option '-W %s' is ambiguous
 %s: option '-W %s' requires an argument
 %s: option requires an argument -- '%c'
 %s: unrecognized option '%c%s'
 %s: unrecognized option '--%s'
 ' (C) (standard input) Binary file %s matches
 Example: %s -i 'hello world' menu.h main.c

Regexp selection and interpretation:
 GNU Grep home page: <%s>
 General help using GNU software: <http://www.gnu.org/gethelp/>
 Invalid back reference Invalid character class name Invalid collation character Invalid content of \{\} Invalid preceding regular expression Invalid range end Invalid regular expression Invocation as `egrep' is deprecated; use `grep -E' instead.
 Invocation as `fgrep' is deprecated; use `grep -F' instead.
 Memory exhausted Mike Haertel No match No previous regular expression PATTERN is a set of newline-separated fixed strings.
 PATTERN is an extended regular expression (ERE).
 PATTERN is, by default, a basic regular expression (BRE).
 Packaged by %s
 Packaged by %s (%s)
 Premature end of regular expression Regular expression too big Report %s bugs to: %s
 Search for PATTERN in each FILE or standard input.
 Success Trailing backslash Try `%s --help' for more information.
 Unknown system error Unmatched ( or \( Unmatched ) or \) Unmatched [ or [^ Unmatched \{ Usage: %s [OPTION]... PATTERN [FILE]...
 Valid arguments are: With no FILE, or when FILE is -, read standard input.  If less than two FILEs
are given, assume -h.  Exit status is 0 if any line was selected, 1 otherwise;
if any error occurs and -q was not given, the exit status is 2.
 Written by %s and %s.
 Written by %s, %s, %s,
%s, %s, %s, %s,
%s, %s, and others.
 Written by %s, %s, %s,
%s, %s, %s, %s,
%s, and %s.
 Written by %s, %s, %s,
%s, %s, %s, %s,
and %s.
 Written by %s, %s, %s,
%s, %s, %s, and %s.
 Written by %s, %s, %s,
%s, %s, and %s.
 Written by %s, %s, %s,
%s, and %s.
 Written by %s, %s, %s,
and %s.
 Written by %s, %s, and %s.
 Written by %s.
 ` `egrep' means `grep -E'.  `fgrep' means `grep -F'.
Direct invocation as either `egrep' or `fgrep' is deprecated.
 ambiguous argument %s for %s character class syntax is [[:space:]], not [:space:] conflicting matchers specified in GREP_COLORS="%s", the "%s" capacity %s in GREP_COLORS="%s", the "%s" capacity is boolean and cannot take a value ("=%s"); skipped in GREP_COLORS="%s", the "%s" capacity needs a value ("=..."); skipped input is too large to count invalid %s%s argument `%s' invalid argument %s for %s invalid character class invalid context length argument invalid matcher %s invalid max count invalid suffix in %s%s argument `%s' lseek failed malformed repeat count memory exhausted no syntax specified others, see <http://git.sv.gnu.org/cgit/grep.git/tree/AUTHORS> recursive directory loop stopped processing of ill-formed GREP_COLORS="%s" at remaining substring "%s" support for the -P option is not compiled into this --disable-perl-regexp binary the -P option only supports a single pattern unbalanced ( unbalanced ) unbalanced [ unfinished \ escape unfinished repeat count unknown binary-files type unknown devices method warning: %s: %s write error writing output Project-Id-Version: grep-2.7
Report-Msgid-Bugs-To: bug-grep@gnu.org
POT-Creation-Date: 2011-06-21 20:06+0200
PO-Revision-Date: 2010-09-21 09:59+0200
Last-Translator: Benno Schulenberg <benno@vertaalt.nl>
Language-Team: Dutch <vertaling@vrijschrift.org>
Language: nl
MIME-Version: 1.0
Content-Type: text/plain; charset=UTF-8
Content-Transfer-Encoding: 8bit
X-Generator: Lokalize 1.0
Plural-Forms: nplurals=2; plural=n != 1;
 
Contextbesturing:
  -B, --before-context=AANTAL   dit AANTAL regels voorafgaande context tonen
  -A, --after-context=AANTAL    dit AANTAL regels nakomende context tonen
  -C, --context=AANTAL          dit AANTAL regels context tonen
 
Dit is vrije software: u mag het vrijelijk wijzigen en verder verspreiden.
De precieze licentie is GPL-3+: GNU General Public License versie 3 of later.
Zie http://gnu.org/licenses/gpl.html voor de volledige (Engelse) tekst.
Deze software kent GEEN GARANTIE, voor zover de wet dit toestaat.

 
Diversen:
  -s, --no-messages         foutmeldingen onderdrukken
  -v, --invert-match        de niet-overeenkomende regels selecteren
  -V, --version             programmaversie tonen en stoppen
      --help                deze hulptekst tonen en stoppen
      --mmap                (genegeerd; herkend voor compatibilteit)
 
Uitvoeropties:
  -m, --max-count=AANTAL    na dit AANTAL overeenkomsten stoppen
  -b, --byte-offset         het byte-adres bij de uitvoerregels tonen
  -n, --line-number         het regelnummer bij de uitvoerregels tonen
      --line-buffered       na elke gevonden regel uitvoer produceren
  -H, --with-filename       bij iedere overeenkomst de bestandsnaam tonen
  -h, --no-filename         nooit de bestandsnaam tonen
      --label=LABEL         dit LABEL als naam voor standaardinvoer gebruiken
 
Rapporteer gebreken in het programma aan <%s>;
meld fouten in de vertaling aan <vertaling@vrijschrift.org>.
       --include=BESTANDSPATROON  alleen bestanden doorzoeken die aan
                                   BESTANDSPATROON voldoen
      --exclude=BESTANDSPATROON  bestanden overslaan die aan BESTANDSPATROON
                                   voldoen
      --exclude-from=BESTAND     bestanden overslaan die aan een patroon in
                                   BESTAND voldoen
      --exclude-dir=PATROON      mappen overslaan die aan PATROON voldoen
   -E, --extended-regexp     PATROON is een uitgebreide reguliere expressie
  -F, --fixed-strings       PATROON is een serie tekenreeksen,
                              elke tekenreeks op een aparte regel
  -G, --basic-regexp        PATROON is een gewone reguliere expressie
  -P, --perl-regexp         PATROON is een reguliere Perl-expressie
   -L, --files-without-match alleen bestandsnamen zonder overeenkomst tonen
  -l, --files-with-matches  alleen bestandsnamen met overeenkomsten tonen
  -c, --count               alleen het aantal overeenkomsten per bestand tonen
  -T, --initial-tab         tabs uitlijnen (eventueel een tab-teken invoegen)
  -Z, --null                een 0-byte invoegen na iedere bestandsnaam
   -AANTAL                       hetzelfde als '--context=AANTAL'
      --color[=WANNEER],
      --colour[=WANNEER]    overeenkomende tekst gekleurd weergeven;
                              WANNEER is 'always', 'never' of 'auto'
                              (ofwel altijd, nooit, of apparaat-afhankelijk)
  -U, --binary              geen CR-tekens weghalen bij regeleinde (MSDOS)
  -u, --unix-byte-offsets   adressen tonen alsof CR's er niet waren (MSDOS)

   -e, --regexp=PATROON      dit PATROON gebruiken bij het zoeken
  -f, --file=BESTAND        patronen uit dit BESTAND halen
  -i, --ignore-case         verschil tussen hoofd- en kleine letters negeren
  -w, --word-regexp         patronen komen alleen overeen met een heel woord
  -x, --line-regexp         patronen komen alleen overeen met een hele regel
  -z, --null-data           regels eindigen op 0-byte, niet op LF-teken
   -o, --only-matching       alleen het overeenkomende regelfragment tonen
  -q, --quiet, --silent     alle normale uitvoer onderdrukken
      --binary-files=TYPE   aannemen dat binaire bestanden van dit TYPE zijn;
                              TYPE is 'binary' (binair), 'text' (als tekst),
                              of 'without-match' (alsof geen overeenkomsten)
  -a, --text                hetzelfde als '--binary-files=text'
 %s kan alleen de patroonsyntax %s gebruiken Webpagina van %s: <%s>
 Webpagina van %s: <http://www.gnu.org/software/%s/>
 argument '%3$s' van %1$s%2$s is te groot %s: ongeldige optie -- '%c'
 %s: optie '%c%s' staat geen argument toe
 %s: optie '--%s' staat geen argument toe
 %s: optie '--%s' vereist een argument
 %s: optie '-W %s' staat geen argument toe
 %s: optie '-W %s' is niet eenduidig
 %s: optie '-W %s' vereist een argument
 %s: optie vereist een argument -- '%c'
 %s: onbekende optie '%c%s'
 %s: onbekende optie '--%s'
 ’ © (standaardinvoer) Binair bestand %s bevat de gezochte tekst.
 Voorbeeld:  %s -i 'hallo wereld' menu.h main.c

Selectie en interpretatie van reguliere expressies:
 Webpagina van GNU grep: <%s>
 Algemene hulp bij gebruik van GNU-software: <http://www.gnu.org/gethelp/>
 Ongeldige terugverwijzing Ongeldige tekenklassenaam Ongeldig samengesteld teken Ongeldige inhoud van \{\} Ongeldige voorafgaande reguliere expressie Ongeldig bereikeinde Ongeldige reguliere expressie Gebruik van 'egrep' wordt ontraden; het is beter 'grep -E' te gebruiken.
 Gebruik van 'fgrep' wordt ontraden; het is beter 'grep -F' te gebruiken.
 Onvoldoende geheugen beschikbaar Mike Haertel Geen overeenkomsten Geen eerdere reguliere expressie PATROON is een serie tekenreeksen, elk op een aparte regel.
 PATROON is een uitgebreide reguliere expressie (ERE).
 PATROON is standaard een gewone reguliere expressie (BRE).
 In pakketvorm gebracht door %s
 In pakketvorm gebracht door %s (%s)
 Voortijdig einde van reguliere expressie Reguliere expressie is te groot Rapporteer gebreken in het programma '%s' aan <%s>;
meld fouten in de vertaling aan <vertaling@vrijschrift.org>.
 Naar PATROON zoeken in ieder gegeven BESTAND, of in standaardinvoer.
 Gelukt Backslash aan het eind Typ '%s --help' voor meer informatie.
 Onbekende systeemfout Ongepaarde ( of \( Ongepaarde ) of \) Ongepaarde [ of [^ Ongepaarde \{ Gebruik:  %s [OPTIE]... PATROON [BESTAND]...
 Geldige argumenten zijn: Indien er geen BESTAND gegeven is, of BESTAND is -, dan wordt standaardinvoer
gelezen.  Indien er minder dan twee BESTANDen gegeven zijn, dan wordt -h
aangenomen.  De afsluitwaarde is 0 in geval van overeenkomsten,
1 indien geen overeenkomsten, en 2 indien er iets mis is.
 Geschreven door %s en %s.
 Geschreven door %s, %s, %s,
%s, %s, %s, %s,
%s, %s en anderen.
 Geschreven door %s, %s, %s,
%s, %s, %s, %s,
%s en %s.
 Geschreven door %s, %s, %s,
%s, %s, %s, %s
en %s.
 Geschreven door %s, %s, %s,
%s, %s, %s en %s.
 Geschreven door %s, %s, %s,
%s, %s en %s.
 Geschreven door %s, %s, %s,
%s en %s.
 Geschreven door %s, %s, %s
en %s.
 Geschreven door %s, %s en %s.
 Geschreven door %s.
 ‘ Het gebruik van 'egrep' en 'fgrep' wordt ontraden;
'egrep' betekent 'grep -E'; 'fgrep' betekent 'grep -F'.

 argument %s van %s is niet eenduidig syntax van tekenklasse is [[:space:]], niet [:space:] conflicterende expressiesoorten GREP_COLORS='%s' --
  Kenmerk '%s': %s. GREP_COLORS='%s' --
  Kenmerk '%s' is booleaans en verwacht geen waarde ('=%s'); genegeerd. GREP_COLORS='%s' --
  Kenmerk '%s' vereist een waarde ('=...'); genegeerd. invoer is te groot om te kunnen tellen ongeldig argument '%3$s' van %1$s%2$s ongeldig argument %s van %s ongeldige tekenklasse ongeldig argument voor contextlengte ongeldige expressiesoort '%s' ongeldig maximum aantal ongeldig achtervoegsel in argument '%3$s' van %1$s%2$s 'lseek' is mislukt onjuist herhalingsaantal onvoldoende geheugen beschikbaar geen syntax opgegeven anderen, zie <http://git.sv.gnu.org/cgit/grep.git/tree/AUTHORS> oneindige lus in de mappen GREP_COLORS='%s' --
  Tekenreeks is onjuist; verwerking is gestopt voor subreeks '%s'. ondersteuning voor de optie '-P' is in deze programmaversie niet opgenomen optie '-P' accepteert slechts één patroon ongepaarde ( ongepaarde ) ongepaarde [ onafgemaakte \-stuurcode onafgemaakt herhalingsaantal onbekend binair bestandstype onbekende apparaten-actie waarschuwing: %s: %s schrijffout schrijven van uitvoer... 