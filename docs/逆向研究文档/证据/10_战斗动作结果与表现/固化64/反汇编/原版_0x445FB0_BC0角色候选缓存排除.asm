
/mnt/data/solid64_inputs/RPG.exe:     file format pei-i386


Disassembly of section .text:

00445fb0 <.text+0x44fb0>:
  445fb0:	53                   	push   ebx
  445fb1:	8b d9                	mov    ebx,ecx
  445fb3:	8a 83 f0 10 00 00    	mov    al,BYTE PTR [ebx+0x10f0]
  445fb9:	84 c0                	test   al,al
  445fbb:	74 60                	je     0x44601d
  445fbd:	56                   	push   esi
  445fbe:	57                   	push   edi
  445fbf:	8d bb b4 0f 00 00    	lea    edi,[ebx+0xfb4]
  445fc5:	b9 1c 00 00 00       	mov    ecx,0x1c
  445fca:	83 c8 ff             	or     eax,0xffffffff
  445fcd:	f3 ab                	rep stos DWORD PTR es:[edi],eax
  445fcf:	33 ff                	xor    edi,edi
  445fd1:	33 f6                	xor    esi,esi
  445fd3:	8b 83 18 02 00 00    	mov    eax,DWORD PTR [ebx+0x218]
  445fd9:	85 c0                	test   eax,eax
  445fdb:	74 22                	je     0x445fff
  445fdd:	8b 8c 30 2c 07 00 00 	mov    ecx,DWORD PTR [eax+esi*1+0x72c]
  445fe4:	03 c6                	add    eax,esi
  445fe6:	85 c9                	test   ecx,ecx
  445fe8:	7e 15                	jle    0x445fff
  445fea:	81 b8 e4 0c 00 00 c0 	cmp    DWORD PTR [eax+0xce4],0xbc0
  445ff1:	0b 00 00 
  445ff4:	74 09                	je     0x445fff
  445ff6:	57                   	push   edi
  445ff7:	50                   	push   eax
  445ff8:	8b cb                	mov    ecx,ebx
  445ffa:	e8 a1 fe ff ff       	call   0x445ea0
  445fff:	81 c6 e4 0d 00 00    	add    esi,0xde4
  446005:	47                   	inc    edi
  446006:	81 fe 40 de 00 00    	cmp    esi,0xde40
  44600c:	7c c5                	jl     0x445fd3
  44600e:	68 60 ea 00 00       	push   0xea60
  446013:	53                   	push   ebx
  446014:	8b cb                	mov    ecx,ebx
  446016:	e8 e5 e4 fe ff       	call   0x434500
  44601b:	5f                   	pop    edi
  44601c:	5e                   	pop    esi
  44601d:	5b                   	pop    ebx
  44601e:	c3                   	ret
