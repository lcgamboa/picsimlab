/* 
 * File:   boards_defs.h
 * Author: gamboa
 *
 * Created on 27 de Outubro de 2015, 12:47
 */

#ifndef BOARDS_DEFS_H
#define	BOARDS_DEFS_H

#include"picsimlab1.h"
#include"board_1.h" 
#include"board_2.h"
#include"board_3.h"
#include"board_4.h"


board * create_board(int *lab,int *lab_)
{
   board * pboard; 
        switch(*lab)
        {
          case 1:
            pboard= new cboard_1();
            break;
          case 2:
            pboard= new cboard_2();
            break;
          case 3:
            pboard= new cboard_3();
            break;
          case 4:
            pboard= new cboard_4();
            break;       
          default:
            mprint(wxT("Invalid Board! Using Default!\n"));
            *lab=1;//default  
            *lab_=1;//default  
            Window1.combo2.SetText(wxT("1"));
            pboard= new cboard_1();
          break;
        }

    return pboard; 
}

#endif	/* BOARDS_DEFS_H */

