/*
	           Hazýrlayan : Osman Arda DEMIR
	           Öðrenci No : 160757053
	               Bölümü : Bilgisayar Mühendisliði
	   Dersin Adý ve Kodu : PARALEL PROGRAMLAMAYA GÝRÝS, BM-308-50-50
   	             Proje No : 2
Dersi Veren Öðretim Üyesi : Dr. Öðr. Üyesi Tolga AYDIN
*/


#include <iostream>// Cout Cin
#include <mpi.h>  //MPI kütüphanesi
#include <cstdio>
#include <fstream> // dosya iþlemleri
#include <sys/time.h>//struct timeval ve gettimeofday
#include <cstdlib>
#include <ctime>
#include <string> //kelime tutmak için

#define ET 16

using namespace std;

void siralamaFonk(int* , int); // Diziyi Küçükten Büyüðe Sýralamamýz Ýçin Fonksiyonu

int main(int argc, char* argv[])
{
	int rank,size;
	int satir,sutun;
	int *bolumPtr;//matrisin bir bölümünü tutmak için
	int *yeniBolumPtr;//oluþan yeni bölümü tutmak için
	int *ciktiMatrisPtr;
 	int *tekDiziPtr;
	int veriYolla[3];  //veriyollama dizisi
	int kismiBoyut;   
	MPI_Status status;
	string arg=argv[1];
	int str;
	int argumanBoyutu=arg.size(); // Arguman Boyutunu Öðrenme
	MPI_Init(&argc,&argv);
	// bilgisayar rankini ve sayýsýný öðrenme
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
	MPI_Comm_size(MPI_COMM_WORLD,&size);
	//master rank i 0 olan pc de çalýþacak kod
		if(rank==0)
		{
			//dosya okuma islemi
			double bitisZamani,baslangicZamani;//geçen zamaný ölçmek icin
			int sonPay;
			ifstream dosyaOku(argv[1]); // Dosya Açma iþlemi
			
			//Dosyadan satýr ve sutununu okuma iþlemi
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
			        dosyaOku>>tekDiziPtr[i*sutun+j];       // Dosyadan Okunan Matris tek Boyutlu Diziye Atýlmasý
			    }
			}
			for (int i = 0; i < satir; i++){
				for (int j = 0; j < sutun; j++)
			    {
			        ciktiMatrisPtr[i*sutun+j]=tekDiziPtr[i*sutun+j];       // Dosyadan Okunan Matrisin çýktýMatris Dizisine Atýlmasý
			    }
			}
		dosyaOku.close();
		//tam bölümüyormu?
			if(diziBoyutu%size==0)
			{
				kismiBoyut=diziBoyutu/size;	
				sonPay=kismiBoyut;                  //tam bölünüyorsa
				str=satir/size;
			}
			else
			{
				kismiBoyut=diziBoyutu/size;                 //tam bölünmüyorsa
				sonPay=diziBoyutu-(kismiBoyut*(size-1));
					
			}
					
			//verileri dizide toplama
			veriYolla[0]=kismiBoyut;
			veriYolla[1]=satir;
			veriYolla[2]=sutun;
			//Sure hesaplamak için baþlangýç zamanýný tut
			baslangicZamani=MPI_Wtime();

			//dizi gonderme blok boyutu satir sutun
			for(int i=1;i<size-1;i++){
				MPI_Send(veriYolla,3,MPI_INT,i,ET,MPI_COMM_WORLD);
			}
			veriYolla[0]=sonPay;
			MPI_Send(veriYolla,3,MPI_INT,(size-1),ET,MPI_COMM_WORLD);
			//diger bilgisayarlarýn payina düsen veriler
			for(int i=1;i<size-1;i++){
				MPI_Send(&tekDiziPtr[(kismiBoyut*i)],kismiBoyut,MPI_INT,i,ET,MPI_COMM_WORLD);
			}
			//en son bilgisayara payina düsen veriler
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
						yedekPtr[a]=tekDiziPtr[sa*sutun+su]; // 5x5 matrisin elemanlarýný yedek diziye ata.
						a++; // sayaç 
					}
				}
				int tut;
				  for(int a=0;a<25;a++)
					{		
						for(int b=a;b<25;b++)				// Sýralama Fonksiyonu
						{
							if(yedekPtr[a]>yedekPtr[b])
							{
								tut  =yedekPtr[a];
								yedekPtr[a]=yedekPtr[b];
								yedekPtr[b]=tut;
							}
						}
					}
				yeniSayi=yedekPtr[12];    // Küçükten Büyüðe sýraladýðýmýz dizinin ortasýndaki sayýyý al.
				ciktiMatrisPtr[(x+2)*sutun+(y+2)]=yeniSayi; // tekDiziPtr ye yeni sayýyý yaz.
		y++;
		}
	x++;
	}
			

			//ciktiMatrisPtr ye kopyalama
			/*for(int i=0;i<kismiBoyut;i++){
					ciktiMatrisPtr[i]=bolumPtr[i];
			}*/
			// bilgisayarlardan dönen dizileri toplama
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
				uzanti=uzanti+arg[sayi];     // Argumandaki dosyanýn ismini alma
				sayi++;
			}
			
			ofstream dosyaCikti(uzanti+"_filtered.txt");
			
			if (dosyaCikti.is_open())
		  	{
			for (int q = 0; q < satir; q++){
		    	for (int t = 0; t < sutun; t++)
			    {
					dosyaCikti<<ciktiMatrisPtr[q*sutun+t]<<"\t";   // Filtrelenmiþ matrisi dosyaya yazdýrma
				}
				dosyaCikti<<endl;
			}
			
			}
			
			dosyaCikti.close();
			delete [] tekDiziPtr;
			delete [] ciktiMatrisPtr;      //bellekte ayýrdýðýmýz yerleri geri býrakýyoruz
			double gecenZaman=(bitisZamani-baslangicZamani)*1000; // geçen süreyi hesaplama
			cout<<"gecen zaman: "<<gecenZaman<<endl;
		}
		//son bilgisayarýn yapacaklarý
		else if(rank==size-1)
		{
			// gelen matrisi bilgilerini alma ve kayýt etmek.
			MPI_Recv(veriYolla,3,MPI_INT,0,ET,MPI_COMM_WORLD,&status);
			kismiBoyut=veriYolla[0];
			satir=veriYolla[1];
			sutun=veriYolla[2];

			//dizi oluþturma
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
						yedekPtr[a]=bolumPtr[sa*sutun+su]; // 5x5 matrisin elemanlarýný yedek diziye ata.
						a++; // sayaç 
					}
				}
				int tut;
				  for(int a=0;a<25;a++)
					{		
						for(int b=a;b<25;b++)				// Sýralama Fonksiyonu
						{
							if(yedekPtr[a]>yedekPtr[b])
							{
								tut  =yedekPtr[a];
								yedekPtr[a]=yedekPtr[b];
								yedekPtr[b]=tut;
							}
						}
					}
				yeniSayi=yedekPtr[12];    // Küçükten Büyüðe sýraladýðýmýz dizinin ortasýndaki sayýyý al.
				yeniBolumPtr[(x+2)*sutun+(y+2)]=yeniSayi; // tekDiziPtr ye yeni sayýyý yaz.	
		y++;
		}
	x++;
	}
			//yeni bölüm dizisini master a geri gönderme
			MPI_Send(&yeniBolumPtr[((((((satir/size)/size)/size)/size)/size)/size)],kismiBoyut,MPI_INT,0,ET,MPI_COMM_WORLD);
			delete [] bolumPtr;
			delete [] yeniBolumPtr;
		}
	
	//diðer bilgisayarlar
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
						yedekPtr[a]=bolumPtr[sa*sutun+su]; // 5x5 matrisin elemanlarýný yedek diziye ata.
						a++; // sayaç 
					}
				}
				int tut;
				  for(int a=0;a<25;a++)
					{		
						for(int b=a;b<25;b++)				// Sýralama Fonksiyonu
						{
							if(yedekPtr[a]>yedekPtr[b])
							{
								tut  =yedekPtr[a];
								yedekPtr[a]=yedekPtr[b];
								yedekPtr[b]=tut;
							}
						}
					}
				yeniSayi=yedekPtr[12];    // Küçükten Büyüðe sýraladýðýmýz dizinin ortasýndaki sayýyý al.
				yeniBolumPtr[(x+2)*sutun+(y+2)]=yeniSayi; // tekDiziPtr ye yeni sayýyý yaz.
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
		for(int b=a;b<x;b++)				// Sýralama Fonksiyonu
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
