/*********************************************************************
*                                                                    *
*                SEGGER Microcontroller GmbH & Co. KG                *
*        Solutions for real time microcontroller applications        *
*                                                                    *
**********************************************************************
*                                                                    *
* C-file generated by:                                               *
*                                                                    *
*        GUI_Builder for emWin version 5.44                          *
*        Compiled Nov 10 2017, 08:53:57                              *
*        (c) 2017 Segger Microcontroller GmbH & Co. KG               *
*                                                                    *
**********************************************************************
*                                                                    *
*        Internet: www.segger.com  Support: support@segger.com       *
*                                                                    *
**********************************************************************
*/

// USER START (Optionally insert additional includes)

#include "rc522.h"
#include "fatfs.h"
#include "stm32f4xx.h"
#include "stm32f4xx_it.h"



char CardID[5];
char MyCard[5] = {0,96,5,2,205};
char ret;
FATFS fs;
FRESULT res;
FIL file;
char line[256];
char strCardID[16];
char PersonelInfo[5][20] = {{0}};
int kayit_flag=0;
int silme_flag=0;

// USER END

#include "DIALOG.h"


/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
#define ID_FRAMEWIN_0    (GUI_ID_USER + 0x00)
#define ID_TEXT_0    (GUI_ID_USER + 0x03)
#define ID_TEXT_1    (GUI_ID_USER + 0x04)
#define ID_TEXT_2    (GUI_ID_USER + 0x05)
#define ID_TEXT_3    (GUI_ID_USER + 0x06)
#define ID_TEXT_4    (GUI_ID_USER + 0x07)
#define ID_TEXT_5    (GUI_ID_USER + 0x08)
#define ID_TEXT_6    (GUI_ID_USER + 0x09)
#define ID_TEXT_7    (GUI_ID_USER + 0x0A)


// USER START (Optionally insert additional defines)
// USER END

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/

// USER START (Optionally insert additional static data)
// USER END

/*********************************************************************
*
*       _aDialogCreate
*/
static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] = {
  { FRAMEWIN_CreateIndirect, "PersonelGecis", ID_FRAMEWIN_0, 120, 0, 200, 240, 0, 0x0, 0 },
  { TEXT_CreateIndirect, "ISIM:", ID_TEXT_0, 7, 18, 49, 20, 0, 0x0, 0 },
  { TEXT_CreateIndirect, "CARD NO:", ID_TEXT_1, 6, 41, 80, 20, 0, 0x0, 0 },
  { TEXT_CreateIndirect, "GOREVI:", ID_TEXT_2, 7, 64, 80, 20, 0, 0x0, 0 },
  { TEXT_CreateIndirect, "DURUM:", ID_TEXT_3, 7, 85, 80, 20, 0, 0x0, 0 },
  { TEXT_CreateIndirect, "Text", ID_TEXT_4, 70, 16, 80, 20, 0, 0x0, 0 },
  { TEXT_CreateIndirect, "Text", ID_TEXT_5, 70, 39, 80, 20, 0, 0x0, 0 },
  { TEXT_CreateIndirect, "Text", ID_TEXT_6, 70, 63, 80, 20, 0, 0x0, 0 },
  { TEXT_CreateIndirect, "Text", ID_TEXT_7, 70, 84, 120, 20, 0, 0x0, 0 },
  // USER START (Optionally insert additional widgets)
  // USER END
};

/*********************************************************************
*
*       Static code
*
**********************************************************************
*/

// USER START (Optionally insert additional static code)
// USER END

/*********************************************************************
*
*       _cbDialog
*/
static void _cbDialog(WM_MESSAGE * pMsg) {
  // USER START (Optionally insert additional variables)
  // USER END

  switch (pMsg->MsgId) {
  // USER START (Optionally insert additional message handling)
  // USER END
  default:
    WM_DefaultProc(pMsg);
    break;
  }
}

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*       CreatePersonel
*/
WM_HWIN CreatePersonel(void);
WM_HWIN CreatePersonel(void) {
  WM_HWIN hWin;

  hWin = GUI_CreateDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), _cbDialog, WM_HBKWIN, 0, 0);
  return hWin;
}

// USER START (Optionally insert additional public code)
char RFID_Card_Check_ID(char *CardID)
{
	  if(MFRC522_Check(CardID) == MI_OK)
	  {
	       HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_SET);
	       return MI_OK;
	  }

	  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_RESET);
	  return MI_NOK;

}

