/* ########################################################################

   PICsimLab - PIC laboratory simulator

   ########################################################################

   Copyright (c) : 2015-2018  Luis Claudio Gambôa Lopes

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


#include <lxrad/lxrad.h>
#include "futils.h"
#include "wx/stdpaths.h"
#include "wx/filesys.h"
#include "wx/zipstrm.h"
#include "wx/fs_zip.h"
#include "wx/wfstream.h"
#include "wx/dir.h"
#include <memory>
#include<dirent.h>


#if __cplusplus >= 201103L
#define xauto_ptr  std::unique_ptr
#else
#define xauto_ptr  std::auto_ptr
#endif

bool UnzipDir(const String &in_filename, const String &out_dirname)
{
    bool ret = true;

        wxFileSystem::AddHandler(new wxZipFSHandler);
        wxFileSystem fs;
           
        xauto_ptr<wxZipEntry> entry(new wxZipEntry);
        do {    
            wxFileInputStream in(in_filename);
            if (!in)
            {
                wxLogError(_T("Can not open file '")+in_filename+_T("'."));
                ret = false;
                break;
            }
            wxZipInputStream zip(in);

            //create dirs
            while (entry.reset(zip.GetNextEntry()), entry.get() != NULL)
            {
                // access meta-data
                wxString name = entry->GetName();
                
                if(dirname(name).Length() > 0)
                {
                    CreateDir(out_dirname+dirname(name));
                }
                
                name = out_dirname + name;

                    
                // read 'zip' to access the entry's data
                if (entry->IsDir())
                {
                    int perm = entry->GetMode();
                    wxFileName::Mkdir(name, perm, wxPATH_MKDIR_FULL);
                }
                else
                {
                    zip.OpenEntry(*entry.get());
                    if (!zip.CanRead())
                    {
                        wxLogError(_T("Can not read zip entry '")+entry->GetName()+_T("'."));
                        ret = false;
                        break;
                    }
                    wxFileOutputStream file(name);
                    if (!file)
                    {
                         wxLogError(_T("Can not create file '")+name+_T("'."));
                         ret = false;
                         break;
                    }
                    zip.Read(file);
                }
                       
            }
        } while(false);

    return ret;
}


bool 
ZipDir(const String &in_dirname, const String &out_filename)
{
   wxString sep(wxFileName::GetPathSeparator());

   wxArrayString files;

   wxDir::GetAllFiles(in_dirname,&files);//the dir contented all the files need to compress.

   wxFFileOutputStream  fileout(out_filename);

   wxZipOutputStream zipout (fileout);

   wxFFileInputStream *in = NULL;   
   wxZipEntry *entry=NULL;
   wxFileName *zipname = NULL;

   for (size_t i = 0;i != files.GetCount(); ++i)
   {
      if (in!=NULL)
      {
         delete in;
         in = NULL;
      }
      if (zipname!=NULL)
      {
         delete zipname;
         zipname = NULL;
      }

      in = new wxFFileInputStream(files[i]);
      zipname = new wxFileName (files[i]);
      if(zipname->MakeRelativeTo(in_dirname))
      {
         entry =  new wxZipEntry(wxT("picsimlab_workspace/")+zipname->GetFullPath());
         zipout.PutNextEntry(entry);
         zipout.Write(*(in));
      }
      else
      {
         return false;
      }
   }
   zipout.CloseEntry();
   zipout.Close();
   fileout.Close();
   if (in!=NULL)
   {
      delete in;
      in = NULL;
   }
   if (zipname!=NULL)
   {
      delete zipname;
      zipname = NULL;
   }
   return true;
}

bool RemoveFile(const char * fname)
{
    return remove(fname);
}

bool RemoveDir(const char* dirname)
{
     DIR  *dp;
     struct dirent *dent;
     struct stat sb;
     char fname[1024];
     
     dp = opendir (dirname);

     if(dp)
     {
       while((dent=readdir(dp))!=NULL)
       {
          snprintf(fname,1024,"%s/%s",dirname,dent->d_name);  
          stat(fname, &sb);
          
          if(S_ISREG(sb.st_mode))
          {
            RemoveFile(fname);
          }
          else if(S_ISDIR(sb.st_mode))
          {
            if(!(!strcmp(dent->d_name,".") || !strcmp(dent->d_name,"..")))
            {    
              RemoveDir(fname);
            }
          }
       }
       closedir(dp);
       return rmdir(dirname);
     }
   return 0;
}    

bool 
CreateDir(const char * dirname)
{
   DIR  *dp; 
   dp = opendir (dirname);

   if(dp)
   {
     return closedir(dp);
   }
   else
   {
#ifndef _WIN_
     return mkdir(dirname, S_IWUSR| S_IRUSR | S_IXUSR | S_IRGRP | S_IROTH ); 
#else     
     return mkdir(dirname);
#endif
   }
   return 0;
}
