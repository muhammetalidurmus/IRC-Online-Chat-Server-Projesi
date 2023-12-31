# ft_irc

IRC, internet üzerindeki metin tabanlı iletişim protokolüdür ve gerçek zamanlı iletişim sağlar. IRC kullanıcıları, doğrudan mesaj alışverişi yapabilir ve grup kanallarına katılabilir. IRC istemcileri, IRC sunucusuna bağlanarak kanallara katılır. IRC sunucuları, bir ağ oluşturmak için birbirine bağlanır.


## Soket Programlama Nedir?

Ağ tabanlı uygulamaların geliştirilmesi yani ağlar arasında iletişim kurmak için kullanılır. Client-Server mantığına dayanır.
**Client**, bir hizmete erişmek isteyen bir bilgisayar olarak düşünülebilir.
**Server**, bir hizmeti sağlayan bilgisayar olarak düşünülebilir.

Client ve server soketleri kullanarak birbirleriyle iletişim kurarlar.

Bir client ve server arasındaki iletişim şu adımlarla gerçekleşir;

1. Server soketi oluşturma: Server, belirli bir port üzerinden gelecek clientlara hizmet vereceği için bir soket oluşturur ve bir port numarası belirler.

2. Server soketini bağlama: Server soketi, belirlenen port numarasına bağlanır ve gelecek clientları dinlemeye başlar.

3. Client soketi oluşturma: Client, serverla iletişim kurmak için bir soket oluşturur.

4. Client soketini servera bağlama: Client soketi, serverın IP adresi ve port numarasıyla bağlantı kurar.

5. Client ve server arasında veri iletişimi: Client ve server, soketleri üzerinden veri alışverişi yaparlar. Client, servera istek gönderir ve server bu isteği işleyerek cevap verir.

6. Client ve serverın bağlantıyı sonlandırması: İşlem tamamlandıktan sonra, client ve server soketlerini kapatır ve bağlantıyı sonlandırır.


Soket programlama, TCP veya UDP gibi iletişim protokollerini kullanarak veri iletişimi sağlar. İkisinden de kısaca bahsetmek gerekirse TCP, güvenilir ve sıralı veri iletimini sağlarken, UDP daha hızlı ancak güvenilirlik gerektirmeyen veri iletimi için kullanılır.

Özetle, soket programlama ağ üzerinde veri paylaşımı, mesajlaşma, dosya aktarımı, uzaktan erişim gibi birçok uygulama senaryosunda kullanılır.


<br />


## İnternet Protokolleri Nelerdir?

Ağ protokolü, 2 ya da daha fazla bilgisayar arasındaki iletişimi sağlamak amacıyla verileri düzenlemeye yarayan, standart olarak kabul edilmiş kurallar dizisidir.

**TCP**, kayıpsız veri gönderimi sağlayabilmek için kullanılan protokoldür. Gönderilen veriler için özel bir TCP kabul paketi (TCP ACK) gönderilir ve gelmiş olan paketlerin doğruluğu kontrol edilir. Gönderen taraf, kabul gelmediği sürece paketi tekrar gönderir, böylece gönderim sağlanmış olur.

**UDP**, veri gönderimini bağlantısız şekilde gerçekleştirmesidir. Ses ve video gönderiminde kullanılır. TCP'ye göre daha hızlıdır fakat güvenli değildir.


<br />


### TCP ve UDP Arasındaki Fark Nedir?
TCP bağlantı tabanlıdır, UDP bağlantı tabanlı değildir. TCP'de akış kontrolü vardır, UDP'de akış kontrolü yoktur. TCP başlığı (header) 20 bayttır, UDP başlığı 8 bayttır. TCP, UDP'den daha yavaştır, çünkü verinin karşı tarafa ulaşıp ulaşmadığını kontrol eder.


<br />


## Soketler ve Çeşitleri

4 farklı soket çeşidi vardır fakat genel olarak 2 tanesi kullanılır. Bunlar:

1. **Stream Soket (SOCK_STREAM)**: Bu tür soketler, güvenilir, veri iletimi için kullanılır. TCP üzerinden verinin doğru ve sıralı bir şekilde iletilmesini sağlarlar.

2. **Datagram Soket (SOCK_DGRAM)**: Bu tür soketler, güvenilirlik veya sıralama gerektirmeyen veri iletimi için kullanılır. UDP üzerinden çalışırlar ve bağlantısız bir iletişim modeli sağlarlar.


<br />


## Kullanılan Tüm Fonksiyonlar

```cpp
socket(AF_INET, SOCK_STREAM, 0);

 //Bir soket oluştururken ağ protokollerini belirlemek için kullanılır.
```


- `AF_INET`: Bir soket oluştururken ağ protokollerini belirlemek için kullanılır.


- `SOCK_STREAM`: TCP soketi oluşturulacağını belirtir.

- `0`: Default protokol kullanılır.






<br />


```cpp
int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen); 

//Fonksiyonu, bir soketi belirli bir adres ve port numarasına bağlamak için kullanılır.
```


- `sockfd`: Dinlemek istediğiniz soketin tanımlayıcısı (soket fd).

- `addr`: Bu parametre, IPv4 için struct sockaddr_in veya IPv6 için struct sockaddr_in6 yapılarından birini işaret edebilir.

- `addrlen`: addr yapısının boyutunu belirten bir socklen_t türünde bir değer.


