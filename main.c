
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

//Tanýmlanan Semaforlar

sem_t berberler;
sem_t musteriler;
sem_t mutex;

//Tanýmlanan Deðiþkenler

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

	// Sonsuz DÖngü OLuþturulur
	while(1)
	{
		//Müþteri YOksa berber zýbarsýn
		if(!mID)
		{
		 	printf("%d.Berber\t-->\tuyudu.\n\n",sayi);
		}
		//Eriþimi Kitleyip Genel Kilidi Kaldýrýyoruz.
		sem_wait(&berberler);
		sem_wait(&mutex);
		
		//Bekleyenlerin arasýndan müþteri seç
		simdikiMusteri=(++simdikiMusteri)%sandalyeSayisi;
		sonrakiMusteri=simdikiMusteri;
		mID=koltuk[sonrakiMusteri];
		koltuk[sonrakiMusteri]=pthread_self();
		
		// Eriþimleri Kaldýr
		sem_post(&mutex);
		sem_post(&musteriler);

		printf("%d. Berber\t-->\t%d. musterinin sacini kesmeye basladi. \n\n", sayi, mID);
		sleep(1);
		printf("%d. Berber\t-->\t%d. musterinin sacini kesmeye bitirdi. \n\n", sayi, mID);
		
		//Bekleyen müþteri yoksa uyu
		if(sandalyeSayisi==bosSandalyeSayisi)
		{
		   printf("%d. Berber\t-->\tuyudu. \n\n", sayi);
        }
	}
	pthread_exit(0);
}

void Musteri (void *kimlik)
{
	
int sayi=*(int*)kimlik+1; //müþterinin kimliði
int oturulanSandalye,bID;

sem_wait(&mutex); //Genel eriþimi kilitle

printf("%d. Musteri\t-->\tdukkana geldi. \n",sayi);

//Bekleme odasinda bos sandalye varsa
if(bosSandalyeSayisi>0)
{
bosSandalyeSayisi--;
printf("%d. Musteri\t-->\tbekleme salonunda bekliyor. \n\n",sayi);

//Bekleme salonundan bir sandalye seçip otur
sandalye= (++sandalye)%sandalyeSayisi;
oturulanSandalye=sandalye;
koltuk[oturulanSandalye]=sayi;

sem_post(&mutex);   //Eriþim kilidini kaldýr
sem_post(&berberler); //Sýradaki uygun berberi uyandýr.

sem_wait(&musteriler);   //Bekleyen müþteriler kuyruðuna katýlýyor.
sem_wait(&mutex); //Koltuða eriþimi kilite.

//Berber koltuðuna geç.
bID=koltuk[oturulanSandalye];
bosSandalyeSayisi++;

sem_post(&mutex);
}
else  //Bekleme salonunda boþ sandalye yoksa dükkandan çýk.
{ 
sem_post(&mutex);
printf("%d. Musteri\t-->\tbekleme salonunda yer bulamadi. Dukkandan ayriliyor. \n\n",sayi);
}

pthread_exit(0);
}//void Müþteri sonu

int main(int argc){

//Gerekli verileri kullanýcýdan alýyoruz.
printf("Musteri sayisini girin: ");
scanf("%d",&musteriSayisi);


bosSandalyeSayisi=sandalyeSayisi;  //boþ sandalyeleleri belirle.
koltuk=(int*) malloc(sizeof(int) * sandalyeSayisi); //koltuk dizisini oluþturur.

int berberKimlikleri[koltukSayisi];

pthread_t berber[koltukSayisi];  //Berber threadleri
pthread_t musteri[musteriSayisi]; // Musteri threadleri

//Semaforlarý baþlat
sem_init(&berberler,0,0);
sem_init(&musteriler,0,0);
sem_init(&mutex,0,1);

printf("\nBerber dukkani acildi.\n\n");
int i;
//Berber threadleri oluþtur
for(i=0; i<koltukSayisi; i++)
{
pthread_create(&berber[i],NULL, (void*)Berber, (void*)&i);
sleep(1); //thread oluþmasý için 1saniye
}
//Musteri threadleri oluþtur
for(i=0; i<musteriSayisi; i++)
{
pthread_create(&musteri[i], NULL, (void*)Musteri, (void*)&i);
srand((unsigned int)time(NULL));
usleep(rand() % (250000 - 50000 + 1) + 50000); //50000-250000 ms
}

//Tüm müþterilerin iþlerinin bitmesini bekle.
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


