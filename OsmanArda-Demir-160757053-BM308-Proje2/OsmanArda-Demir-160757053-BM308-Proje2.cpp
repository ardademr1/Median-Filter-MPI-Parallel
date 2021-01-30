/*
	           Haz�rlayan : Osman Arda DEMIR
	           ��renci No : 160757053
	               B�l�m� : Bilgisayar M�hendisli�i
	   Dersin Ad� ve Kodu : PARALEL PROGRAMLAMAYA G�R�S, BM-308-50-50
   	             Proje No : 2
Dersi Veren ��retim �yesi : Dr. ��r. �yesi Tolga AYDIN
*/


#include <iostream>// Cout Cin
#include <mpi.h>  //MPI k�t�phanesi
#include <cstdio>
#include <fstream> // dosya i�lemleri
#include <sys/time.h>//struct timeval ve gettimeofday
#include <cstdlib>
#include <ctime>
#include <string> //kelime tutmak i�in

#define ET 16

using namespace std;

void siralamaFonk(int* , int); // Diziyi K���kten B�y��e S�ralamam�z ��in Fonksiyonu

int main(int argc, char* argv[])
{
	int rank,size;
	int satir,sutun;
	int *bolumPtr;//matrisin bir b�l�m�n� tutmak i�in
	int *yeniBolumPtr;//olu�an yeni b�l�m� tutmak i�in
	int *ciktiMatrisPtr;
 	int *tekDiziPtr;
	int veriYolla[3];  //veriyollama dizisi
	int kismiBoyut;   
	MPI_Status status;
	string arg=argv[1];
	int str;
	int argumanBoyutu=arg.size(); // Arguman Boyutunu ��renme
	MPI_Init(&argc,&argv);
	// bilgisayar rankini ve say�s�n� ��renme
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
	MPI_Comm_size(MPI_COMM_WORLD,&size);
	//master rank i 0 olan pc de �al��acak kod
		if(rank==0)
		{
			//dosya okuma islemi
			double bitisZamani,baslangicZamani;//ge�en zaman� �l�mek icin
			int sonPay;
			ifstream dosyaOku(argv[1]); // Dosya A�ma i�lemi
			
			//Dosyadan sat�r ve sutununu okuma i�lemi
			dosyaOku>>satir; 
			dosyaOku>>sutun;
			int diziBoyutu=satir*sutun;
			int sayac,yeniSayi;    
			int temp;
			tekDiziPtr=new int[satir*sutun];
			ciktiMatrisPtr=new int[satir*sutun];
			//diziye atma
			string arg=argv[1];
			int argumanBoyutu=arg.size(); // Arguman Boyutunu Al
			for (int i = 0; i < satir; i++){
				for (int j = 0; j < sutun; j++)
			    {
			        dosyaOku>>tekDiziPtr[i*sutun+j];       // Dosyadan Okunan Matris tek Boyutlu Diziye At�lmas�
			    }
			}
			for (int i = 0; i < satir; i++){
				for (int j = 0; j < sutun; j++)
			    {
			        ciktiMatrisPtr[i*sutun+j]=tekDiziPtr[i*sutun+j];       // Dosyadan Okunan Matrisin ��kt�Matris Dizisine At�lmas�
			    }
			}
		dosyaOku.close();
		//tam b�l�m�yormu?
			if(diziBoyutu%size==0)
			{
				kismiBoyut=diziBoyutu/size;	
				sonPay=kismiBoyut;                  //tam b�l�n�yorsa
				str=satir/size;
			}
			else
			{
				kismiBoyut=diziBoyutu/size;                 //tam b�l�nm�yorsa
				sonPay=diziBoyutu-(kismiBoyut*(size-1));
					
			}
					
			//verileri dizide toplama
			veriYolla[0]=kismiBoyut;
			veriYolla[1]=satir;
			veriYolla[2]=sutun;
			//Sure hesaplamak i�in ba�lang�� zaman�n� tut
			baslangicZamani=MPI_Wtime();

			//dizi gonderme blok boyutu satir sutun
			for(int i=1;i<size-1;i++){
				MPI_Send(veriYolla,3,MPI_INT,i,ET,MPI_COMM_WORLD);
			}
			veriYolla[0]=sonPay;
			MPI_Send(veriYolla,3,MPI_INT,(size-1),ET,MPI_COMM_WORLD);
			//diger bilgisayarlar�n payina d�sen veriler
			for(int i=1;i<size-1;i++){
				MPI_Send(&tekDiziPtr[(kismiBoyut*i)],kismiBoyut,MPI_INT,i,ET,MPI_COMM_WORLD);
			}
			//en son bilgisayara payina d�sen veriler
			MPI_Send(&tekDiziPtr[(kismiBoyut*(size-1))],sonPay,MPI_INT,(size-1),ET,MPI_COMM_WORLD);

			//(rank=0)master bilgisayarin payi
			cout<<satir<<" "<<sutun<<endl;
			cout<<((kismiBoyut+satir+satir)/satir)<<endl;
			cout<<kismiBoyut<<endl;
			int yedekPtr[25];
	int a;
	int x=0;
		while(x<((satir/size)-2)) //satir
		{
		int y=0;
		while(y<(sutun)-2) //sutun
		{
				a=0;
				for(int sa=x;sa<x+5;sa++) //satir 
					{
					for(int su=y;su<y+5;su++) //sutun
					{
						yedekPtr[a]=tekDiziPtr[sa*sutun+su]; // 5x5 matrisin elemanlar�n� yedek diziye ata.
						a++; // saya� 
					}
				}
				int tut;
				  for(int a=0;a<25;a++)
					{		
						for(int b=a;b<25;b++)				// S�ralama Fonksiyonu
						{
							if(yedekPtr[a]>yedekPtr[b])
							{
								tut  =yedekPtr[a];
								yedekPtr[a]=yedekPtr[b];
								yedekPtr[b]=tut;
							}
						}
					}
				yeniSayi=yedekPtr[12];    // K���kten B�y��e s�ralad���m�z dizinin ortas�ndaki say�y� al.
				ciktiMatrisPtr[(x+2)*sutun+(y+2)]=yeniSayi; // tekDiziPtr ye yeni say�y� yaz.
		y++;
		}
	x++;
	}
			

			//ciktiMatrisPtr ye kopyalama
			/*for(int i=0;i<kismiBoyut;i++){
					ciktiMatrisPtr[i]=bolumPtr[i];
			}*/
			// bilgisayarlardan d�nen dizileri toplama
			cout<<"kb:"<<kismiBoyut<<endl;
			for(int i=1;i<size-1;i++)
				MPI_Recv(&ciktiMatrisPtr[i*kismiBoyut],kismiBoyut,MPI_INT,i,ET,MPI_COMM_WORLD,&status);
			// gelen matrisi al ve diziye kaydet
			MPI_Recv(&ciktiMatrisPtr[(size-1)*kismiBoyut],sonPay,MPI_INT,size-1,ET,MPI_COMM_WORLD,&status);
			bitisZamani=MPI_Wtime();
			//dosya olusturma islemleri
			string uzanti;
			int sayi=22;
			while(sayi<argumanBoyutu-4)
			{
				uzanti=uzanti+arg[sayi];     // Argumandaki dosyan�n ismini alma
				sayi++;
			}
			
			ofstream dosyaCikti(uzanti+"_filtered.txt");
			
			if (dosyaCikti.is_open())
		  	{
			for (int q = 0; q < satir; q++){
		    	for (int t = 0; t < sutun; t++)
			    {
					dosyaCikti<<ciktiMatrisPtr[q*sutun+t]<<"\t";   // Filtrelenmi� matrisi dosyaya yazd�rma
				}
				dosyaCikti<<endl;
			}
			
			}
			
			dosyaCikti.close();
			delete [] tekDiziPtr;
			delete [] ciktiMatrisPtr;      //bellekte ay�rd���m�z yerleri geri b�rak�yoruz
			double gecenZaman=(bitisZamani-baslangicZamani)*1000; // ge�en s�reyi hesaplama
			cout<<"gecen zaman: "<<gecenZaman<<endl;
		}
		//son bilgisayar�n yapacaklar�
		else if(rank==size-1)
		{
			// gelen matrisi bilgilerini alma ve kay�t etmek.
			MPI_Recv(veriYolla,3,MPI_INT,0,ET,MPI_COMM_WORLD,&status);
			kismiBoyut=veriYolla[0];
			satir=veriYolla[1];
			sutun=veriYolla[2];

			//dizi olu�turma
			bolumPtr=new int[kismiBoyut+satir+satir];
			yeniBolumPtr=new int[kismiBoyut+satir+satir];
			// gelen matrisi al ve diziye kaydet
			MPI_Recv(bolumPtr,kismiBoyut,MPI_INT,0,ET,MPI_COMM_WORLD,&status);
			int yedekPtr[25];
	int yeniSayi;
	int a;
	int x=-2;
		while(x<((satir/size)-2))
		{
		int y=-2;
		while(y<(sutun-2))
		{
				a=0;
				for(int sa=x;sa<x+5;sa++)
					{
					for(int su=y;su<y+5;su++)
					{
						yedekPtr[a]=bolumPtr[sa*sutun+su]; // 5x5 matrisin elemanlar�n� yedek diziye ata.
						a++; // saya� 
					}
				}
				int tut;
				  for(int a=0;a<25;a++)
					{		
						for(int b=a;b<25;b++)				// S�ralama Fonksiyonu
						{
							if(yedekPtr[a]>yedekPtr[b])
							{
								tut  =yedekPtr[a];
								yedekPtr[a]=yedekPtr[b];
								yedekPtr[b]=tut;
							}
						}
					}
				yeniSayi=yedekPtr[12];    // K���kten B�y��e s�ralad���m�z dizinin ortas�ndaki say�y� al.
				yeniBolumPtr[(x+2)*sutun+(y+2)]=yeniSayi; // tekDiziPtr ye yeni say�y� yaz.	
		y++;
		}
	x++;
	}
			//yeni b�l�m dizisini master a geri g�nderme
			MPI_Send(&yeniBolumPtr[((((((satir/size)/size)/size)/size)/size)/size)],kismiBoyut,MPI_INT,0,ET,MPI_COMM_WORLD);
			delete [] bolumPtr;
			delete [] yeniBolumPtr;
		}
	
	//di�er bilgisayarlar
	else
	{
		MPI_Recv(veriYolla,3,MPI_INT,0,ET,MPI_COMM_WORLD,&status);
		//gelen verileri al
		kismiBoyut=veriYolla[0];
		satir=veriYolla[1];
		sutun=veriYolla[2];
		bolumPtr=new int[kismiBoyut];
		yeniBolumPtr=new int[kismiBoyut];
		// gelen matrisi al ve diziye kaydet
		MPI_Recv(bolumPtr,kismiBoyut,MPI_INT,0,ET,MPI_COMM_WORLD,&status);
		//filtreleme
		int yedekPtr[25];
	int yeniSayi;
	int a;
	int x=-2;
		while(x<((satir/size)-2))
		{
		int y=-2;
		while(y<(sutun-2))
		{
				a=0;
				for(int sa=x;sa<x+5;sa++)
					{
					for(int su=y;su<y+5;su++)
					{
						yedekPtr[a]=bolumPtr[sa*sutun+su]; // 5x5 matrisin elemanlar�n� yedek diziye ata.
						a++; // saya� 
					}
				}
				int tut;
				  for(int a=0;a<25;a++)
					{		
						for(int b=a;b<25;b++)				// S�ralama Fonksiyonu
						{
							if(yedekPtr[a]>yedekPtr[b])
							{
								tut  =yedekPtr[a];
								yedekPtr[a]=yedekPtr[b];
								yedekPtr[b]=tut;
							}
						}
					}
				yeniSayi=yedekPtr[12];    // K���kten B�y��e s�ralad���m�z dizinin ortas�ndaki say�y� al.
				yeniBolumPtr[(x+2)*sutun+(y+2)]=yeniSayi; // tekDiziPtr ye yeni say�y� yaz.
		y++;
		}
	x++;
	}
		//fitreleyip matrisi master a geri gonderiyoruz
		MPI_Send(&yeniBolumPtr[((((((satir/size)/size)/size)/size)/size)/size)],kismiBoyut,MPI_INT,0,ET,MPI_COMM_WORLD);
		delete [] bolumPtr;
		delete [] yeniBolumPtr;
	}
	MPI_Finalize();
return 0;
}
void siralamaFonk(int dizi[], int x) {
  int tut;
  for(int a=0;a<x;a++)
	{		
		for(int b=a;b<x;b++)				// S�ralama Fonksiyonu
		{
			if(dizi[a]>dizi[b])
			{
				tut  =dizi[a];
				dizi[a]=dizi[b];
				dizi[b]=tut;
			}
		}
	}
}
