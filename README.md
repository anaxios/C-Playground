# On the Grid

*This has not been tested for actual use*

### Build:
*Only tested on Linux*

Requires libgcrypt

Just run `> make`

### Run:

Type the command into your terminal followed by your master password then the URL 
and it will produce your grid and your site password.


for example:
`> ./latin-square monkey123 aol.com`

```
0 1 2 3 4 5 6 7 8 9 - . 
L R C T S Y W J Q M I X V H A O D E G B U K Z N P F 
V U D L E B S H J P N O M A I T Q R F C Z X K G Y W 
R I T E H X J M V Z B W U P Y S L A D O N F G C K Q 
K F M Z N L I B Y O Q R X C D U P G H V W E S J T A 
J M F Q L N T E S A Z C H R U D W V X G P B Y K I O 
X W P K G V N C B T J U O D Q Z Y F A M S R E H L I 
F Q Z G C R B O X S V I W T L N K D P U J A H M E Y 
I B E A P W M Z U G O J N K X H R Y L S C Q D T F V 
E A O S J K Q V L U Y F R M P W T H C X I G N B Z D 
T E B O W P F Q D V A K L J H X C S N Y R Z U I M G 
D L N C O A X W F J R Y Q S E B G T Z I V P M U H K 
Y X H P Z Q U N I C W V B G F M A K E J O L T S D R 
P K J M U D R I A B F L Y N G V H Z S Q X T O W C E 
G D U N B E Y X K W L A F O T I Z C M R Q H J V S P 
O S Y X F M G D C L H Z T Q J K B W I P E U R A V N 
H P W J V G L R E I K D A U Z Q S M O F Y C B X N T 
Z G V U I T A Y P X D E K B C R M N J L F S W Q O H 
C T I B X H K F G Q E P D W S Y N O U A L M V R J Z 
S H X W Q Z D L T R P G E V M F O J B K A N I Y U C 
M Z Q V R C E A H Y G T P I N L J U W D K O X F B S 
N C R I Y S P K Z F T H G X O A U B V E D J Q L W M 
A Y S H M F V U R N X Q I Z K J E P T W B D C O G L 
Q V G D T I O S W H U B J E R C F L K N M Y P Z A X 
U N L R A O H P M K C S Z Y B E V I Q T G W F D X J 
B O A Y K J Z G N D S M C F W P I X R H T V L E Q U 
W J K F D U C T O E M N S L V G X Q Y Z H I A P R B 

Password: ECJUFPQRUDWJ
```

If you only put your master password and no url it will produce your grid.

If you want a random grid: `> ./latin-square`

### How to Use:
based on Steve Gibson's cool [Off the Grid](https://www.grc.com/offthegrid.htm) System. Check there to learn what it is and how to use it.

### Latin Square Dance:
I call the algorithm used to scramble the grid "Latin Square Dance".

**caveat emptor** I have no idea how cryptography works. I don't even have a reasonable guess as to how encrypted the grids come out. Nevertheless, I will try to describe the algorithm.

- Produce a 26 x 26 latin square of the alphabet row wise in order, shifting each row one character to the left.

```
A B C D E F G H I J K L M N O P Q R S T U V W X Y Z 
B C D E F G H I J K L M N O P Q R S T U V W X Y Z A 
C D E F G H I J K L M N O P Q R S T U V W X Y Z A B 
D E F G H I J K L M N O P Q R S T U V W X Y Z A B C 
E F G H I J K L M N O P Q R S T U V W X Y Z A B C D 
F G H I J K L M N O P Q R S T U V W X Y Z A B C D E 
G H I J K L M N O P Q R S T U V W X Y Z A B C D E F 
H I J K L M N O P Q R S T U V W X Y Z A B C D E F G 
I J K L M N O P Q R S T U V W X Y Z A B C D E F G H 
J K L M N O P Q R S T U V W X Y Z A B C D E F G H I 
K L M N O P Q R S T U V W X Y Z A B C D E F G H I J 
L M N O P Q R S T U V W X Y Z A B C D E F G H I J K 
M N O P Q R S T U V W X Y Z A B C D E F G H I J K L 
N O P Q R S T U V W X Y Z A B C D E F G H I J K L M 
O P Q R S T U V W X Y Z A B C D E F G H I J K L M N 
P Q R S T U V W X Y Z A B C D E F G H I J K L M N O 
Q R S T U V W X Y Z A B C D E F G H I J K L M N O P 
R S T U V W X Y Z A B C D E F G H I J K L M N O P Q 
S T U V W X Y Z A B C D E F G H I J K L M N O P Q R 
T U V W X Y Z A B C D E F G H I J K L M N O P Q R S 
U V W X Y Z A B C D E F G H I J K L M N O P Q R S T 
V W X Y Z A B C D E F G H I J K L M N O P Q R S T U 
W X Y Z A B C D E F G H I J K L M N O P Q R S T U V 
X Y Z A B C D E F G H I J K L M N O P Q R S T U V W 
Y Z A B C D E F G H I J K L M N O P Q R S T U V W X 
Z A B C D E F G H I J K L M N O P Q R S T U V W X Y
```

- Create a sha512 hash from a passphrase of your choosing:
  - passphrase: "monkey123"
  - sha512: `f4461bf763358e97d9a49c28bae22c360711cde072238e67b624fb75a0f2cbb40ad4e36765ca086c94ab4c60a72fcf154bc97de8e2255f9dc5a9378cd5868d55`
- using each character but the last (511 characters) of the hash swap either two sets of characters, two rows or two columns picking between them using a modulus of 3. *(These operation preserve the "Latinness" of the square.)*
  - 0 -> swap characters
  - 1 -> swap rows
  - 2 -> swap columns
- The two rows, columns, or characters to be swapped are:
  - The current character used in the previous operation and
  - The one immediately following. *(Which is why only 511 operations are done.)*
  - These are modulo the length of the alphabet.

At this point I'm not sure the size of each character being used whether it's a byte or an unsighned long or whatever. I'm not even sure if it matters.

*Future:
- make upper and lower case based on the sha512 hash.*