<br />


```cpp
listen(int sockfd, int backlog);

// Fonksiyonu, bir soketi belirli bağlantı taleplerini dinlemek için kullanılan sokete dönüştürür.
```



- `sockfd`: Dinlemek istediğiniz soketin tanımlayıcısı (soket fd).

- `backlog`: Gelen bağlantı taleplerinin kuyruğunda bekleyebilecek maksimum sayı. Bu, aynı anda kabul edilebilecek bağlantı sayısını belirtir.


<br /> 


```cpp
accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
// Fonksiyonu, yeni bir client bağlantısını kabul etmek için kullanılır. 
//Bağlantıyı oluşturan clientın soket dosya tanımlayıcısını (clientSocket) döndürür.
```

- `sockfd`: Dinlemek istediğiniz soketin tanımlayıcısı (soket fd).

- `addr`: Yeni bağlantının adres bilgilerini tutar. Bağlantıyı sağlayan clientın IP adresi ve port numarası gibi bilgileri almak için kullanılır. Bu parametreyi NULL olarak belirtebilirsiniz.

- `addrlen`: addr boyutunu tutar. Bu parametreyi NULL olarak belirtebilirsiniz.



<br />


```cpp
ssize_t recv(int sockfd, void *buf, size_t len, int flags);
// Fonksiyonu, soketten veri almak için kullanılır. Client ve server tarafında veri alışverişi sağlar.
```


- `sockfd`: Dinlemek istediğiniz soketin tanımlayıcısı (soket fd).

- `buf`: Alınan verinin hedef bellek alanını temsil eden bir pointer.
	
- `len`: buf bellek alanının boyutu (byte cinsinden).
	
- `flags`: İsteğe bağlı. Özel bir işlem yapmak için kullanılabilir veya 0 olarak belirtilebilir.


<br />

```cpp
fcntl(sock, F_SETFL, O_NONBLOCK);
// Fonksiyonu, soketin fd üzerindeki dosya özelliklerini değiştirmek için kullanılır.
```
- `F_GETFL` Okuma işlemi yapılır. Dosya tanımlayıcısının mevcut dosya özelliklerini döndürür.

- `F_SETFL` Yazma işlemi yapılır. Dosya tanımlayıcısının dosya özelliklerini belirtilen değerlere göre değiştirir.

- `O_NONBLOCK`: Engellenmeyen modda açmak için kullanılan bir dosya açma flagidir.



<br />


```cpp
int poll(struct pollfd *fds, nfds_t nfds, int timeout);
//Fonksiyonu, çoklu soket girişini aynı anda takip etmek ve olayları yönetmek için kullanılır.
//Bir dizi soketi izleyerek belirli bir olayın gerçekleşip gerçekleşmediğini kontrol eder. 
```

- `fds`: struct pollfd türünden bir dizi, izlenecek soketlerin ve beklenen olayların bilgisini içerir.
	
- `nfds`: fds dizisinin boyutu, izlenecek soket sayısını belirtir.
	
- `timeout`: İşlemin zaman aşımı süresini belirtir. Bu süre milisaniye cinsinden ifade edilir. Negatif değerler süresiz beklemeyi, 0 değeri anında dönüşü sağlar.



<br />

<!-- commands table -->
## KOMUTLAR


| Komut | Açıklama |
| :-----------: | :----------- |
| Join | Client'ın verilen kanala katılmak istediğini, her kanalın kendisi için verilen anahtarı kullandığını belirtir.  |
| Kick | Kullanıcının bir kanaldan zorla çıkarılmasını talep etmek için kullanılabilir.  |
| List | Her kanal hakkında bazı bilgilerle birlikte kanalların bir listesini almak için kullanılır.  |
| Nick | Client'a bir takma ad vermek veya bir öncekini değiştirmek için kullanılır.  |
| Notice | Hem kullanıcılar arasında hem de kanallara bildirim göndermek için kullanılır. "NOTICE" ve "PRIVMSG" arasındaki fark, "NOTICE" mesajına yanıt olarak otomatik yanıtların asla gönderilmemesidir.  |
| Pass | Bir "bağlantı şifresi" ayarlamak için kullanılır. Ayarlanırsa, bağlantıyı kaydetmeye yönelik herhangi bir girişimde bulunulmadan önce parola ayarlanmalıdır.  |
| Ping | Uygulama katmanında, bağlantının diğer tarafının hala bağlı olup olmadığını kontrol etmek veya bağlantı gecikmesini kontrol etmek için clientlar veya serverlar tarafından gönderilir.  |
| Pong | Serverlar arasında bağlantı kontrolü yapar. "PING" mesajı gönderildikten sonra kullanıcı client'ı server'a cevap olarak "PONG" mesajı gönderir. Bu, server ile client arasındaki bağlantının sağlıklı olduğunu gösterir ve iletişimin devam edebileceğini işaret eder.  |
| Privmsg | Kullanıcılar arasında özel mesaj göndermek ve ayrıca kanallara mesaj göndermek için kullanılır.  |
| Quit | Bir clientın serverla olan bağlantısını sonlandırmak için kullanılır. Server bunu bir "ERROR" mesajıyla yanıtlayarak ve client bağlantısını kapatarak onaylar.  |
| User | Yeni bir kullanıcının kullanıcı adını ve gerçek adını belirtmek için bir bağlantının başlangıcında kullanılır.  |