char CheckDataBase(char *CardID)
{


    // Kart ID'yi uygun formata dönüştür
    sprintf(strCardID, "%03d%03d%03d%03d%03d", CardID[0], CardID[1], CardID[2], CardID[3], CardID[4]);

    printf("Checking CardID: %s\n", strCardID);

    // Dosyayı aç
    res = f_open(&file, "liste.txt", FA_READ);
    if (res == FR_OK)
    {
        // Satır satır oku
        while (f_gets(line, sizeof(line), &file) != NULL)
        {
            printf("Read line: %s\n", line);

            // Kart ID'sini satırda ara
            if (strncmp(line, strCardID, strlen(strCardID)) == 0)
            {
                printf("Match found for CardID: %s\n", line);

                // Kart ID bulundu, bilgileri ayır
                char *token = strtok(line, "\t");
                int i = 0;

                while (token != NULL && i < 5)
                {
                    printf("Token[%d]: %s\n", i, token);

                    strncpy(PersonelInfo[i], token, sizeof(PersonelInfo[i]) - 1);
                    PersonelInfo[i][sizeof(PersonelInfo[i]) - 1] = '\0'; // Null-terminator ekle.
                    token = strtok(NULL, "\t");
                    i++;
                }

                // Bilgileri yazdır

                f_close(&file);
                return 1; // Başarılı
            }

        }

        f_close(&file);
        printf("CardID not found.\n");
        return 0; // Kart ID bulunamadı
    }

    printf("File open failed.\n");
    return 0; // Dosya açılamadı
}

char _acBuffer[1024];
static int _GetData(void * p, const U8 ** ppData, unsigned NumBytesReq, U32 Offset) {
    unsigned int NumBytesRead;
    f_lseek((FIL *) p, Offset); //Set file pointer to the required position
    /*
     * Read file function
     * FIL*     fp      -	Pointer to the file object
     * void*    buff    -	Pointer to data buffer
     * UINT     btr     -	Number of bytes to read
     * UINT*    br      -	Pointer to number of bytes read
     */
    f_read((FIL *) p, (void *) _acBuffer, NumBytesReq, &NumBytesRead); //Read data into buffer
    *ppData = (const U8 *) _acBuffer; //Set pointer to the buffer
    return NumBytesRead;
}

void RFID_Reader()
{
	  WM_HWIN hWin;
	  TEXT_Handle hText4,hText5,hText6,hText7;

	  ret = f_mount(&fs, "", 0);
	  if(ret != FR_OK )
	  {
		  while(1);
	  }

	  hWin = CreatePersonel();
	  hText4 = WM_GetDialogItem(hWin, ID_TEXT_4);
	  hText5 = WM_GetDialogItem(hWin, ID_TEXT_5);
	  hText6 = WM_GetDialogItem(hWin, ID_TEXT_6);
	  hText7 = WM_GetDialogItem(hWin, ID_TEXT_7);

	  while(1)
	  {
		  ret = RFID_Card_Check_ID(CardID);
		  if(ret == MI_OK)
		  {
			  if(CheckDataBase(CardID))
			  {
				  TEXT_SetText(hText4, &PersonelInfo[1][0]);
				  TEXT_SetText(hText5, &PersonelInfo[0][0]);
				  TEXT_SetText(hText6, &PersonelInfo[2][0]);

					res = f_open(&file, &PersonelInfo[4][0], FA_OPEN_EXISTING | FA_READ);
					if (res == FR_OK)
						GUI_BMP_DrawEx(_GetData, &file, 14, 30);
					f_close(&file);
					TEXT_SetText(hText7, "basarili");



					if(silme_flag==1)
					{
						sdcard_sil();
						TEXT_SetText(hText7, "silindi");
						silme_flag=0;
						HAL_GPIO_WritePin(ledc_GPIO_Port, ledc_Pin, 0); //silme modu
					}

			  }
			  else
			  {
				 TEXT_SetText(hText7, "kayitli degil");

				  if(kayit_flag==1)
				  {
					  sdcard_kayit();
					  kayit_flag=0;
					  HAL_GPIO_WritePin(ledk_GPIO_Port, ledk_Pin, 0);
					  TEXT_SetText(hText7, "kaydedildi");
				  }


			  }
		  }
		  HAL_Delay(250);
		  GUI_Exec();
	  }

}

