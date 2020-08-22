#include<stdio.h>

#define wxT
#define mprint printf

class lxString
{
public:
   lxString(){};
   lxString(char *s){};

   char * char_str (void){return NULL;};
};

class CControl{};

class CPWindow
{
public:	
  CPWindow(){};	
};

class CCombo
{
public:	
  void SetEnable(int e){};
  lxString GetText(void);
  void SetItems(const char* i){};
  void SetText(const char* t){};
};

class CToggleButton
{
public:	
   int GetCheck(void){return 1;};
   void SetCheck (int c){};
   void SetText(const char *str){};
};

class CTimer
{
public:	
   void SetRunState (int s){};
};


class CText
{
public:
  void AddLine (const char*){};
  void Clear (void);     
};

