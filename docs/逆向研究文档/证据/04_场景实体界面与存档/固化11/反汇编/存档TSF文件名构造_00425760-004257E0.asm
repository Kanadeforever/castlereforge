; Oracle: /mnt/data/RPG.exe
; SHA256: b10c65f56051e5a625b6c34857bcb73bd002efe3c158b6bd0cc2bb17fa871dcf  /mnt/data/RPG.exe
; objdump -D -Mintel --start-address=0x425760 --stop-address=0x4257e0 RPG.exe

/mnt/data/RPG.exe:     file format pei-i386


Disassembly of section .text:

00425760 <.text+0x24760>:
  425760:	64 a1 00 00 00 00    	mov    eax,fs:0x0
  425766:	6a ff                	push   0xffffffff
  425768:	68 14 e4 45 00       	push   0x45e414
  42576d:	50                   	push   eax
  42576e:	b8 e4 e1 00 00       	mov    eax,0xe1e4
  425773:	64 89 25 00 00 00 00 	mov    DWORD PTR fs:0x0,esp
  42577a:	e8 21 ca 02 00       	call   0x4521a0
  42577f:	8b 84 24 f4 e1 00 00 	mov    eax,DWORD PTR [esp+0xe1f4]
  425786:	53                   	push   ebx
  425787:	56                   	push   esi
  425788:	68 3c a4 46 00       	push   0x46a43c
  42578d:	50                   	push   eax
  42578e:	68 30 a4 46 00       	push   0x46a430
  425793:	8d 4c 24 18          	lea    ecx,[esp+0x18]
  425797:	68 24 a4 46 00       	push   0x46a424
  42579c:	51                   	push   ecx
  42579d:	bb 01 00 00 00       	mov    ebx,0x1
  4257a2:	e8 cc c2 02 00       	call   0x451a73
  4257a7:	6a 14                	push   0x14
  4257a9:	e8 b1 be 02 00       	call   0x45165f
  4257ae:	83 c4 18             	add    esp,0x18
  4257b1:	89 44 24 08          	mov    DWORD PTR [esp+0x8],eax
  4257b5:	85 c0                	test   eax,eax
  4257b7:	c7 84 24 f4 e1 00 00 	mov    DWORD PTR [esp+0xe1f4],0x0
  4257be:	00 00 00 00 
  4257c2:	74 0c                	je     0x4257d0
  4257c4:	53                   	push   ebx
  4257c5:	8b c8                	mov    ecx,eax
  4257c7:	e8 24 bf 01 00       	call   0x4416f0
  4257cc:	8b f0                	mov    esi,eax
  4257ce:	eb 02                	jmp    0x4257d2
  4257d0:	33 f6                	xor    esi,esi
  4257d2:	6a 00                	push   0x0
  4257d4:	8d 54 24 10          	lea    edx,[esp+0x10]
  4257d8:	53                   	push   ebx
  4257d9:	52                   	push   edx
  4257da:	8b ce                	mov    ecx,esi
  4257dc:	c7 84 24 00    	mov    DWORD PTR [esp+0xe200],0xffffffff
  4257e3:	    
