#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fusion-c/header/msx_fusion.h"
#include "fusion-c/header/vdp_graph2.h"
#include "fusion-c/header/vars_msxSystem.h"
#include "fusion-c/header/pt3replayer.h"
// MSX Murcia 2021
// MSX2 
// PT3 Music + Screen 8

//Functions declarations
//Manejador de errores
void FT_errorHandler(char n, char *name);
//Introduce el nombre en la structura del archivo FCB
void FT_SetName(FCB *p_fcb, const char *p_name); 
//Carga el archivo en el buffer (en la RAM) y
// le hace un copy a partir del start_y=256 para que lo ponga en la page 1 (que no se ve) 
int FT_LoadSc8Image(char *file_name, unsigned int start_Y, char *buffer);
//Para cargar la música en el buffer
int FT_LoadData(char *file_name, char *buffer, int size, int skip);
//Copia cuadraditos de la VRAM page 1 a la VRAM page 0
void FT_ScreenCopy(char x1, char y1, char dx, char dy, char x2, char y2, char src , char dst, char mode);
//Genera un número aleatorio
char FT_RandomNumber (char a, char b);

//Member declarations
//Solo se utiliza para crear el buffer
#define BUFFER_SIZE 9000
//Necesaria para manejar los archivos
static FCB TStruct_file; 
//Necesaria para utilizar la función fLMMM de Fusion-C
MMMtask  TStruct_mask;
//Buffer que almacena en la RAM los datos de la imagen o la música
unsigned char buffer[BUFFER_SIZE];  














/* ---------------------------------
            FT_errorHandler
          In case of Error
-----------------------------------*/ 
void FT_errorHandler(char n, char *name)
{
  InitPSG();
  Screen(0);
  SetColors(15,6,6);
  switch (n)
  {
      case 1:
          printf("\n\rFAILED: fcb_open(): %s ",name);
      break;

      case 2:
          printf("\n\rFAILED: fcb_close(): %s",name);
      break;  

      case 3:
          printf("\n\rSORRY: this game does not work on %s",name);
      break; 
  }
Exit(0);
}

/* ---------------------------------
                FT_SetName
    Set the name of a file to load
                (MSX DOS)
-----------------------------------*/ 
void FT_SetName(FCB *p_fcb, const char *p_name) {
  char i, j;
  memset( p_fcb, 0, sizeof(FCB) );
  for( i = 0; i < 11; i++ ) {
    p_fcb->name[i] = ' ';
  }
  for( i = 0; (i < 8) && (p_name[i] != 0) && (p_name[i] != '.'); i++ ) {
    p_fcb->name[i] =  p_name[i];
  }
  if( p_name[i] == '.' ) {
    i++;
    for( j = 0; (j < 3) && (p_name[i + j] != 0) && (p_name[i + j] != '.'); j++ ) {
      p_fcb->ext[j] =  p_name[i + j] ;
    }
  }
}

/* ---------------------------------
          FT_LoadSc8Image
    Load a SC8 Picture and put datas
  on screen, begining at start_Y line
-----------------------------------*/ 
int FT_LoadSc8Image(char *file_name, unsigned int start_Y, char *buffer)
    {
        int rd=4352;
        int nbl=0;
        InitPSG();
        FT_SetName( &TStruct_file, file_name );
        if(fcb_open( &TStruct_file ) != FCB_SUCCESS) 
        {
          FT_errorHandler(1, file_name);
          return (0);
        }
        fcb_read( &TStruct_file, buffer, 7 );  // Skip 7 first bytes of the file
        while(rd!=0)
        {

            rd = fcb_read( &TStruct_file, buffer, 4352 );
                if (rd!=0)
                {
   
                  nbl=rd/256;
                  HMMC(buffer, 0,start_Y,256,nbl ); // Move the buffer to VRAM. 17 lines x 256 pixels  from memory
                  start_Y=start_Y+nbl; 
         
              }
        }
      if( fcb_close( &TStruct_file ) != FCB_SUCCESS ) 
      {
         FT_errorHandler(2, file_name);
          return (0);
      }

return(1);
}


/* ---------------------------------
          FT_LoadData
  Load Data to a specific pointer
  size is the size of data to read
  skip represent the number of Bytes
  you want to skip from the begining of the file
  Example: skip=7 to skip 7 bytes of a MSX bin
-----------------------------------*/ 
int FT_LoadData(char *file_name, char *buffer, int size, int skip)
{
    FT_SetName( &TStruct_file, file_name );
    if(fcb_open( &TStruct_file ) != FCB_SUCCESS) 
    {
      FT_errorHandler(1, file_name);
      return (0);
    }
    if (skip>0)
    {
      fcb_read( &TStruct_file, buffer, skip );
    }
    fcb_read( &TStruct_file, buffer, size );
  
    if( fcb_close( &TStruct_file ) != FCB_SUCCESS ) 
    {
      FT_errorHandler(2, file_name);
      return (0);
    }
    return(0);
}

/* ---------------------------------
          FT_ScreenCopy
   Copy a screen zone to another place
   x1 & y1 =Top left coordonate  pixel of the zone to copy
   dx = Width Size in pixels, of the zone to copy
   dy = Height Size in pixels, of the zone to copy
   x2 & y2 = Destination coordonate where to copy the zone
   src = Source Page number of the Zone
   dst = Destination number of the zone
   mode = OP mode of the copy
-----------------------------------*/ 
void FT_ScreenCopy(char x1, char y1, char dx, char dy, char x2, char y2, char src , char dst, char mode)  
{
    int src_y=0;
    int dst_y=0;

    if (src==1)
        src_y=256;
    if (dst==1)
        dst_y=256;
    
    TStruct_mask.X = x1; 
    TStruct_mask.Y = src_y+y1; 
    TStruct_mask.DX = dx;  
    TStruct_mask.DY = dy;     
    TStruct_mask.X2 = x2; 
    TStruct_mask.Y2 = dst_y+y2;           
    TStruct_mask.s0 = 0; 
    TStruct_mask.DI = 0; 
    TStruct_mask.LOP = mode;
    fLMMM(&TStruct_mask);
}

char FT_RandomNumber (char a, char b)
{
    char random;
    random = rand()%(b-a)+a;  // Random number between a and b-1
    return(random);
}

void main (void){
  int x,y,d;
  char *file[3]={"Music1.pt3","Music2.pt3","Music3.pt3"};
  int size[3]={8417,2501,4637};
  y=0;
  x=0;
  d=2;
  Screen(8);
  //int FT_LoadSc8Image(char *file_name, unsigned int start_Y, char *buffer)
  FT_LoadSc8Image("LOADER.S08", 256, buffer);
  if(ReadMSXtype()==3)  // IF MSX is Turbo-R Switch CPU to Z80 Mode
  {
      ChangeCPU(3);
  }
  InitPSG();
  //int FT_LoadData(char *file_name, char *buffer, int size, int skip)
  FT_LoadData(file[0], buffer, size[0], 0);
  PT3Init (buffer+99, 0);
  Cls();
  //Main loop
  while (1)
  {
    Halt();
    DisableInterupt();
    //Prepara el bloque de datos que se van a reproducir
    PT3Rout();
    //Reproduce el bloque de datos escribiendo en el PSG
    PT3Play();
    EnableInterupt();
    //FT_RandomNumber genera un número aleatorio
    x=FT_RandomNumber(0,255);
    y=FT_RandomNumber(0,212);
    //Copia cuadraditos de 8x8 pixeles al azar todo el rato
    FT_ScreenCopy(x, y, 8, 8, x, y, 1 , 0, 0);
  }
}