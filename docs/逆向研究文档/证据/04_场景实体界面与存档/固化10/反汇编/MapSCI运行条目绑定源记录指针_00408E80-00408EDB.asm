
/mnt/data/RPG.exe:     file format pei-i386


Disassembly of section .text:

00408e80 <.text+0x7e80>:
  408e80:	64 a1 00 00 00 00    	mov    eax,fs:0x0
  408e86:	6a ff                	push   0xffffffff
  408e88:	68 92 d0 45 00       	push   0x45d092
  408e8d:	50                   	push   eax
  408e8e:	64 89 25 00 00 00 00 	mov    DWORD PTR fs:0x0,esp
  408e95:	83 ec 64             	sub    esp,0x64
  408e98:	53                   	push   ebx
  408e99:	8b 5c 24 78          	mov    ebx,DWORD PTR [esp+0x78]
  408e9d:	55                   	push   ebp
  408e9e:	56                   	push   esi
  408e9f:	57                   	push   edi
  408ea0:	8b f1                	mov    esi,ecx
  408ea2:	8d 7b 74             	lea    edi,[ebx+0x74]
  408ea5:	89 1e                	mov    DWORD PTR [esi],ebx
  408ea7:	80 3f 00             	cmp    BYTE PTR [edi],0x0
  408eaa:	0f 84 e1 00 00 00    	je     0x408f91
  408eb0:	33 c0                	xor    eax,eax
  408eb2:	33 ed                	xor    ebp,ebp
  408eb4:	8a 83 d8 00 00 00    	mov    al,BYTE PTR [ebx+0xd8]
  408eba:	2b c5                	sub    eax,ebp
  408ebc:	74 55                	je     0x408f13
  408ebe:	48                   	dec    eax
  408ebf:	0f 85 f1 00 00 00    	jne    0x408fb6
  408ec5:	68 84 00 00 00       	push   0x84
  408eca:	e8 90 87 04 00       	call   0x45165f
  408ecf:	83 c4 04             	add    esp,0x4
  408ed2:	89 84 24 84 00 00 00 	mov    DWORD PTR [esp+0x84],eax
  408ed9:	3b c5                	cmp    eax,ebp
