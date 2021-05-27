/* ########################################################################

   PICsimLab - PIC laboratory simulator

   ########################################################################

   Copyright (c) : 2010-2020  Luis Claudio Gambôa Lopes

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

   For e-mail suggestions :  lcgamboa@yahoo.com
   ######################################################################## */


#include<stdio.h>
#include"io_MCP23X17.h"

#define dprintf if (1) {} else printf

//IOCON bits
#define BANK      0x80
#define MIRROR    0x40
#define SEQOP     0x20
#define DISSLW    0x10
#define HAEN      0x08
#define ODR       0x04
#define INTPOL    0x02

static unsigned char
decode_addr(io_MCP23X17_t *mcp)
{
 if (mcp->regs[IOCON] & BANK)
  {
   switch (mcp->reg_addr)
    {
    case 0x00: return 0x00;
     break;
    case 0x01: return 0x02;
     break;
    case 0x02: return 0x04;
     break;
    case 0x03: return 0x06;
     break;
    case 0x04: return 0x08;
     break;
    case 0x05: return 0x0A;
     break;
    case 0x06: return 0x0C;
     break;
    case 0x07: return 0x0E;
     break;
    case 0x08: return 0x10;
     break;
    case 0x09: return 0x12;
     break;
    case 0x0A: return 0x14;
     break;
     //case 0x0B: return 0x10; break;
     //case 0x0C: return 0x10; break;
     //case 0x0D: return 0x10; break;
     //case 0x0E: return 0x10; break;
     //case 0x0F: return 0x10; break;
    case 0x10: return 0x01;
     break;
    case 0x11: return 0x03;
     break;
    case 0x12: return 0x05;
     break;
    case 0x13: return 0x07;
     break;
    case 0x14: return 0x09;
     break;
    case 0x15: return 0x0B;
     break;
    case 0x16: return 0x0D;
     break;
    case 0x17: return 0x0F;
     break;
    case 0x18: return 0x11;
     break;
    case 0x19: return 0x13;
     break;
    case 0x1A: return 0x15;
     break;
    default:return 0;
     dprintf ("addr error!!!\n");
     break;
    }
  }
 else
  {
   return mcp->reg_addr;
  }
}

static void
write_reg(io_MCP23X17_t *mcp, unsigned char val)
{
 unsigned char addr_ = decode_addr (mcp);
 mcp->regs[addr_] = val;

 if ((mcp->regs[IOCON] & SEQOP) == 0)
  {
   mcp->reg_addr++;

   //FIXME only for BANK=0;
   if (mcp->reg_addr > 0x15)mcp->reg_addr = 0;
  }

 switch (addr_)
  {
  case IOCON:
   mcp->regs[IOCON_] = val;
   break;
  case IOCON_:
   mcp->regs[IOCON] = val;
   break;
  case GPIOA:
   mcp->regs[OLATA] = val;
   break;
  case GPIOB:
   mcp->regs[OLATB] = val;
   break;
  }
}

static unsigned char
read_reg(io_MCP23X17_t *mcp)
{
 unsigned char addr_ = decode_addr (mcp);
 return mcp->regs[addr_];
}

void
io_MCP23X17_rst(io_MCP23X17_t *mcp)
{
 for (int i = 0; i < 22; i++)
  mcp->regs[i] = 0;
 mcp->reg_addr = 0;
 mcp->addr = 0x40;

 mcp->data = 0;
 mcp->bit = 0xFF;
 mcp->byte = 0xFF;
 mcp->asck = -1;
 mcp->op = 0;

 //i2c
 mcp->sdao = -1; //0
 mcp->datab = 0;
 mcp->ctrl = 0;
 mcp->ret = 0;

 dprintf ("mcp rst\n");
}

void
io_MCP23X17_init(io_MCP23X17_t *mcp)
{
 dprintf ("mcp init\n");
 io_MCP23X17_rst (mcp);
}

void
io_MCP23X17_set_addr(io_MCP23X17_t *mcp, unsigned char addr)
{
 mcp->addr = addr << 1;
}