void sdcard_kayit(void) {
    // strCardID oluştur
    char strCardID[32]; // Yeterli boyutta bir buffer tanımlayın
    sprintf(strCardID, "%03d%03d%03d%03d%03d", CardID[0], CardID[1], CardID[2], CardID[3], CardID[4]);

    // Personel bilgilerini ayarla
    strncpy(PersonelInfo[0], strCardID, sizeof(PersonelInfo[0]) - 1);
    PersonelInfo[0][sizeof(PersonelInfo[0]) - 1] = '\0'; // Null-terminator ekle

    for (int i = 1; i < 4; i++) { // Kalan alanlar için "misafir" bilgisi yaz
        strncpy(PersonelInfo[i], "misafir", sizeof(PersonelInfo[i]) - 1);
        PersonelInfo[i][sizeof(PersonelInfo[i]) - 1] = '\0'; // Null-terminator ekle
    }

    // Resim dosyasını PersonelInfo[4][0]'a ekle
    strncpy(PersonelInfo[4], "misafir.bmp", sizeof(PersonelInfo[4]) - 1);
    PersonelInfo[4][sizeof(PersonelInfo[4]) - 1] = '\0'; // Null-terminator ekle

    // Yeni kayıt bilgilerini oluştur
    char newRecord[256];
    sprintf(newRecord, "%s\t%s\t%s\t%s\t%s\n",
            PersonelInfo[0],  // strCardID: CardID'den üretilen değer
            PersonelInfo[1],  // Kart Numarası: misafir
            PersonelInfo[2],  // Görevi: misafir
            PersonelInfo[3],  // Durumu: misafir
            PersonelInfo[4]); // Resim dosyası: misafir.bmp

    // Dosyayı yazma modunda aç (ekleme modunda)
    FRESULT res = f_open(&file, "liste.txt", FA_WRITE | FA_OPEN_APPEND);
    if (res == FR_OK) {
        UINT bytesWritten;

        // Yeni kaydı dosyaya ekle
        res = f_write(&file, newRecord, strlen(newRecord), &bytesWritten);
        if (res == FR_OK && bytesWritten == strlen(newRecord)) {
            printf("Yeni kayıt başarıyla eklendi: %s\n", newRecord);
        } else {
            printf("Kayıt eklenirken bir hata oluştu. (Yazma Hatası)\n");
        }

        // Dosyayı kapat
        f_close(&file);
    } else {
        printf("liste.txt dosyası açılamadı, kayıt ekleme başarısız. (Hata Kodu: %d)\n", res);
    }
}




void sdcard_sil(void) {
    // Kart ID'nin karşılaştırılması için geçici dosya
    FIL tempFile;
    char tempLine[256];
    int matchFound = 0;

    // Geçici dosyayı oluştur
    if (f_open(&tempFile, "temp.txt", FA_WRITE | FA_CREATE_ALWAYS) == FR_OK) {
        if (f_open(&file, "liste.txt", FA_READ) == FR_OK) {
            // Orijinal dosyadan satır satır oku
            while (f_gets(tempLine, sizeof(tempLine), &file) != NULL) {
                // Kart ID'si ile eşleşmeyenleri geçici dosyaya yaz
                if (strncmp(tempLine, PersonelInfo[0], strlen(PersonelInfo[0])) != 0) {
                    UINT bytesWritten;
                    f_write(&tempFile, tempLine, strlen(tempLine), &bytesWritten);
                } else {
                    matchFound = 1;  // Kart ID eşleşti, bu kayıt silinecek
                }
            }
            f_close(&file);
        } else {
            printf("Orijinal dosya açılamadı, silme başarısız.\n");
        }

        f_close(&tempFile);

        // Eğer eşleşme bulunduysa dosyaları değiştir
        if (matchFound) {
            f_unlink("liste.txt");               // Orijinal dosyayı sil
            f_rename("temp.txt", "liste.txt");   // Geçici dosyayı yeni dosya yap
            printf("Kart bilgileri başarıyla silindi: %s\n", strCardID);
        } else {
            printf("Kart bulunamadı, silme işlemi yapılmadı.\n");
            f_unlink("temp.txt");  // Geçici dosyayı sil
        }
    } else {
        printf("Geçici dosya oluşturulamadı.\n");
    }
}



void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if(GPIO_Pin == button_Pin)
	{
		silme_flag=1;
		HAL_GPIO_WritePin(ledc_GPIO_Port, ledc_Pin, 1); //silme modu

	}
	if(GPIO_Pin == button2_Pin)
	{
		kayit_flag=1;
		HAL_GPIO_WritePin(ledk_GPIO_Port, ledk_Pin, 1);
	}


}


// USER END

/*************************** End of file ****************************/
