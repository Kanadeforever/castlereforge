; Oracle: RPG.exe SHA256 b10c65f56051e5a625b6c34857bcb73bd002efe3c158b6bd0cc2bb17fa871dcf
; objdump -d -Mintel

  432130:	6a ff                	push   0xffffffff
  432132:	68 f6 ec 45 00       	push   0x45ecf6
  432137:	64 a1 00 00 00 00    	mov    eax,fs:0x0
  43213d:	50                   	push   eax
  43213e:	64 89 25 00 00 00 00 	mov    DWORD PTR fs:0x0,esp
  432145:	51                   	push   ecx
  432146:	56                   	push   esi
  432147:	8b f1                	mov    esi,ecx
  432149:	89 74 24 04          	mov    DWORD PTR [esp+0x4],esi
  43214d:	c7 06 c4 0d 46 00    	mov    DWORD PTR [esi],0x460dc4
  432153:	a1 4c 1c 8e 00       	mov    eax,ds:0x8e1c4c
  432158:	56                   	push   esi
  432159:	c7 44 24 14 01 00 00 	mov    DWORD PTR [esp+0x14],0x1
  432160:	00 
  432161:	c6 80 48 02 00 00 00 	mov    BYTE PTR [eax+0x248],0x0
  432168:	e8 23 26 00 00       	call   0x434790
  43216d:	8b ce                	mov    ecx,esi
  43216f:	e8 1c 06 00 00       	call   0x432790
  432174:	8b ce                	mov    ecx,esi
  432176:	e8 a5 26 00 00       	call   0x434820
  43217b:	8d 8e 2c 02 00 00    	lea    ecx,[esi+0x22c]
  432181:	c7 05 f0 01 8b 00 00 	mov    DWORD PTR ds:0x8b01f0,0x0
  432188:	00 00 00 
  43218b:	c6 44 24 10 00       	mov    BYTE PTR [esp+0x10],0x0
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
