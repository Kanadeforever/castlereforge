
/mnt/data/work54/input/RPG.exe:     file format pei-i386


Disassembly of section .text:

004321d0 <.text+0x311d0>:
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
  43221e:	64 89      	mov    DWORD PTR fs:0x0,esp
