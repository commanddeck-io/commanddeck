// Modes
#define ARRAY_SIZE(x) sizeof(x)/sizeof(x[0])

const byte n_rows = 4;
const byte n_cols = 4;

// Define the names of your modes, and on the array below map the keypad keys to commands, in the same order you defined the modes
static char *modes[] PROGMEM = {"Spacemacs Clojure",
                                "Spacemacs Arduino",
                                "Spacemacs Go",
                                "Spacemacs i3",
                                "OBS",
                                "test",
                                "Info"};

const byte numModes = ARRAY_SIZE(modes);

// Define your keybindings here
/* Just think of your keypad positions
   1 | 2 | 3 | A
   4 | 5 | 6 | B
   7 | 8 | 9 | C
   * | 0 | # | D
   */

/*
  You can use the following resources:

  // Modifiers
  L_CTRL  = Left Control
  L_ALT   = Left Alt
  L_SHIFT = Left Shift
  R_CTRL  = Right Control
  R_ALT   = Right Alt
  R_SHIFT = Right Shift
  L_WIN   = Left Windows Key
  R_WIN   = Right Windows Key

  // Space
  SPC = a literal space, needed since by default spaces are command separators

  // A releaseAll is always invoked at the end, but you can issue it at any time
  REL = Release all modifier keys (Ctrl, Alt, Shift, Win)

  // Arrow keys
  L_ARROW = Left Arrow
  R_ARROW = Right Arrow
  U_ARROW = Up Arrow
  D_ARROW = Down Arrow

  // Other keys
  RET      = Enter
  ESC      = Esc
  BACK     = Backspace
  TAB      = Tab
  INS      = Insert
  DEL      = Delete
  PG_UP    = Page Up
  PG_DOWN  = Page Down
  HOME     = Home
  END      = End
  CAPS     = Caps Lock

  // Function keys
  F1  = Function Key 1
  F2  = Function Key 2
  F3  = Function Key 3
  F4  = Function Key 4
  F5  = Function Key 5
  F6  = Function Key 6
  F7  = Function Key 7
  F8  = Function Key 8
  F9  = Function Key 9
  F10 = Function Key 10
  F11 = Function Key 11
  F12 = Function Key 12

  // Timing commands
  T_10    = wait for 10 miliseconds, change 10 to any number you want
  DT_10   = wait for 10 miliseconds between every command after this one, change 10 to any number you want, default is no wait
  +L_CTRL = hold a special key, replace L_CTRL with any other key
  -L_CTRL = release a special key, replace L_CTRL with any other key

  Any other string will be interpreted literally, except for spaces (use SPC for that)
*/

/* Example: Ctrl+Alt+Del
   > +L_CTRL +L_ALT +L_DEL

   Example: Input with delay
   > S T_300 L T_300 O T_300 W T_300 SPC H T_300 E T_300 L T_300 L T_300 O

   Example: Input with a default delay, same as above with less commands
   > DT_300 S L O W SPC H E L L O

   Example: typing literals, just put a space anywhere
   > This SPC will SPC input SPC L_ CTRL SPC because SPC it SPC has SPC an SPC space

*/
static char *keys[numModes][n_rows][n_cols] PROGMEM = {
    { // Clojure
      {"#ESC|#ESC|meb", "#ESC|#ESC|mef", "lein clean; lein lint; lein lint-fix; lein test|#RET", ""},
      {""                 , ""                , ""                                                                    , ""},
      {""                 , ""                , ""                                                                    , ""},
      {"Clojure"          , ""                , ""                                                                    , ""}
    },
    { // Arduino
     {"#ESC|#ESC|#+L_CTRL C|#-L_CTRL|iu", "", ""                             , ""},
     {""                             , "", ""                             , ""},
     {""                             , "", ""                             , ""},
     {"Arduino"                      , "", "#ESC|#ESC|#+L_CTRL_C|#-L_CTRL|ic", ""}
    },
    { // Go
     {""   , "", "", ""},
     {""   , "", "", ""},
     {""   , "", "", ""},
     {"Go" , "" ,"", ""}
    },
    { // i3
      {"#+L_WIN|1"  , "#+L_WIN|2", "#+L_WIN|3"         , "#+L_WIN|#+L_SHIFT|R"},
      {"#+L_WIN|4"  , "#+L_WIN|5", "#+L_WIN|6"         , "#+L_WIN|T"         },
      {"#+L_WIN|7"  , "#+L_WIN|8", "#+L_WIN|9"         , ""                 },
      {"i3"        , "#+L_WIN|0", "#+L_WIN|#+L_SHIFT|E", ""                 }
    },
    { // OBS
     {""   , "", "", ""},
     {""   , "", "", ""},
     {""   , "", "", ""},
     {"OBS", "", "", ""},
    },
    { // test

     {"lein clean; lein lint; lein lint-fix; lein test|#RET"   , "lein clean; lein lint; lein lint-fix; lein test|#RET", "lein clean; lein lint; lein lint-fix; lein test|#RET", "lein clean; lein lint; lein lint-fix; lein test|#RET"},
     {"lein clean; lein lint; lein lint-fix; lein test|#RET"   , "lein clean; lein lint; lein lint-fix; lein test|#RET", "lein clean; lein lint; lein lint-fix; lein test|#RET", "lein clean; lein lint; lein lint-fix; lein test|#RET"},
     {"lein clean; lein lint; lein lint-fix; lein test|#RET"   , "lein clean; lein lint; lein lint-fix; lein test|#RET", "lein clean; lein lint; lein lint-fix; lein test|#RET", "lein clean; lein lint; lein lint-fix; lein test|#RET"},
     {"lein clean; lein lint; lein lint-fix; lein test|#RET", "lein clean; lein lint; lein lint-fix; lein test|#RET", "lein clean; lein lint; lein lint-fix; lein test|#RET", "lein clean; lein lint; lein lint-fix; lein test|#RET"},
    }
};
