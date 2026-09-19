
/mnt/data/RPG.exe:     file format pei-i386


Disassembly of section .text:

00442900 <.text+0x41900>:
  442900:	4e                   	dec    esi
  442901:	75 ef                	jne    0x4428f2
  442903:	83 c7 36             	add    edi,0x36
  442906:	83 c3 1b             	add    ebx,0x1b
  442909:	4d                   	dec    ebp
  44290a:	75 dd                	jne    0x4428e9
  44290c:	5f                   	pop    edi
  44290d:	5e                   	pop    esi
  44290e:	5d                   	pop    ebp
  44290f:	5b                   	pop    ebx
  442910:	c3                   	ret
  442911:	90                   	nop
  442912:	90                   	nop
  442913:	90                   	nop
  442914:	90                   	nop
  442915:	90                   	nop
  442916:	90                   	nop
  442917:	90                   	nop
  442918:	90                   	nop
  442919:	90                   	nop
  44291a:	90                   	nop
  44291b:	90                   	nop
  44291c:	90                   	nop
  44291d:	90                   	nop
  44291e:	90                   	nop
  44291f:	90                   	nop
  442920:	a0 0c 24 8e 00       	mov    al,ds:0x8e240c
  442925:	57                   	push   edi
  442926:	84 c0                	test   al,al
  442928:	8b f9                	mov    edi,ecx
  44292a:	0f 84 af 00 00 00    	je     0x4429df
  442930:	8b 0d 48 1c 8e 00    	mov    ecx,DWORD PTR ds:0x8e1c48
  442936:	53                   	push   ebx
  442937:	bb 01 00 00 00       	mov    ebx,0x1
  44293c:	56                   	push   esi
  44293d:	8b 81 f4 00 00 00    	mov    eax,DWORD PTR [ecx+0xf4]
  442943:	3b c3                	cmp    eax,ebx
  442945:	7e 11                	jle    0x442958
  442947:	8a 15 dc 96 46 00    	mov    dl,BYTE PTR ds:0x4696dc
  44294d:	84 d2                	test   dl,dl
  44294f:	74 07                	je     0x442958
  442951:	48                   	dec    eax
  442952:	89 81 f4 00 00 00    	mov    DWORD PTR [ecx+0xf4],eax
  442958:	a1 74 fd 89 00       	mov    eax,ds:0x89fd74
  44295d:	8a 88 fc 05 00 00    	mov    cl,BYTE PTR [eax+0x5fc]
  442963:	84 c9                	test   cl,cl
  442965:	74 3c                	je     0x4429a3
  442967:	c6 05 dc 96 46 00 00 	mov    BYTE PTR ds:0x4696dc,0x0
  44296e:	8a 87 30 e3 00 00    	mov    al,BYTE PTR [edi+0xe330]
  442974:	84 c0                	test   al,al
  442976:	c7 87 24 e2 00 00 08 	mov    DWORD PTR [edi+0xe224],0x8
  44297d:	00 00 00 