unsigned char
io_MCP23X17_SPI_io(io_MCP23X17_t *mcp, unsigned char si, unsigned char sck, unsigned char rst, unsigned char cs)
{

 if (cs)
  {
   mcp->bit = 0;
   mcp->byte = 0;
   return 0;
  }

 if (!rst)
  {
   io_MCP23X17_rst (mcp);
   return 0;
  }


 //transicao
 if ((mcp->asck == 0)&&(sck == 1))//rising edge
  {
   if (mcp->bit == 8) mcp->bit = 0;

   if (mcp->bit < 8)
    {
     if (si)
      {
       mcp->data = (mcp->data << 1) | 1;
      }
     else
      {
       mcp->data = (mcp->data << 1) & 0xFE;
      }
     mcp->bit++;
    }

   if (mcp->bit == 8)
    {
     //dprintf ("-------mcp data 0x%02X  byte=%i\n", mcp->data,mcp->byte);
     mcp->bit = 0;
     switch (mcp->byte)
      {
      case 0:
       if ((mcp->data & 0xFE) == mcp->addr)
        {
         mcp->op = mcp->data & 0x01;
         mcp->byte++;
         dprintf ("mcp addr 0x%02X OK\n", mcp->addr);
        }
       else
        {
         mcp->bit = 0xFF;
         mcp->byte = 0xFF;
         dprintf ("mcp addr 0x%02X ERROR\n", mcp->addr);
        }
       break;
      case 1:
       mcp->reg_addr = mcp->data;
       mcp->byte++;
       dprintf ("mcp reg addr 0x%02X\n", mcp->data);
       break;
      default:
       if (mcp->op)
        {
         dprintf ("mcp data read [0x%02X] 0x%02X\n", mcp->reg_addr, mcp->data);
         mcp->datas = read_reg (mcp);
        }
       else
        {
         dprintf ("mcp data write [0x%02X] 0x%02X\n", mcp->reg_addr, mcp->data);
         write_reg (mcp, mcp->data);
        }
       break;
      }
    }
  }

 mcp->asck = sck;

 return 0;
}

unsigned char
io_MCP23X17_I2C_io(io_MCP23X17_t *mcp, unsigned char scl, unsigned char sda)
{

 if ((mcp->sdao == sda)&&(mcp->asck == scl))return mcp->ret;

 if ((mcp->sdao == 1)&&(sda == 0)&&(scl == 1)&&(mcp->asck == 1)) //start
  {
   mcp->bit = 0;
   mcp->byte = 0;
   mcp->datab = 0;
   mcp->ctrl = 0;
   mcp->ret = 0;
   dprintf ("mcp start!\n");
  }

 if ((mcp->sdao == 0)&&(sda == 1)&&(scl == 1)&&(mcp->asck == 1)) //stop
  {
   mcp->bit = 0xFF;
   mcp->byte = 0xFF;
   mcp->ctrl = 0;
   mcp->ret = 0;
   dprintf ("mcp stop!\n");
  }


 if ((mcp->bit < 9)&&(mcp->asck == 0)&&(scl == 1)) //data 
  {

   if (mcp->bit < 8)
    {
     mcp->datab |= (sda << (7 - mcp->bit));
    }

   mcp->bit++;
  }

 if ((mcp->bit < 9)&&(mcp->asck == 1)&&(scl == 0)&&(mcp->ctrl == 0x071)) //data 
  {
   if (mcp->bit < 8)
    {
     mcp->ret = ((mcp->datas & (1 << (7 - mcp->bit))) > 0);
     //dprintf("mcp send %i %i (%02X)\n",mcp->bit,mcp->ret,mcp->datas);  
    }
   else
    {
     mcp->ret = 0;
    }
  }


 if (mcp->bit == 9)
  {
   dprintf ("mcp data %02X\n", mcp->datab);

   if (mcp->byte == 0)
    {
     mcp->ctrl = mcp->datab;
     dprintf ("mcp ctrl = %02X\n", mcp->ctrl);
     mcp->ret = 0;

    }

   if ((mcp->ctrl) == mcp->addr)
    {

     if (((mcp->byte > 0)&&(mcp->ctrl & 0x01) == 0))
      {
       dprintf ("write mcp =%02X\n", mcp->datab);

       mcp->data = mcp->datab;

       mcp->ret = 0;

      }
    }
   else if ((mcp->ctrl) == (mcp->addr | 1)) //read
    {

     mcp->datas = mcp->data;
     dprintf ("mcp read =%02X\n", mcp->datas);
    }




   mcp->bit = 0;
   mcp->datab = 0;
   mcp->byte++;
  }


 mcp->sdao = sda;
 mcp->asck = scl;
 return mcp->ret;
}