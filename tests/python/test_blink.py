#run with: pytest -v --tb=no --no-header

import time,re,subprocess
import os.path
from pathlib import Path
from PICSimLab_rcontrol import PICSimLab_rcontrol
import pytest

def run_picsimlab(board, processor, file_path):
   exe_path = r"/usr/bin/picsimlab"
   if file_path.is_file() == 0: 
      raise  print(f"File not found {file_path}")
   return subprocess.Popen([exe_path, board, processor, file_path], stdout=subprocess.DEVNULL)
  
dir_path = Path("firmware/")
files = [f.name for f in dir_path.iterdir() if f.is_file() and f.suffix in ('.hex', '.bin')]

@pytest.mark.parametrize("file", files, ids=files)
def test_blink(file):
   file_path = Path(dir_path / file)
   name_itens= file_path.stem.split() 
   process= run_picsimlab(name_itens[1], name_itens[2], file_path)
   print("=================================================")
   print(f"{name_itens[1]} {name_itens[2]}:Blink time measure test started ....")
   time.sleep(2)
   try:  
      with PICSimLab_rcontrol(5000) as rc: 
         rc.debug=0
         rc.cmd_spshow(0)
         rc.cmd_oscshow(1)
         rc.cmd_oscwrcfg(0, "osc_cfg,100,400,0:20.000000,0.000000,1,1,2.500000,3,2,9,7,8")
         rc.cmd_oscwrcfg(1, f"osc_ch1,0,0,0:2.000000,0.000000,1,#FF0000,0,{name_itens[3]}")
         rc.cmd_oscwrcfg(2, "osc_ch2,0,0,0:2.000000,-6.000000,0,#00FF00,0,0")
         while True:
                  time.sleep(0.1)
                  rc.cmd_oscmeasures(1)
                  val = rc.get_cmd_response()
                  fcyc = float((val.split('\n')[9]).split()[1])
                  if fcyc > 30 :
                     break
         time.sleep(4)
         rc.cmd_oscmeasures(1)
         val= rc.get_cmd_response()
            
         pcyc=float((val.split('\n')[7]).split()[1]) 
         ncyc=float((val.split('\n')[8]).split()[1]) 
         
         rc.cmd_exit()  
         process.wait()

         assert pcyc == pytest.approx(20, abs=2) 
         assert ncyc == pytest.approx(20, abs=2)  

   except ConnectionError as e:
      print(e)
               
         