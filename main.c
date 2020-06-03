
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

//Tan�mlanan Semaforlar

sem_t berberler;
sem_t musteriler;
sem_t mutex;

//Tan�mlanan De�i�kenler

int koltukSayisi=1;
int musteriSayisi=0;
int sandalyeSayisi=5;
int bosSandalyeSayisi=0;
int simdikiMusteri=0;
int sandalye=0;
int *koltuk;

void Berber (void *kimlik){

// Berbere Kimlik Veriyoruz
int sayi=*(int*)kimlik+1;
int mID, sonrakiMusteri;

printf("%d.Berber\t-->\tdukkana geldi.\n",sayi);

	// Sonsuz D�ng� OLu�turulur
	while(1)
	{
		//M��teri YOksa berber z�bars�n
		if(!mID)
		{
		 	printf("%d.Berber\t-->\tuyudu.\n\n",sayi);
		}
		//Eri�imi Kitleyip Genel Kilidi Kald�r�yoruz.
		sem_wait(&berberler);
		sem_wait(&mutex);
		
		//Bekleyenlerin aras�ndan m��teri se�
		simdikiMusteri=(++simdikiMusteri)%sandalyeSayisi;
		sonrakiMusteri=simdikiMusteri;
		mID=koltuk[sonrakiMusteri];
		koltuk[sonrakiMusteri]=pthread_self();
		
		// Eri�imleri Kald�r
		sem_post(&mutex);
		sem_post(&musteriler);

		printf("%d. Berber\t-->\t%d. musterinin sacini kesmeye basladi. \n\n", sayi, mID);
		sleep(1);
		printf("%d. Berber\t-->\t%d. musterinin sacini kesmeye bitirdi. \n\n", sayi, mID);
		
		//Bekleyen m��teri yoksa uyu
		if(sandalyeSayisi==bosSandalyeSayisi)
		{
		   printf("%d. Berber\t-->\tuyudu. \n\n", sayi);
        }
	}
	pthread_exit(0);
}

void Musteri (void *kimlik)
{
	
int sayi=*(int*)kimlik+1; //m��terinin kimli�i
int oturulanSandalye,bID;

sem_wait(&mutex); //Genel eri�imi kilitle

printf("%d. Musteri\t-->\tdukkana geldi. \n",sayi);

//Bekleme odasinda bos sandalye varsa
if(bosSandalyeSayisi>0)
{
bosSandalyeSayisi--;
printf("%d. Musteri\t-->\tbekleme salonunda bekliyor. \n\n",sayi);

//Bekleme salonundan bir sandalye se�ip otur
sandalye= (++sandalye)%sandalyeSayisi;
oturulanSandalye=sandalye;
koltuk[oturulanSandalye]=sayi;

sem_post(&mutex);   //Eri�im kilidini kald�r
sem_post(&berberler); //S�radaki uygun berberi uyand�r.

sem_wait(&musteriler);   //Bekleyen m��teriler kuyru�una kat�l�yor.
sem_wait(&mutex); //Koltu�a eri�imi kilite.

//Berber koltu�una ge�.
bID=koltuk[oturulanSandalye];
bosSandalyeSayisi++;

sem_post(&mutex);
}
else  //Bekleme salonunda bo� sandalye yoksa d�kkandan ��k.
{ 
sem_post(&mutex);
printf("%d. Musteri\t-->\tbekleme salonunda yer bulamadi. Dukkandan ayriliyor. \n\n",sayi);
}

pthread_exit(0);
}//void M��teri sonu

int main(int argc){

//Gerekli verileri kullan�c�dan al�yoruz.
printf("Musteri sayisini girin: ");
scanf("%d",&musteriSayisi);


bosSandalyeSayisi=sandalyeSayisi;  //bo� sandalyeleleri belirle.
koltuk=(int*) malloc(sizeof(int) * sandalyeSayisi); //koltuk dizisini olu�turur.

int berberKimlikleri[koltukSayisi];

pthread_t berber[koltukSayisi];  //Berber threadleri
pthread_t musteri[musteriSayisi]; // Musteri threadleri

//Semaforlar� ba�lat
sem_init(&berberler,0,0);
sem_init(&musteriler,0,0);
sem_init(&mutex,0,1);

printf("\nBerber dukkani acildi.\n\n");
int i;
//Berber threadleri olu�tur
for(i=0; i<koltukSayisi; i++)
{
pthread_create(&berber[i],NULL, (void*)Berber, (void*)&i);
sleep(1); //thread olu�mas� i�in 1saniye
}
//Musteri threadleri olu�tur
for(i=0; i<musteriSayisi; i++)
{
pthread_create(&musteri[i], NULL, (void*)Musteri, (void*)&i);
srand((unsigned int)time(NULL));
usleep(rand() % (250000 - 50000 + 1) + 50000); //50000-250000 ms
}

//T�m m��terilerin i�lerinin bitmesini bekle.
for(i=0; i<musteriSayisi; i++)
{
pthread_join(musteri[i],NULL);
}

sleep(2);

//Semaforlari yok et
sem_destroy(&berberler);
sem_destroy(&musteriler);
sem_destroy(&mutex);

printf("Musterilere hizmet verildi. Dukkan kapatilacak...\n\n");

return 0;
}


