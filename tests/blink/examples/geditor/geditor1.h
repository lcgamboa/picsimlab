#ifndef CPWINDOW1 
#define CPWINDOW1

#include<lxrad.h>

class CPWindow1:public CWindow
{
  public:
  /*#Controls*/
  CMenu menu1;
  CFileDialog filedialog1;
  CFileDialog filedialog2;
  CPMenu menu1_Arquivo;
  CItemMenu menu1_Arquivo_Abrir;
  CItemMenu menu1_Arquivo_Salvar;
  CItemMenu menu1_Arquivo_Sair;
  CGrid grid1;
  CPMenu menu1_Editar;
  CPMenu menu1_Ajuda;
  CItemMenu menu1_Editar_Limpar;
  CItemMenu menu1_Ajuda_Sobre;
  CLabel label1;
  /*#Events*/
  void _EvOnCreate(CControl * control);
  void menu1_Arquivo_Abrir_EvMenuActive(CControl * control);
  void menu1_Arquivo_Salvar_EvMenuActive(CControl * control);
  void menu1_Arquivo_Sair_EvMenuActive(CControl * control);
  void grid1_EvOnGridCellLeftClick(CControl * control);
  void menu1_Editar_Limpar_EvMenuActive(CControl * control);
  void menu1_Ajuda_Sobre_EvMenuActive(CControl * control);

  /*#Others*/
  CPWindow1(void);
};
 extern CPWindow1 Window1 ;

#endif /*#CPWINDOW1*/

