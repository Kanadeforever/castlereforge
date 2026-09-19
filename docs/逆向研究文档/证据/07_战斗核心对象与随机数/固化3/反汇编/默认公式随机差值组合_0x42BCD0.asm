
/mnt/data/RPG.exe:     file format pei-i386


Disassembly of section .text:

0042bcd0 <.text+0x2acd0>:
  42bcd0:	8b 44 24 04          	mov    eax,DWORD PTR [esp+0x4]
  42bcd4:	56                   	push   esi
  42bcd5:	8b 70 3c             	mov    esi,DWORD PTR [eax+0x3c]
  42bcd8:	b8 1f 85 eb 51       	mov    eax,0x51eb851f
  42bcdd:	f7 ee                	imul   esi
  42bcdf:	c1 fa 03             	sar    edx,0x3
  42bce2:	8b c2                	mov    eax,edx
  42bce4:	c1 e8 1f             	shr    eax,0x1f
  42bce7:	03 d0                	add    edx,eax
  42bce9:	b8 67 66 66 66       	mov    eax,0x66666667
  42bcee:	52                   	push   edx
  42bcef:	f7 ee                	imul   esi
  42bcf1:	c1 fa 02             	sar    edx,0x2
  42bcf4:	8b c2                	mov    eax,edx
  42bcf6:	c1 e8 1f             	shr    eax,0x1f
  42bcf9:	03 d0                	add    edx,eax
  42bcfb:	52                   	push   edx
  42bcfc:	e8 af 0f 00 00       	call   0x42ccb0
  42bd01:	8b 4c 24 0c          	mov    ecx,DWORD PTR [esp+0xc]
  42bd05:	8b 74 24 10          	mov    esi,DWORD PTR [esp+0x10]
  42bd09:	2b ce                	sub    ecx,esi
  42bd0b:	5e                   	pop    esi
  42bd0c:	03 c1                	add    eax,ecx
  42bd0e:	85 c0                	test   eax,eax
  42bd10:	7f 05                	jg     0x42bd17
  42bd12:	b8 01 00 00 00       	mov    eax,0x1
  42bd17:	c2 10 00             	ret    0x10
