
/mnt/data/RPG.exe:     file format pei-i386


Disassembly of section .text:

00432190 <.text+0x31190>:
  432190:	e8 7b f5 00 00       	call   0x441710
  432195:	8b ce                	mov    ecx,esi
  432197:	c7 44 24 10 ff ff ff 	mov    DWORD PTR [esp+0x10],0xffffffff
  43219e:	ff 
  43219f:	e8 6c 22 00 00       	call   0x434410
  4321a4:	8b 4c 24 08          	mov    ecx,DWORD PTR [esp+0x8]
  4321a8:	5e                   	pop    esi
  4321a9:	64 89 0d 00 00 00 00 	mov    DWORD PTR fs:0x0,ecx
  4321b0:	83 c4 10             	add    esp,0x10
  4321b3:	c3                   	ret
  4321b4:	90                   	nop
  4321b5:	90                   	nop
  4321b6:	90                   	nop
  4321b7:	90                   	nop
  4321b8:	90                   	nop
  4321b9:	90                   	nop
  4321ba:	90                   	nop
  4321bb:	90                   	nop
  4321bc:	90                   	nop
  4321bd:	90                   	nop
  4321be:	90                   	nop
  4321bf:	90                   	nop
  4321c0:	56                   	push   esi
  4321c1:	8b f1                	mov    esi,ecx
  4321c3:	8b 0d 4c 1c 8e 00    	mov    ecx,DWORD PTR ds:0x8e1c4c
  4321c9:	e8 22 c0 00 00       	call   0x43e1f0
  4321ce:	85 c0                	test   eax,eax
  4321d0:	89 86 28 02 00 00    	mov    DWORD PTR [esi+0x228],eax
  4321d6:	5e                   	pop    esi
  4321d7:	0f 9d c0             	setge  al
  4321da:	c3                   	ret
  4321db:	90                   	nop
  4321dc:	90                   	nop
  4321dd:	90                   	nop
  4321de:	90                   	nop
  4321df:	90                   	nop
  4321e0:	a1 48 1c 8e 00       	mov    eax,ds:0x8e1c48
  4321e5:	8b 88 b4 00 00 00    	mov    ecx,DWORD PTR [eax+0xb4]
  4321eb:	89 0d ac 01 8a 00    	mov    DWORD PTR ds:0x8a01ac,ecx
  4321f1:	8b 90 ac 00 00 00    	mov    edx,DWORD PTR [eax+0xac]
  4321f7:	89 15 b0 01 8a 00    	mov    DWORD PTR ds:0x8a01b0,edx
  4321fd:	b0 01                	mov    al,0x1
  4321ff:	c3                   	ret
  432200:	33 c0                	xor    eax,eax
  432202:	a3 ac 01 8a 00       	mov    ds:0x8a01ac,eax
  432207:	a3 b0 01 8a 00       	mov    ds:0x8a01b0,eax
  43220c:	c3                   	ret
  43220d:	90                   	nop
  43220e:	90                   	nop
  43220f:	90                   	nop
  432210:	6a ff                	push   0xffffffff
  432212:	68 0b ed 45 00       	push   0x45ed0b
  432217:	64 a1 00 00 00 00    	mov    eax,fs:0x0
  43221d:	50                   	push   eax
  43221e:	64 89 25 00 00 00 00 	mov    DWORD PTR fs:0x0,esp
  432225:	51                   	push   ecx
  432226:	56                   	push   esi
  432227:	8b 35 a4 01 8a 00    	mov    esi,DWORD PTR ds:0x8a01a4
  43222d:	85 f6                	test   esi,esi
  43222f:	74                   	.byte 0x74
