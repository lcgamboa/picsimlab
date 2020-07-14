
#include "boards_defs.h"
#include"../picsimlab1.h"

char boards_list[BOARDS_MAX][30];

board * (* boards_createf[BOARDS_MAX]) (void) ;

int BOARDS_LAST = 0;

//boards object creation

board *
create_board(int *lab, int *lab_)
{
 board * pboard = NULL;


 if ((*lab >= 0)&&(*lab < BOARDS_LAST))
  {
   pboard = boards_createf[*lab]();
  }
 else
  {
   mprint (lxT ("Invalid Board! Using Default!\n"));
   *lab = 1; //default  
   *lab_ = 1; //default  
   pboard = new cboard_1 ();
  }

 return pboard;
}

void
board_register(int num, const char * name, board_create_func bcreate)
{
 //TODO error handler
 boards_createf[num] = bcreate;
 strncpy (boards_list[num], name, 30);
 BOARDS_LAST++;
}